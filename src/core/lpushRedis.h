#include <lpushCommon.h>
#include <hiredis/hiredis.h>

namespace lpush 
{
  
class LPushRedisClient;  
  
extern LPushRedisClient * redis_client;  

extern bool RedisInitializer();

extern bool RedisClose();

class LPushRedisClient
{
private:
  redisContext * 		context;
  redisReply  * 		reply;
  struct timeval 		timeout;
  const char * 			hostname;
  int 				port;
public:
  LPushRedisClient(const char * _hostname,int _port);
  virtual ~LPushRedisClient();
public:
  virtual bool initRedis();
  virtual void setTimeout(int seconds,int naseconds);
  virtual bool selectDb(int dbnum);
  virtual bool auth(std::string pass);
  virtual std::string set(std::string key,std::string value);
  virtual std::string set(std::string key,int value);
  virtual std::string setForBinary(const char* buf,int blen,const char* value,int vlen);
  virtual std::string get(std::string key);
  virtual void del(std::string key);
  virtual long long incr();
  virtual bool lPushForList(std::string key,std::string value);
  virtual std::vector<std::string> list(std::string key,int _begin, int _end);
  virtual bool lpop(std::string key);
  virtual bool rpop(std::string key);
  virtual bool closeRedis();
  
public:
  //map controller
  
  virtual bool hset(std::string key,std::string field,std::string value);
  
  virtual std::string hget(std::string key,std::string field);
  
  virtual bool hsetnx(std::string key,std::string field,std::string value);
  
  virtual std::map<std::string,std::string> hgetall(std::string key);
  
  virtual bool hdel(std::string key,std::string field);
public:
  virtual bool expire(std::string key,int time);
};
    
}