#pragma once 
#include <lpushCommon.h>

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
  

class ILPushProtocolStatistic
{
public:
    ILPushProtocolStatistic();
    virtual ~ILPushProtocolStatistic();
// for protocol
public:
    /**
    * get the total recv bytes over underlay fd.
    */
    virtual int64_t get_recv_bytes() = 0;
    /**
    * get the total send bytes over underlay fd.
    */
    virtual int64_t get_send_bytes() = 0;
};

/**
* the reader for the protocol to read from whatever channel.
*/
class ILPushProtocolReader : public virtual ILPushBufferReader, public virtual ILPushProtocolStatistic
{
public:
    ILPushProtocolReader();
    virtual ~ILPushProtocolReader();
// for protocol
public:
    /**
    * set the recv timeout in us, recv will error when timeout.
    * @remark, if not set, use ST_UTIME_NO_TIMEOUT, never timeout.
    */
    virtual void set_recv_timeout(int64_t timeout_us) = 0;
    /**
    * get the recv timeout in us.
    */
    virtual int64_t get_recv_timeout() = 0;
// for handshake.
public:
    /**
    * read specified size bytes of data
    * @param nread, the actually read size, NULL to ignore.
    */
    virtual int read_fully(void* buf, size_t size, ssize_t* nread) = 0;
};

/**
* the writer for the protocol to write to whatever channel.
*/
class ILPushProtocolWriter : public virtual ILPushBufferWriter, public virtual ILPushProtocolStatistic
{
public:
    ILPushProtocolWriter();
    virtual ~ILPushProtocolWriter();
// for protocol
public:
    /**
    * set the send timeout in us, send will error when timeout.
    * @remark, if not set, use ST_UTIME_NO_TIMEOUT, never timeout.
    */
    virtual void set_send_timeout(int64_t timeout_us) = 0;
    /**
    * get the send timeout in us.
    */
    virtual int64_t get_send_timeout() = 0;
};

/**
* the reader and writer.
*/
class ILPushProtocolReaderWriter : public virtual ILPushProtocolReader, public virtual ILPushProtocolWriter
{
public:
    ILPushProtocolReaderWriter();
    virtual ~ILPushProtocolReaderWriter();
// for protocol
public:
    /**
    * whether the specified timeout_us is never timeout.
    */
    virtual bool is_never_timeout(int64_t timeout_us) = 0;
};

}