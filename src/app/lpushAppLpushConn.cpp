#include <lpushAppLpushConn.h>
#include <lpushServer.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushUtils.h>
#include <lpushProtocolStack.h>
#include <lpushSource.h>
#include <lpushRecvThread.h>
#include <lpushConsumThread.h>
#include <lpushJson.h>
#include <lpushRedis.h>
namespace lpush{
  
#define LP_PAUSED_SEND_TIMEOUT_US (int64_t)(30*60*1000*1000LL)
// if timeout, close the connection.
#define LP_PAUSED_RECV_TIMEOUT_US (int64_t)(30*60*1000*1000LL)
  

LPushConn::LPushConn(LPushServer* _server, st_netfd_t client_stfd): LPushConnection(_server, client_stfd)
{
    before_data_time = 0;
    dispose = false;
    skt = new LPushStSocket(client_stfd);
    skt->set_recv_timeout(LP_PAUSED_RECV_TIMEOUT_US);
    skt->set_send_timeout(LP_PAUSED_SEND_TIMEOUT_US);
    client =NULL;
    lpushProtocol = new LPushProtocol(skt);
    trd = NULL;
    trd2 = NULL;
    
}

LPushConn::~LPushConn()
{
    SafeDelete(lpushProtocol);
    SafeDelete(skt);
    SafeDelete(client);
    SafeDelete(trd);
    SafeDelete(trd2);
}


int LPushConn::do_cycle()
{
    int ret = ERROR_SUCCESS;
   LPushHandshakeMessage lpsm;
    if((ret = handshake(lpsm)) != ERROR_SUCCESS)
    {
	lp_error("conn handshake error");
       return ret;
    }
    lphandshakeMsg = &lpsm;
    if((ret = lpushProtocol->sendHandshake(lpsm))!=ERROR_SUCCESS)
    {
	lp_error("lpush send handshake error");
	return ret;
    }
    if((ret = createConnection()) != ERROR_SUCCESS)
    {
      lp_error("conn createConnection error");
       return ret;
    }
    trd =new LPushRecvThread (client,this,350);
    
    trd->start();
    
    trd2 = new LPushConsumThread(client,350);
    
    trd2->start();
    while(!dispose)
    {
	st_usleep(350*1000);
    }
    return ret;
}

int LPushConn::handshake(LPushHandshakeMessage &msk)
{
     int ret = ERROR_SUCCESS;
     LPushChunk lpc;
    if((ret = lpushProtocol->readMessage(skt,lpc))!=ERROR_SUCCESS)
    {
	lp_error("conn readMessage error");
	return ret;
    }
    
    return lpushProtocol->handshake(&lpc,msk);
}

int LPushConn::createConnection()
{
    int ret = ERROR_SUCCESS;
     LPushCreateMessage lpcm;
     LPushChunk lpc;
    if((ret = lpushProtocol->readMessage(skt,lpc))!=ERROR_SUCCESS)
    {
	lp_error("conn readMessage error");
	return ret;
    }
    if((ret = lpushProtocol->createConnection(&lpc,lpcm))!=ERROR_SUCCESS)
    {
	lp_error("conn createConnection error %d",ret);
	return ret;
    }
    
    char buf[6];
    clientKey = lphandshakeMsg->appId + lphandshakeMsg->userId;
    sprintf(buf,"%d",conf->port);
    std::string  hostname = conf->localhost+":"+std::string(buf);
    redis_client->set(clientKey,hostname);
    
    LPushSource * source = LPushSource::create(stfd);
    
    client = LPushSource::create(stfd,source,lphandshakeMsg,this);
    
    return ret;
}



int LPushConn::hreatbeat()
{
      int ret = ERROR_SUCCESS;
      long long now = getCurrentTime();
      
      return ret;
}


int LPushConn::readMessage(LPushChunk *message)
{
    int ret = ERROR_SUCCESS;
    LPushChunk lp;
    if((ret = lpushProtocol->readMessage(skt,lp))!=ERROR_SUCCESS)
    {
      lp_error("readMessage error");
      return ret;
    }
    message = lp.copy();
    return ret;
}

void LPushConn::do_dispose()
{
    dispose = true;
    trd->stop();
    trd2->stop();
    redis_client->del(clientKey);
}

//recv thread use
int LPushConn::forwardServer(LPushChunk *message)
{
    int ret = ERROR_SUCCESS;
    int type = (int)message->header.dataType;
    switch(type)
    {
      case LPUSH_HEADER_TYPE_TEST:
	break;
      case LPUSH_HEADER_TYPE_REQUEST_SOURCE:
	break;
      case LPUSH_HEADER_TYPE_HREATBEAT:
	break;
      case LPUSH_HEADER_TYPE_CLOSE:
	do_dispose();
	break;
      case LPUSH_HEADER_TYPE_TEST_PUSH:
	break;
      case LPUSH_HEADER_TYPE_PUSH:
	break;
      default:
	break;
    }
    return ret;
}
//consum thread use
int LPushConn::sendForward(LPushWorkerMessage* message)
{
    int ret = ERROR_SUCCESS;
    int type = 0;
    //LPushChunk lp;
    //lpushProtocol->sendPacket();
    return ret;
}

  
  
}