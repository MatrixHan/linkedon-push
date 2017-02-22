#include <lpush_utest_mongo.h>
#include <lpushUtils.h>
#include <lpushMongoClient.h>
#include <lpushProtocolStack.h>
#include <lpushJson.h>
using namespace std;
using namespace lpush;

void LPushUtestMongo::test1()
{
    bson_error_t error;
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
    params.insert(make_pair("appKey","LOFFICIEL"));
    vector<string> result = mclient->queryToListJson("test","test",params);
    vector<string>::iterator itr = result.begin();
    for(;itr!=result.end();++itr)
    {
        string resultJson = *itr;
	map<string,string> entity = mclient->jsonToMap(resultJson);
	mclient->delFromCollectionToJson("test","test",entity["_id"]);
    }
    if(result.size()==0)
    mclient->insertFromCollectionToJson("test","test",lpmap);
    map<string,string>  cpmap = lphm.toMongomap();
    cpmap["status"]="0";
    cpmap.insert(make_pair("time",string(buf)));
   // mclient->updateFromCollectionToJson("test","test","",cpmap);
   
    delete mclient;
}
