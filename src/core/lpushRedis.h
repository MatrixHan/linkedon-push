#include <lpushCommon.h>
#include <hiredis/hiredis.h>

namespace lpush 
{
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
  virtual std::string set(std::string key,std::string value);
  virtual std::string setForBinary(std::string key,std::string value);
  virtual std::string get(std::string key);
  virtual void del(std::string key);
  virtual long long incr();
  virtual bool lPushForList(std::string key,std::string value);
  virtual std::vector<std::string> list(std::string key,int page,int pageNo);
  virtual bool closeRedis();
  
};
    
}