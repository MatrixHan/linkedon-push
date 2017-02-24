#include <lpushProtocolStack.h>

#include <lpushLogger.h>
#include <lpushFastBuffer.h>
#include <lpushSystemErrorDef.h>

#include <lpushFmt.h>
#include <lpushMath.h>

namespace lpush 
{
  
LPushChunk* LPushChunk::copy()
{
    LPushChunk * lpc = new LPushChunk(header,data);
    return lpc;
}

  
  
LPushProtocol::LPushProtocol(ILPushProtocolReaderWriter* skt)
{
    lst=skt;
    fast_buffer = new LPushFastBuffer();
}
LPushProtocol::~LPushProtocol()
{
    SafeDelete(fast_buffer);
}



int LPushProtocol::readHeader(ILPushProtocolReaderWriter* skt,LPushHeader& lph)
{
    //			       4C  50  55  53  48
    static char LPushFlag[5]={'L','P','U','S','H'};
    int ret = ERROR_SUCCESS;
    unsigned char* buf = (unsigned char*)fast_buffer->read_slice(5);
    if(strstr((char*)buf,LPushFlag)==NULL)
    {
	return ERROR_SYSTEM_FILE_READ;
    }
    buf = (unsigned char*)fast_buffer->read_slice(4);
    int timestamp = *buf++<<24 | *buf++<<16 | *buf++<<8 | *buf++;
    unsigned char dataType = (unsigned char)fast_buffer->read_1byte();
    buf = (unsigned char*)fast_buffer->read_slice(4);
    int dataLen = *buf++<<24 | *buf++<<16 | *buf++<<8 | *buf++;
    if(dataLen<=0)
    {
      ret = ERROR_DATA_EMPTY;
      return ret;
    }
    if(dataLen > LPUSH_HANDSHAKE_DATA_MAX)
    {
      ret = ERROR_DATA_MAX;
      return ret;
    }
    LPushHeader ph(LPushFlag,(int)timestamp,dataType,(int)dataLen);
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
      if((ret = readHeader(skt,lph)) != ERROR_SUCCESS)
      {
	  lp_error("readMessage header is error");
	  return ret;
      }
      if(lph.datalenght>0){
      if((ret = fast_buffer->grow(skt,lph.datalenght)) != ERROR_SUCCESS)
      {
	  lp_error("readMessage socket data body is error");
	  return ret;
      }
      
      char *buf = fast_buffer->read_slice(lph.datalenght);
      lpc.setData(lph,(unsigned char*)buf);
      lp_info("lpc data %0X",lpc.data);
      }else
      {
	lpc.setData(lph,NULL);
      }
      return ret;
}

int LPushProtocol::handshake(LPushChunk* message,LPushHandshakeMessage &msk)
{
      int ret = ERROR_SUCCESS;
      if((message->header.dataType&0x0F)!=LPUSH_HEADER_TYPE_HANDSHAKE)
      {
	  lp_warn("lpush handshake data header type not ok!");
	  return ERROR_SYSTEM_HANDSHAKE;
      }
      if(LPushFMT::isType(message->data)!=LPUSH_FMT_JSON)
      {
	  lp_warn("lpush handshake data must be json data");
	  return ERROR_SYSTEM_HANDSHAKE;
      }
      
      std::map<std::string,std::string> parms;
      int len = LPushFMT::decodeJson(message->data,parms);
      if(len!=message->header.datalenght)
      {
	    lp_warn("lpush handshake data length error");
	    return ERROR_SYSTEM_HANDSHAKE;
      }
      msk.setParams(parms);
      if(!msk.check())
      {
	return ERROR_SYSTEM_HANDSHAKE;
      }
      return ret;
}

int LPushProtocol::createConnection(LPushChunk* message, LPushCreateMessage& pcm)
{
      int ret = ERROR_SUCCESS;
       if((message->header.dataType&0x0F)!=LPUSH_HEADER_TYPE_CREATE_CONNECTION)
      {
	  lp_warn("lpush create connection data header type not ok!");
	  return ERROR_SYSTEM_HANDSHAKE;
      }
      if(message->header.datalenght>0){
      char *buf = new char[message->header.datalenght];
      memset(buf,0,message->header.datalenght);
      memcpy(buf,message->data,message->header.datalenght);
      buf[message->header.datalenght+1]='\0';
      pcm.setStr(std::string(buf));
      delete buf;
      }
      return ret;
}


int LPushProtocol::recvhreatbeat(LPushChunk* message)
{
      int ret = ERROR_SUCCESS;
      if((message->header.dataType&0x0F)!=LPUSH_HEADER_TYPE_HREATBEAT)
      {
	  lp_warn("lpush hreatbeat data header type not ok!");
	  return ERROR_SYSTEM_HANDSHAKE;
      }
      return ret;
}

int LPushProtocol::sendHandshake(LPushHandshakeMessage lphm)
{	
    int ret = ERROR_SUCCESS;
    int time = (int)getCurrentTime();
    std::string data = LPushConfig::mapToJsonStr(lphm.tomap());
    LPushHeader lp("LPUSH",time,LPUSH_CALLBACK_TYPE_HANDSHAKE,data.length()+1);
    char *buf = new char[data.length()+5];
    memset(buf,0,data.length()+1);
    char  *b = buf;
    b[0] = LPUSH_FMT_JSON;
    int  jsonlen = data.length();
    b[1] |=jsonlen>>24 &0xFF;
    b[2] |=jsonlen>>16 &0xFF;
    b[3] |=jsonlen>>8 &0xFF;
    b[4] |=jsonlen & 0xFF;
    memcpy(&b[5],data.c_str(),data.length());
    LPushChunk* message=new LPushChunk(lp,(unsigned char*)buf);
    SafeDelete(buf);
    if((ret=sendPacket(message))!=ERROR_SUCCESS)
    {
	lp_error("send packet error");
	SafeDelete(message);
	return ret;
    }
    SafeDelete(message);
    return ret;
}

int LPushProtocol::sendCreateConnection(LPushCreateMessage lpcm)
{
    int ret = ERROR_SUCCESS;
    int time = (int)getCurrentTime();
    LPushHeader lp("LPUSH",time,LPUSH_CALLBACK_TYPE_CONNECTION_IS_OK,1);
    unsigned char buf = 0x01;
    LPushChunk* message=new LPushChunk(lp,(unsigned char*)&buf);
    if((ret=sendPacket(message))!=ERROR_SUCCESS)
    {
	lp_error("send packet error");
	SafeDelete(message);
	return ret;
    }
    SafeDelete(message);
    return ret;
}

int LPushProtocol::sendHreatbeat(LPushChunk* message)
{
    return sendPacket(message);
}

int LPushProtocol::sendPacket(LPushChunk* message)
{
    int ret = ERROR_SUCCESS;
    iovec iovs[2];
    LPushHeader *header = &(message->header);
    char *buf=new char[14];
    char *pbuf  = buf;
    iovs[0].iov_base = pbuf+0;
    memset(buf,0,14);
    *pbuf++ |='L';
    *pbuf++ |='P';
    *pbuf++ |='U';
    *pbuf++ |='S';
    *pbuf++ |='H';
    *pbuf++ |=(header->timestamp>>24)&0xFF;
    *pbuf++ |=(header->timestamp>>16)&0xFF;
    *pbuf++ |=(header->timestamp>>8)&0xFF;
    *pbuf++ |=(header->timestamp)&0xFF;
    *pbuf++ |=header->dataType;
    *pbuf++ |=(header->datalenght>>24)&0xFF;
    *pbuf++ |=(header->datalenght>>16)&0xFF;
    *pbuf++ |=(header->datalenght>>8)&0xFF;
    *pbuf++ |=(header->datalenght)&0xFF;
    iovs[0].iov_len = 14;
    iovs[1].iov_base = message->data;
    iovs[1].iov_len = header->datalenght;
    
    if((ret=lst->writev(iovs,2,NULL))!=ERROR_SUCCESS)
    {
	lp_error("send message packet error");
	SafeDelete(buf);
	return ret;
    }
    SafeDelete(buf);
    return ret;
}

LPushHandshakeMessage::LPushHandshakeMessage()
{

}


LPushHandshakeMessage::LPushHandshakeMessage(std::map<std::string,std::string> parms)
{
    appId = parms["appId"];
    screteKey = parms["screteKey"];
    md5Data = parms["md5Data"];
    userId = parms["userId"];
    clientFlag = parms["clientFlag"];
    identity   = parms["identity"];
    devices   = parms["devices"];
}

LPushHandshakeMessage::~LPushHandshakeMessage()
{

}

void LPushHandshakeMessage::setParams(std::map<std::string,std::string> parms)
{
    appId = parms["appId"];
    screteKey = parms["screteKey"];
    md5Data = parms["md5Data"];
    userId = parms["userId"];
    clientFlag = parms["clientFlag"];
    identity   = parms["identity"];
    devices   = parms["devices"];
}

bool LPushHandshakeMessage::check()
{
     std::string md5Src    = userId + appId + screteKey;
     std::string md5Temple = md5Encoder(md5Src);
     if(!isIntergeStr(userId))
      {
	  lp_warn("handshake userId  type is not match");
	  return false;
      }
     if(md5Data.length()!=md5Temple.length())
     {
       lp_warn("handshake Authorization error");
       return false;
    }
     if(md5Data.find(md5Temple.c_str())==std::string::npos)
     {
       lp_warn("handshake Authorization error");
       return false;
    }
     return true;
}
std::map< std::string, std::string > LPushHandshakeMessage::tomap()
{
    std::map< std::string, std::string > map;
    map.insert(std::make_pair("appId",appId));
    map.insert(std::make_pair("screteKey",screteKey));
    map.insert(std::make_pair("userId",userId));
    map.insert(std::make_pair("clientFlag",clientFlag));
    map.insert(std::make_pair("md5Data",md5Data));
    map.insert(std::make_pair("identity",identity));
    map.insert(std::make_pair("devices",devices));
    return map;
}

std::map< std::string, std::string > LPushHandshakeMessage::toMongomap()
{
      std::map< std::string, std::string > map;
    map.insert(std::make_pair("appKey",appId));
    map.insert(std::make_pair("userId",userId));
    map.insert(std::make_pair("identity",identity));
    map.insert(std::make_pair("devices",devices));
    map.insert(std::make_pair("status","1"));
    return map;
}


LPushHreatbeat::LPushHreatbeat()
{

}

LPushHreatbeat::LPushHreatbeat(const char flag)
{
    clientFlag.append(&flag);
}

LPushHreatbeat::~LPushHreatbeat()
{

}

LPushCreateMessage::LPushCreateMessage()
{

}


LPushCreateMessage::LPushCreateMessage(std::string cs)
{
    createString = cs;
}

LPushCreateMessage::~LPushCreateMessage()
{

}
void LPushCreateMessage::setStr(std::string cs)
{
    createString = cs;
}



}