#include <lpushAppSt.h>
#include <lpushSystemErrorDef.h>
#include <lpushLogger.h>


namespace lpush 
{
  
  LPushStSocket::LPushStSocket(st_netfd_t client_stfd)
{
    stfd = client_stfd;
    send_timeout = recv_timeout = ST_UTIME_NO_TIMEOUT;
    recv_bytes = send_bytes = 0;
}

LPushStSocket::~LPushStSocket()
{
}

bool LPushStSocket::is_never_timeout(int64_t timeout_us)
{
    return timeout_us == (int64_t)ST_UTIME_NO_TIMEOUT;
}

void LPushStSocket::set_recv_timeout(int64_t timeout_us)
{
    recv_timeout = timeout_us;
}

int64_t LPushStSocket::get_recv_timeout()
{
    return recv_timeout;
}

void LPushStSocket::set_send_timeout(int64_t timeout_us)
{
    send_timeout = timeout_us;
}

int64_t LPushStSocket::get_send_timeout()
{
    return send_timeout;
}

int64_t LPushStSocket::get_recv_bytes()
{
    return recv_bytes;
}

int64_t LPushStSocket::get_send_bytes()
{
    return send_bytes;
}

int LPushStSocket::read(void* buf, size_t size, ssize_t* nread)
{
    int ret = ERROR_SUCCESS;
    
    ssize_t nb_read = st_read(stfd, buf, size, recv_timeout);
    if (nread) {
        *nread = nb_read;
    }
    
    // On success a non-negative integer indicating the number of bytes actually read is returned
    // (a value of 0 means the network connection is closed or end of file is reached).
    // Otherwise, a value of -1 is returned and errno is set to indicate the error.
    if (nb_read <= 0) {
        // @see https://github.com/ossrs/srs/issues/200
        if (nb_read < 0 && errno == ETIME) {
            return ERROR_SOCKET_TIMEOUT;
        }
        
        if (nb_read == 0) {
            errno = ECONNRESET;
        }
        
        return ERROR_SOCKET_READ;
    }
    
    recv_bytes += nb_read;
    
    return ret;
}

int LPushStSocket::read_fully(void* buf, size_t size, ssize_t* nread)
{
    int ret = ERROR_SUCCESS;
    
    ssize_t nb_read = st_read_fully(stfd, buf, size, recv_timeout);
    if (nread) {
        *nread = nb_read;
    }
    
    // On success a non-negative integer indicating the number of bytes actually read is returned
    // (a value less than nbyte means the network connection is closed or end of file is reached)
    // Otherwise, a value of -1 is returned and errno is set to indicate the error.
    if (nb_read != (ssize_t)size) {
        // @see https://github.com/ossrs/srs/issues/200
        if (nb_read < 0 && errno == ETIME) {
            return ERROR_SOCKET_TIMEOUT;
        }
        
        if (nb_read >= 0) {
            errno = ECONNRESET;
        }
        
        return ERROR_SOCKET_READ_FULLY;
    }
    
    recv_bytes += nb_read;
    
    return ret;
}

int LPushStSocket::write(void* buf, size_t size, ssize_t* nwrite)
{
    int ret = ERROR_SUCCESS;
    
    ssize_t nb_write = st_write(stfd, buf, size, send_timeout);
    if (nwrite) {
        *nwrite = nb_write;
    }
    
    // On success a non-negative integer equal to nbyte is returned.
    // Otherwise, a value of -1 is returned and errno is set to indicate the error.
    if (nb_write <= 0) {
        // @see https://github.com/ossrs/srs/issues/200
        if (nb_write < 0 && errno == ETIME) {
            return ERROR_SOCKET_TIMEOUT;
        }
        
        return ERROR_SOCKET_WRITE;
    }
    
    send_bytes += nb_write;
    
    return ret;
}

int LPushStSocket::writev(const iovec *iov, int iov_size, ssize_t* nwrite)
{
    int ret = ERROR_SUCCESS;
    
    ssize_t nb_write = st_writev(stfd, iov, iov_size, send_timeout);
    if (nwrite) {
        *nwrite = nb_write;
    }
    
    // On success a non-negative integer equal to nbyte is returned.
    // Otherwise, a value of -1 is returned and errno is set to indicate the error.
    if (nb_write <= 0) {
        // @see https://github.com/ossrs/srs/issues/200
        if (nb_write < 0 && errno == ETIME) {
            return ERROR_SOCKET_TIMEOUT;
        }
        
        return ERROR_SOCKET_WRITE;
    }
    
    send_bytes += nb_write;
    
    return ret;
}

#ifdef __linux__
#include <sys/epoll.h>

bool lp_st_epoll_is_supported(void)
{
    struct epoll_event ev;

    ev.events = EPOLLIN;
    ev.data.ptr = NULL;
    /* Guaranteed to fail */
    epoll_ctl(-1, EPOLL_CTL_ADD, -1, &ev);

    return (errno != ENOSYS);
}
#endif

int lp_st_init()
{
    int ret = ERROR_SUCCESS;
    
#ifdef __linux__
    // check epoll, some old linux donot support epoll.
    if (!lp_st_epoll_is_supported()) {
        ret = ERROR_ST_SET_EPOLL;
        lp_error("epoll required on Linux. ret=%d", ret);
        return ret;
    }
#endif
    
    // Select the best event system available on the OS. In Linux this is
    // epoll(). On BSD it will be kqueue.
    if (st_set_eventsys(ST_EVENTSYS_ALT) == -1) {
        ret = ERROR_ST_SET_EPOLL;
        lp_error("st_set_eventsys use %s failed. ret=%d", st_get_eventsys_name(), ret);
        return ret;
    }
    lp_trace("st_set_eventsys to %s", st_get_eventsys_name());

    if(st_init() != 0){
        ret = ERROR_ST_INITIALIZE;
        lp_error("st_init failed. ret=%d", ret);
        return ret;
    }
    lp_trace("st_init success, use %s", st_get_eventsys_name());
    
    return ret;
}

void lp_close_stfd(st_netfd_t& stfd)
{
    if (stfd) {
        int fd = st_netfd_fileno(stfd);
        st_netfd_close(stfd);
        stfd = NULL;
        
        // st does not close it sometimes, 
        // close it manually.
        close(fd);
    }
}
}