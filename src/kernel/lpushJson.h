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

struct LPushConfig
{

  std::string 		ip;
  int 			port;
  int 			maxconnect;
  std::string 		logdir;
  std::string 		logfilename;
  int 			loglevel;
  std::string 		localhost;
  LPushRedisConfig      *redisConfig;
  LPushConfig();
  ~LPushConfig();
  
  static LPushConfig* parse(std::string confName);
  static int 	    writeConf(LPushConfig* config);
  
  static std::string mapToJsonStr(std::map<std::string,std::string> params);
};

  
}