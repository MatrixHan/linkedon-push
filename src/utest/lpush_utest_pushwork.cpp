#include <lpush_utest_pushwork.h>
#include <lpushSource.h>
#include <lpushRedis.h>
#include <lpushUtils.h>


namespace lpush {

int LPushWork::pushWork()
{
    int ret = 10000;
    for(;ret<13000;ret++)
    {
      char buf[20];
      memset(buf,0,20);
      sprintf(buf,"%d",ret);
    LPushWorkerMessage lwm;
    std::string queuename = "task_list_172.16.104.21:9732";
    lwm.title = "hello world!";
    lwm.userId=std::string(buf);
    lwm.appKey="LOFFICIEL";
    lwm.appSecret="123456";
    lwm.content="the is test message!";
    lwm.createTime = getCurrentTime();
    lwm.expiresTime = 10;
    lwm.msgId = "123123123";
    std::string json = lwm.toAllString();
    redis_client->rPushForList(queuename,json);
    }
    return 0;
}

int LPushWork::pushWork(std::string user)
{
   
    LPushWorkerMessage lwm;
    std::string queuename = "task_list_172.16.104.21:9732";
    lwm.title = "hello world!";
    lwm.userId=user;
    lwm.appKey="LOFFICIEL";
    lwm.appSecret="123456";
    lwm.content="the is test message!";
    lwm.createTime = getCurrentTime();
    lwm.expiresTime = 10;
    lwm.msgId = "123123123";
    std::string json = lwm.toAllString();
    redis_client->rPushForList(queuename,json);
    
    return 0;
}
}