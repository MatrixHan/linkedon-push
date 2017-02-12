#include <lpushProtocolStack.h>

#include <lpushLogger.h>
#include <lpushFastBuffer.h>
#include <lpushSystemErrorDef.h>

namespace lpush 
{
LPushProtocol::LPushProtocol(ILPushProtocolReaderWriter* skt)
{
    lst=skt;
    fast_buffer = new LPushFastBuffer();
}
LPushProtocol::~LPushProtocol()
{
    
}



int LPushProtocol::readHeader(ILPushProtocolReaderWriter* skt,LPushHeader& lph)
{
    //			       4C  50  55  53  48
    static char LPushFlag[5]={'L','P','U','S','H'};
    int ret = ERROR_SUCCESS;
    char* buf = fast_buffer->read_slice(5);
    if(strstr(buf,LPushFlag)==NULL)
    {
	return ERROR_SYSTEM_FILE_READ;
    }
    buf = fast_buffer->read_slice(4);
    long long timestamp = (*buf++)<<24 | (*buf++)<<16 | (*buf++)<<8 | (*buf++);
    unsigned char dataType = (unsigned char)fast_buffer->read_1byte();
    buf = fast_buffer->read_slice(4);
    int dataLen = (*buf++)<<24 | (*buf++)<<16 | (*buf++)<<8 | (*buf++);
    LPushHeader ph(LPushFlag,timestamp,dataType,dataLen);
    lph = ph;
    return ret;
}

int LPushProtocol::readMessage(ILPushProtocolReaderWriter* skt,LPushChunk& lpc)
{
      int ret = ERROR_SUCCESS;
      if((ret = fast_buffer->grow(skt,14)) != ERROR_SUCCESS)
      {
	  lp_error("readMessage socket read header is error");
	  return ret;
      }
      LPushHeader lph;
      if((ret = readHeader(skt,&lph)) != ERROR_SUCCESS)
      {
	  lp_error("readMessage header is error");
	  return ret;
      }
      
      if((ret = fast_buffer->grow(skt,lph.datalenght)) != ERROR_SUCCESS)
      {
	  lp_error("readMessage socket data body is error");
	  return ret;
      }
      
      char *buf = fast_buffer->read_slice(lph.datalenght);
      LPushChunk pc(&lph,(unsigned char*)buf);
      lpc = pc;
      return ret;
}



}