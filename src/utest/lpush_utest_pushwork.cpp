#include <lpush_utest_pushwork.h>
#include <lpushSource.h>
#include <lpushRedis.h>
#include <lpushUtils.h>


namespace lpush {

int LPushWork::pushWork()
{
    
    LPushWorkerMessage lwm;
    std::string queuename = "172.16.104.21:9732";
    lwm.userId="";
    lwm.appKey="";
    lwm.appSecret="";
    lwm.content="";
    lwm.createTime = getCurrentTime();
    lwm.expiresTime = 10;
    lwm.msgId = "";
    std::string json = lwm.toJsonString();
    redis_client->lPushForList(queuename,json);
    return 0;
}
}