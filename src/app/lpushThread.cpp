#include <lpushThread.h>
#include <lpushSystemErrorDef.h>
#include <lpushLogger.h>


namespace lpush 
{
ILPushThreadHandler::ILPushThreadHandler()
{

}

ILPushThreadHandler::~ILPushThreadHandler()
{

}

int ILPushThreadHandler::on_before_cycle()
{
    return ERROR_SUCCESS;
}

int ILPushThreadHandler::on_end_cycle()
{
    return ERROR_SUCCESS;
}

void ILPushThreadHandler::on_thread_start()
{

}

void ILPushThreadHandler::on_thread_stop()
{

}


LPushThread::LPushThread(const char* name, ILPushThreadHandler* handler, int64_t interval_us, bool joinable)
{
        _name = name;
        _handler = handler;
        cycle_interval_us = interval_us;
        _joinable = joinable;
        tid = NULL;
        loop = false;
        really_terminated = true;
        _cid = -1;
        disposed = false;
        // in start(), the thread cycle method maybe stop and remove the thread itself,
        // and the thread start() is waiting for the _cid, and segment fault then.
        // @see https://github.com/ossrs/srs/issues/110
        // thread will set _cid, callback on_thread_start(), then wait for the can_run signal.
        can_run = false;
}

LPushThread::~LPushThread()
{
     stop();
}

bool LPushThread::can_loop()
{
      return can_run;
}

int LPushThread::cid()
{
    return _cid;
}

void LPushThread::stop_loop()
{

}

void LPushThread::stop()
{
    if (!tid) {
            return;
        }
        
        loop = false;
        
        dispose();
        
        _cid = -1;
        can_run = false;
        tid = NULL;   
}




int LPushThread::start()
{
	int ret = ERROR_SUCCESS;
        
	if (tid) {
		lp_info("thread %s already running.", _name);
		return ret;
	}
	
	if ((tid = st_thread_create(thread_fun, this, (_joinable? 1:0), 0)) == NULL){
		ret = ERROR_ST_CREATE_CYCLE_THREAD;
		lp_error("st_thread_create failed. ret=%d", ret);
		return ret;
	}
	
	disposed = false;
	// we set to loop to true for thread to run.
	loop = true;
	
	// wait for cid to ready, for parent thread to get the cid.
	while (_cid < 0) {
		st_usleep(10 * 1000);
	}
	
	// now, cycle thread can run.
	can_run = true;
	
	return ret;
}

void LPushThread::dispose()
{
    if (disposed) {
            return;
        }
        
        // the interrupt will cause the socket to read/write error,
        // which will terminate the cycle thread.
        st_thread_interrupt(tid);
        
        // when joinable, wait util quit.
        if (_joinable) {
            // wait the thread to exit.
            int ret = st_thread_join(tid, NULL);
            if (ret) {
                lp_warn("core: ignore join thread failed.");
            }
        }
        
        // wait the thread actually terminated.
        // sometimes the thread join return -1, for example,
        // when thread use st_recvfrom, the thread join return -1.
        // so here, we use a variable to ensure the thread stopped.
        while (!really_terminated) {
            st_usleep(10 * 1000);
            
            if (really_terminated) {
                break;
            }
            lp_warn("core: wait thread to actually terminated");
        }
        
        disposed = true;
}

void LPushThread::thread_cycle()
{
	int ret = ERROR_SUCCESS;
        
	log_context->generate_id();
	lp_info("thread %s cycle start", _name);
	
	_cid = log_context->get_id();
	
	assert(_handler);
	_handler->on_thread_start();
	
	// thread is running now.
	really_terminated = false;
	
	// wait for cid to ready, for parent thread to get the cid.
	while (!can_run && loop) {
		st_usleep(10 * 1000);
	}
	
	while (loop) {
		if ((ret = _handler->on_before_cycle()) != ERROR_SUCCESS) {
			lp_warn("thread %s on before cycle failed, ignored and retry, ret=%d", _name, ret);
			goto failed;
		}
		lp_info("thread %s on before cycle success",st_thread_self());
		
		if ((ret = _handler->cycle()) != ERROR_SUCCESS) {
			lp_warn("thread %s cycle failed, ignored and retry, ret=%d", _name, ret);
			goto failed;
		}
		lp_info("thread %s cycle success", _name);
		
		if ((ret = _handler->on_end_cycle()) != ERROR_SUCCESS) {
			lp_warn("thread %s on end cycle failed, ignored and retry, ret=%d", _name, ret);
			goto failed;
		}
		lp_info("thread %s on end cycle success", _name);
		
	failed:
		if (!loop) {
			break;
		}
		
		// to improve performance, donot sleep when interval is zero.
		// @see: https://github.com/ossrs/srs/issues/237
		if (cycle_interval_us != 0) {
			st_usleep(cycle_interval_us);
		}
	}
	
	// readly terminated now.
	really_terminated = true;
	
	// when thread terminated normally, also disposed.
	// we must set to disposed before the on_thread_stop, which may free the thread.
	// @see https://github.com/ossrs/srs/issues/546
	disposed = true;
	
	_handler->on_thread_stop();
	lp_info("thread %s cycle finished", _name);
}

void* LPushThread::thread_fun(void* arg)
{
    LPushThread *thread = (LPushThread*)(arg);
    assert(thread);
    thread->thread_cycle();
    st_thread_exit(NULL);
        
    return NULL;
}
  
  
  
//**********
ILPushReusableThreadHandler::ILPushReusableThreadHandler()
{
}

ILPushReusableThreadHandler::~ILPushReusableThreadHandler()
{
}

void ILPushReusableThreadHandler::on_thread_start()
{
}

int ILPushReusableThreadHandler::on_before_cycle()
{
    return ERROR_SUCCESS;
}

int ILPushReusableThreadHandler::on_end_cycle()
{
    return ERROR_SUCCESS;
}

void ILPushReusableThreadHandler::on_thread_stop()
{
}

LPushReusableThread::LPushReusableThread(const char* n, ILPushReusableThreadHandler* h, int64_t interval_us)
{
    handler = h;
    pthread = new LPushThread(n, this, interval_us, true);
}

LPushReusableThread::~LPushReusableThread()
{
    pthread->stop();
    SafeDelete(pthread);
}

int LPushReusableThread::start()
{
    return pthread->start();
}

void LPushReusableThread::stop()
{
    pthread->stop();
}

int LPushReusableThread::cid()
{
    return pthread->cid();
}

int LPushReusableThread::cycle()
{
    return handler->cycle();
}

void LPushReusableThread::on_thread_start()
{
    handler->on_thread_start();
}

int LPushReusableThread::on_before_cycle()
{
    return handler->on_before_cycle();
}

int LPushReusableThread::on_end_cycle()
{
    return handler->on_end_cycle();
}

void LPushReusableThread::on_thread_stop()
{
    handler->on_thread_stop();
}
  
ILPushEndlessThreadHandler::ILPushEndlessThreadHandler()
{

}

ILPushEndlessThreadHandler::~ILPushEndlessThreadHandler()
{

}

int ILPushEndlessThreadHandler::on_before_cycle()
{
    return ERROR_SUCCESS;
}

int ILPushEndlessThreadHandler::on_end_cycle()
{
    return ERROR_SUCCESS;
}

void ILPushEndlessThreadHandler::on_thread_start()
{

}

void ILPushEndlessThreadHandler::on_thread_stop()
{

}

  
  
LPushEndlessThread::LPushEndlessThread(const char* n, ILPushEndlessThreadHandler* h)
{
      handler = h;
      pthread = new LPushThread(n, this, 0, false);
}

LPushEndlessThread::~LPushEndlessThread()
{
     pthread->stop();
      SafeDelete(pthread);
}

int LPushEndlessThread::on_before_cycle()
{
  return handler->on_before_cycle();
}

int LPushEndlessThread::cycle()
{
  return handler->cycle();
}


int LPushEndlessThread::on_end_cycle()
{
return handler->on_end_cycle();
}
void LPushEndlessThread::on_thread_start()
{
      handler->on_thread_start();
}

void LPushEndlessThread::on_thread_stop()
{
      handler->on_thread_stop();
}

int LPushEndlessThread::start()
{
      pthread->start();
}

  
}