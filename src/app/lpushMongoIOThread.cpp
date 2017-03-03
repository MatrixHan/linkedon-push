#include <lpushMongoIOThread.h>
#include <lpushLogger.h>
#include <lpushMongoClient.h>
#include <lpushSystemErrorDef.h>
#include <lpushSource.h>
#include <lpushJson.h>
using namespace std;
namespace lpush 
{
  
LPushMongoIOThread::LPushMongoIOThread()
{
    trd = new LPushReusableThread("mongoIOThread",this,350);
    mclient = new LPushMongodbClient(conf->mongodbConfig->url.c_str());
    mclient->initMongodbClient();
}

LPushMongoIOThread::~LPushMongoIOThread()
{
      SafeDelete(trd);
      SafeDelete(mclient);
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
	 st_usleep(350 * 1000);
	 continue;
      }
      MongoIOEntity * mie = NULL;
      if((ret = pop(&mie))!=ERROR_SUCCESS)
      {
	  ret = ERROR_QUEUE_POP;
      }
      if (ret == ERROR_SUCCESS) {
	
            ret = selectMongoHistoryWork(mie);
	    SafeDelete(mie);
        }
        if(ret == ERROR_MONGODB_RESET)
	{
	    continue;
	}
       if (ret != ERROR_SUCCESS) {
           
	    SafeDelete(mie);
            // we use no timeout to recv, should never got any error.
            trd->interrupt();
	    
            return ret;
        }
      st_usleep(15 * 1000);
    }
    return ret;
}

int LPushMongoIOThread::selectMongoHistoryWork(MongoIOEntity *mie)
{
  
    int ret = ERROR_SUCCESS;
    int64_t begint = st_utime();
    int64_t bt ,et;
    map<string,string> params;
    params.insert(make_pair("UserId",mie->userId));
    bool isExist = mclient->selectOneIsExist(mie->db,
							   mie->collectionName,params);
    if(!isExist)
    {
      return ERROR_MONGODB_RESET;
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
	 
	if((ret = selectMongoHistoryLimit(mie,params,1*index,50000*index))!=ERROR_SUCCESS)
	{
	      return ret;
	}
	isup = mclient->skipParamsIsExist(mie->db,
							   mie->collectionName,params,50000*index);
	index++;
       }
	if((ret = selectMongoHistoryLimit(mie,params,1*index,50000*index))!=ERROR_SUCCESS)
	{
	      return ret;
	}
    }
//     bt = st_utime();
//     if((ret = mclient->delFromQuery(mie->db,mie->collectionName,params))!=ERROR_SUCCESS)
//     {
// 	lp_warn("mongodb del from query error %d",ret);
// 	return ret;
//     }
//     et = st_utime();
//     lp_warn("current delFromQuery function run time %lld",(et-bt)/1000L);
    int64_t endt = st_utime();
    int internalt = endt-begint;
    lp_warn("current selectMongoHistoryWork function run time %lld",internalt/1000L);
    return 0;
}

int LPushMongoIOThread::selectMongoHistoryLimit(MongoIOEntity *mie, map< string, string > params, int page, int pageSize)
{
  
    int64_t bt ,et;
    int ret = ERROR_SUCCESS;
    LPushClient *client = LPushSource::instance(mie->userId,mie->appKey,mie->secreteKey);
    if(!client)
    {
	lp_warn("this user not online %d",ret);
	return ret;
    }
    bt = st_utime();
    vector<string> result = mclient->queryToListJsonLimit(mie->db,
							   mie->collectionName,params,page,pageSize);
    et = st_utime();
    lp_warn("current queryToListJsonLimit function run time %lld",(et-bt)/1000L);
	if(result.size()>0)
	{
	   bt = st_utime();
	  vector<string>::iterator itr = result.begin();
	  for(;itr!=result.end();++itr)
	  {
	      string json = *itr;
	      map<string,string> entity = mclient->jsonToMap(json);
	      LPushWorkerMessage lpwm(entity);
	      client->push(lpwm.copy());
	      mclient->delFromCollectionToJson(mie->db,mie->collectionName,entity["_oid"]);
	  }
	  et = st_utime();
	  lp_warn("current for function run time %lld",(et-bt)/1000L);
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
      queue.push_back(mie);
}


int LPushMongoIOThread::pop(MongoIOEntity **src)
{
       if(!(getLength()>0)){
	    lp_warn("mongodb queue size is 0");
	}
	MongoIOEntity *mie = NULL;
	std::vector<MongoIOEntity*>::iterator itr = queue.begin();
	mie = *itr;
	*src = mie->copy();
	itr=queue.erase(itr);
	SafeDelete(mie);
	return 0;
}

int LPushMongoIOThread::getLength()
{
    return queue.size();
}

}