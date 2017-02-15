#pragma once

#include <lpushCommon.h>
#include <st.h>
namespace lpush 
{
  

  
class LPushHandshakeMessage;
class LPushSource;
class LPushClient
{
private:
  st_netfd_t cstfd;
  std::string appId;
  std::string screteKey;
  std::string userId;
private:
  LPushSource *source;
public:
  LPushClient(st_netfd_t cstfd,LPushSource *lpsource,LPushHandshakeMessage *message);
  virtual ~LPushClient();
};
  
  
class LPushSource
{
private:
  static std::map<st_netfd_t,LPushSource*> sources;
public:
  LPushSource();
  virtual ~LPushSource();
public:
  static LPushSource * create(st_netfd_t stfd);
  static LPushSource * instance(st_netfd_t stfd);
};
  
}