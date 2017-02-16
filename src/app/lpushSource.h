#pragma once

#include <lpushCommon.h>
#include <st.h>
namespace lpush 
{
class LPushHandshakeMessage;
class LPushSource;
class LPushConn;

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
  
  virtual bool empty();
  
  virtual void clear();
  
};
  
  

class LPushClient
{
private:
  st_netfd_t cstfd;
  std::string appId;
  std::string screteKey;
  std::string userId;
  std::string clientFlag; 
private:
  bool     can_play;
private:
  LPushSource *source;
  LPushConn * conn;
public:
  LPushClient(st_netfd_t _cstfd,LPushSource *lpsource,LPushHandshakeMessage *message,LPushConn *_conn);
  virtual ~LPushClient();
  
public:
  virtual int playing();
  
  virtual bool can_loop();
};
  


class LPushSource
{
private:
  static std::map<st_netfd_t,LPushSource*> sources;
  static std::map<std::string,LPushClient*> clients;
  LPushFastQueue *queue;
private:
  LPushSource();
  virtual ~LPushSource();
public:
  static LPushSource * create(st_netfd_t stfd);
  static LPushSource * instance(st_netfd_t stfd);
  
  static LPushClient * create(st_netfd_t _cstfd,LPushSource *lpsource,LPushHandshakeMessage *message,LPushConn *_conn);
  static LPushClient * instance(std::string userId,std::string appId,std::string screteKey);
  
  static void destroy(st_netfd_t stfd);
  
  static int destroyAll();
  
public:
  virtual int push(LPushWorkerMessage* msg);
  
  virtual int pop(LPushWorkerMessage** msg);
  
  virtual bool empty();
};
  
}