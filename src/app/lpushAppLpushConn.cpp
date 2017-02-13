#include <lpushAppLpushConn.h>
#include <lpushServer.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>

namespace lpush{

LPushConn::LPushConn(LPushServer* _server, st_netfd_t client_stfd): LPushConnection(_server, client_stfd)
{

}

LPushConn::~LPushConn()
{

}


int LPushConn::do_cycle()
{
    int ret = ERROR_SUCCESS;
    
    
    return ret;
}

  
}