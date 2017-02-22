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
  class LPushRecvThread;
  class LPushConsumThread;
  class LPushAPPKey;
  class LPushPlatform;
  class LPushMongodbClient;
  class LPushConn : virtual public LPushConnection
  {
  private:
    long long before_data_time;
    bool dispose;
    std::string  clientKey;
    std::string  hostname;
  private:
    LPushProtocol *lpushProtocol;
    LPushMongodbClient *mongodbClient;
    LPushStSocket *skt;
    LPushHandshakeMessage *lphandshakeMsg;
    LPushClient		  *client;
  private:
    LPushAPPKey		  *redisApp;
    LPushPlatform	  *redisPlatform;
  private:
    LPushRecvThread *trd;
    LPushConsumThread *trd2;
  public:
      LPushConn(LPushServer* _server, st_netfd_t client_stfd);
      virtual ~LPushConn();
      
  public:
      virtual int do_cycle();
  public:
      virtual int handshake(LPushHandshakeMessage &msk);
      
      virtual int createConnection();
      
      virtual int hreatbeat(LPushChunk *message);
      
      virtual int recvPushCallback(LPushChunk *message);
      
      virtual int userInsertMongodb(LPushHandshakeMessage *msg);
  public:
      virtual int readMessage(LPushChunk **message);
      
      virtual int forwardServer(LPushChunk *message);
      
  public:
      virtual int sendForward(LPushWorkerMessage *message);
      
      virtual void do_dispose();
  };
  
}