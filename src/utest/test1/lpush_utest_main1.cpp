#include <lpushCommon.h>
#include <lpushRedis.h>
#include <lpushJson.h>
#include <lpushLogger.h>
#include <lpush_utest_pushwork.h>
#include <lpush_utest_mongo.h>
#include <lpushMongoPool.h>

using namespace std;
using namespace lpush;

int test2()
{
	
	RedisInitializer();
	LPushWork lpw;
	lpw.pushWork("10001");
	//lpw.testJson();
	RedisClose();
	return 0;
}

int test1()
{
  LPushUtestMongo lpum;
  //lpum.test1();
  lpum.test5();
  return 0;
}

int main(void)
{
    initConfig("");
    InitLog(DEFAULT_LOG_FILE_NAME);
    initialzerMongoPool();
    test1();
    destroyMongoPool();
    CloseLog();
    return 0;
}