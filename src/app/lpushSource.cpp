#include <lpushSource.h>
#include <lpushProtocolStack.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushAppLpushConn.h>
#include <lpushMath.h>
#include <lpushJson.h>
#include <lpushRedis.h>
namespace lpush 
{
  
std::string LPushSystemStatus::statusToJson(int conns)
{
    std::string result;
    std::map<std::string,std::string> status;
    std::string localhost = conf->localhost;
    status.insert(std::make_pair("localhost",localhost));
    char buf[6];
    memset(buf,0,6);
    sprintf(buf,"%d",getpid());
    status.insert(std::make_pair("processId",std::string(buf)));
    memset(buf,0,6);
    sprintf(buf,"%d",conns);
    status.insert(std::make_pair("serverConnectionNO",std::string(buf)));
    
    result = LPushConfig::mapToJsonStr(status);
    return result;
}
  
  
LPushWorkerMessage::LPushWorkerMessage()
{
    createTime = 0;
    expiresTime = 0;
}
LPushWorkerMessage::LPushWorkerMessage(std::__cxx11::string jsonStr)
{
    Json::Value json;
    Json::Reader reader;
    if(!reader.parse(jsonStr,json,false))
    {
	createTime = 0;
	expiresTime = 0;
    }else{
    taskId = json["TaskId"].asString();
    msgId = json["MsgId"].asString();
    appKey = json["AppKey"].asString();
    appSecret = json["AppSecret"].asString();
    userId = json["UserId"].asString();
    ext = json["Ext"].asString();
    createTime = json["CreateTime"].asInt();
    expiresTime = json["ExpiresTime"].asInt();
    }
}


LPushWorkerMessage::~LPushWorkerMessage()
{

}


std::__cxx11::string LPushWorkerMessage::toJsonString()
{
    Json::Value pjson;
    if(!empty(title))
    {
       pjson["Title"] = title;
    }
    if(!empty(content))
    {
      pjson["Content"] = content;
    }
    if(!empty(ext))
    {
      pjson["Ext"] = ext;
    }
    if(!empty(createTime))
    {
      pjson["CreateTime"] = createTime;
    }
    if(!empty(expiresTime))
    {
      pjson["ExpiresTime"] = expiresTime;
    }
    return pjson.toStyledString();
}


LPushWorkerMessage* LPushWorkerMessage::copy()
{
    LPushWorkerMessage *result = this;
    return result;
}


LPushFastQueue::LPushFastQueue()
{
    befor_time=0;
    
}

LPushFastQueue::~LPushFastQueue()
{

}

void LPushFastQueue::setLength(int size)
{
    queue.resize(size);
}

int LPushFastQueue::getLength()
{
    return queue.size();
}

int LPushFastQueue::push(LPushWorkerMessage* msg)
{
    LPushWorkerMessage *lpwm =msg->copy();
    queue.push_back(lpwm);
    return 0;
}

int LPushFastQueue::pop(LPushWorkerMessage** msg)
{
    LPushWorkerMessage *lpwm;
    if(!(getLength()>0)){
	lp_warn("queue size is 0");
    }
    std::vector<LPushWorkerMessage*>::iterator itr = queue.begin();
    lpwm = *itr;
    *msg = lpwm->copy();
    itr=queue.erase(itr);
    SafeDelete(lpwm);
    return 0;
}

bool LPushFastQueue::empty()
{
    return queue.empty();
}


void LPushFastQueue::clear()
{
    queue.clear();
}

LPushClient::LPushClient(st_netfd_t _cstfd, LPushSource* lpsource, LPushHandshakeMessage* message,LPushConn *_conn)
{
    cstfd = _cstfd;
    source = lpsource;
    conn = _conn;
    appId = message->appId;
    screteKey = message->screteKey;
    userId = message->userId;
    clientFlag = message->clientFlag;
    can_play  = true;
}

LPushClient::~LPushClient()
{

}

int LPushClient::playing()
{
    int ret = ERROR_SUCCESS;
	LPushWorkerMessage *work;
	if((ret = source->pop(&work))!=ERROR_SUCCESS)
	{
	   lp_error("source pop error");
	   return ret;
	}
	if((ret = conn->sendForward(work))!=ERROR_SUCCESS)
	{
	    lp_error("source send conn forward error!");
	    return ret;
	}
    return ret;
}

bool LPushClient::can_loop()
{
    return !source->empty();
}

int LPushClient::pop(LPushWorkerMessage** msg)
{
    return source->pop(msg);
}

int LPushClient::push(LPushWorkerMessage* msg)
{
    return source->push(msg);
}



std::map<st_netfd_t,LPushSource*> LPushSource::sources;
std::map<std::string,LPushClient*> LPushSource::clients;
LPushSource::LPushSource()
{
    queue = new LPushFastQueue();
}

LPushSource::~LPushSource()
{
   SafeDelete(queue);
}

LPushSource* LPushSource::create(st_netfd_t stfd)
{
      LPushSource * lps = new LPushSource();
      sources.insert(std::make_pair(stfd,lps));
      return lps;
}

LPushSource* LPushSource::instance(st_netfd_t stfd)
{
      std::map<st_netfd_t,LPushSource*>::iterator itr = sources.find(stfd);
      if(itr!=sources.end())
      {
	return (LPushSource*)itr->second;
      }
      
      return NULL;
}

LPushClient* LPushSource::create(st_netfd_t _cstfd,LPushSource *lpsource,LPushHandshakeMessage *message,LPushConn *_conn)
{
    LPushClient *lps = new LPushClient(_cstfd,lpsource,message,_conn);
    std::string key = message->userId + message->appId +message->screteKey;
    clients.insert(std::make_pair(key,lps));
    return lps;
}

LPushClient* LPushSource::instance(std::string userId, std::string appId, std::string screteKey)
{
  std::string key = userId + appId +screteKey;
   std::map<std::string,LPushClient*>::iterator itr = clients.find(key);
   if(itr!=clients.end())
   {
     return (LPushClient*)itr->second;
  }
  return NULL;
}

bool LPushSource::empty()
{
    return queue->empty();
}

int LPushSource::cycle_all(std::string queueName)
{
    std::vector<std::string> works=redis_client->list(queueName,0,100);
    std::vector<std::string>::iterator itr = works.begin();
    for(;itr!=works.end();++itr)
    {
	std::string str = *itr;
	LPushWorkerMessage lwm(str);
	LPushClient *client = LPushSource::instance(lwm.userId,lwm.appKey,lwm.appSecret);
	if(client)
	{
	   client->push(&lwm);
	}
    }
    
}


void LPushSource::destroy(st_netfd_t stfd)
{
    std::map<st_netfd_t,LPushSource*>::iterator itr = sources.find(stfd);
    if(itr!=sources.end())
      {
	LPushSource *lps = itr->second;
	sources.erase(itr);
	SafeDelete(lps);
      }
    
}

int LPushSource::destroyAll()
{
    std::map<st_netfd_t,LPushSource*>::iterator itr= sources.begin();
    for(;itr!=sources.end();++itr)
    {
	LPushSource *lps = itr->second;
	sources.erase(itr);
	SafeDelete(lps);
    }
    sources.clear();
    return 0;
}


int LPushSource::push(LPushWorkerMessage* msg)
{
    return queue->push(msg);
}

int LPushSource::pop(LPushWorkerMessage** msg)
{
    return queue->pop(msg);
}

  
}
