#pragma once

#include <lpushCommon.h>
#include <lpushIO.h>
namespace lpush 
{
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
 */  
class LPushHeader;
class LPushChunk;
class LPushFastBuffer;
class LPushProtocol
{
private:
    
  ILPushProtocolReaderWriter 		*lst;
  
  LPushFastBuffer  			*fast_buffer;
  
public:
  LPushProtocol(ILPushProtocolReaderWriter *skt);
  virtual ~LPushProtocol();
public:
  
};
  
typedef struct LPushHeader
{

  char * lpush_flag;
  
  long long timestamp;
  
  unsigned char dataType;
  
  int		datalenght;
  LPushHeader(char * flag,long long time,unsigned char type,int len):lpush_flag(flag),timestamp(time),
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
  LPushChunk(LPushHeader &hd,unsigned char * chunk):header(hd)
  {
    data = (unsigned char*)malloc(sizeof(unsigned char)*hd.datalenght);
    memcpy(data,chunk,hd.datalenght);
  }
  
  ~LPushChunk()
  {
    SafeDelete(data);
  }
};



}
