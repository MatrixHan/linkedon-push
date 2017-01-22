#pragma once

#include <lpushCommon.h>
#include <fstream>
#include <json/json.h>

namespace lpush
{
class LPushConfig;
  
extern LPushConfig * conf;
int initConfig();
struct LPushConfig
{
  int 			gopsize;
  int 			port;
  std::string 		movieDir;
  std::string 		vhost;
  int 			maxconnect;
  
  LPushConfig();
  ~LPushConfig();
  
  static LPushConfig* parse(std::string confName);
  static int 	    writeConf(LPushConfig* config);
};

  
}