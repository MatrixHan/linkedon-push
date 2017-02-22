#pragma once
#include <lpushCommon.h>
#include <mongoc.h>
namespace lpush 
{
  
class LPushMongodbClient
{
private:
    mongoc_client_t *		client;
    mongoc_cursor_t *		cursor;
    bson_error_t 		error;
    const bson_t *		doc;
    const char *		uristr ;
public:
  /**
   *uristr example "mongodb://127.0.0.1/?appname=client-example";
   */
  LPushMongodbClient(const char * _uristr);
  virtual ~LPushMongodbClient();
public:
  virtual bool initMongodbClient();
  mongoc_collection_t* excute(const char* db,const char * collectionName);
  bson_t excute(std::map<std::string,std::string> params);
  void destory_bson(bson_t *b);
  void destory_collection(mongoc_collection_t *cll);
  virtual std::vector<std::string> queryFromCollectionToJson(bson_t *_query,mongoc_collection_t *cll);
  virtual std::vector<std::string> queryToListJson(std::string db,std::string collectionName,std::map<std::string,std::string> params);
  virtual int insertFromCollectionToJson(std::string db,std::string collectionName,std::map<std::string,std::string> params);
  virtual void close();
};
  
  
}