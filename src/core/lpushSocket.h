#pragma once 

#include <lpushCommon.h>
#include <lpushUtils.h>
#include <errno.h>
#include <sys/uio.h>

#define SOCKET_TIMEOUT 30

namespace lpush  
{
  
class ILPushBufferReader
{
public:
  ILPushBufferReader();
  virtual ~ILPushBufferReader();
public:
  virtual int read(const void* buf, size_t size, ssize_t* nread)=0;
};

class ILPushBufferWriter
{
public:
  ILPushBufferWriter();
  virtual ~ILPushBufferWriter();
public:
  virtual int write(const void* buf, size_t size, ssize_t* nwrite)=0;
  virtual int writev(const iovec *iov, int iov_size, ssize_t* nwrite)=0;
};
  
class LPushSocket: public virtual ILPushBufferReader,public virtual ILPushBufferWriter
{
private:
	int64_t 	recv_timeout;
	int64_t 	send_timeout;
	int64_t 	recv_bytes;
	int64_t 	send_bytes;
	int64_t 	start_time_ms;
	st_netfd_t stfd;
public:

  LPushSocket(st_netfd_t client_stfd);
  ~LPushSocket();
  
  virtual void 		set_recv_timeout(int64_t timeout_us);
  virtual int64_t 	get_recv_timeout();
  virtual void 		set_send_timeout(int64_t timeout_us);
  virtual int64_t 	get_recv_bytes();
  virtual int64_t 	get_send_bytes();
  virtual int64_t 	get_send_timeout();
  virtual int 		get_recv_kbps();
  virtual int 		get_send_kbps();
public:
    virtual int read(const void* buf, size_t size, ssize_t* nread);
    virtual int read_fully(const void* buf, size_t size, ssize_t* nread);
    virtual int write(const void* buf, size_t size, ssize_t* nwrite);
    virtual int writev(const iovec *iov, int iov_size, ssize_t* nwrite);
};
  
}