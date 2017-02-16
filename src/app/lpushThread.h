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
  st_thread_t tid;
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
  LPushThread(const char * name, ILPushThreadHandler *handler, int64_t interval_us,bool joinable);
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
  

/**
   * 此处参考 
   * open project simple rtmp server
 * the reuse thread is a thread stop and start by other thread.
 *       user can create thread and stop then start again and again,
 *       generally must provides a start and stop method, @see LPushIngester.
 *       the step to create a thread stop by other thread:
 *       1. create LPushReusableThread field.
 *       2. must manually stop the thread when started it.
 *       for example:
 *           class LPushIngester : public ILPushReusableThreadHandler {
 *               public: LPushIngester() { pthread = new LPushReusableThread("ingest", this, SRS_AUTO_INGESTER_SLEEP_US); }
 *               public: virtual int start() { return pthread->start(); }
 *               public: virtual void stop() { pthread->stop(); }
 *               public: virtual int cycle() {
 *                   // check status, start ffmpeg when stopped.
 *               }
 *           };
 */
class ILPushReusableThreadHandler
{
public:
    ILPushReusableThreadHandler();
    virtual ~ILPushReusableThreadHandler();
public:
    /**
     * the cycle method for the one cycle thread.
     * @remark when the cycle has its inner loop, it must check whether
     * the thread is interrupted.
     */
    virtual int cycle() = 0;
public:
    /**
     * other callback for handler.
     * @remark all callback is optional, handler can ignore it.
     */
    virtual void on_thread_start();
    virtual int on_before_cycle();
    virtual int on_end_cycle();
    virtual void on_thread_stop();
};
class LPushReusableThread : public ILPushThreadHandler
{
private:
    LPushThread* pthread;
    ILPushReusableThreadHandler* handler;
public:
    LPushReusableThread(const char* n, ILPushReusableThreadHandler* h, int64_t interval_us = 0);
    virtual ~LPushReusableThread();
public:
    /**
     * for the reusable thread, start and stop by user.
     */
    virtual int start();
    /**
     * stop the thread, wait for the thread to terminate.
     * @remark user can stop multiple times, ignore if already stopped.
     */
    virtual void stop();
public:
    /**
     * get the context id. @see: ILPushThreadContext.get_id().
     * used for parent thread to get the id.
     * @remark when start thread, parent thread will block and wait for this id ready.
     */
    virtual int cid();
// interface internal::ILPushThreadHandler
    
    virtual void interrupt();
    /**
     * whether the thread is interrupted,
     * for the cycle has its loop, the inner loop should quit when thread
     * is interrupted.
     */
    virtual bool interrupted();
public:
    virtual int cycle();
    virtual void on_thread_start();
    virtual int on_before_cycle();
    virtual int on_end_cycle();
    virtual void on_thread_stop();
};


/**
 * the endless thread is a loop thread never quit.
 *      user can create thread always running util server terminate.
 *      the step to create a thread never stop:
 *      1. create LPushEndlessThread field.
 *      for example:
 *          class LPushStreamCache : public ILPushEndlessThreadHandler {
 *               public: LPushStreamCache() { pthread = new LPushEndlessThread("http-stream", this); }
 *               public: virtual int cycle() {
 *                   // do some work never end.
 *               }
 *          }
 * @remark user must use block method in cycle method, for example, sleep or socket io.
 */
class ILPushEndlessThreadHandler
{
public:
    ILPushEndlessThreadHandler();
    virtual ~ILPushEndlessThreadHandler();
public:
    /**
     * the cycle method for the common thread.
     * @remark user must use block method in cycle method, for example, sleep or socket io.
     */
    virtual int cycle() = 0;
public:
    /**
     * other callback for handler.
     * @remark all callback is optional, handler can ignore it.
     */
    virtual void on_thread_start();
    virtual int on_before_cycle();
    virtual int on_end_cycle();
    virtual void on_thread_stop();
};
class LPushEndlessThread : public ILPushThreadHandler
{
private:
    LPushThread* pthread;
    ILPushEndlessThreadHandler* handler;
public:
    LPushEndlessThread(const char* n, ILPushEndlessThreadHandler* h);
    virtual ~LPushEndlessThread();
public:
    /**
     * for the endless thread, never quit.
     */
    virtual int start();

public:
    virtual int cycle();
    virtual void on_thread_start();
    virtual int on_before_cycle();
    virtual int on_end_cycle();
    virtual void on_thread_stop();
};

/**
 * the one cycle thread is a thread do the cycle only one time,
 * that is, the thread will quit when return from the cycle.
 *       user can create thread which stop itself,
 *       generally only need to provides a start method,
 *       the object will destroy itself then terminate the thread, @see LPushConnection
 *       1. create LPushThread field
 *       2. the thread quit when return from cycle.
 *       for example:
 *           class LPushConnection : public ILPushOneCycleThreadHandler {
 *               public: LPushConnection() { pthread = new LPushOneCycleThread("conn", this); }
 *               public: virtual int start() { return pthread->start(); }
 *               public: virtual int cycle() {
 *                   // serve client.
 *                   // set loop to stop to quit, stop thread itself.
 *                   pthread->stop_loop();
 *               }
 *               public: virtual void on_thread_stop() {
 *                   // remove the connection in thread itself.
 *                   server->remove(this);
 *               }
 *           };
 */
}