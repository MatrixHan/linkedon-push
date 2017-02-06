#include <lpushSocket.h>
#include <lpushSystemErrorDef.h>

namespace lpush {
  
LPushSocket::LPushSocket(st_netfd_t client_stfd)
{
	stfd = client_stfd;
	send_timeout = recv_timeout = SOCKET_TIMEOUT;
	recv_bytes = send_bytes = 0;
	start_time_ms = getCurrentTime();
}
LPushSocket::~LPushSocket()
{

}

  

void LPushSocket::set_recv_timeout(int64_t timeout_us)
{
	recv_timeout = timeout_us;
}

int64_t LPushSocket::get_recv_timeout()
{
	return recv_timeout;
}

void LPushSocket::set_send_timeout(int64_t timeout_us)
{
	send_timeout = timeout_us;
}

int64_t LPushSocket::get_recv_bytes()
{
	return recv_bytes;
}

int64_t LPushSocket::get_send_bytes()
{
	return send_bytes;
}

int LPushSocket::get_recv_kbps()
{
	int64_t diff_ms = getCurrentTime() - start_time_ms;
	
	if (diff_ms <= 0) {
		return 0;
	}
	
	return recv_bytes * 8 / diff_ms;
}

int LPushSocket::get_send_kbps()
{
	int64_t diff_ms = getCurrentTime() - start_time_ms;
	
	if (diff_ms <= 0) {
		return 0;
	}
	
	return send_bytes * 8 / diff_ms;
}

int64_t LPushSocket::get_send_timeout()
{
    return send_timeout;
}

int LPushSocket::read(const void* buf, size_t size, ssize_t* nread)
{
    int ret = ERROR_SUCCESS;
    
    *nread = st_read(stfd, (void*)buf, size, recv_timeout);
    
    // On success a non-negative integer indicating the number of bytes actually read is returned 
    // (a value of 0 means the network connection is closed or end of file is reached).
    if (*nread <= 0) {
		if (errno == ETIME) {
			return ERROR_SOCKET_TIMEOUT;
		}
		
        if (*nread == 0) {
            errno = ECONNRESET;
        }
        
        return ERROR_SOCKET_READ;
    }
    
    recv_bytes += *nread;
        
    return ret;
}

int LPushSocket::read_fully(const void* buf, size_t size, ssize_t* nread)
{
    int ret = ERROR_SUCCESS;
    
    *nread = st_read_fully(stfd, (void*)buf, size, recv_timeout);
    
    // On success a non-negative integer indicating the number of bytes actually read is returned 
    // (a value less than nbyte means the network connection is closed or end of file is reached)
    if (*nread != (ssize_t)size) {
		if (errno == ETIME) {
			return ERROR_SOCKET_TIMEOUT;
		}
		
        if (*nread >= 0) {
            errno = ECONNRESET;
        }
        
        return ERROR_SOCKET_READ_FULLY;
    }
    
    recv_bytes += *nread;
    
    return ret;
}

int LPushSocket::write(const void* buf, size_t size, ssize_t* nwrite)
{
    int ret = ERROR_SUCCESS;
    
    *nwrite = st_write(stfd, (void*)buf, size, send_timeout);
    
    if (*nwrite <= 0) {
		if (errno == ETIME) {
			return ERROR_SOCKET_TIMEOUT;
		}
		
        return ERROR_SOCKET_WRITE;
    }
    
    send_bytes += *nwrite;
        
    return ret;
}

int LPushSocket::writev(const iovec *iov, int iov_size, ssize_t* nwrite)
{
    int ret = ERROR_SUCCESS;
    
    *nwrite = st_writev(stfd, iov, iov_size, send_timeout);
    
    if (*nwrite <= 0) {
		if (errno == ETIME) {
			return ERROR_SOCKET_TIMEOUT;
		}
		
        return ERROR_SOCKET_WRITE;
    }
    
    send_bytes += *nwrite;
    
    return ret;
}

}
