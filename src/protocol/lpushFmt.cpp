#include <lpushFmt.h>
#include <lpushMath.h>
#include <lpushUtils.h>
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
    Json::Value::Members members;  
    char * jsonData ;
    int jsonLenght = 0;
    if((*buf++ & 0x0F) == LPUSH_FMT_JSON)
    {
	jsonLenght = (*buf++ & 0xFF) << 24 | (*buf++ & 0xFF) << 16 | (*buf++ & 0xFF) << 8 | (*buf++ & 0xFF); 
	jsonData = (char*)malloc(jsonLenght);
	memset(jsonData,0,jsonLenght);
	memcpy(jsonData,buf,jsonLenght);
	buf+=jsonLenght;
	char *begin ,*end;
	begin=jsonData;
	end = jsonData+jsonLenght;
	if (!reader.parse(begin , end, root, false))
	{
	    return ERROR_FMT;
	}
	members = root.getMemberNames();
	Json::Value::Members::iterator itr = members.begin();
	for(;itr!=members.end();++itr)
	{
	  std::string key = *itr;
	  std::string value = root[key].asString();
	  ret.insert(std::make_pair(key,value));
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
	memset(stringData,0,stringlen);
	memcpy(stringData,buf,stringlen);
	buf+=stringlen;
	stringData[stringlen++] = '\0';
	ret = std::string(stringData);
	SafeDelete(stringData);
    }
    return buf-data;
}

///

int LPushFMT::encodeBool(bool src,unsigned char **ret,int &len)
{
    unsigned char *buf = (unsigned char *)malloc(2);
     memset(buf,0,2);
     unsigned char *p = buf;
    *p++ |= LPUSH_FMT_BOOL;
    if(src)
    {
      *p++ |= 0x01;
    }else
    {
      *p++ |= 0x00;
    }
    len = 2;
    *ret = buf;
    return 0;
}

int LPushFMT::encodeFloat(float src,unsigned char **ret,int &len)
{
    int up = (int)src;
    int down = getDecimal(src);
    unsigned char *buf = (unsigned char*)malloc(9);
     memset(buf,0,9);
     unsigned char *p = buf;
    *p++ |= LPUSH_FMT_FLOAT;
    *p++ |= (up>>24)&0xFF;
    *p++ |= (up>>16)&0xFF;
    *p++ |= (up>>8)&0xFF;
    *p++ |= (up)&0xFF;
    *p++ |= (down>>24)&0xFF;
    *p++ |= (down>>16)&0xFF;
    *p++ |= (down>>8)&0xFF;
    *p++ |= (down)&0xFF;
    len = 9;
    *ret = buf;
    return 0;
}

int LPushFMT::encodeInt(int src,unsigned char **ret,int &len)
{
    unsigned char *buf = (unsigned char*)malloc(5);
     memset(buf,0,5);
     unsigned char *p = buf;
    *p++ |=LPUSH_FMT_INT;
    *p++ |=(src>>24)&0xFF;
    *p++ |=(src>>16)&0xFF;
    *p++ |=(src>>8)&0xFF;
    *p++ |=(src)&0xFF;
    len = 5;
    *ret = buf;
    return 0;
}

int LPushFMT::encodeString(std::string src,unsigned char **ret,int &tlen)
{
    int len = src.size();
    unsigned char *buf = (unsigned char*)malloc(5);
     memset(buf,0,5);
     unsigned char *p = buf;
    *p++ |=LPUSH_FMT_STRING;
    *p++ |=(len>>24)&0xFF;
    *p++ |=(len>>16)&0xFF;
    *p++ |=(len>>8)&0xFF;
    *p++ |=(len)&0xFF;
    memcpy(p++,src.c_str(),src.size());
    len = 5+src.size();
    *ret = buf;
    return 0;
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
     char *p=buf;
    int len = person.toStyledString().size();
    *p++ |= LPUSH_FMT_JSON;
    *p++ |=(len>>24)&0xFF;
    *p++ |=(len>>16)&0xFF;
    *p++ |=(len>>8)&0xFF;
    *p++ |=(len)&0xFF;
    ret.append(buf);
    ret.append(person.toStyledString());
    SafeDelete(buf);
    return ret;
}

int LPushFMT::encodeLong(long long int src,unsigned char **ret,int &len)
{
    unsigned char *buf = (unsigned char*)malloc(9);
     memset(buf,0,9);
     unsigned char *p = buf;
    *p++ |= LPUSH_FMT_LONG;
    *p++ |= (src>>56)&0xFF;
    *p++ |= (src>>48)&0xFF;
    *p++ |= (src>>40)&0xFF;
    *p++ |= (src>>32)&0xFF;
    *p++ |= (src>>24)&0xFF;
    *p++ |= (src>>16)&0xFF;
    *p++ |= (src>>8)&0xFF;
    *p++ |= (src)&0xFF;
    len = 9;
    *ret = buf;
    return 0;
}



  
}