#pragma once

#include <lpushCommon.h>
#include <lpushThread.h>
#include <lpushAppListener.h>
#include <lpushAppLpushConn.h>
using namespace std;

namespace lpush 
{
class LPushServer;
class LPushStreamListener;

class LPushSignalManager:public ILPushEndlessThreadHandler
{
private:
  /* Per-process pipe which is used as a signal queue. */
    /* Up to PIPE_BUF/sizeof(int) signals can be queued up. */
    int sig_pipe[2];
    st_netfd_t signal_read_stfd;
private:
    LPushServer *server;
    LPushEndlessThread *pthread;
public:
    LPushSignalManager(LPushServer *_server);
    virtual ~LPushSignalManager();
public:
    virtual int initializer();
    virtual int start();   
public:
    virtual int cycle();
    
public:
    static LPushSignalManager *instance;    
    static void signal_catcher(int signo);
};
  

class LPushStreamListener:virtual public ILPushTcpHandler
{
private:
  LPushTcpListener *tcpListener;
  LPushServer   * server;
  std::string  ip;
  int 		port;
public:
  LPushStreamListener(LPushServer *_server);
  virtual ~LPushStreamListener();
public:
    virtual int listen(std::string i, int p);
public:
    virtual int on_tcp_client(st_netfd_t stfd);
};
  
  
class LPushConnection;
class LPushServer
{
private: 
    int pid_fd;   
    vector<LPushConnection *> conns;   
    vector<LPushStreamListener *>  listeners;   
    LPushSignalManager *signalManager;        
    bool signal_reload ;
    bool signal_gracefully_quit;
    long long startTime;
    long long beforeTime;
    std::string serverKey;
public:
  LPushServer();
  virtual ~LPushServer();
public:
  virtual int initializer(); 
  virtual int initializer_st();  
  virtual int signal_init(); 
  virtual int signal_register(); 
  virtual int listen(); 
  virtual int cycle();
  virtual int do_cycle(); 
  virtual void dispose(); 
  virtual void destroy();
private:
  virtual void close_listeners();
  virtual void close_conns();
public:
  virtual int  hreatRedis();
public:
  virtual void remove(LPushConnection *conn);  
  virtual void on_signal(int signo);    
  virtual int acquire_pid_file();  
public:
  virtual int listen_lpush();  
  virtual int accept_client(st_netfd_t client_stfd);
};
}