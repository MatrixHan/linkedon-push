#pragma once
#include <lpushCommon.h>
#include <lpushJson.h>
#include <lpushSystemErrorDef.h>
#include <lpushLogger.h>

namespace lpush 
{
#define LPUSH_FMT_STRING		0x01
  
#define LPUSH_FMT_BOOL			0x02
  
#define LPUSH_FMT_INT			0x03
  
#define LPUSH_FMT_LONG			0x04
  
#define LPUSH_FMT_FLOAT			0x05
  
#define LPUSH_FMT_JSON			0x06
  
  
class LPushFMT
{
public:
  static int isType(unsigned char *data);
public:
  static int decodeString(unsigned char *data,std::string &ret);
  static int decodeInt(unsigned char *data,int &ret);
  static int decodeBool(unsigned char *data,bool &ret);
  static int decodeLong(unsigned char *data,long long &ret);
  static int decodeFloat(unsigned char *data,float &ret);
  static int decodeJson(unsigned char *data,std::map<std::string,std::string> &ret);
public:
  static int encodeString(std::string src,unsigned char **ret,int &tlen);
  static int encodeInt(int src,unsigned char **ret,int &len);
  static int encodeBool(bool src,unsigned char **ret,int &len);
  static int encodeLong(long long src,unsigned char **ret,int &len);
  static int encodeFloat(float src,unsigned char **ret,int &len);
  static std::string encodeJson(std::map<std::string,std::string> src);
};

}