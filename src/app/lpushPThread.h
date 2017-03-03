#pragma once

#include <lpushCommon.h>
#include <pthread.h>

namespace lpush 
{
  
class ILPushPThreadHandle
{
public:
  ILPushPThreadHandle();
  virtual ~ILPushPThreadHandle();
public:
  virtual int cycle()=0;
};

class LPushPThread
{
private:
  ILPushPThreadHandle *_handle;
  pthread_t	      _pid;
  bool 			can_run;
  bool 			loop;
  bool 			disposed;
  const char *		_name;
private:
  int64_t 	     _internal_us_time;
public:
  LPushPThread(ILPushPThreadHandle *handle,const char *name,int64_t internal_us_time);
  virtual ~LPushPThread();
  
public:
  virtual int start();
  
  virtual void stop();
  
  virtual void dispose();
  
  virtual bool can_loop();
  
  virtual void stop_loop();
  
  virtual int thread_cycle();
  
  virtual void regis_signal();
  
  static void *thread_funtion(void *arg);
  
  static void sighandler(int signo);
};
  
}