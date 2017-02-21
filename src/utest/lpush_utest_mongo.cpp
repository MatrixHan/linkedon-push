#include <lpush_utest_mongo.h>
#include <lpushMongoClient.h>
using namespace std;
using namespace lpush;

void LPushUtestMongo::test1()
{
    LPushMongodbClient *mclient = new LPushMongodbClient("mongodb://127.0.0.1/?appname=client-example");
    mclient->initMongodbClient();
    map<string,string> params;
    params.insert(make_pair("name","lisi"));
    mongoc_collection_t * cll = mclient->excute("test","test");
    bson_t query = mclient->excute(params);
    vector<string> result = mclient->queryFromCollectionToJson(&query,cll);
    mclient->destory_collection(cll);
    delete mclient;
}
