#pragma once


#include <lpushThread.h>
#include <lpushAppSt.h>


namespace lpush 
{
  


/**
* the udp packet handler.
*/
class ILPushUdpHandler
{
public:
    ILPushUdpHandler();
    virtual ~ILPushUdpHandler();
public:
    /**
     * when fd changed, for instance, reload the listen port,
     * notify the handler and user can do something.
     */
    virtual int on_stfd_change(st_netfd_t fd);
public:
    /**
    * when udp listener got a udp packet, notice server to process it.
    * @param type, the client type, used to create concrete connection,
    *       for instance RTMP connection to serve client.
    * @param from, the udp packet from address.
    * @param buf, the udp packet bytes, user should copy if need to use.
    * @param nb_buf, the size of udp packet bytes.
    * @remark user should never use the buf, for it's a shared memory bytes.
    */
    virtual int on_udp_packet(sockaddr_in* from, char* buf, int nb_buf) = 0;
};

/**
* the tcp connection handler.
*/
class ILPushTcpHandler
{
public:
    ILPushTcpHandler();
    virtual ~ILPushTcpHandler();
public:
    /**
    * when got tcp client.
    */
    virtual int on_tcp_client(st_netfd_t stfd) = 0;
};

/**
* bind udp port, start thread to recv packet and handler it.
*/
class LPushUdpListener : public ILPushReusableThreadHandler
{
private:
    int _fd;
    st_netfd_t _stfd;
    LPushReusableThread* pthread;
private:
    char* buf;
    int nb_buf;
private:
    ILPushUdpHandler* handler;
    std::string ip;
    int port;
public:
    LPushUdpListener(ILPushUdpHandler* h, std::string i, int p);
    virtual ~LPushUdpListener();
public:
    virtual int fd();
    virtual st_netfd_t stfd();
public:
    virtual int listen();
// interface ILPushReusableThreadHandler.
public:
    virtual int cycle();
};

/**
* bind and listen tcp port, use handler to process the client.
*/
class LPushTcpListener : public ILPushReusableThreadHandler
{
private:
    int _fd;
    st_netfd_t _stfd;
    LPushReusableThread* pthread;
private:
    ILPushTcpHandler* handler;
    std::string ip;
    int port;
public:
    LPushTcpListener(ILPushTcpHandler* h, std::string i, int p);
    virtual ~LPushTcpListener();
public:
    virtual int fd();
public:
    virtual int listen();
// interface ILPushReusableThreadHandler.
public:
    virtual int cycle();
};

  
}