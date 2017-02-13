#pragma once 

#include <lpushCommon.h>
#include <lpushConnection.h>

namespace lpush 
{
  class LPushServer;
  
  class LPushConn : virtual public LPushConnection
  {
  private:
  public:
      LPushConn(LPushServer* _server, st_netfd_t client_stfd);
      virtual ~LPushConn();
      
  public:
      virtual int do_cycle();
  };
  
}