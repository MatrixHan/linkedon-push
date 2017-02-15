#pragma once

#include <lpushCommon.h>
#include <lpushIO.h>
namespace lpush 
{
/**
 *client request server data packet type
 * 
 * 
*/
  
#define LPUSH_HEADER_TYPE_HANDSHAKE  			0x01
  
#define LPUSH_HEADER_TYPE_TEST				0x02
  
#define LPUSH_HEADER_TYPE_REQUEST_SOURCE		0x03
  
#define LPUSH_HEADER_TYPE_CREATE_CONNECTION		0x04
  
#define LPUSH_HEADER_TYPE_HREATBEAT			0x05
  
#define LPUSH_HEADER_TYPE_CLOSE				0x06
  
#define LPUSH_HEADER_TYPE_TEST_PUSH			0x07
  
#define LPUSH_HEADER_TYPE_PUSH				0x08
  
  
/**
 * server callback client data packet type
 * 
 */  
#define LPUSH_CALLBACK_TYPE_HANDSHAKE			0x01

#define LPUSH_CALLBACK_TYPE_TEST			0x02

#define LPUSH_CALLBACK_TYPE_SOURCE			0x03

#define LPUSH_CALLBACK_TYPE_CONNECTION_IS_OK		0x04

#define LPUSH_CALLBACK_TYPE_HREATBEAT			0x05

#define LPUSH_CALLBACK_TYPE_IS_CLOSE			0x06

#define LPUSH_CALLBACK_TYPE_TEST_PUSH			0x07

#define LPUSH_CALLBACK_TYPE_PUSH			0x08

/**
 *  OS clientFlag
 */  
#define LPUSH_CLIENT_FLAG_IOS				0x01

#define LPUSH_CLIENT_FLAG_ANDROID			0x02

#define LPUSH_CLIENT_FLAG_WIN_PHONE			0x03

#define LPUSH_CLIENT_FLAG_WIN_PC			0x04

#define LPUSH_CLIENT_FLAG_WEB				0x05
  
/**
 * lpush tcp header data chunk
 *
 *
 *  flag    timestamp   datatype  datalenght      data
 * ************************************************************
 *         |	     |		  |	   |		      *
 *   5byte |  4byte  |   1byte    |  4byte | datalenght(byte) *  
 *	   | 	     |		  | 	   |		      *
 * ************************************************************
 * lpush  5byte  (L P U S H)
 * timestamp 4byte 
 * dataType  1byte
 * datalenght 4byte
 * data	      (datalenght{byte})
 * header totle 14 bytes 
 */  
class LPushHeader;
class LPushChunk;
class LPushFastBuffer;
class LPushHandshakeMessage;
class LPushCreateMessage;
class LPushProtocol
{
private:
    
  ILPushProtocolReaderWriter 		*lst;
  
  LPushFastBuffer  			*fast_buffer;
  
public:
  LPushProtocol(ILPushProtocolReaderWriter *skt);
  virtual ~LPushProtocol();
public:
  virtual int readHeader(ILPushProtocolReaderWriter *skt,LPushHeader& lph);
  
  virtual int readMessage(ILPushProtocolReaderWriter *skt,LPushChunk& lpc);
  
  virtual int handshake(LPushChunk *message,LPushHandshakeMessage &msk);
  
  virtual int createConnection(LPushChunk *message,LPushCreateMessage &pcm);
  
  virtual int recvhreatbeat(LPushChunk *message);
public:
  virtual int sendHandshake(LPushChunk *message);
  
  virtual int sendCreateConnection(LPushChunk *message);
  
  virtual int sendHreatbeat(LPushChunk *message);
  
  virtual int sendPacket(LPushChunk *message);
  
};
  


typedef struct LPushHeader
{

  char * lpush_flag;
  
  int timestamp;
  
  unsigned char dataType;
  
  int		datalenght;
  LPushHeader()
  {
    lpush_flag = NULL;
    timestamp = 0L;
    dataType  = -1;
    datalenght = 0;
  }
  LPushHeader(char * flag,int time,unsigned char type,int len):lpush_flag(flag),timestamp(time),
	      dataType(type),datalenght(len)
	      {}
  LPushHeader(LPushHeader &header):lpush_flag(header.lpush_flag),timestamp(header.timestamp),
	      dataType(header.dataType),datalenght(header.datalenght)
  {
    
  }
  ~LPushHeader()
  {
    SafeDelete(lpush_flag);
  }
}LPushHeader;

struct LPushChunk
{
  LPushHeader header;
  unsigned char *data;
  LPushChunk(){}
  LPushChunk(LPushHeader &hd,unsigned char * chunk):header(hd)
  {
    data = (unsigned char*)malloc(sizeof(unsigned char)*hd.datalenght);
    memcpy(data,chunk,hd.datalenght);
  }
  ~LPushChunk()
  {
    SafeDelete(data);
  }
  
  LPushChunk* copy();
};

class LPushHandshakeMessage
{
private:
  std::string appId;
  std::string screteKey;
  std::string userId;
  std::string clientFlag;
  std::string md5Data;
public:
  LPushHandshakeMessage();
  LPushHandshakeMessage(std::map<std::string,std::string> parms);
  virtual ~LPushHandshakeMessage();
public:
  virtual bool check();
};

class LPushHreatbeat
{
private:
  std::string clientFlag;
public:
  LPushHreatbeat();
  LPushHreatbeat(const char flag);
  virtual ~LPushHreatbeat();
};

class LPushCreateMessage
{
private:
  std::string createString;
public:
  LPushCreateMessage();
  LPushCreateMessage(std::string cs);
  virtual ~LPushCreateMessage();
};

}
