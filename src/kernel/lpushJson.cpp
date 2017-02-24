#include <lpushJson.h>
#include <lpushUtils.h>
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
   mongodbConfig = new LPushMongodbConfig();
}

LPushConfig::~LPushConfig()
{
    SafeDelete(redisConfig);
    SafeDelete(mongodbConfig);
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
      cf->isdaemon = root["isdaemon"].asInt();
      cf->port   = root["port"].asInt();
      cf->loglevel= root["loglevel"].asInt();
      cf->ip     = root["ip"].asString();
      cf->logdir = root["logdir"].asString();
      cf->logfilename= root["logfilename"].asString();
      cf->localhost  = root["localhost"].asString();
      cf->resultList = root["resultList"].asString();
      cf->appKeys = root["lpushkey.db"].asString();
      cf->serverList = root["serversKey"].asCString();
      cf->task_prefix = root["task_prefix"].asCString();
      
      cf->redisConfig->host = root["redis.host"].asString();
      cf->redisConfig->pass = root["redis.pass"].asString();
      cf->redisConfig->port = root["redis.port"].asInt();
      cf->redisConfig->db = root["redis.db"].asInt();
      
      cf->mongodbConfig->url = root["mongo.url"].asString();
      cf->mongodbConfig->db = root["mongo.db"].asString();
      
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

map< string, string > LPushConfig::jsonStrToMap(string str)
{
    map< string, string > result;
    Json::Reader reader;
    Json::Value  root;
    Json::Value::Members members;
      if(!reader.parse(str, root, false))
      {
	    return result;
      }
    members = root.getMemberNames();
    Json::Value::Members::iterator itr = members.begin();
      for(;itr!=members.end();++itr)
      {
	  std::string key = *itr;
	  
	 std::string str = root[key].asString();
	  result.insert(std::make_pair(key,str));
      }
    return result;
}

string LPushConfig::mapToJsonStr(map< string, string > params)
{
    Json::FastWriter writer;
    Json::Value person;
    std::string ret;
    std::map< std::string, std::string >::iterator itr=params.begin();
    for(;itr!=params.end();++itr)
    {
      person[itr->first]=itr->second;
    }
    ret.append(person.toStyledString());
    Trim(ret);
    return ret;
}

  
}