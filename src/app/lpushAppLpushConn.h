#pragma once 

#include <lpushCommon.h>
#include <lpushConnection.h>
#include <lpushAppSt.h>
namespace lpush 
{
  class LPushServer;
  class LPushProtocol;
  class ILPushProtocolReaderWriter;
  class LPushConn : virtual public LPushConnection
  {
  private:
    long long before_data_time;
    LPushProtocol *lpushProtocol;
    LPushStSocket *skt;
  public:
      LPushConn(LPushServer* _server, st_netfd_t client_stfd);
      virtual ~LPushConn();
      
  public:
      virtual int do_cycle();
  public:
      virtual int handshake();
      
      virtual int hreatbeat();
      
      virtual int controller_loop();
  };
  
}