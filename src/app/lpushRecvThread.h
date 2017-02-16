#pragma once

#include <lpushCommon.h>
#include <lpushThread.h>

namespace lpush 
{
  
class LPushChunk;
class ILPushRecvHandler
{
public:
  ILPushRecvHandler();
  virtual ~ILPushRecvHandler();
public:
  virtual bool can_loop()=0;
  
  virtual int handler(LPushChunk *lc)=0;
};
  
class LPushReusableThread;
class LPushConn;
class LPushClient;
class LPushRecvThread:public ILPushRecvHandler,public ILPushReusableThreadHandler
{
private:
  std::vector<LPushChunk*> queue;
  LPushReusableThread *trd;
  LPushConn  *stack;
  LPushClient		*client;
  int64_t 			timeout;
public:
  LPushRecvThread(LPushClient* cli,LPushConn *_stack,int64_t _timeout);
  virtual ~LPushRecvThread();
public:
  virtual bool can_loop();
  
  virtual int handler(LPushChunk *lc);
  
   virtual int cycle() ;
   
public:
    
    virtual int start();
   
    virtual void stop();
  
};
  


}