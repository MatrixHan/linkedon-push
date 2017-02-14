#include <lpushAppLpushConn.h>
#include <lpushServer.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushUtils.h>
#include <lpushProtocolStack.h>
namespace lpush{

LPushConn::LPushConn(LPushServer* _server, st_netfd_t client_stfd): LPushConnection(_server, client_stfd)
{
    skt = new LPushStSocket(client_stfd);
    
    lpushProtocol = new LPushProtocol(skt);
}

LPushConn::~LPushConn()
{

}


int LPushConn::do_cycle()
{
    int ret = ERROR_SUCCESS;
    
    
    return ret;
}

int LPushConn::handshake()
{

}

int LPushConn::controller_loop()
{
    
}

int LPushConn::hreatbeat()
{
      int ret = ERROR_SUCCESS;
      long long now = getCurrentTime();
      
}

  
  
}