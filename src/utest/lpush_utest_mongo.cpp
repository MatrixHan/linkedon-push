#include <lpush_utest_mongo.h>
#include <lpushUtils.h>
#include <lpushMongoClient.h>
#include <lpushProtocolStack.h>
using namespace std;
using namespace lpush;

void LPushUtestMongo::test1()
{
  
    LPushHandshakeMessage lphm;
    lphm.appId = "LOFFICIEL";
    lphm.devices="212112";
    lphm.identity = "vcssa123sasfc";
    lphm.userId  = "10000";
    map<string,string>  lpmap = lphm.toMongomap();
    int time = getCurrentTime();
    char buf[20];
    memset(buf,0,20);
    sprintf(buf,"%d",time);
    lpmap.insert(make_pair("time",string(buf)));
    LPushMongodbClient *mclient = new LPushMongodbClient("mongodb://127.0.0.1/?appname=client-example");
    mclient->initMongodbClient();
    map<string,string> params;
    params.insert(make_pair("userId","10000"));
    params.insert(make_pair("appId","LOFFICIEL"));
    vector<string> result = mclient->queryToListJson("test","test",params);
    if(result.size()==0)
    mclient->insertFromCollectionToJson("test","test",lpmap);
   
    delete mclient;
}
