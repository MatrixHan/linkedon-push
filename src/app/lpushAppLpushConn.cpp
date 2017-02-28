#include <lpushAppLpushConn.h>
#include <lpushServer.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushUtils.h>
#include <lpushProtocolStack.h>
#include <lpushSource.h>
#include <lpushRecvThread.h>
#include <lpushConsumThread.h>
#include <lpushFmt.h>
#include <lpushJson.h>
#include <lpushRedis.h>
#include <lpushMongoClient.h>
namespace lpush{
  
#define LP_PAUSED_SEND_TIMEOUT_US (int64_t)(60*1000*1000LL)
// if timeout, close the connection.
#define LP_PAUSED_RECV_TIMEOUT_US (int64_t)(60*1000*1000LL)
  
#define LPT_PAUSED_SEND_TIMEOUT_US (int64_t)(10*1000*1000LL)
// if timeout, close the connection.
#define LPT_PAUSED_RECV_TIMEOUT_US (int64_t)(10*1000*1000LL)
  
#define LP_HREAT_TIMEOUT_US (int64_t)(60*1000*1000LL)
  
#define LP_HREAT_TIMEOUT_US_MIN (int64_t)(10*1000LL)
  

LPushConn::LPushConn(LPushServer* _server, st_netfd_t client_stfd): LPushConnection(_server, client_stfd)
{
    before_data_time =hreat_data_time= 0;
    dispose = false;
    skt = new LPushStSocket(client_stfd);
    client =NULL;
    redisApp = NULL;
    redisPlatform = NULL;
    lpushProtocol = new LPushProtocol(skt);
    trd = NULL;
    trd2 = NULL;
    
}

LPushConn::~LPushConn()
{
    SafeDelete(lpushProtocol);
    SafeDelete(skt);
    SafeDelete(trd);
    SafeDelete(trd2);
    SafeDelete(redisApp);
    SafeDelete(redisPlatform);
    if(stfd)
    LPushSource::destroy(stfd);
    if(client)
    LPushSource::destroy(client);
}


int LPushConn::do_cycle()
{
    int ret = ERROR_SUCCESS;
    before_data_time =hreat_data_time= st_utime();
    skt->set_recv_timeout(LP_PAUSED_RECV_TIMEOUT_US);
    skt->set_send_timeout(LP_PAUSED_SEND_TIMEOUT_US);
   LPushHandshakeMessage lpsm;
    if((ret = handshake(lpsm)) != ERROR_SUCCESS)
    {
	lp_warn("conn handshake error");
	lpushProtocol->sendHandshake(0x00);
	return ret;
    }
    lphandshakeMsg = &lpsm;
    if((ret = checkUserMessage(lpsm)) != ERROR_SUCCESS)
    {
	lp_warn("lpush user identity match error %d",ret);
	lpushProtocol->sendHandshake(0x02);
	return ret;
    }
    if((ret = lpushProtocol->sendHandshake(0x01))!=ERROR_SUCCESS)
    {
	lp_warn("lpush send handshake error");
	return ret;
    }
    if((ret = createConnection()) != ERROR_SUCCESS)
    {
      lp_warn("conn createConnection error");
       return ret;
    }
    trd =new LPushRecvThread(client,this,350);
    
    trd->start();
    
    trd2 = new LPushConsumThread(client,350);
    
    trd2->start();
    while(!dispose)
    {
 	long long now = st_utime();
	long long internal = now - before_data_time;
	if(internal > LP_HREAT_TIMEOUT_US )
	{
	   ret = ERROR_CONN_HREATBEAT_TIMEOUT;
	   break;
	}
	st_usleep(350*1000);
    }
    return ret;
}
bool LPushConn::is_active()
{
    return trd && trd2;
}


int LPushConn::handshake(LPushHandshakeMessage &msk)
{
     int ret = ERROR_SUCCESS;
     LPushChunk lpc;
    if((ret = lpushProtocol->readMessage(skt,lpc))!=ERROR_SUCCESS)
    {
	lp_warn("conn readMessage error");
	return ret;
    }
    
    return lpushProtocol->handshake(&lpc,msk);
}

int LPushConn::createConnection()
{
    int ret = ERROR_SUCCESS;
     LPushCreateMessage lpcm;
     LPushChunk lpc;
    if((ret = lpushProtocol->readMessage(skt,lpc))!=ERROR_SUCCESS)
    {
	lp_warn("conn readMessage error");
	return ret;
    }
    if((ret = lpushProtocol->createConnection(&lpc,lpcm))!=ERROR_SUCCESS)
    {
	lp_warn("conn createConnection error %d",ret);
	return ret;
    }
    char buf[6];
    clientKey = lphandshakeMsg->appId + lphandshakeMsg->userId;
    sprintf(buf,"%d",conf->port);
    hostname = conf->localhost+":"+std::string(buf);
    redis_client->set(clientKey,hostname);
    redis_client->expire(clientKey,60);
    redisApp = new LPushAPPKey(lphandshakeMsg->appId,lphandshakeMsg->userId,
			       conf->localhost,conf->port);
    redisPlatform = new LPushPlatform(lphandshakeMsg->clientFlag,lphandshakeMsg->appId,
				      lphandshakeMsg->userId,conf->localhost,conf->port);
    userInsertMongodb(lphandshakeMsg);
    long long time = st_utime();
    redis_client->zadd(redisApp->appKey,time/1000L,redisApp->key);
    redis_client->zadd(redisPlatform->platformKey,time/1000L,redisPlatform->key);
  
    
    LPushClient *clia = LPushSource::instance(lphandshakeMsg->userId,
				   lphandshakeMsg->appId,lphandshakeMsg->screteKey);
    if(clia)
    {
	ret = ERROR_USER_IS_EXIST;
	lp_warn("already user conn! %d",ret);
	lpushProtocol->sendCreateConnection(0x02);
	return ret;
    }
    LPushSource * source = LPushSource::create(stfd);
    
    client = LPushSource::create(stfd,source,lphandshakeMsg,this);
    if((ret=lpushProtocol->sendCreateConnection(0x01))!=ERROR_SUCCESS)
    {
	lp_warn("conn sendCreateConnection error %d",ret);
	return ret;
    }
    selectMongoHistoryWork();
    return ret;
}

int LPushConn::checkUserMessage(LPushHandshakeMessage msg)
{
     int ret = ERROR_SUCCESS;
     string screteKey = redis_client->hget(conf->appKeys,msg.appId);
     if(screteKey.empty()){
	  ret = ERROR_USER_SCRETE_NO_EXIST;
	  lp_warn("handshake public key message not found this system %d",ret);
	  return ret;
     }
     if(screteKey.find(msg.screteKey.c_str())==string::npos)
     {
	  ret = ERROR_USER_SCRETE_MISMATCH;
	  lp_warn("handshake screte key message mismatch this system %d",ret);
	  return ret;
     }
     return ret;
}


int LPushConn::userInsertMongodb(LPushHandshakeMessage *msg)
{
    int ret = ERROR_SUCCESS;
    static std::string prefix = "MEMBER_";
    std::string collectionName = prefix + msg->appId;
    map<string,string>  lpmap = msg->toMongomap();
    int time = getCurrentTime();
    char buf[20];
    memset(buf,0,20);
    sprintf(buf,"%d",time);
    lpmap.insert(make_pair("time",string(buf)));
    map<string,string> params;
    params.insert(make_pair("userId",msg->userId));
    params.insert(make_pair("appKey",msg->appId));
    vector<string> result = mongodb_client->queryToListJson(conf->mongodbConfig->db,collectionName,params);
    if(result.size()==0)
    mongodb_client->insertFromCollectionToJson(conf->mongodbConfig->db,collectionName,lpmap);
    return 0;
}

int LPushConn::selectMongoHistoryWork()
{
    static std::string prefix = "TASK_PULL_";
    std::string collectionName = prefix + lphandshakeMsg->appId;
    map<string,string> params;
    params.insert(make_pair("UserId",lphandshakeMsg->userId));
    params.insert(make_pair("AppKey",lphandshakeMsg->appId));
    int64_t num =  mongodb_client->count(conf->mongodbConfig->db,
							   collectionName,params);
    if(num<1000){
    vector<string> result = mongodb_client->queryToListJson(conf->mongodbConfig->db,
							   collectionName,params);
    if(result.size()>0)
    {
       vector<string>::iterator itr = result.begin();
        for(;itr!=result.end();++itr)
        {
	      string json = *itr;
	      map<string,string> entity = mongodb_client->jsonToMap(json);
	      LPushWorkerMessage lpwm(entity);
	      client->push(lpwm.copy());
	      mongodb_client->delFromCollectionToJson(conf->mongodbConfig->db,
						     collectionName,entity["_id"]);
	}
    }
      
    }else
    {
       while(num>0)
       {
	 
	vector<string> result = mongodb_client->queryToListJsonLimit(conf->mongodbConfig->db,
							   collectionName,params,1,1000);
	if(result.size()>0)
	{
	  vector<string>::iterator itr = result.begin();
	  for(;itr!=result.end();++itr)
	  {
	      string json = *itr;
	      map<string,string> entity = mongodb_client->jsonToMap(json);
	      LPushWorkerMessage lpwm(entity);
	      client->push(lpwm.copy());
	      mongodb_client->delFromCollectionToJson(conf->mongodbConfig->db,
						     collectionName,entity["_id"]);
	  }
	  num-=result.size();
	}
      }
    }
    
}



int LPushConn::hreatbeat(LPushChunk *message)
{
      int ret = ERROR_SUCCESS;
      before_data_time = st_utime();
      long long internal = before_data_time - hreat_data_time;
      if(internal < LP_HREAT_TIMEOUT_US_MIN || internal > LP_HREAT_TIMEOUT_US)
      {
	   ret = ERROR_CONN_HREATBEAT_TIMEOUT;
	   return ret;
      }	
      hreat_data_time  = before_data_time;
      lp_trace("recv hreatbeat %d",before_data_time);
      redis_client->zadd(redisApp->appKey,hreat_data_time/1000L,redisApp->key);
      redis_client->zadd(redisPlatform->platformKey,hreat_data_time/1000L,redisPlatform->key);
      redis_client->expire(clientKey,60);
      return ret;
}

int LPushConn::recvPushCallback(LPushChunk* message)
{
      int ret = ERROR_SUCCESS;
      int result = 0;
      std::string taskId,msgId;
      unsigned char * buf = message->data;
      if((result = LPushFMT::decodeString(buf,taskId)) <= 0)
      {
	 lp_warn("FMT String decode error %d",ret);
	 ret = -2;
	 return ret;
      }
      buf += result;
       if(LPushFMT::decodeString(buf,msgId) <= 0)
      {
	 lp_warn("FMT String decode error %d",ret);
	 ret = -2;
	 return ret;
      }
      if(msgId.empty()||taskId.empty())
      {
	 ret = ERROR_OBJECT_NOT_EXIST;
	 lp_warn("recv result list message match error");
	 return ret;
      }
      std::string  resultList = conf->resultList+ msgId; 
      redis_client->lPushForList(resultList,taskId);
      return ret;
}


int LPushConn::readMessage(LPushChunk **message)
{
    int ret = ERROR_SUCCESS;
    LPushChunk lp;
    if(!lpushProtocol)
    {
       ret = ERROR_OBJECT_NOT_EXIST;
        lp_warn("lpushProtocol not exist error");
       return ret;
    }
    if((ret = lpushProtocol->readMessage(skt,lp))!=ERROR_SUCCESS)
    {
      lp_warn("readMessage error");
      return ret;
    }
    *message = lp.copy();
    return ret;
}

void LPushConn::do_dispose()
{
    dispose = true;
    redis_client->del(clientKey);
    redis_client->zrem(redisApp->appKey,redisApp->key);
    redis_client->zrem(redisPlatform->platformKey,redisPlatform->key);
}

//recv thread use
int LPushConn::forwardServer(LPushChunk *message)
{
    int ret = ERROR_SUCCESS;
    int type = (int)message->header.dataType;
    switch(type)
    {
      case LPUSH_HEADER_TYPE_TEST:
	break;
      case LPUSH_HEADER_TYPE_REQUEST_SOURCE:
	break;
      case LPUSH_HEADER_TYPE_HREATBEAT:
	ret = hreatbeat(message);
	break;
      case LPUSH_HEADER_TYPE_CLOSE:
	do_dispose();
	break;
      case LPUSH_HEADER_TYPE_TEST_PUSH:
	break;
      case LPUSH_HEADER_TYPE_PUSH:
	ret = recvPushCallback(message);
	break;
      default:
	break;
    }
    return ret;
}
//consum thread use
int LPushConn::sendForward(LPushWorkerMessage* message)
{
    int ret = ERROR_SUCCESS;
    int type = 0;
    int time = getCurrentTime();
    std::string json = message->toJsonString();
    lp_trace("%d %s",json.size(),json.c_str());
    LPushHeader lh("LPUSH",time,LPUSH_CALLBACK_TYPE_PUSH,json.size()+5);
    LPushChunk lc;
    int jsonLen = json.size();
    char *buf = new char[json.size()+5];
    char *bufp = buf;
    memset(bufp,0,json.size()+5);
    bufp[0] = LPUSH_FMT_JSON;
    bufp[1] = jsonLen >> 24 & 0xFF;
    bufp[2] = jsonLen >> 16 & 0xFF;
    bufp[3] = jsonLen >> 8 & 0xFF;
    bufp[4] = jsonLen & 0xFF;
    memcpy(&bufp[5],json.c_str(),json.size());
    lc.setData(lh,(unsigned char*)buf);
    if(!lpushProtocol)
    {
      ret = ERROR_OBJECT_NOT_EXIST;
      return ret;
    }
    if((ret = lpushProtocol->sendPacket(&lc))!=ERROR_SUCCESS)
    {
       SafeDelete(buf);
       return ret;
    }
    SafeDelete(buf);
    return ret;
}

  
  
}