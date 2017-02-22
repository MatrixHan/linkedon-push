#include <lpushRecvThread.h>

#include <lpushSystemErrorDef.h>
#include <lpushLogger.h>

#include <lpushSource.h>
#include <lpushProtocolStack.h>
#include <lpushAppLpushConn.h>
namespace lpush 
{

ILPushRecvHandler::ILPushRecvHandler()
{

}

ILPushRecvHandler::~ILPushRecvHandler()
{

}

  
LPushRecvThread::LPushRecvThread(LPushClient* cli, LPushConn* _stack, int64_t _timeout):
stack(_stack),client(cli)
{
    timeout = _timeout;
    trd = new LPushReusableThread("recv",this,_timeout);
}

LPushRecvThread::~LPushRecvThread()
{
    SafeDelete(trd);
}

bool LPushRecvThread::can_loop()
{
    return queue.empty();
}

int LPushRecvThread::handler(LPushChunk* lc)
{
     int ret = ERROR_SUCCESS;
     ret = stack->forwardServer(lc);
     return ret;
}

int LPushRecvThread::cycle()
{
    int ret = ERROR_SUCCESS;
    while(!trd->interrupted())
    {
        
      LPushChunk *lpc;
      ret = stack->readMessage(&lpc);
      
	if (ret == ERROR_SUCCESS) {
            ret = handler(lpc);
	    SafeDelete(lpc);
        }
       if (ret != ERROR_SUCCESS) {
           
    
            // we use no timeout to recv, should never got any error.
            trd->interrupt();
	    
	    stack->do_dispose();
            return ret;
        }
        lp_verbose("thread loop recv message. ret=%d", ret);
	st_usleep(timeout*1000);
    }
    return ret;
}

int LPushRecvThread::start()
{
  return trd->start();
}

void LPushRecvThread::stop()
{
    trd->stop();
}

  
  
}