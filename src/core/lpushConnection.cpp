#include <lpushConnection.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushServer.h>
namespace lpush 
{
LPushConnection::LPushConnection(LPushServer* _server, st_netfd_t client_stfd)
{
    server = _server;
    stfd   = client_stfd;
}

LPushConnection::~LPushConnection()
{
    if(stfd)
    {
      st_netfd_close(stfd);
      stfd = NULL;
    }
}

void LPushConnection::cycle()
{
      int ret = ERROR_SUCCESS;
      log_context->generate_id();
      
      ret = do_cycle();
	
	// if socket io error, set to closed.
	if (ret == ERROR_SOCKET_READ || ret == ERROR_SOCKET_READ_FULLY || ret == ERROR_SOCKET_WRITE) {
		ret = ERROR_SOCKET_CLOSED;
	}
    
	// success.
	if (ret == ERROR_SUCCESS) {
		lp_trace("client process normally finished. ret=%d", ret);
	}
	
	// client close peer.
	if (ret == ERROR_SOCKET_CLOSED) {
		lp_trace("client disconnect peer. ret=%d", ret);
	}
	
	server->remove(this);
}

void* LPushConnection::cycle_thread(void* arg)
{
	LPushConnection *conn = (LPushConnection*)(arg);
	assert(conn);
	conn->start();
	return NULL;
}

int LPushConnection::start()
{
      int ret = ERROR_SUCCESS;
    
    if (st_thread_create(cycle_thread, this, 0, 0) == NULL) {
        ret = ERROR_ST_CREATE_CYCLE_THREAD;
        lp_error("st_thread_create conn cycle thread error. ret=%d", ret);
        return ret;
    }
    lp_verbose("create st conn cycle thread success.");
	
	return ret;
}

  
}