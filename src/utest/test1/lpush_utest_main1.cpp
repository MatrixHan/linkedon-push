#include <lpushCommon.h>
#include <lpushRedis.h>
#include <lpushJson.h>
#include <lpushLogger.h>
#include <lpush_utest_pushwork.h>

using namespace std;
using namespace lpush;

int test2()
{
	initConfig();
	InitLog(DEFAULT_LOG_FILE_NAME);
	RedisInitializer();
	LPushWork::pushWork();
	RedisClose();
	CloseLog();
	return 0;
}

int main(void)
{
    test2();
    return 0;
}