#pragma once

#include <lpushCommon.h>
#include <mongoc.h>
#include <stdio.h>
namespace lpush 
{
class LPushMongoPool;
  
extern LPushMongoPool  * lpMongoPool;

extern bool initialzerMongoPool();

extern bool destroyMongoPool();

class LPushMongoPool
{
private:
   const char *uristr ;
   mongoc_uri_t *uri;
   mongoc_client_pool_t *pool;
public:
  LPushMongoPool(const char *url);
  virtual ~LPushMongoPool();
public:
  bool initialzer();
  virtual mongoc_client_pool_t* getPool();
  void destroy();
};
  
}