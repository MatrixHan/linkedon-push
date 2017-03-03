#include <lpushMongoIOThread.h>
#include <lpushLogger.h>
#include <lpushMongoClient.h>
#include <lpushSystemErrorDef.h>
#include <lpushSource.h>
#include <lpushJson.h>
#include <lpushRedis.h>

#define printf(p,...)
using namespace std;
namespace lpush 
{
  
LPushMongoIOThread::LPushMongoIOThread()
{
    trd = new LPushPThread(this,"mongoIOThread",350);
    mclient = new LPushMongodbClient(conf->mongodbConfig->url.c_str());
    mclient->initMongodbClient();
    rclient = new LPushRedisClient(conf->redisConfig->host.c_str(),conf->redisConfig->port);
    rclient->initRedis();
    rclient->auth(conf->redisConfig->pass);
    rclient->selectDb(conf->redisConfig->db);
    int port = conf->port;
    char buf[5];
    sprintf(buf,"%d",port);
    string pfs = conf->localhost+":"+std::string(buf);
    serverTaskdbName = conf->task_prefix+pfs;
    pthread_mutex_init(&mutex,NULL);
}

LPushMongoIOThread::~LPushMongoIOThread()
{
      SafeDelete(trd);
      SafeDelete(mclient);
      rclient->closeRedis();
      SafeDelete(rclient);
      if(getLength()>0)
     {
       std::vector<MongoIOEntity*>::iterator itr = queue.begin();
       for(;itr!=queue.end();)
       {
	  MongoIOEntity *lpwm;
	  lpwm = *itr;
	  itr=queue.erase(itr);
	  SafeDelete(lpwm);
      }
     }
}


bool LPushMongoIOThread::can_loop()
{
    return !queue.empty();
}

int LPushMongoIOThread::cycle()
{
    int ret = ERROR_SUCCESS;
    while(true)
    {
      if(!can_loop())
      {
	 usleep(350 * 1000);
	 continue;
      }
      ret = do_cycle();
      printf("mongodb work queue size %d",getLength());
       if (ret != ERROR_SUCCESS) {
           
            // we use no timeout to recv, should never got any error.
            trd->stop();
	    
            return ret;
        }
     // usleep(10 * 1000);
    }
    return ret;
}

int LPushMongoIOThread::do_cycle()
{
       int ret = ERROR_SUCCESS;
       MongoIOEntity * mie = NULL;
      int  callindex = 1;
      while(callindex%10!=0)
      {		
	  callindex++;
	  if(!can_loop())
	    break;
	  ret = pop(&mie);
	  ret = selectMongoHistoryWork(mie);
	  SafeDelete(mie);
      }
     return ret;
}


int LPushMongoIOThread::selectMongoHistoryWork(MongoIOEntity *mie)
{
  
    int ret = ERROR_SUCCESS;
    int64_t bt ,et;
    map<string,string> params;
    params.insert(make_pair("UserId",mie->userId));
    bool isExist = mclient->selectOneIsExist(mie->db,
							   mie->collectionName,params);
    if(!isExist)
    {
      return 0;
    }
    bool isup =  mclient->skipParamsIsExist(mie->db,
							   mie->collectionName,params,50000);
    if(!isup){
    if((ret = selectMongoHistoryLimit(mie,params,1,50000))!=ERROR_SUCCESS)
    {
	return ret;
    }
    }else
    {
      int index = 1;
       while(isup)
       {
	 
	if((ret = selectMongoHistoryLimit(mie,params,1,50000))!=ERROR_SUCCESS)
	{
	      return ret;
	}
	isup = mclient->skipParamsIsExist(mie->db,
							   mie->collectionName,params,50000);
	index++;
       }
	if((ret = selectMongoHistoryLimit(mie,params,1,50000))!=ERROR_SUCCESS)
	{
	      return ret;
	}
    }
//     bt = st_utime();
//     if((ret = mclient->delFromQuery(mie->db,mie->collectionName,params))!=ERROR_SUCCESS)
//     {
// 	printf("mongodb del from query error %d",ret);
// 	return ret;
//     }
//     et = st_utime();
//     printf("current delFromQuery function run time %lld",(et-bt)/1000L);

    return 0;
}

int LPushMongoIOThread::selectMongoHistoryLimit(MongoIOEntity *mie, map< string, string > params, int page, int pageSize)
{
  
    int64_t bt ,et;
    int ret = ERROR_SUCCESS;
   
    bt = st_utime();
    vector<string> result = mclient->queryToListJsonLimit(mie->db,
							   mie->collectionName,params,page,pageSize);
    et = st_utime();
    printf("current queryToListJsonLimit function run time %lld",(et-bt)/1000L);
	if(result.size()>0)
	{
	   bt = st_utime();
	  vector<string>::iterator itr = result.begin();
	  for(;itr!=result.end();++itr)
	  {
	      string json = *itr;
	      map<string,string> entity = mclient->jsonToMap(json);
	      LPushWorkerMessage lpwm(entity);
	      rclient->lPushForList(serverTaskdbName,lpwm.toAllString());
	      mclient->delFromCollectionToJson(mie->db,mie->collectionName,entity["_id"]);
	  }
	  et = st_utime();
	  printf("current for function run time %lld",(et-bt)/1000L);
	}
	return ret;
}

int LPushMongoIOThread::start()
{
  return trd->start();
}

void LPushMongoIOThread::stop()
{
   trd->stop();
}

void LPushMongoIOThread::push(string _db, string _collectionName, string _appKey, string _userId, string _secreteKey)
{
      MongoIOEntity *mie = new MongoIOEntity();
      mie->setDate(_db,_collectionName,_appKey,_userId,_secreteKey);
      pthread_mutex_lock(&mutex);
      queue.push_back(mie);
      pthread_mutex_unlock(&mutex);
}


int LPushMongoIOThread::pop(MongoIOEntity **src)
{
	
       if(!(getLength()>0)){
	    printf("mongodb queue size is 0");
	}
	pthread_mutex_lock(&mutex);
	MongoIOEntity *mie = NULL;
	std::vector<MongoIOEntity*>::iterator itr = queue.begin();
	mie = *itr;
	*src = mie->copy();
	itr=queue.erase(itr);
	SafeDelete(mie);
	pthread_mutex_unlock(&mutex);
	return 0;
}

int LPushMongoIOThread::getLength()
{	
    pthread_mutex_lock(&mutex);
    int size = queue.size();
    pthread_mutex_unlock(&mutex);
    return  size;
}

}