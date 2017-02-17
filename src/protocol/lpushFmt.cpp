#include <lpushFmt.h>
#include <lpushMath.h>

namespace lpush 
{
  
int LPushFMT::isType(unsigned char* data)
{
    //look lpushFmt define fmt type
    return *data&0x0F;
}

  
int LPushFMT::decodeBool(unsigned char* data, bool& ret)
{
    unsigned char *buf = data;
    if((*buf++ & 0x0F) == LPUSH_FMT_BOOL)
    {
        if((*buf++ & 0x0F) == 0x01 )
	{
	   ret = true;
	}else
	{
	   ret = false;
	}
    }
    return buf-data;
}

int LPushFMT::decodeFloat(unsigned char* data, float& ret)
{
    unsigned char *buf = data;
    if((*buf++ & 0x0F) == LPUSH_FMT_FLOAT)
    {
      int up = (*buf++ & 0xFF) << 24 | (*buf++ & 0xFF) << 16 | (*buf++ & 0xFF) << 8 | (*buf++ & 0xFF);
      int down = (*buf++ & 0xFF) << 24 | (*buf++ & 0xFF) << 16 | (*buf++ & 0xFF) << 8 | (*buf++ & 0xFF);
      ret = up + ((float)down/(32*10));
    }
    return buf-data;
}

int LPushFMT::decodeInt(unsigned char* data, int& ret)
{
    unsigned char *buf = data;
    if((*buf++ & 0x0F) == LPUSH_FMT_INT)
    {
      ret = (*buf++ & 0xFF) << 24 | (*buf++ & 0xFF) << 16 | (*buf++ & 0xFF) << 8 | (*buf++ & 0xFF); 
    }
    return buf-data;
}

int LPushFMT::decodeJson(unsigned char* data, std::map< std::string, std::string >& ret)
{
    std::string str;
    unsigned char *buf = data;
    Json::Reader reader;
    Json::Value  root;
    char * jsonData ;
    int jsonLenght = 0;
    if((*buf++ & 0x0F) == LPUSH_FMT_JSON)
    {
	jsonLenght = (*buf++ & 0xFF) << 24 | (*buf++ & 0xFF) << 16 | (*buf++ & 0xFF) << 8 | (*buf++ & 0xFF); 
	jsonData = (char*)malloc(jsonLenght);
	memcpy(jsonData,buf,jsonLenght);
	buf+=jsonLenght;
	jsonData[jsonLenght++] = '\0';
	str = std::string(jsonData);
	if (!reader.parse(str, root, false))
	{
	    return NULL;
	}
	Json::Value::iterator itr = root.begin();
	for(;itr!=root.end();++itr)
	{
	  ret.insert(std::make_pair(itr.name(),itr.key().asString()));
	}
	SafeDelete(jsonData);
    }
    return buf-data;
}

int LPushFMT::decodeLong(unsigned char* data, long long int& ret)
{
    unsigned char *buf = data;
    if((*buf++ & 0x0F) == LPUSH_FMT_LONG)
    {
      ret = (*buf++ & 0xFF) << 56 | (*buf++ & 0xFF) << 48 | (*buf++ & 0xFF) << 40 | (*buf++ & 0xFF)<<32 |
      (*buf++ & 0xFF) << 24 | (*buf++ & 0xFF) << 16 | (*buf++ & 0xFF) << 8 | (*buf++ & 0xFF); 
    }
    return buf-data;
}

int LPushFMT::decodeString(unsigned char* data, std::string& ret)
{
    std::string str;
    unsigned char *buf = data;
    char *stringData;
    int stringlen = 0;
    if((*buf++ & 0x0F) == LPUSH_FMT_STRING)
    {
	stringlen = (*buf++ & 0xFF) << 24 | (*buf++ & 0xFF) << 16 | (*buf++ & 0xFF) << 8 | (*buf++ & 0xFF); 
	stringData = (char*)malloc(stringlen);
	memcpy(stringData,buf,stringlen);
	buf+=stringlen;
	stringData[stringlen++] = '\0';
	ret = std::string(stringData);
	SafeDelete(stringData);
    }
    return buf-data;
}

///

std::string LPushFMT::encodeBool(bool src)
{
    std::string ret;
    char *buf = (char *)malloc(2);
     memset(buf,0,2);
    *buf++ |= LPUSH_FMT_BOOL;
    if(src)
    {
      
      *buf++ |= 0x01;
    }else
    {
      *buf++ |= 0x00;
    }
    ret.append(buf);
    SafeDelete(buf);
    return ret;
}

std::string LPushFMT::encodeFloat(float src)
{
    std::string ret;
    int up = (int)src;
    int down = getDecimal(src);
    char *buf = (char*)malloc(9);
     memset(buf,0,9);
    *buf++ |= LPUSH_FMT_FLOAT;
    *buf++ |= (up>>24)&0xFF;
    *buf++ |= (up>>16)&0xFF;
    *buf++ |= (up>>8)&0xFF;
    *buf++ |= (up)&0xFF;
    *buf++ |= (down>>24)&0xFF;
    *buf++ |= (down>>16)&0xFF;
    *buf++ |= (down>>8)&0xFF;
    *buf++ |= (down)&0xFF;
    ret.append(buf);
    SafeDelete(buf);
    return ret;
}

std::string LPushFMT::encodeInt(int src)
{
    std::string ret;
    
    char *buf = (char*)malloc(5);
     memset(buf,0,5);
    *buf++ |=LPUSH_FMT_INT;
    *buf++ |=(src>>24)&0xFF;
    *buf++ |=(src>>16)&0xFF;
    *buf++ |=(src>>8)&0xFF;
    *buf++ |=(src)&0xFF;
    ret.append(buf);
    SafeDelete(buf);
    return ret;
    
}

std::string LPushFMT::encodeString(std::string src)
{
    std::string ret;
    int len = src.size();
    char *buf = (char*)malloc(5);
     memset(buf,0,5);
    *buf++ |=LPUSH_FMT_STRING;
    *buf++ |=(len>>24)&0xFF;
    *buf++ |=(len>>16)&0xFF;
    *buf++ |=(len>>8)&0xFF;
    *buf++ |=(len)&0xFF;
    ret.append(buf);
    ret.append(src);
    SafeDelete(buf);
    return ret;
}


std::string LPushFMT::encodeJson(std::map< std::string, std::string > src)
{
    Json::FastWriter writer;
    Json::Value person;
    std::string ret;
    std::map< std::string, std::string >::iterator itr=src.begin();
    for(;itr!=src.end();++itr)
    {
      person[itr->first]=itr->second;
    }
    char *buf = (char*)malloc(5);
    memset(buf,0,5);
    int len = person.toStyledString().size();
    *buf++ |= LPUSH_FMT_JSON;
    *buf++ |=(len>>24)&0xFF;
    *buf++ |=(len>>16)&0xFF;
    *buf++ |=(len>>8)&0xFF;
    *buf++ |=(len)&0xFF;
    ret.append(buf);
    ret.append(person.toStyledString());
    SafeDelete(buf);
    return ret;
}

std::string LPushFMT::encodeLong(long long int src)
{
    std::string ret;
    char *buf = (char*)malloc(9);
     memset(buf,0,9);
    *buf++ |= LPUSH_FMT_LONG;
    *buf++ |= (src>>56)&0xFF;
    *buf++ |= (src>>48)&0xFF;
    *buf++ |= (src>>40)&0xFF;
    *buf++ |= (src>>32)&0xFF;
    *buf++ |= (src>>24)&0xFF;
    *buf++ |= (src>>16)&0xFF;
    *buf++ |= (src>>8)&0xFF;
    *buf++ |= (src)&0xFF;
    ret.append(buf);
    SafeDelete(buf);
    return ret;
}



  
}