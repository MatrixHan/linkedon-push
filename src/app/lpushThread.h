#pragma once 

#include <lpushCommon.h>

namespace lpush 
{

class ILPushThreadHandler
{
    public:
        ILPushThreadHandler();
        virtual ~ILPushThreadHandler();
    public:
        virtual void on_thread_start();
        virtual int on_before_cycle();
        virtual int cycle() = 0;
        virtual int on_end_cycle();
        virtual void on_thread_stop();
};

class LPushThread
{
private:
  st_netfd_t tid;
  int _cid;
  bool loop;
  bool can_run;
  bool really_terminated;
  bool _joinable;
  const char* _name;
  bool disposed;
private:
  ILPushThreadHandler * _handler;
  int64_t cycle_interval_us;
public:
  /**
         * initialize the thread.
         * @param name, human readable name for st debug.
         * @param thread_handler, the cycle handler for the thread.
         * @param interval_us, the sleep interval when cycle finished.
         * @param joinable, if joinable, other thread must stop the thread.
         * @remark if joinable, thread never quit itself, or memory leak.
         * @remark about st debug, see st-1.9/README, _st_iterate_threads_flag
         */
        /**
         * TODO: FIXME: maybe all thread must be reap by others threads,
	 */
  LPushThread(const char * name,ILPushThreadHandler *handler,int64_t interval_us,bool joinable);
  virtual ~LPushThread();
public:
  
  virtual int cid();
  
  virtual int start();
  
  virtual void stop();
public:
  virtual bool can_loop();
  
  virtual void stop_loop();
  
private:
  virtual void dispose();
  virtual void thread_cycle();
  static void* thread_fun(void* arg);
};
  
}