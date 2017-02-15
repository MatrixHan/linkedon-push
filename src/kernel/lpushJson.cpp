#include <lpushJson.h>
using namespace std;
namespace lpush 
{
 LPushConfig *conf = NULL;
  
 int initConfig()
{	
  conf = new LPushConfig();
  conf = LPushConfig::parse(CONFIG_DEFAULT_FILE_NAME);
  return 0;
}

 
LPushConfig::LPushConfig()
{
   port = maxconnect = 0;
}

LPushConfig::~LPushConfig()
{

}


LPushConfig* LPushConfig::parse(std::string confName)
{
      ifstream ifs;
      ifs.open(confName.c_str());
      assert(ifs.is_open());
      Json::Reader reader;
      Json::Value  root;
      if (!reader.parse(ifs, root, false))
      {
	return NULL;
      }
      int size = root.size();
      LPushConfig * cf = new LPushConfig();
      
      cf->maxconnect = root["maxconnect"].asInt();
      cf->port   = root["port"].asInt();
      cf->loglevel= root["loglevel"].asInt();
      cf->ip     = root["ip"].asString();
      cf->logdir = root["logdir"].asString();
      cf->logfilename= root["logfilename"].asString();;
      return cf;
}

int LPushConfig::writeConf(LPushConfig* config)
{
    Json::Value root;
    Json::FastWriter writer;
    Json::Value person;
    std::string json_file = writer.write(root);
    
   
    person["maxconnect"] = config->maxconnect;
    person["port"] = config->port;
    
    
 
    ofstream ofs;
    ofs.open(CONFIG_DEFAULT_FILE_NAME);
    assert(ofs.is_open());
    ofs<<json_file;
 
    return 0;
    
}

  
}