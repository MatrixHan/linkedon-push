#include <lpushMongoIOThread.h>
#include <lpushLogger.h>
#include <lpushMongoClient.h>
#include <lpushSystemErrorDef.h>
#include <lpushSource.h>
#include <lpushJson.h>
#include <lpushRedis.h>
#include <lpushMongoPool.h>

#define printf(p,...)
using namespace std;
namespace lpush 
{
  
LPushMongoIOThread::LPushMongoIOThread()
{
    trd = new LPushReusableThread("mongoIOThread",this,350);
    mclient = new LPushMongodbClient();
    mclient->initMongodbClient(conf->mongodbConfig->url.c_str());
    //rclient = new LPushRedisClient(conf->redisConfig->host.c_str(),conf->redisConfig->port);
    //rclient->initRedis();
    //rclient->auth(conf->redisConfig->pass);
    //rclient->selectDb(conf->redisConfig->db);
    int port = conf->port;
    char buf[5];
    sprintf(buf,"%d",port);
    string pfs = conf->localhost+":"+std::string(buf);
    serverTaskdbName = conf->task_prefix+pfs;
    //pthread_mutex_init(&mutex,NULL);
}

LPushMongoIOThread::~LPushMongoIOThread()
{
      SafeDelete(trd);
      mclient->closePool();
      SafeDelete(mclient);
      //rclient->closeRedis();
      //SafeDelete(rclient);
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
	 st_usleep(1000 * 1000L);
	 continue;
      }
      ret = do_cycle();
      //printf("mongodb work queue size %d",getLength());
       if (ret != ERROR_SUCCESS) {
           
            // we use no timeout to recv, should never got any error.
            trd->stop();
	    
            return ret;
        }
      st_usleep(20 * 1000L);
    }
    return ret;
}

int LPushMongoIOThread::do_cycle()
{
       int ret = ERROR_SUCCESS;
       MongoIOEntity * mie = NULL;
	  if(!can_loop())
	    return ret;
	  ret = pop(&mie);
	  ret = selectMongoHistoryWork(mie);
	  SafeDelete(mie);
	  if(ret == ERROR_OBJECT_NOT_EXIST)
	  {
	     ret = ERROR_SUCCESS;
	  }
     return ret;
}


int LPushMongoIOThread::selectMongoHistoryWork(MongoIOEntity *mie)
{
  
    int ret = ERROR_SUCCESS;
    //int64_t bt ,et;
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
     //bt = st_utime();
//      if((ret = mclient->delFromQuery(mie->db,mie->collectionName,params))!=ERROR_SUCCESS)
//      {
//  	lp_info("mongodb del from query error %d",ret);
//  	return ret;
//      }
     //et = st_utime();
     //lp_info("current delFromQuery function run time %lld",(et-bt)/1000L);

    return 0;
}

int LPushMongoIOThread::selectMongoHistoryLimit(MongoIOEntity *mie, map< string, string > params, int page, int pageSize)
{
  
    //int64_t bt ,et;
    int ret = ERROR_SUCCESS;
   
    LPushClient * client = LPushSource::instance(mie->userId,mie->appKey,mie->secreteKey);
    
    if(!client)
    {
      ret = ERROR_OBJECT_NOT_EXIST;
      return ret;
    }
    
    //bt = st_utime();
    vector<string> result = mclient->queryToListJsonLimit(mie->db,
							   mie->collectionName,params,page,pageSize);
    //et = st_utime();
    //lp_info("current queryToListJsonLimit function run time %lld",(et-bt)/1000L);
    //std::cout << "current queryToListJsonLimit function run time "<< (et-bt)/1000L <<std::endl;
	if(result.size()>0)
	{
	   //bt = st_utime();
	  vector<string>::iterator itr = result.begin();
	  for(;itr!=result.end();++itr)
	  {
	      string json = *itr;
	      map<string,string> entity = mclient->jsonToMap(json);
	      LPushWorkerMessage lpwm(entity);
	      //rclient->lPushForList(serverTaskdbName,lpwm.toAllString());
	      client->push(lpwm.copy());
	  }
	  //et = st_utime();
	  //lp_info("current for function run time %lld",(et-bt)/1000L);
	 //std::cout << "current for function run time "<< (et-bt)/1000L <<std::endl;
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

void LPushMongoIOThread::push(MongoIOEntity *mie)
{
     // pthread_mutex_lock(&mutex);
      queue.push_back(mie);
      //pthread_mutex_unlock(&mutex);
}


int LPushMongoIOThread::pop(MongoIOEntity **src)
{
	
       if(!(getLength()>0)){
	    printf("mongodb queue size is 0");
	}
	//pthread_mutex_lock(&mutex);
	MongoIOEntity *mie = NULL;
	std::vector<MongoIOEntity*>::iterator itr = queue.begin();
	mie = *itr;
	*src = mie->copy();
	itr=queue.erase(itr);
	SafeDelete(mie);
	//pthread_mutex_unlock(&mutex);
	return 0;
}
int LPushMongoIOThread::findPop(MongoIOEntity *mie)
{
	//pthread_mutex_lock(&mutex);
	
	std::vector<MongoIOEntity*>::iterator itr = std::find(queue.begin(),queue.end(),mie);
	if(itr == queue.end())
	{
	  //lp_info("not found it mie");
	  return -2;
	}
	queue.erase(itr);
	SafeDelete(mie);
	//pthread_mutex_unlock(&mutex);
	return 0;
}


int LPushMongoIOThread::getLength()
{	
   // pthread_mutex_lock(&mutex);
    int size = queue.size();
    //pthread_mutex_unlock(&mutex);
    return  size;
}

}