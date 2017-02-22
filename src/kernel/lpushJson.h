#pragma once

#include <lpushCommon.h>
#include <fstream>
#include <json/json.h>

namespace lpush
{
class LPushConfig;
  
extern LPushConfig * conf;
int initConfig();
struct LPushRedisConfig
{
  std::string host;
  int 		port;
  std::string pass;
  int 		db;
};

struct LPushMongodbConfig
{
  std::string url;
  std::string db;
};

struct LPushObject
{
  //1 int   2 string  
  int format;
  int data;
  std::string str;
};

struct LPushConfig
{

  std::string 		ip;
  int 			port;
  int 			maxconnect;
  std::string 		logdir;
  std::string 		logfilename;
  int 			loglevel;
  std::string 		localhost;
  std::string 		resultMap;
  std::string     	appKeys;
  LPushRedisConfig      *redisConfig;
  LPushMongodbConfig    *mongodbConfig;
  LPushConfig();
  ~LPushConfig();
  
  static LPushConfig* parse(std::string confName);
  static int 	    writeConf(LPushConfig* config);
  
  static std::map<std::string,std::string> jsonStrToMap(std::string str);
  static std::string mapToJsonStr(std::map<std::string,std::string> params);
};

  
}