#include <lpush_utest_pushwork.h>
#include <lpushSource.h>
#include <lpushRedis.h>
#include <lpushMath.h>
#include <lpushUtils.h>
#include <lpushJson.h>
#include <lpushProtocolStack.h>

using namespace std;

namespace lpush {

int LPushWork::pushWork(int loopNum)
{
    int ret = 10000;
    for(;ret<loopNum;ret++)
    {
      char buf[20];
      memset(buf,0,20);
      sprintf(buf,"%d",ret);
    LPushWorkerMessage lwm;
    std::string queuename = conf->task_prefix+"106.3.138.173:9732";
    lwm.taskId = uuidinit();
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
    std::string uuid = uuidinit();
    
    std::string queuename = conf->task_prefix+"106.3.138.173:9732";
    lwm.taskId = uuid;
    lwm.title = "你好阿!";
    lwm.userId=user;
    lwm.appKey="48947381";
    lwm.appSecret="83EB8022CEF82135EA5AE3D627D18026";
    lwm.content="这是一个测试的文档！";
    lwm.createTime = getCurrentTime();
    lwm.expiresTime = 10;
    lwm.msgId = "123123123";
    std::string json = lwm.toAllString();
    cout << json <<endl;
    redis_client->rPushForList(queuename,json);
    
    return 0;
}

std::string LPushWork::uuidinit()
{
    uuid_t uu;
    uuid_generate(uu);
    std::string uuid;
      for(int i = 0;i<16 ; i++)
    {
       std::string child = uint8To2Char(uu[i]);
       uuid.append(child.c_str());
    }
    return uuid;
}

void LPushWork::testJson()
{
   map<string,string> params;
   params.insert(make_pair("appId","48947381"));
   params.insert(make_pair("screteKey","83EB8022CEF82135EA5AE3D627D18026"));
   params.insert(make_pair("userId","10001"));
   string src;
   src.append("10001");
   src.append("48947381");
   src.append("83EB8022CEF82135EA5AE3D627D18026");
   string md5 = md5Encoder(src);
   params.insert(make_pair("md5Data",md5));
   params.insert(make_pair("clientFlag","1"));
   params.insert(make_pair("identity",uuidinit()));
   params.insert(make_pair("devices","3d:u2:pc:21:f2"));
   string  str = LPushConfig::mapToJsonStr(params);
   cout << str <<endl;
}


}