#include <lpushJson.h>
#include <lpushUtils.h>
using namespace std;
namespace lpush 
{
 LPushConfig *conf = NULL;
  
 int initConfig(std::string file)
{	
    conf = new LPushConfig();
  if(file.empty())
    conf = LPushConfig::parse(CONFIG_DEFAULT_FILE_NAME);
  else
    conf = LPushConfig::parse(file);
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
      
      if(root.isMember("maxconnect"))
      cf->maxconnect = root["maxconnect"].asInt();
      if(root.isMember("isdaemon"))
      cf->isdaemon = root["isdaemon"].asInt();
      if(root.isMember("port"))
      cf->port   = root["port"].asInt();
      if(root.isMember("pid_file"))
      cf->pid_file  = root["pid_file"].asString();
      if(root.isMember("loglevel"))
      cf->loglevel= root["loglevel"].asInt();
      if(root.isMember("ip"))
      cf->ip     = root["ip"].asString();
      if(root.isMember("logdir"))
      cf->logdir = root["logdir"].asString();
      if(root.isMember("logfilename"))
      cf->logfilename= root["logfilename"].asString();
      if(root.isMember("localhost"))
      cf->localhost  = root["localhost"].asString();
      if(root.isMember("resultList"))
      cf->resultList = root["resultList"].asString();
      if(root.isMember("lpushkey.db"))
      cf->appKeys = root["lpushkey.db"].asString();
      if(root.isMember("serversKey"))
      cf->serverList = root["serversKey"].asCString();
      if(root.isMember("task_prefix"))
      cf->task_prefix = root["task_prefix"].asCString();
      if(root.isMember("project_dir"))
      cf->project_dir = root["project_dir"].asString();
      if(root.isMember("redis.host"))
      cf->redisConfig->host = root["redis.host"].asString();
      if(root.isMember("redis.pass"))
      cf->redisConfig->pass = root["redis.pass"].asString();
      if(root.isMember("redis.port"))
      cf->redisConfig->port = root["redis.port"].asInt();
      if(root.isMember("redis.db"))
      cf->redisConfig->db = root["redis.db"].asInt();
      
	
      if(root.isMember("mongo.url"))
      cf->mongodbConfig->url = root["mongo.url"].asString();
      if(root.isMember("mongo.db"))
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