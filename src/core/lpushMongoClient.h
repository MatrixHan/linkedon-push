#pragma once
#include <lpushCommon.h>
#include <mongoc.h>
#include <bcon.h>
namespace lpush 
{
class LPushMongodbClient;

extern LPushMongodbClient * mongodb_client;

extern bool MongodbClientInit();

extern bool CloseMongodbClient();
  
class LPushMongodbClient
{
private:
    mongoc_client_t *		client;
    mongoc_cursor_t *		cursor;
    bson_error_t 		error;
    const bson_t *		doc;
    mongoc_client_pool_t *	_pool;
public:
  /**
   *uristr example "mongodb://127.0.0.1/?appname=client-example";
   */
  LPushMongodbClient();
  virtual ~LPushMongodbClient();
public:
  virtual bool initMongodbClient(mongoc_client_pool_t *pool);
  virtual bool initMongodbClient(const char *url);
  mongoc_collection_t* excute(const char* db,const char * collectionName);
  bson_t excute(std::map<std::string,std::string> params);
  void destory_bson(bson_t *b);
  void destory_collection(mongoc_collection_t *cll);
  virtual bool selectOneIsExist(std::string db,std::string collectionName,std::map<std::string,std::string> params);
  virtual bool skipParamsIsExist(std::string db,std::string collectionName,std::map<std::string,std::string> params,int skipnum);
  virtual std::vector<std::string> queryFromCollectionToJson(bson_t *_query,mongoc_collection_t *cll);
  virtual std::vector<std::string> queryToListJson(std::string db,std::string collectionName,std::map<std::string,std::string> params);
  virtual std::vector<std::string> queryToListJsonLimit(std::string db,std::string collectionName,std::map<std::string,std::string> params,int page,int pageSize);
  virtual int64_t count(std::string db,std::string collectionName,std::map<std::string,std::string> params);
  virtual int insertFromCollectionToJson(std::string db,std::string collectionName,std::map<std::string,std::string> params);
  virtual int delFromCollectionToJson(std::string db,std::string collectionName,std::string oid);
  virtual int delFromQuery(std::string db,std::string collectionName,std::map<std::string,std::string> params);
  virtual int updateFromCollectionToJson(std::string db,std::string collectionName,std::string oid,std::map<std::string,std::string> uparams);
  virtual bson_oid_t getOidByJsonStr(std::string json);
  virtual std::map<std::string,std::string> jsonToMap(std::string json);
  virtual void closePool();
  virtual void close();
};
  
  
}