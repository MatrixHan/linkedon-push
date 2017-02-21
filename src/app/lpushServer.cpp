#include <lpushServer.h>
#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>
#include <lpushAppSt.h>

#include <lpushConnection.h>
#include <lpushSource.h>
#include <lpushJson.h>
#include <lpushRedis.h>
#include <lpushUtils.h>

namespace lpush 
{
  
  
LPushSignalManager* LPushSignalManager::instance = NULL;
LPushSignalManager::LPushSignalManager(LPushServer* _server)
{
	LPushSignalManager::instance = this;
	server = _server;
	sig_pipe[0] = sig_pipe[1] = -1;
	pthread = new LPushEndlessThread("signal", this);
	signal_read_stfd = NULL;
}

LPushSignalManager::~LPushSignalManager()
{
    lp_close_stfd(signal_read_stfd);
    
    if (sig_pipe[0] > 0) {
        ::close(sig_pipe[0]);
    }
    if (sig_pipe[1] > 0) {
        ::close(sig_pipe[1]);
    }
    
    SafeDelete(pthread);
}

int LPushSignalManager::initializer()
{
      int ret = ERROR_SUCCESS;
    
    /* Create signal pipe */
    if (pipe(sig_pipe) < 0) {
        ret = ERROR_SYSTEM_CREATE_PIPE;
        lp_error("create signal manager pipe failed. ret=%d", ret);
        return ret;
    }
    
    if ((signal_read_stfd = st_netfd_open(sig_pipe[0])) == NULL) {
        ret = ERROR_SYSTEM_CREATE_PIPE;
        lp_error("create signal manage st pipe failed. ret=%d", ret);
        return ret;
    }
    
    return ret;
}

int LPushSignalManager::start()
{
    /**
    * Note that if multiple processes are used (see below), 
    * the signal pipe should be initialized after the fork(2) call 
    * so that each process has its own private pipe.
    */
    struct sigaction sa;
    
    /* Install sig_catcher() as a signal handler */
    sa.sa_handler = LPushSignalManager::signal_catcher;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGHUP, &sa, NULL);
    
    sa.sa_handler = LPushSignalManager::signal_catcher;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    
    sa.sa_handler = LPushSignalManager::signal_catcher;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    
    sa.sa_handler = LPushSignalManager::signal_catcher;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR2, &sa, NULL);
    
    lp_trace("signal installed");
    
    return pthread->start();
}

int LPushSignalManager::cycle()
{
      int ret = ERROR_SUCCESS;

    int signo;
    
    /* Read the next signal from the pipe */
    st_read(signal_read_stfd, &signo, sizeof(int), ST_UTIME_NO_TIMEOUT);
    
    /* Process signal synchronously */
    server->on_signal(signo);
    
    return ret;
}

void LPushSignalManager::signal_catcher(int signo)
{
    int err;
    
    /* Save errno to restore it after the write() */
    err = errno;
    
    /* write() is reentrant/async-safe */
    int fd = LPushSignalManager::instance->sig_pipe[1];
    write(fd, &signo, sizeof(int));
    
    errno = err;
}

LPushStreamListener::LPushStreamListener(LPushServer* _server)
{
    tcpListener = NULL;
    server = _server;
}

LPushStreamListener::~LPushStreamListener()
{
    SafeDelete(tcpListener);
}

int LPushStreamListener::listen(string i, int p)
{
      int ret = ERROR_SUCCESS;
      
      ip = i;
      port = p;
      SafeDelete(tcpListener);
      tcpListener = new LPushTcpListener(this, ip, port);

      if ((ret = tcpListener->listen()) != ERROR_SUCCESS) {
	  lp_error("tcp listen failed. ret=%d", ret);
	  return ret;
	}
    
	lp_info("listen thread current_cid=%d, "
        "listen at port=%d, type=tcp, fd=%d started success, ep=%s:%d",
        log_context->get_id(), p, tcpListener->fd(), i.c_str(), p);

      lp_trace(" listen at tcp://%s:%d, fd=%d", ip.c_str(), port, tcpListener->fd());

      return ret;
}

int LPushStreamListener::on_tcp_client(st_netfd_t stfd)
{
	int ret = ERROR_SUCCESS;
    
    if ((ret = server->accept_client(stfd)) != ERROR_SUCCESS) {
        lp_warn("accept client error. ret=%d", ret);
        return ret;
    }

    return ret;
}


  
LPushServer::LPushServer()
{
    signal_reload = false;
    signal_gracefully_quit = false;
    startTime = beforeTime = 0L;
    signalManager = NULL;
}

LPushServer::~LPushServer()
{
    destroy();
}

  
  
int LPushServer::initializer()
{
    int ret = ERROR_SUCCESS;
    
    assert(!signalManager);
    signalManager = new LPushSignalManager(this);
    
    return ret;
}

int LPushServer::initializer_st()
{
    int ret = ERROR_SUCCESS;
    
    // init st
    if ((ret = lp_st_init()) != ERROR_SUCCESS) {
        lp_error("init st failed. ret=%d", ret);
        return ret;
    }
    
    // @remark, st alloc segment use mmap, which only support 32757 threads,
    // if need to support more, for instance, 100k threads, define the macro MALLOC_STACK.
    // TODO: FIXME: maybe can use "sysctl vm.max_map_count" to refine.
    if (conf->maxconnect > 32756) {
        ret = ERROR_ST_EXCEED_THREADS;
        lp_error("st mmap for stack allocation must <= %d threads, "
                  "@see Makefile of st for MALLOC_STACK, please build st manually by "
                  "\"make EXTRA_CFLAGS=-DMALLOC_STACK linux-debug\", ret=%d", ret);
        return ret;
    }
    
    // set current log id.
    log_context->generate_id();
    lp_trace("server main cid=%d", log_context->get_id());
    
    return ret;
}

void LPushServer::destroy()
{
    lp_warn("server begin destroy");
    
    dispose();
    
    if (pid_fd > 0) {
        ::close(pid_fd);
        pid_fd = -1;
    }
    
    SafeDelete(signalManager);
}

void LPushServer::dispose()
{
    close_listeners();
    close_conns();
    redis_client->hdel("serverList",serverKey);
}

void LPushServer::close_listeners()
{
      std::vector<LPushStreamListener*>::iterator it;
    for (it = listeners.begin(); it != listeners.end();) {
        LPushStreamListener* listener = *it;
        
        SafeDelete(listener);
        it = listeners.erase(it);
    }
}
void LPushServer::close_conns()
{
    std::vector<LPushConnection*>::iterator it;
    for(it = conns.begin();it != conns.end();)
    {
      LPushConnection *conn = *it;
      SafeDelete(conn);
      it = conns.erase(it);
    }
}


int LPushServer::listen()
{
    int ret = ERROR_SUCCESS;
    if ((ret = listen_lpush()) != ERROR_SUCCESS)
    {
      return ret;
    }
    return ret;
}

int LPushServer::listen_lpush()
{
    int ret = ERROR_SUCCESS;
    std::string ip = conf->ip;
    int port = conf->port;
    LPushStreamListener *lpsl = new LPushStreamListener(this);
    
    close_listeners();
    
    listeners.push_back(lpsl);
    
    if ((ret = lpsl->listen(ip, port)) != ERROR_SUCCESS) {
            lp_error("lpush stream listen at %s:%d failed. ret=%d", ip.c_str(), port, ret);
            return ret;
    }
       
    return ret;
}

int LPushServer::accept_client(st_netfd_t client_stfd)
{
     int ret = ERROR_SUCCESS;
    
    int fd = st_netfd_fileno(client_stfd);
    
    int max_connections = conf->maxconnect;
    if ((int)conns.size() >= max_connections) {
        lp_error("exceed the max connections, drop client: "
            "clients=%d, max=%d, fd=%d", (int)conns.size(), max_connections, fd);
            
        lp_close_stfd(client_stfd);
        
        return ret;
    }
    
    // avoid fd leak when fork.
    if (true) {
        int val;
        if ((val = fcntl(fd, F_GETFD, 0)) < 0) {
            ret = ERROR_SYSTEM_PID_GET_FILE_INFO;
            lp_error("fnctl F_GETFD error! fd=%d. ret=%#x", fd, ret);
            lp_close_stfd(client_stfd);
            return ret;
        }
        val |= FD_CLOEXEC;
        if (fcntl(fd, F_SETFD, val) < 0) {
            ret = ERROR_SYSTEM_PID_SET_FILE_INFO;
            lp_error("fcntl F_SETFD error! fd=%d ret=%#x", fd, ret);
            lp_close_stfd(client_stfd);
            return ret;
        }
    }
    
    LPushConnection *conn ;
    
    conn = new LPushConn(this, client_stfd);
    
    assert(conn);
    
    conns.push_back(conn);
    
     if ((ret = conn->start()) != ERROR_SUCCESS) {
        return ret;
    }
    lp_verbose("conn started success.");

    lp_verbose("accept client finished. conns=%d, ret=%d", (int)conns.size(), ret);
    
    return ret;
}



int LPushServer::cycle()
{
    int ret = ERROR_SUCCESS;

    ret = do_cycle();


    lp_warn("main cycle terminated, system quit normally.");
    dispose();
    lp_trace("srs terminated");
    exit(0);

    return ret;
}

int LPushServer::do_cycle()
{
    int ret = ERROR_SUCCESS;
    

    startTime=beforeTime = getCurrentTime();
    // the deamon thread, update the time cache
    while (true) {

        // the interval in config.
        int heartbeat_max_resolution = (int)(9.9 / 1000);
        
        // dynamic fetch the max.
        int temp_max = 1;
        temp_max = Max(temp_max, heartbeat_max_resolution);
        
	if((ret = hreatRedis())!=ERROR_SUCCESS)
	{
	    lp_warn("redis conn hreat error");
	}
	
        for (int i = 0; i < temp_max; i++) {
            st_usleep(100 * 1000);
            
            // gracefully quit for SIGINT or SIGTERM.
            if (signal_gracefully_quit) {
                lp_info("cleanup for gracefully terminate.");
                return ret;
            }
        
        
            
            // update the cache time
            if ((i % 1) == 0) {
                lp_info("update current time cache.");
            }
            if((ret = LPushSource::cycle_all(serverKey))!=ERROR_SUCCESS)
	    {
	       lp_info("source cycle all ret %d",ret);
	    }
            
            lp_info("server main thread loop");
        }
    }

    return ret;
    
}

int LPushServer::hreatRedis()
{
    int ret = ERROR_SUCCESS;
    long long nowtime = getCurrentTime();
    if(nowtime-beforeTime>5){
    int port = conf->port;
    char buf[5];
    sprintf(buf,"%d",port);
    serverKey = conf->localhost+":"+std::string(buf);
    std::string status = LPushSystemStatus::statusToJson(conns.size());
    redis_client->hset("serverList",serverKey,status);
    beforeTime = getCurrentTime();
    }
    return ret;
}


int LPushServer::signal_init()
{
    return signalManager->initializer();
}

int LPushServer::signal_register()
{
    return signalManager->start();
}


int LPushServer::acquire_pid_file()
{
    int ret = ERROR_SUCCESS;
  
    int mode = S_IRUSR | S_IWUSR |  S_IRGRP | S_IROTH;
    
    int fd;
    
    std::string pid_file = std::string(DEFAULT_PID_FILE_NAME);
    
    // open pid file
    if ((fd = ::open(pid_file.c_str(), O_WRONLY | O_CREAT, mode)) < 0) {
        ret = ERROR_SYSTEM_PID_ACQUIRE;
        lp_error("open pid file %s error, ret=%#x", pid_file.c_str(), ret);
        return ret;
    }
    
    // require write lock
    struct flock lock;

    lock.l_type = F_WRLCK; // F_RDLCK, F_WRLCK, F_UNLCK
    lock.l_start = 0; // type offset, relative to l_whence
    lock.l_whence = SEEK_SET;  // SEEK_SET, SEEK_CUR, SEEK_END
    lock.l_len = 0;
    
    if (fcntl(fd, F_SETLK, &lock) < 0) {
        if(errno == EACCES || errno == EAGAIN) {
            ret = ERROR_SYSTEM_PID_ALREADY_RUNNING;
            lp_error("lp is already running! ret=%#x", ret);
            return ret;
        }
        
        ret = ERROR_SYSTEM_PID_LOCK;
        lp_error("require lock for file %s error! ret=%#x", pid_file.c_str(), ret);
        return ret;
    }

    // truncate file
    if (ftruncate(fd, 0) < 0) {
        ret = ERROR_SYSTEM_PID_TRUNCATE_FILE;
        lp_error("truncate pid file %s error! ret=%#x", pid_file.c_str(), ret);
        return ret;
    }

    int pid = (int)getpid();
    
    // write the pid
    char buf[512];
    snprintf(buf, sizeof(buf), "%d", pid);
    if (write(fd, buf, strlen(buf)) != (int)strlen(buf)) {
        ret = ERROR_SYSTEM_PID_WRITE_FILE;
        lp_error("write our pid error! pid=%d file=%s ret=%#x", pid, pid_file.c_str(), ret);
        return ret;
    }

    // auto close when fork child process.
    int val;
    if ((val = fcntl(fd, F_GETFD, 0)) < 0) {
        ret = ERROR_SYSTEM_PID_GET_FILE_INFO;
        lp_error("fnctl F_GETFD error! file=%s ret=%#x", pid_file.c_str(), ret);
        return ret;
    }
    val |= FD_CLOEXEC;
    if (fcntl(fd, F_SETFD, val) < 0) {
        ret = ERROR_SYSTEM_PID_SET_FILE_INFO;
        lp_error("fcntl F_SETFD error! file=%s ret=%#x", pid_file.c_str(), ret);
        return ret;
    }
    
    lp_trace("write pid=%d to %s success!", pid, pid_file.c_str());
    pid_fd = fd;
    
    return ret;
}


void LPushServer::remove(LPushConnection* conn)
{
    std::vector<LPushConnection*>::iterator it = std::find(conns.begin(), conns.end(), conn);
    
    // removed by destroy, ignore.
    if (it == conns.end()) {
        lp_warn("server moved connection, ignore.");
        return;
    }
    
    conns.erase(it);
    
    lp_info("conn removed. conns=%d", (int)conns.size());
    
    // all connections are created by server,
    // so we free it here.
    SafeDelete(conn);
}

void LPushServer::on_signal(int signo)
{
      if (signo == SIGHUP) {
        signal_reload = true;
        return;
    }
    
    if (signo == SIGINT || signo == SIGUSR2) {
        lp_trace("user terminate program");
        exit(0);
        return;
    }
    
    if (signo == SIGTERM && !signal_gracefully_quit) {
        lp_trace("user terminate program, gracefully quit.");
        signal_gracefully_quit = true;
        return;
    }
}

}