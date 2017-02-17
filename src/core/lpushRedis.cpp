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
}

bool RedisClose()
{
    SafeDelete(redis_client);
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
      lp_info("redisClient checkout db  %d",dbnum);
      freeReplyObject(reply);
      return true;
}
bool LPushRedisClient::auth(std::string pass)
{
      reply = (redisReply*)redisCommand(context,"AUTH %s",pass.c_str());
      lp_info("redisClient AUTH   %s",reply->str);
      freeReplyObject(reply);
      return true;
}

bool LPushRedisClient::closeRedis()
{
    /* Disconnects and frees the context */
    if(context)
    redisFree(context);
}

std::string LPushRedisClient::set(std::string key, std::string value)
{
      reply = (redisReply*)redisCommand(context,"SET %s %s",key.c_str(),value.c_str());
      std::string result = reply->str;
      lp_info("redisClient SET result return %s",reply->str);
      freeReplyObject(reply);
      return result;
}

std::string LPushRedisClient::set(std::string key, int value)
{
      reply = (redisReply*)redisCommand(context,"SET %s %d",key.c_str(),value);
      std::string result = reply->str;
      lp_info("redisClient SET result return %s",reply->str);
      freeReplyObject(reply);
      return result;
}

std::string LPushRedisClient::get(std::string key)
{
      reply = (redisReply*)redisCommand(context,"GET %s",key.c_str());
      std::string result = std::string(reply->str);
      lp_info("redisClient GET key %s result %s",key.c_str(),result.c_str());
      freeReplyObject(reply);
      return result;
}

void LPushRedisClient::del(std::string key)
{
      reply = (redisReply*)redisCommand(context,"DEL %s",key.c_str());
      lp_info("redisClient DEL key %s",key.c_str());
      freeReplyObject(reply);
}

long long LPushRedisClient::incr()
{
      reply = (redisReply*)redisCommand(context,"INCR counter");
      long long result = reply->integer;
      return result;
}

std::vector< std::string > LPushRedisClient::list(std::string key, int page, int pageNo)
{
    std::vector<std::string> lists;
    int begin = (page -1)*pageNo;
    int end  = page*pageNo;
    if(page == 0 && pageNo == -1)
    {
      begin = 0;
      end = -1;
    }
    reply = (redisReply*)redisCommand(context,"LRANGE %s %d %d",key.c_str(),begin,end);
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (int j = 0; j < reply->elements; j++) {
            lp_info("%u) %s\n", j, reply->element[j]->str);
	    lists.push_back(std::string(reply->element[j]->str));
        }
    }
    freeReplyObject(reply);
    return lists;
}

bool LPushRedisClient::lPushForList(std::string key, std::string value)
{
     reply = (redisReply*)redisCommand(context,"LPUSH %s %s",key.c_str(),value.c_str());
     lp_info("redisClient LPUSH key %s value %s",key.c_str(),value.c_str());
     freeReplyObject(reply);
     return true;
}

bool LPushRedisClient::lpop(std::__cxx11::string key)
{
      reply = (redisReply*)redisCommand(context,"lpop %s ",key.c_str());
     lp_info("redisClient lpop beging top  key %s  ",key.c_str());
     freeReplyObject(reply);
     return true;
}

bool LPushRedisClient::rpop(std::__cxx11::string key)
{
      reply = (redisReply*)redisCommand(context,"rpop %s ",key.c_str());
     lp_info("redisClient rpop beging last  key %s ",key.c_str());
     freeReplyObject(reply);
     return true;
}


std::string LPushRedisClient::setForBinary(const char* buf,int blen,const char* value,int vlen)
{
     reply = (redisReply*)redisCommand(context,"SET %b %b",buf,blen,value,vlen);
      std::string result = std::string(reply->str);
      lp_info("redisClient SET result return %s",reply->str);
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
      lp_info("redis client hset key %s field %s ",key.c_str(),field.c_str());
      freeReplyObject(reply);
      return true;
}

std::__cxx11::string LPushRedisClient::hget(std::__cxx11::string key, std::__cxx11::string field)
{
      reply = (redisReply*)redisCommand(context,"hget %s %s ",key.c_str(),field.c_str());
      std::string result = std::string(reply->str);
      lp_info("redis client hget key %s field %s value %s",key.c_str(),field.c_str(),reply->str);
      freeReplyObject(reply);
      return result;
}

bool LPushRedisClient::hsetnx(std::__cxx11::string key, std::__cxx11::string field, std::__cxx11::string value)
{
      reply = (redisReply*)redisCommand(context,"hsetnx %s %s %s",key.c_str(),field.c_str(),value.c_str());
      lp_info("redis client hsetnx key %s field %s ",key.c_str(),field.c_str());
      freeReplyObject(reply);
      return true;
}

std::map< std::__cxx11::string, std::__cxx11::string > LPushRedisClient::hmget(char *format, ...)
{
      std::map<std::string,std::string > result;
      va_list ap;
      va_start(ap, format);
      reply = (redisReply*)redisCommand(context,format,ap);
      va_end(ap);
      lp_info("redis client hmget ");
      freeReplyObject(reply);
      return result;
}

std::map< std::__cxx11::string, std::__cxx11::string > LPushRedisClient::hgetall(std::__cxx11::string key)
{
      std::map<std::string,std::string > result;
      reply = (redisReply*)redisCommand(context,"hgetall %s",key.c_str());
      
      freeReplyObject(reply);
      
      return result;
}


}