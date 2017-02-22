#include <lpushRedis.h>
#include <lpushLogger.h>
#include <lpushJson.h>
namespace lpush 
{
  
LPushRedisClient *redis_client =NULL;  

bool RedisInitializer()
{
    LPushRedisConfig *redisConf = conf->redisConfig;
    if(!redis_client)
    {
	 redis_client = new LPushRedisClient(redisConf->host.c_str(),redisConf->port);
	 redis_client->initRedis();
	 redis_client->auth(redisConf->pass);
	 redis_client->selectDb(redisConf->db);
    }
    return true;
}

bool RedisClose()
{
    SafeDelete(redis_client);
    return true;
}
  
LPushRedisClient::LPushRedisClient(const char* _hostname, int _port):hostname(_hostname),port(_port)
{
    timeout  = {1 , 500000};
}
LPushRedisClient::~LPushRedisClient()
{
    closeRedis();
}
bool LPushRedisClient::initRedis()
{
    context = redisConnectWithTimeout(hostname,port,timeout);
    if (context == NULL || context->err) {
        if (context) {
            printf("Connection error: %s\n", context->errstr);
            redisFree(context);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
	return false;
    }
    return true;
}
bool LPushRedisClient::selectDb(int dbnum)
{
      reply = (redisReply*)redisCommand(context,"select %d",dbnum);
      lp_trace("redisClient checkout db  %d",dbnum);
      freeReplyObject(reply);
      return true;
}
bool LPushRedisClient::auth(std::string pass)
{
      reply = (redisReply*)redisCommand(context,"AUTH %s",pass.c_str());
      lp_trace("redisClient AUTH   %s",reply->str);
      freeReplyObject(reply);
      return true;
}

bool LPushRedisClient::closeRedis()
{
    /* Disconnects and frees the context */
    if(context)
    redisFree(context);
    return true;
}

std::string LPushRedisClient::set(std::string key, std::string value)
{
      reply = (redisReply*)redisCommand(context,"SET %s %s",key.c_str(),value.c_str());
      std::string result = reply->str;
      lp_trace("redisClient SET result return %s",reply->str);
      freeReplyObject(reply);
      return result;
}

std::string LPushRedisClient::set(std::string key, int value)
{
      reply = (redisReply*)redisCommand(context,"SET %s %d",key.c_str(),value);
      std::string result = reply->str;
      lp_trace("redisClient SET result return %s",reply->str);
      freeReplyObject(reply);
      return result;
}

std::string LPushRedisClient::get(std::string key)
{
      reply = (redisReply*)redisCommand(context,"GET %s",key.c_str());
      std::string result = std::string(reply->str);
      lp_trace("redisClient GET key %s result %s",key.c_str(),result.c_str());
      freeReplyObject(reply);
      return result;
}

void LPushRedisClient::del(std::string key)
{
      reply = (redisReply*)redisCommand(context,"DEL %s",key.c_str());
      lp_trace("redisClient DEL key %s",key.c_str());
      freeReplyObject(reply);
}

long long LPushRedisClient::incr()
{
      reply = (redisReply*)redisCommand(context,"INCR counter");
      long long result = reply->integer;
      return result;
}

std::vector< std::string > LPushRedisClient::list(std::string key, int _begin, int _end)
{
    std::vector<std::string> lists;
    int begin = _begin;
    int end  = _end;
    if(_begin == 0 && _end == -1)
    {
      begin = 0;
      end = -1;
    }
    reply = (redisReply*)redisCommand(context,"LRANGE %s %d %d",key.c_str(),begin,end);
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (int j = 0; j < reply->elements; j++) {
            lp_trace("%u) %s\n", j, reply->element[j]->str);
	    lists.push_back(std::string(reply->element[j]->str));
        }
    }
    freeReplyObject(reply);
    return lists;
}
//top
bool LPushRedisClient::lPushForList(std::string key, std::string value)
{
     reply = (redisReply*)redisCommand(context,"LPUSH %s %s",key.c_str(),value.c_str());
     lp_trace("redisClient LPUSH key %s value %s",key.c_str(),value.c_str());
     freeReplyObject(reply);
     return true;
}
//last
bool LPushRedisClient::rPushForList(std::__cxx11::string key, std::__cxx11::string value)
{
     reply = (redisReply*)redisCommand(context,"RPUSH %s %s",key.c_str(),value.c_str());
     lp_trace("redisClient RPUSH key %s value %s",key.c_str(),value.c_str());
     freeReplyObject(reply);
     return true;
}

bool LPushRedisClient::lpop(std::__cxx11::string key)
{
      reply = (redisReply*)redisCommand(context,"lpop %s ",key.c_str());
     lp_trace("redisClient lpop beging top  key %s  ",key.c_str());
     freeReplyObject(reply);
     return true;
}

bool LPushRedisClient::rpop(std::__cxx11::string key)
{
      reply = (redisReply*)redisCommand(context,"rpop %s ",key.c_str());
     lp_trace("redisClient rpop beging last  key %s ",key.c_str());
     freeReplyObject(reply);
     return true;
}

bool LPushRedisClient::lrem(std::__cxx11::string key,int count, std::__cxx11::string value)
{
     reply = (redisReply*)redisCommand(context,"lrem %s %d %s",key.c_str(),count,value.c_str());
     lp_trace("redisClient lrem %d  key %s ",count,key.c_str());
     freeReplyObject(reply);
     return true;
}

bool LPushRedisClient::ltrim(std::__cxx11::string key, int begin, int end)
{
       reply = (redisReply*)redisCommand(context,"ltrim %s %d %d",key.c_str(),begin,end);
     lp_trace("redisClient ltrim key %s  begin %d  end %d ",key.c_str(),begin,end);
     freeReplyObject(reply);
     return true;
}


int LPushRedisClient::llen(std::__cxx11::string key)
{
      int len = 0;
      reply = (redisReply*)redisCommand(context,"llen %s ",key.c_str());
     lp_trace("redisClient llen   key %s ",key.c_str());
      len = reply->integer;
     freeReplyObject(reply);
     return len;
}


std::string LPushRedisClient::setForBinary(const char* buf,int blen,const char* value,int vlen)
{
     reply = (redisReply*)redisCommand(context,"SET %b %b",buf,blen,value,vlen);
      std::string result = std::string(reply->str);
      lp_trace("redisClient SET result return %s",reply->str);
      freeReplyObject(reply);
      return result;
}

void LPushRedisClient::setTimeout(int seconds, int naseconds)
{
    timeout = {seconds,naseconds};
}


bool LPushRedisClient::hset(std::__cxx11::string key, std::__cxx11::string field, std::__cxx11::string value)
{
      reply = (redisReply*)redisCommand(context,"hset %s %s %s",key.c_str(),field.c_str(),value.c_str());
      lp_trace("redis client hset key %s field %s ",key.c_str(),field.c_str());
      freeReplyObject(reply);
      return true;
}

std::__cxx11::string LPushRedisClient::hget(std::__cxx11::string key, std::__cxx11::string field)
{	
      std::string result;
      reply = (redisReply*)redisCommand(context,"hget %s %s ",key.c_str(),field.c_str());
      if(reply->str)
      result = std::string(reply->str);
      lp_trace("redis client hget key %s field %s value %s",key.c_str(),field.c_str(),reply->str);
      freeReplyObject(reply);
      return result;
}

bool LPushRedisClient::hsetnx(std::__cxx11::string key, std::__cxx11::string field, std::__cxx11::string value)
{
      reply = (redisReply*)redisCommand(context,"hsetnx %s %s %s",key.c_str(),field.c_str(),value.c_str());
      lp_trace("redis client hsetnx key %s field %s ",key.c_str(),field.c_str());
      freeReplyObject(reply);
      return true;
}

std::map< std::__cxx11::string, std::__cxx11::string > LPushRedisClient::hgetall(std::__cxx11::string key)
{
      std::map<std::string,std::string > result;
      reply = (redisReply*)redisCommand(context,"hgetall %s",key.c_str());
      if (reply->type == REDIS_REPLY_ARRAY) {
        for (int j = 0; j < reply->elements; j+=2) {
            lp_trace("key) %s  value) %s\n", reply->element[j]->str, reply->element[j+1]->str);
	    result.insert(std::make_pair(std::string(reply->element[j]->str),
					 std::string(reply->element[j+1]->str)));
        }
    }
      freeReplyObject(reply);
      
      return result;
}

bool LPushRedisClient::hdel(std::__cxx11::string key, std::__cxx11::string field)
{
      reply = (redisReply*)redisCommand(context,"hdel %s %s ",key.c_str(),field.c_str());
      lp_trace("redis client hdel key %s field %s ",key.c_str(),field.c_str());
      freeReplyObject(reply);
      return true;
}


bool LPushRedisClient::expire(std::__cxx11::string key, int time)
{
    reply = (redisReply*)redisCommand(context,"expire %s %d",key.c_str(),time);
    lp_trace("redis client option key %s,expire time is %d",key.c_str(),time);
    freeReplyObject(reply);
    return true;
}



}