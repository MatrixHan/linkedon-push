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

  std::string 		ip;
  int 			port;
  int 			maxconnect;
  std::string 		logdir;
  std::string 		logfilename;
  int 			loglevel;
  LPushConfig();
  ~LPushConfig();
  
  static LPushConfig* parse(std::string confName);
  static int 	    writeConf(LPushConfig* config);
};

  
}