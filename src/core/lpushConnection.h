#pragma once

#include <lpushCommon.h>

namespace lpush 
{
class LPushServer;
class LPushConnection
{
protected:
  LPushServer *server;
  st_netfd_t   stfd;
public:
  LPushConnection(LPushServer *_server,st_netfd_t client_stfd);
  virtual ~LPushConnection();
public:
	virtual int start();
protected:
	virtual int do_cycle() = 0;
private:
	virtual void cycle();
	static void* cycle_thread(void* arg);
};
  
}