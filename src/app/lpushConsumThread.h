#pragma once 

#include <lpushCommon.h>
#include <lpushThread.h>
namespace lpush 
{


  
class LPushClient;
class LPushConsumThread :public ILPushReusableThreadHandler
{
private:
  LPushReusableThread *trd;
  LPushClient         *client;
  bool			can_run;
  int 			_timeout;
public:  
    LPushConsumThread(LPushClient*cli,int timeout);
    virtual ~LPushConsumThread();
public:
   virtual int cycle();
   
public:
  virtual int start();
  
  virtual void stop();
  
};
  
  
}