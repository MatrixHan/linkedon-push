#include <lpush_utest_pushwork.h>
#include <lpushSource.h>
#include <lpushRedis.h>
#include <lpushUtils.h>


namespace lpush {

int LPushWork::pushWork()
{
    
    LPushWorkerMessage lwm;
    std::string queuename = "172.16.104.21:9732";
    lwm.title = "hello world!";
    lwm.userId="10000";
    lwm.appKey="LOFFICIEL";
    lwm.appSecret="654321";
    lwm.content="the is test message!";
    lwm.createTime = getCurrentTime();
    lwm.expiresTime = 10;
    lwm.msgId = "123123123";
    std::string json = lwm.toAllString();
    redis_client->lPushForList(queuename,json);
    return 0;
}
}