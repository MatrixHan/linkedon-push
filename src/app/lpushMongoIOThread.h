#pragma once

#include <lpushCommon.h>
#include <lpushPThread.h>

namespace lpush 
{

struct MongoIOEntity
{
  std::string db;
  std::string collectionName;
  std::string appKey;
  std::string userId;
  std::string secreteKey;
  void setDate(std::string _db,std::string _collectionName
		    ,std::string _appKey,std::string _userId,std::string _secreteKey)
  {
    db = _db;
    collectionName = _collectionName;
    appKey = _appKey;
    userId = _userId;
    secreteKey = _secreteKey;
  }
  MongoIOEntity* copy()
  {
     MongoIOEntity * c = new MongoIOEntity();
     c->setDate(db,collectionName,appKey,userId,secreteKey);
     return c;
  }
};
class LPushMongodbClient;
class LPushRedisClient;
class LPushMongoIOThread:public  ILPushPThreadHandle
{
private:
  std::vector<MongoIOEntity*> queue;
    LPushPThread *trd;
    LPushMongodbClient *mclient;
    LPushRedisClient	*rclient;
    pthread_mutex_t     mutex;
    std::string 	serverTaskdbName;
public:
    LPushMongoIOThread();
    virtual ~LPushMongoIOThread();
public:
  virtual bool can_loop();
  
  virtual int cycle();
  
  virtual int do_cycle();
  
  virtual int selectMongoHistoryWork(MongoIOEntity *mie);
      
  virtual int selectMongoHistoryLimit(MongoIOEntity *mie,std::map<std::string,std::string> params,int page,int pageSize);
public:
    virtual int start();
   
    virtual void stop();
public:
  virtual void push(std::string _db,std::string _collectionName
		    ,std::string _appKey,std::string _userId,std::string _secreteKey);
private:
  virtual int pop(MongoIOEntity **src);
  
  virtual int getLength();
};
  
}