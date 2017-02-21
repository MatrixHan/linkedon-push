#include <lpushCommon.h>
#include <lpushRedis.h>
#include <lpushJson.h>
#include <lpushLogger.h>
#include <lpush_utest_pushwork.h>
#include <lpush_utest_mongo.h>

using namespace std;
using namespace lpush;

int test2()
{
	initConfig();
	InitLog(DEFAULT_LOG_FILE_NAME);
	RedisInitializer();
	LPushWork lpw;
	lpw.pushWork("10001");
	RedisClose();
	CloseLog();
	return 0;
}

int test1()
{
  LPushUtestMongo lpum;
  lpum.test1();
  return 0;
}

int main(void)
{
    test1();
    return 0;
}