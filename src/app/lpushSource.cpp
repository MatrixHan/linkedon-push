#include <lpushSource.h>
#include <lpushProtocolStack.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushAppLpushConn.h>
#include <lpushMath.h>
#include <lpushJson.h>
#include <lpushRedis.h>
#include <lpushUtils.h>
namespace lpush 
{
  
std::string LPushSystemStatus::statusToJson(int conns)
{
    std::string result;
    std::map<std::string,std::string> status;
    std::string localhost = conf->localhost;
    status.insert(std::make_pair("localhost",localhost));
    char buf[20];
    memset(buf,0,20);
    sprintf(buf,"%d",getpid());
    status.insert(std::make_pair("processId",std::string(buf)));
    memset(buf,0,20);
    sprintf(buf,"%d",conns);
    status.insert(std::make_pair("serverConnectionNO",std::string(buf)));
    int time = getCurrentTime();
    memset(buf,0,20);
    sprintf(buf,"%d",time);
    status.insert(std::make_pair("serverBeforeHreatTime",std::string(buf)));
    result = LPushConfig::mapToJsonStr(status);
    return result;
}
  
  
LPushWorkerMessage::LPushWorkerMessage()
{
    createTime = 0;
    expiresTime = 0;
}
LPushWorkerMessage::LPushWorkerMessage(std::string jsonStr)
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
    content = json["Content"].asString();
    userId = json["UserId"].asString();
    title = json["Title"].asString();
    ext = json["Ext"].asString();
    createTime = json["CreateTime"].asInt64();
    expiresTime = json["ExpiresTime"].asInt64();
    }
}

LPushWorkerMessage::LPushWorkerMessage(std::map< std::string, std::string > params)
{
    taskId = params["TaskId"];
    msgId = params["MsgId"];
    appKey = params["AppKey"];
    appSecret = params["AppSecret"];
    content = params["Content"];
    userId = params["UserId"];
    title = params["Title"];
    ext = params["Ext"];
    
    createTime = StringToLong(params["CreateTime"]);
    expiresTime = StringToLong(params["ExpiresTime"]);
}


LPushWorkerMessage::~LPushWorkerMessage()
{

}

std::string LPushWorkerMessage::toAllString()
{
    Json::Value pjson;
    if(!empty(userId))
    {
      pjson["UserId"] = userId;
    }
    if(!empty(appKey))
    {
      pjson["AppKey"] = appKey;
    }
    if(!empty(msgId))
    {
      pjson["MsgId"] = msgId;
    }
    if(!empty(appSecret))
    {
      pjson["AppSecret"] = appSecret;
    }
    if(!empty(taskId))
    {
      pjson["TaskId"] = taskId;
    }
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

std::string LPushWorkerMessage::toJsonString()
{
    Json::Value pjson;
    if(!empty(taskId))
    {
      pjson["TaskId"] = taskId;
    }
    if(!empty(msgId))
    {
      pjson["MsgId"] = msgId;
    }
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
    LPushWorkerMessage *result = new LPushWorkerMessage();
    result->appKey = appKey;
    result->appSecret = appSecret;
    result->content = content;
    result->createTime = createTime;
    result->expiresTime = expiresTime;
    result->ext = ext;
    result->msgId = msgId;
    result->taskId =taskId;
    result->userId = userId;
    result->title = title;
    return result;
}

LPushAPPKey::LPushAPPKey(std::string appId, std::string uid, std::string ip, int port)
{
      appKey = "";
      appKey.append("USER_APPKEY_ZSET_");
      appKey.append(appId);
      key = uid;
      char buf[20];
      memset(buf,0,20);
      sprintf(buf,"%d",port);
      value = ip+":"+std::string(buf);
}

LPushAPPKey::~LPushAPPKey()
{

}
/**
 *  OS clientFlag
  
#define LPUSH_CLIENT_FLAG_IOS				0x01

#define LPUSH_CLIENT_FLAG_ANDROID			0x02

#define LPUSH_CLIENT_FLAG_WIN_PHONE			0x03

#define LPUSH_CLIENT_FLAG_WIN_PC			0x04

#define LPUSH_CLIENT_FLAG_WEB				0x05
*/ 
LPushPlatform::LPushPlatform(std::string platform, std::string appId, std::string uid, std::string ip, int port)
{
      platformKey = "";
      platformKey.append("USER_");
      if(platform.find("1")!=std::string::npos)
      {
	 platformKey.append("IOS_");
      }else if(platform.find("2")!=std::string::npos)
      {
	platformKey.append("ANDROID_");
      }else if(platform.find("3")!=std::string::npos)
      {
	platformKey.append("WIN_PHONE_");
      }else if(platform.find("4")!=std::string::npos)
      {
	platformKey.append("WIN_PC_");
      }else if(platform.find("5")!=std::string::npos)
      {
	platformKey.append("WEB_");
      }
      platformKey.append("ZSET_");
      platformKey.append(appId);
      key = uid;
      char buf[20];
      memset(buf,0,20);
      sprintf(buf,"%d",port);
      value = ip+":"+std::string(buf);
}

LPushPlatform::~LPushPlatform()
{

}



LPushFastQueue::LPushFastQueue()
{
    befor_time=0;
    
}

LPushFastQueue::~LPushFastQueue()
{
     if(getLength()>0)
     {
       std::vector<LPushWorkerMessage*>::iterator itr = queue.begin();
       for(;itr!=queue.end();)
       {
	  LPushWorkerMessage *lpwm;
	  lpwm = *itr;
	  itr=queue.erase(itr);
	  SafeDelete(lpwm);
      }
     }
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
    LPushWorkerMessage *lpwm =msg;
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

LPushClient::LPushClient(st_netfd_t _cstfd, LPushHandshakeMessage* message,LPushConn *_conn)
{
    cstfd = _cstfd;
    source = NULL;
    LPushSource::create(_cstfd,&source);
    conn = _conn;
    appId = message->appId;
    screteKey = message->screteKey;
    userId = message->userId;
    clientFlag = message->clientFlag;
    can_play  = true;
}

LPushClient::~LPushClient()
{
    LPushSource::destroy(cstfd);
}

int LPushClient::playing()
{
    int ret = ERROR_SUCCESS;
	LPushWorkerMessage *work=NULL;
	if(!source)
	{
	  lp_error("source not found!");
	  return ERROR_OBJECT_NOT_EXIST;
	}
	if((ret = source->pop(&work))!=ERROR_SUCCESS)
	{
	   lp_error("source pop error");
	   return ret;
	}
	if(!conn)
	{
	  lp_error("conn not found!");
	  SafeDelete(work);
	  return ERROR_OBJECT_NOT_EXIST;
	}
	if((ret = conn->sendForward(work))!=ERROR_SUCCESS)
	{	
	    conn->do_dispose();
	    SafeDelete(work);
	    lp_error("source send conn forward error!");
	    return ret;
	}
	SafeDelete(work);
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

int LPushSource::create(st_netfd_t stfd,LPushSource **source1)
{
      LPushSource * lps = new LPushSource();
      sources.insert(std::make_pair(stfd,lps));
      *source1 = lps;
      return 0;
}

LPushSource* LPushSource::instance(st_netfd_t stfd)
{
      LPushSource* res=NULL;
      std::map<st_netfd_t,LPushSource*>::iterator itr = sources.find(stfd);
      if(itr!=sources.end())
      {
	res = itr->second;
      }
      
      return res;
}

LPushClient* LPushSource::create(st_netfd_t _cstfd,LPushHandshakeMessage *message,LPushConn *_conn)
{
    LPushClient *lps = new LPushClient(_cstfd,message,_conn);
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
    int ret = ERROR_SUCCESS;
    int index = 0;
    std::vector<std::string> works=redis_client->list(queueName,0,1000);
    std::vector<std::string>::iterator itr = works.begin();
    for(;itr!=works.end();++itr)
    {
	std::string str = *itr;
	LPushWorkerMessage lwm(str);
	LPushClient *client = LPushSource::instance(lwm.userId,lwm.appKey,lwm.appSecret);
	index++;
	if(!client)
	{
	   
// 	   redis_client->lpop(queueName);
	   continue;
	}
	client->push(lwm.copy());
// 	redis_client->lpop(queueName);
	
    }
    redis_client->ltrim(queueName,index,-1);
    return ret;
}

void LPushSource::destroy(std::string key)
{
    
    std::map<std::string,LPushClient*>::iterator itr = clients.find(key);
    if(itr!=clients.end())
      {
	itr = clients.erase(itr);
      }
    
}

void LPushSource::destroyClientAll()
{
     std::map<std::string,LPushClient*>::iterator itr= clients.begin();
    for(;itr!=clients.end();)
    {
	LPushClient *lps = itr->second;
	clients.erase(itr++);
	SafeDelete(lps);
    }
    clients.clear();
}

void LPushSource::destroy(st_netfd_t stfd)
{
    std::map<st_netfd_t,LPushSource*>::iterator itr = sources.find(stfd);
    if(itr!=sources.end())
      {
	LPushSource *lps = itr->second;
	itr = sources.erase(itr);
	SafeDelete(lps);
      }
    
}

int LPushSource::destroyAll()
{
    std::map<st_netfd_t,LPushSource*>::iterator itr= sources.begin();
    for(;itr!=sources.end();)
    {
	LPushSource *lps = itr->second;
	sources.erase(itr++);
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
