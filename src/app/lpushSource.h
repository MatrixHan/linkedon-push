#pragma once

#include <lpushCommon.h>
#include <st.h>
namespace lpush 
{
  
class LPushWorkerMessage
{
private:
  int workerType;
  std::string workString;
  std::string workContent;
public:
  LPushWorkerMessage();
  LPushWorkerMessage(int type,std::string _workString,std::string _workContent);
  virtual ~LPushWorkerMessage();
public:
  LPushWorkerMessage* copy();
};
  

class LPushFastQueue
{
private:
   std::vector<LPushWorkerMessage*> queue;
   int befor_time;
public:
  LPushFastQueue();
  virtual ~LPushFastQueue();
public:
  virtual void setLength(int size);
  
  virtual int getLength();
  
  virtual int push(LPushWorkerMessage* msg);
  
  virtual int pop(LPushWorkerMessage** msg);
  
  virtual void clear();
  
};
  
  
class LPushHandshakeMessage;
class LPushSource;
class LPushClient
{
private:
  st_netfd_t cstfd;
  std::string appId;
  std::string screteKey;
  std::string userId;
  std::string clientFlag; 	      
private:
  LPushSource *source;
public:
  LPushClient(st_netfd_t _cstfd,LPushSource *lpsource,LPushHandshakeMessage *message);
  virtual ~LPushClient();
};
  

extern LPushSource *lSource;

class LPushSource
{
private:
  static std::map<st_netfd_t,LPushSource*> sources;
  LPushFastQueue *queue;
private:
  LPushSource();
  virtual ~LPushSource();
public:
  static LPushSource * create(st_netfd_t stfd);
  static LPushSource * instance(st_netfd_t stfd);
  
  static void destroy(st_netfd_t stfd);
  
  static int destroyAll();
  
public:
  virtual int push(LPushWorkerMessage* msg);
  
  virtual int pop(LPushWorkerMessage** msg);
};
  
}