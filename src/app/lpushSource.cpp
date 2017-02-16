#include <lpushSource.h>
#include <lpushProtocolStack.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
namespace lpush 
{
LPushWorkerMessage::LPushWorkerMessage()
{
      workerType = 0;
      
}
LPushWorkerMessage::LPushWorkerMessage(int type, std::__cxx11::string _workString, std::__cxx11::string _workContent):
workerType(type),workString(_workString),workContent(_workContent)
{

}

LPushWorkerMessage::~LPushWorkerMessage()
{

}

LPushWorkerMessage* LPushWorkerMessage::copy()
{
    LPushWorkerMessage *result = new LPushWorkerMessage(workerType,workString,workContent);
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
void LPushFastQueue::clear()
{

}

LPushClient::LPushClient(st_netfd_t _cstfd, LPushSource* lpsource, LPushHandshakeMessage* message)
{
    cstfd = _cstfd;
    source = lpsource;
    appId = message->appId;
    screteKey = message->screteKey;
    userId = message->userId;
    clientFlag = message->clientFlag;
}

LPushClient::~LPushClient()
{

}

std::map<st_netfd_t,LPushSource*> LPushSource::sources;

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
