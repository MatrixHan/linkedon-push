#include <lpushAppLpushConn.h>
#include <lpushServer.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushUtils.h>
#include <lpushProtocolStack.h>
#include <lpushSource.h>
#include <lpushRecvThread.h>
#include <lpushConsumThread.h>
namespace lpush{
  
#define LP_PAUSED_SEND_TIMEOUT_US (int64_t)(30*60*1000*1000LL)
// if timeout, close the connection.
#define LP_PAUSED_RECV_TIMEOUT_US (int64_t)(30*60*1000*1000LL)
  

LPushConn::LPushConn(LPushServer* _server, st_netfd_t client_stfd): LPushConnection(_server, client_stfd)
{
    before_data_time = 0;
    dispose = false;
    skt = new LPushStSocket(client_stfd);
    client =NULL;
    lpushProtocol = new LPushProtocol(skt);
    
}

LPushConn::~LPushConn()
{
    SafeDelete(lpushProtocol);
    SafeDelete(skt);
    SafeDelete(client);
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
    if((ret = createConnection()) != ERROR_SUCCESS)
    {
      lp_error("conn createConnection error");
       return ret;
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
    
    LPushSource * source = LPushSource::create(stfd);
    
    client = LPushSource::create(stfd,source,lphandshakeMsg,this);
    
    LPushRecvThread trd(client,this,350);
    
    trd.start();
    
    LPushConsumThread trd2(client,350);
    
    trd2.start();
    
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

int LPushConn::sendForward(LPushWorkerMessage* message)
{
    int ret = ERROR_SUCCESS;
    int type = 0;
    //LPushChunk lp;
    //lpushProtocol->sendPacket();
    return ret;
}

  
  
}