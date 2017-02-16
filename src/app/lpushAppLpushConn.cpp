#include <lpushAppLpushConn.h>
#include <lpushServer.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushUtils.h>
#include <lpushProtocolStack.h>
#include <lpushSource.h>
namespace lpush{
  
#define LP_PAUSED_SEND_TIMEOUT_US (int64_t)(30*60*1000*1000LL)
// if timeout, close the connection.
#define LP_PAUSED_RECV_TIMEOUT_US (int64_t)(30*60*1000*1000LL)
  

LPushConn::LPushConn(LPushServer* _server, st_netfd_t client_stfd): LPushConnection(_server, client_stfd)
{
    before_data_time = 0;
    dispose = false;
    skt = new LPushStSocket(client_stfd);
    
    lpushProtocol = new LPushProtocol(skt);
    
}

LPushConn::~LPushConn()
{
    SafeDelete(lpushProtocol);
    SafeDelete(skt);
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
    
    ret = controller_loop();
    
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
    
    return lpushProtocol->createConnection(&lpc,lpcm);
}


int LPushConn::controller_loop()
{
    int ret = ERROR_SUCCESS;
    while(!dispose)
    {
      ret = server_loop();
      
      if(ret == ERROR_SUCCESS)
      {
	 continue;
      }
      
      if(ret==ERROR_SOCKET_TIMEOUT)
      {
	skt->set_recv_timeout(LP_PAUSED_RECV_TIMEOUT_US);
	skt->set_send_timeout(LP_PAUSED_SEND_TIMEOUT_US);
	continue;
      }
      
      lp_error("controller loop back error");
      return ret;
    }
    return ret;
}

int LPushConn::hreatbeat()
{
      int ret = ERROR_SUCCESS;
      long long now = getCurrentTime();
      
}

int LPushConn::server_loop()
{
      
}

  
  
}