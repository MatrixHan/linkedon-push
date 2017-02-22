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
  
#define LP_PAUSED_SEND_TIMEOUT_US (int64_t)(30*1000*1000LL)
// if timeout, close the connection.
#define LP_PAUSED_RECV_TIMEOUT_US (int64_t)(30*1000*1000LL)
  
#define LPT_PAUSED_SEND_TIMEOUT_US (int64_t)(10*1000*1000LL)
// if timeout, close the connection.
#define LPT_PAUSED_RECV_TIMEOUT_US (int64_t)(10*1000*1000LL)
  
#define LP_HREAT_TIMEOUT_US (int64_t)(60)
  

LPushConn::LPushConn(LPushServer* _server, st_netfd_t client_stfd): LPushConnection(_server, client_stfd)
{
    before_data_time = 0;
    dispose = false;
    skt = new LPushStSocket(client_stfd);
    mongodbClient = new LPushMongodbClient(conf->mongodbConfig->url.c_str());
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
    mongodbClient->close();
    SafeDelete(mongodbClient);
    if(stfd)
    LPushSource::destroy(stfd);
    if(client)
    LPushSource::destroy(client);
}


int LPushConn::do_cycle()
{
    int ret = ERROR_SUCCESS;
    mongodbClient->initMongodbClient();
    before_data_time = getCurrentTime();
    skt->set_recv_timeout(LP_PAUSED_RECV_TIMEOUT_US);
    skt->set_send_timeout(LP_PAUSED_SEND_TIMEOUT_US);
   LPushHandshakeMessage lpsm;
    if((ret = handshake(lpsm)) != ERROR_SUCCESS)
    {
	lp_error("conn handshake error");
       return ret;
    }
    lphandshakeMsg = &lpsm;
    if((ret = lpushProtocol->sendHandshake(lpsm))!=ERROR_SUCCESS)
    {
	lp_error("lpush send handshake error");
	return ret;
    }
    if((ret = createConnection()) != ERROR_SUCCESS)
    {
      lp_error("conn createConnection error");
       return ret;
    }
    trd =new LPushRecvThread (client,this,0);
    
    trd->start();
    
    trd2 = new LPushConsumThread(client,1000);
    
    trd2->start();
    while(!dispose)
    {
	long long now = getCurrentTime();
	if(now-before_data_time > LP_HREAT_TIMEOUT_US)
	{
	   ret = ERROR_CONN_HREATBEAT_TIMEOUT;
	   break;
	}
	st_usleep(350*1000);
    }
    return ret;
}

int LPushConn::handshake(LPushHandshakeMessage &msk)
{
     int ret = ERROR_SUCCESS;
     LPushChunk lpc;
    if((ret = lpushProtocol->readMessage(skt,lpc))!=ERROR_SUCCESS)
    {
	lp_error("conn readMessage error");
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
	lp_error("conn readMessage error");
	return ret;
    }
    if((ret = lpushProtocol->createConnection(&lpc,lpcm))!=ERROR_SUCCESS)
    {
	lp_error("conn createConnection error %d",ret);
	return ret;
    }
    
    char buf[6];
    clientKey = lphandshakeMsg->appId + lphandshakeMsg->userId;
    sprintf(buf,"%d",conf->port);
    hostname = conf->localhost+":"+std::string(buf);
    redis_client->set(clientKey,hostname);
    
    redisApp = new LPushAPPKey(lphandshakeMsg->appId,lphandshakeMsg->userId,
			       conf->localhost,conf->port);
    redisPlatform = new LPushPlatform(lphandshakeMsg->clientFlag,lphandshakeMsg->appId,
				      lphandshakeMsg->userId,conf->localhost,conf->port);
    userInsertMongodb(lphandshakeMsg);
    
    redis_client->hset(redisApp->appKey,redisApp->key,redisApp->value);
    redis_client->hset(redisPlatform->platformKey,redisPlatform->key,redisPlatform->value);
    LPushSource * source = LPushSource::create(stfd);
    
    client = LPushSource::create(stfd,source,lphandshakeMsg,this);
    if((ret=lpushProtocol->sendCreateConnection(lpcm))!=ERROR_SUCCESS)
    {
	lp_error("conn sendCreateConnection error %d",ret);
	return ret;
    }
    return ret;
}

int LPushConn::userInsertMongodb(LPushHandshakeMessage *msg)
{
    int ret = ERROR_SUCCESS;
    static std::string prefix = "member_";
    std::string collectionName = prefix + msg->appId;
    map<string,string>  lpmap = msg->toMongomap();
    int time = getCurrentTime();
    char buf[20];
    memset(buf,0,20);
    sprintf(buf,"%d",time);
    lpmap.insert(make_pair("time",string(buf)));
    map<string,string> params;
    params.insert(make_pair("userId","10000"));
    params.insert(make_pair("appKey","LOFFICIEL"));
    vector<string> result = mongodbClient->queryToListJson(conf->mongodbConfig->db,collectionName,params);
    if(result.size()==0)
    mongodbClient->insertFromCollectionToJson(conf->mongodbConfig->db,collectionName,lpmap);
    return 0;
}


int LPushConn::hreatbeat(LPushChunk *message)
{
      int ret = ERROR_SUCCESS;
      before_data_time = getCurrentTime();
      lp_trace("recv hreatbeat %d",before_data_time);
      return ret;
}

int LPushConn::recvPushCallback(LPushChunk* message)
{
      int ret = ERROR_SUCCESS;
      before_data_time = getCurrentTime();
      std::string taskId;
      if((ret = LPushFMT::decodeString(message->data,taskId)) != ERROR_SUCCESS)
      {
	 lp_error("FMT String decode error %d",ret);
	 return ret;
      }
      redis_client->hset(conf->resultMap,taskId,"1");
}


int LPushConn::readMessage(LPushChunk **message)
{
    int ret = ERROR_SUCCESS;
    LPushChunk lp;
    if((ret = lpushProtocol->readMessage(skt,lp))!=ERROR_SUCCESS)
    {
      lp_error("readMessage error");
      return ret;
    }
    *message = lp.copy();
    return ret;
}

void LPushConn::do_dispose()
{
    dispose = true;
    redis_client->del(clientKey);
    redis_client->hdel(redisApp->appKey,redisApp->key);
    redis_client->hdel(redisPlatform->platformKey,redisPlatform->key);
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
	hreatbeat(message);
	break;
      case LPUSH_HEADER_TYPE_CLOSE:
	do_dispose();
	break;
      case LPUSH_HEADER_TYPE_TEST_PUSH:
	break;
      case LPUSH_HEADER_TYPE_PUSH:
	recvPushCallback(message);
	break;
      default:
	break;
    }
    before_data_time = getCurrentTime();
    return ret;
}
//consum thread use
int LPushConn::sendForward(LPushWorkerMessage* message)
{
    int ret = ERROR_SUCCESS;
    int type = 0;
    int time = getCurrentTime();
    std::string json = message->toJsonString();
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
    if((ret = lpushProtocol->sendPacket(&lc))!=ERROR_SUCCESS)
    {
       SafeDelete(buf);
       return ret;
    }
    SafeDelete(buf);
    return ret;
}

  
  
}