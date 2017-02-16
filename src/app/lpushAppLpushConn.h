#pragma once 

#include <lpushCommon.h>
#include <lpushConnection.h>
#include <lpushAppSt.h>
namespace lpush 
{
  class LPushServer;
  class LPushProtocol;
  class ILPushProtocolReaderWriter;
  struct LPushChunk;
  class LPushHandshakeMessage;
  class LPushClient;
  class LPushWorkerMessage;
  class LPushConn : virtual public LPushConnection
  {
  private:
    int before_data_time;
    bool dispose;
    
    LPushProtocol *lpushProtocol;
    LPushStSocket *skt;
    LPushHandshakeMessage *lphandshakeMsg;
    LPushClient		  *client;
  public:
      LPushConn(LPushServer* _server, st_netfd_t client_stfd);
      virtual ~LPushConn();
      
  public:
      virtual int do_cycle();
  public:
      virtual int handshake(LPushHandshakeMessage &msk);
      
      virtual int createConnection();
      
      virtual int hreatbeat();
      
  public:
      virtual int readMessage(LPushChunk *message);
      
      virtual int forwardServer(LPushChunk *message);
      
  public:
      virtual int sendForward(LPushWorkerMessage *message);
  };
  
}