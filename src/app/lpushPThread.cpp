#include <lpushPThread.h>
#include <lpushSystemErrorDef.h>
namespace lpush 
{
#define printf(p,...)
  
  
ILPushPThreadHandle::ILPushPThreadHandle()
{

}

ILPushPThreadHandle::~ILPushPThreadHandle()
{

}

LPushPThread::LPushPThread(ILPushPThreadHandle* handle, const char *name,int64_t internal_us_time)
{
    _name = name;
    _handle = handle;
    _internal_us_time = internal_us_time;
    can_run = false;
    loop = false;
    _pid = -1;
    disposed = true;
}

LPushPThread::~LPushPThread()
{
   stop();
}

int LPushPThread::start()
{
    int ret = ERROR_SUCCESS;
    // we set to loop to true for thread to run.
	loop = true;
    if((ret = pthread_create(&_pid,NULL,thread_funtion,this))!=ERROR_SUCCESS)
    {
		ret = ERROR_PTHREAD_CREATE;
		printf("pthread_create failed. ret=%d", ret);
		return ret;
    }
    
	disposed = false;
	
	
	// wait for cid to ready, for parent thread to get the cid.
	while (_pid < 0) {
		st_usleep(10 * 1000);
	}
	// now, cycle thread can run.
	can_run = true;
    
    return ret;
}

void LPushPThread::stop()
{
        
        loop = false;
        
        dispose();
        
        can_run = false;
   
}

bool LPushPThread::can_loop()
{
    return can_run;
}

void LPushPThread::stop_loop()
{
      loop = false;
}

void LPushPThread::dispose()
{
    if (disposed) {
            return;
        }
        
        //pthread_kill(_pid,SIGQUIT);
       
        int ret = pthread_join(_pid, NULL);
         
        
        disposed = true;
}


int LPushPThread::thread_cycle()
{
      int ret = ERROR_SUCCESS;
      
      assert(_handle);
      while (!can_run && loop) {
		usleep(20 * 1000);
	}
	//std::cout<<loop<<std::endl;
	while (loop) {
	
		printf("thread %d on before cycle success",pthread_self());
		
		if ((ret = _handle->cycle()) != ERROR_SUCCESS) {
			printf("thread %s cycle failed, ignored and retry, ret=%d", _name, ret);
			goto failed;
		}
		printf("thread %s cycle success", _name);
		

		printf("thread %s on end cycle success", _name);
		
	failed:
		if (!loop) {
			break;
		}
		
	}
	

	disposed = true;	
	
	printf("pthread %s cycle finished", _name);
	//std::cout<<loop<<std::endl;
	return ret;
}
void LPushPThread::regis_signal()
{
     struct sigaction    action;
     action.sa_flags = 0;
     action.sa_handler = sighandler;
     int err = sigaction(SIGQUIT, &action, NULL);
}

void* LPushPThread::thread_funtion(void* arg)
{
    LPushPThread *trd = (LPushPThread*)arg;
    assert(trd);
    trd->regis_signal();
    trd->thread_cycle();

    pthread_exit(NULL);
    return NULL;
}
void LPushPThread::sighandler(int signo)
{
    if(signo == SIGQUIT)
    {
       
    }
}


  
}