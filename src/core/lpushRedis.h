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
public:
  virtual bool lPushForList(std::string key,std::string value);
  virtual bool rPushForList(std::string key,std::string value);
  virtual std::vector<std::string> list(std::string key,int _begin, int _end);
  virtual bool lpop(std::string key);
  virtual bool rpop(std::string key);
  
  virtual bool lrem(std::string key,int count,std::string value);
  virtual bool ltrim(std::string key,int begin,int end);
  virtual int  llen(std::string key);
  virtual bool closeRedis();
  
public:
  //map controller
  
  virtual bool hset(std::string key,std::string field,std::string value);
  
  virtual std::string hget(std::string key,std::string field);
  
  virtual bool hsetnx(std::string key,std::string field,std::string value);
  
  virtual std::map<std::string,std::string> hgetall(std::string key);
  
  virtual bool hdel(std::string key,std::string field);
public:
  virtual bool zadd(std::string key,long long index,std::string value);
  
  virtual bool zaddList(std::string key,std::map<long long,std::string> values);
  
  virtual long long zcard(std::string key);
  
  virtual std::vector<std::string> zrange(std::string key,int begin,int end);
  
  virtual std::map<long long ,std::string> zrangeWithscores(std::string key,int begin,int end);
  //获取该值多下标
  virtual int zrank(std::string key,std::string value);
  //拿begin 到 end分数
  virtual long long zcount(std::string key,int begin,int end);
  //delete  value  in set
  virtual bool zrem(std::string key,std::string value);
  //delete values in set
  virtual bool zremList(std::string key,std::vector<std::string> values);
  //获取指定分数的直
  virtual long long zscore(std::string key,std::string value);
public:
  virtual bool expire(std::string key,int time);
};
    
}