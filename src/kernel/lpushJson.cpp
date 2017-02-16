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
   redisConfig = new LPushRedisConfig();
}

LPushConfig::~LPushConfig()
{
    SafeDelete(redisConfig);
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
      cf->logfilename= root["logfilename"].asString();
      cf->localhost  = root["localhost"].asString();
      
      cf->redisConfig->host = root["redis.host"].asString();
      cf->redisConfig->pass = root["redis.pass"].asString();
      cf->redisConfig->port = root["redis.port"].asInt();
      cf->redisConfig->db = root["redis.db"].asInt();
      
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
string LPushConfig::parse(map< string, string > params)
{
    Json::Value root;
    Json::FastWriter writer;
    Json::Value person;
    std::string json_file = writer.write(root);
    std::string ret;
    std::map< std::string, std::string >::iterator itr=params.begin();
    for(;itr!=params.end();++itr)
    {
      person[itr->first]=itr->second;
    }
    ret.append(json_file);
    return ret;
}

  
}