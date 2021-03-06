#include <lpushConsumThread.h>

#include <lpushSystemErrorDef.h>
#include <lpushLogger.h>
#include <lpushSource.h>

namespace lpush 
{
LPushConsumThread::LPushConsumThread(LPushClient* cli, int timeout):
client(cli)
{
    _timeout = timeout;
    trd = new LPushReusableThread("Consum",this,timeout);
    can_run = false;
}

LPushConsumThread::~LPushConsumThread()
{
    stop();
    SafeDelete(trd);
}

int LPushConsumThread::cycle()
{
      int ret = ERROR_SUCCESS;
      while(can_run){
	if(!client)
	{
	   ret = ERROR_OBJECT_NOT_EXIST;
	   lp_warn("client not exist error %d",ret);
	   break;
	}
	if(!client->can_loop())
	{
	    st_usleep(350*1000);
	    continue;
	}
      if((ret = client->playing())!=ERROR_SUCCESS)
      {
	  lp_error("client playing work error");
	  trd->interrupt();
	  return ret;
      }	
      }
      return ret;
}

int LPushConsumThread::start()
{
    can_run = true;
    return trd->start();
}

void LPushConsumThread::stop()
{
    can_run = false;
}

  
}