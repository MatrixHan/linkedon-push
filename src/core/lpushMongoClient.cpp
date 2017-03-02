#include <lpushMongoClient.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushMath.h>
#include <lpushUtils.h>
#include <lpushJson.h>
namespace lpush 
{
 
  LPushMongodbClient *mongodb_client = NULL;
  
bool mongoClientInit()
{
  if(!mongodb_client){
    mongodb_client = new LPushMongodbClient(conf->mongodbConfig->url.c_str());
    mongodb_client->initMongodbClient();
  }
}

bool mongoClientClose()
{
  if(mongodb_client)
    mongodb_client->close();
}

  
  
LPushMongodbClient::LPushMongodbClient(const char* _uristr):uristr(_uristr)
{
    cursor = NULL;
    cursor = NULL;
    doc    = NULL;
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

bool LPushMongodbClient::selectOneIsExist(std::string db, std::string collectionName, std::map< std::string, std::string > params)
{
    bson_t cmd = LPushMongodbClient::excute(params);
    mongoc_collection_t * cll = LPushMongodbClient::excute(db.c_str(),collectionName.c_str());
    cursor = mongoc_collection_find(cll,
				    MONGOC_QUERY_NONE,0,1,0,&cmd,NULL,
				    NULL); /* read prefs, NULL for default */
    while (mongoc_cursor_next (cursor, &doc)) {
      return true;
    }
    return false;
}


bool LPushMongodbClient::skipParamsIsExist(std::string db, std::string collectionName, std::map< std::string, std::string > params, int skipnum)
{
    bson_t cmd = LPushMongodbClient::excute(params);
    mongoc_collection_t * cll = LPushMongodbClient::excute(db.c_str(),collectionName.c_str());
    cursor = mongoc_collection_find(cll,
				    MONGOC_QUERY_NONE,skipnum,1,0,&cmd,NULL,
				    NULL); /* read prefs, NULL for default */
    while (mongoc_cursor_next (cursor, &doc)) {
      return true;
    }
    return false;
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
std::vector< std::string > LPushMongodbClient::queryToListJson(std::string db, std::string collectionName, std::map< std::string, std::string > params)
{
    bson_t cmd = LPushMongodbClient::excute(params);
    mongoc_collection_t * cll = LPushMongodbClient::excute(db.c_str(),collectionName.c_str());
    std::vector< std::string > result= queryFromCollectionToJson(&cmd,cll);
     mongoc_collection_destroy (cll);
    return result;
}

std::vector< std::string > 
LPushMongodbClient::queryToListJsonLimit
(std::string db, std::string collectionName,std::map< std::string, std::string > params, int page, int pageSize)
{
    std::vector< std::string > result;
    bson_t cmd = LPushMongodbClient::excute(params);
    mongoc_collection_t * cll = LPushMongodbClient::excute(db.c_str(),collectionName.c_str());
    char *str;
   cursor = mongoc_collection_find(cll,
				    MONGOC_QUERY_NONE,(page-1)*pageSize,pageSize,0,&cmd,NULL,
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
    mongoc_collection_destroy (cll);
    return result;
}

int64_t LPushMongodbClient::count(std::string db, std::string collectionName, std::map< std::string, std::string > params)
{
    int64_t result=0;
    bson_t cmd = LPushMongodbClient::excute(params);
    mongoc_collection_t * cll = LPushMongodbClient::excute(db.c_str(),collectionName.c_str());
    result = mongoc_collection_count(cll,MONGOC_QUERY_NONE,&cmd,0,0,NULL,&error);
    //fprintf (stderr, "Count Failure: %s\n", error.message);
    mongoc_collection_destroy (cll);
    return result;
}


int LPushMongodbClient::insertFromCollectionToJson(std::string db, std::string collectionName, std::map< std::string, std::string > params)
{
    bson_t cmd = LPushMongodbClient::excute(params);
    mongoc_collection_t * cll = LPushMongodbClient::excute(db.c_str(),collectionName.c_str());
    if (!mongoc_collection_insert (cll, MONGOC_INSERT_NONE, &cmd, NULL, &error)) {
	lp_error("%s ",error.message);
	mongoc_collection_destroy (cll);
	return ERROR_MONGODB_INSERT;
      }  
      mongoc_collection_destroy (cll);
      return 0;
}

int LPushMongodbClient::delFromCollectionToJson(std::string db, std::string collectionName, std::string oid)
{
    bson_t *cmd = bson_new();
    bson_oid_t  id;
    bson_oid_init_from_string(&id,oid.c_str());
    BSON_APPEND_OID(cmd, "_id", &id);
    mongoc_collection_t * cll = LPushMongodbClient::excute(db.c_str(),collectionName.c_str());
    if(!mongoc_collection_delete(cll, MONGOC_DELETE_NONE, cmd, NULL, &error))
    {
	lp_error("%s",error.message);
	bson_destroy(cmd);
	mongoc_collection_destroy (cll);
	return ERROR_MONGODB_DELETE;
    }
    bson_destroy(cmd);
    mongoc_collection_destroy (cll);
    return 0;
}
int LPushMongodbClient::delFromQuery(std::string db, std::string collectionName, std::map< std::string, std::string > params)
{
    bson_t cmd = LPushMongodbClient::excute(params);
    mongoc_collection_t * cll = LPushMongodbClient::excute(db.c_str(),collectionName.c_str());
    if(!mongoc_collection_delete(cll, MONGOC_DELETE_NONE, &cmd, NULL, &error))
    {
	lp_error("%s",error.message);
	mongoc_collection_destroy (cll);
	return ERROR_MONGODB_DELETE;
    }
    mongoc_collection_destroy (cll);
    return 0;
}


int LPushMongodbClient::updateFromCollectionToJson(std::string db, std::string collectionName,std::string oid,std::map<std::string,std::string> uparams)
{
  
    bson_oid_t _id;
    bson_oid_init_from_string(&_id,oid.c_str());
    bson_t *cmd = bson_new();
    BSON_APPEND_OID(cmd, "_id", &_id);
    bson_t updateDoc = LPushMongodbClient::excute(uparams);
    mongoc_collection_t * cll = LPushMongodbClient::excute(db.c_str(),collectionName.c_str());
    if(!mongoc_collection_update(cll, MONGOC_UPDATE_NONE, cmd,&updateDoc, NULL, &error))
    {
	lp_error("%s",error.message);
	bson_destroy(cmd);
	mongoc_collection_destroy (cll);
	return ERROR_MONGODB_UPDATE;
    }
    bson_destroy(cmd);
    mongoc_collection_destroy (cll);
    return 0;
}
bson_oid_t LPushMongodbClient::getOidByJsonStr(std::string json)
{
    bson_oid_t ret;
    bson_iter_t itr;
    bson_t *data = bson_new_from_json(
      (const uint8_t*)json.c_str(), json.size(), &error);
    if(!bson_iter_init_find(&itr,data,"_id"))
      {
	  lp_error("bson itr init error %s",error.message);
	  bson_destroy(data);
	  return ret;
      }
    bson_value_t  s;
    s = itr.value;
    if(s.value_type==7)
    ret = s.value.v_oid;
    bson_destroy(data);
    return ret;
}

std::map< std::string, std::string > LPushMongodbClient::jsonToMap(std::string json)
{
    std::map< std::string, std::string > map;
    bson_iter_t itr;
    bson_t *data = bson_new_from_json(
      (const uint8_t*)json.c_str(), json.size(), &error);
    bson_iter_init(&itr,data);
    while(bson_iter_next(&itr))
    {	
	bson_type_t type=bson_iter_type(&itr);
       lp_trace("bson itr type  %d",type);
       if(type == BSON_TYPE_UTF8){
       const char * key  =bson_iter_key(&itr);
       const char * value  =bson_iter_utf8(&itr,NULL);
       map.insert(std::make_pair(std::string(key),std::string(value)));
	}
	else if(type == BSON_TYPE_OID)
	{
	  const char * key  =bson_iter_key(&itr);
	  
	  const bson_oid_t   *ret;
	 
	  ret = bson_iter_oid(&itr);
	  std::string value;
	  for(int i=0;i<12;i++)
	  {
	     std::string rea = uint8To2Char(ret->bytes[i]);
	     value.append(rea.c_str());
	  }
	  //std::cout << value <<std::endl;
	  lp_trace("Mongo OID %s",value.c_str());
	  map.insert(std::make_pair(std::string(key),value));
	}else if(type == BSON_TYPE_INT32)
	{
	    const char * key  =bson_iter_key(&itr);
	    int32_t val = bson_iter_int32(&itr);
	    std::string value = IntToString(val);
	    map.insert(std::make_pair(std::string(key),value));
	}else if(type ==BSON_TYPE_INT64)
	{
	    const char * key  =bson_iter_key(&itr);
	    int64_t val = bson_iter_int64(&itr);
	    std::string value = LongToString(val);
	    map.insert(std::make_pair(std::string(key),value));
	}
    }
    
    return map;
}



void LPushMongodbClient::close()
{	
      if(cursor)
      mongoc_cursor_destroy (cursor);
    
      if(client)
      mongoc_client_destroy (client);

      mongoc_cleanup ();
}

  
}