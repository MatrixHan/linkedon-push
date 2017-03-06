#include <lpushMongoPool.h>
#include <lpushJson.h>
namespace lpush 
{
  
LPushMongoPool  * lpMongoPool = NULL;

bool initialzerMongoPool()
{
    if(!lpMongoPool)
    {
       lpMongoPool = new LPushMongoPool(conf->mongodbConfig->url.c_str());
       lpMongoPool->initialzer();
    }
}

bool destroyMongoPool()
{
    SafeDelete(lpMongoPool);
}

  
LPushMongoPool::LPushMongoPool(const char* url)
{
    uristr = url;
    uri = NULL;
    pool = NULL;
}

LPushMongoPool::~LPushMongoPool()
{
    destroy();
}

bool LPushMongoPool::initialzer()
{
    mongoc_init ();
    uri = mongoc_uri_new (uristr);
    if (!uri) {
      fprintf (stderr, "Failed to parse URI: \"%s\".\n", uristr);
      return EXIT_FAILURE;
   }

   pool = mongoc_client_pool_new (uri);
   mongoc_client_pool_set_error_api (pool, 2);
}

mongoc_client_pool_t* LPushMongoPool::getPool()
{
      return pool;
}

void LPushMongoPool::destroy()
{
   mongoc_client_pool_destroy (pool);
   pool = 0x0;
   mongoc_uri_destroy (uri);
   uri = 0x0;
   mongoc_cleanup ();
}

  
}