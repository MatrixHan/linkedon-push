#include <lpushConsumThread.h>

#include <lpushSystemErrorDef.h>
#include <lpushLogger.h>
#include <lpushSource.h>

namespace lpush 
{
LPushConsumThread::LPushConsumThread(LPushClient* cli, int timeout):
client(cli)
{
    trd = new LPushReusableThread("Consum",this,timeout);
}

LPushConsumThread::~LPushConsumThread()
{
    SafeDelete(trd);
}

int LPushConsumThread::cycle()
{
      int ret = ERROR_SUCCESS;
      while(!trd->interrupted()){
	if(!client->can_loop())
	{
	    st_usleep(350*1000);
	    continue;
	}
      if((ret = client->playing())!=ERROR_SUCCESS)
      {
	  lp_error("client playing work error");
	  return ret;
      }	
      }
      return ret;
}

int LPushConsumThread::start()
{
    return trd->start();
}

void LPushConsumThread::stop()
{
    trd->interrupt();
}

  
}