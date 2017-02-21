#include <lpushMongoClient.h>
#include <lpushLogger.h>

namespace lpush 
{
  
LPushMongodbClient::LPushMongodbClient(const char* _uristr):uristr(_uristr)
{

}

LPushMongodbClient::~LPushMongodbClient()
{
    close();
}

bool LPushMongodbClient::initMongodbClient()
{
     mongoc_init ();
     client = mongoc_client_new (uristr);
      if (!client) {
      fprintf (stderr, "Failed to parse URI.\n");
      return false;
    }
    mongoc_client_set_error_api (client, 2);
    return true;
}

mongoc_collection_t* LPushMongodbClient::excute(const char* db, const char* collectionName)
{
    mongoc_collection_t * cll = mongoc_client_get_collection(client,db,collectionName);
    return cll;
}

void LPushMongodbClient::destory_bson(bson_t* b)
{
    if(b){
      bson_destroy(b);
      b = 0x0;
    }
}

void LPushMongodbClient::destory_collection(mongoc_collection_t* cll)
{
    if(cll){
       mongoc_collection_destroy (cll);
       cll = 0x0;
    }
}


bson_t LPushMongodbClient::excute(std::map< std::string, std::string > params)
{
   
     
    bson_t  query;
    bson_init(&query);
     if(!params.empty()){
    std::map< std::string, std::string >::iterator itr = params.begin();
    for(;itr!=params.end();++itr)
    {
      std::string key = itr->first;
      std::string value = itr->second;
      bson_append_utf8(&query,key.c_str(),key.size(),value.c_str(),value.size());
    }       
    }
    return query;
}


std::vector<std::string> LPushMongodbClient::queryFromCollectionToJson(bson_t *_query,mongoc_collection_t *cll)
{
  std::vector<std::string> result;
    char *str;
   cursor = mongoc_collection_find_with_opts (cll,
                                              _query,
                                              NULL,  /* additional options */
                                              NULL); /* read prefs, NULL for default */

   while (mongoc_cursor_next (cursor, &doc)) {
      str = bson_as_json (doc, NULL);
      lp_info("mongos result %s",str);
      result.push_back(std::string(str));
      bson_free (str);
   }

   if (mongoc_cursor_error (cursor, &error)) {
      fprintf (stderr, "Cursor Failure: %s\n", error.message);
      result.clear();
   }
   return result;
}

void LPushMongodbClient::close()
{	
      mongoc_cursor_destroy (cursor);
    
      mongoc_client_destroy (client);

      mongoc_cleanup ();
}

  
}