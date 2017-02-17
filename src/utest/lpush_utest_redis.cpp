#include <lpush_utest_redis.h>
#include <lpushRedis.h>

using namespace lpush;
void LPushUtestRedis::test1()
{
  LPushRedisClient *client = new LPushRedisClient("127.0.0.1",6379);
  client->setTimeout(1,500000);
  client->initRedis();
  client->set("foo","t11");
  string result = client->get("foo"); 
  client->setForBinary("tpp",3,"saa",3);
  result = client->get("tpp"); 
  client->lPushForList("mylist","{\"name\":\"lisi\"}");
  client->list("mylist",0,-1);
  client->lPushForList("mylist","{\"name\":\"张三\"}");
  client->list("mylist",0,-1);
  delete client;
}
