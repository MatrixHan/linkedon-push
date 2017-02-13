#include <lpushAppListener.h>
#include <lpushSystemErrorDef.h>
#include <lpushLogger.h>

namespace lpush 
{
  // set the max packet size.
#define LPUSH_UDP_MAX_PACKET_SIZE 65535

// sleep in ms for udp recv packet.
#define LPUSH_UDP_PACKET_RECV_CYCLE_INTERVAL_MS 0

// nginx also set to 512
#define SERVER_LISTEN_BACKLOG 512

ILPushUdpHandler::ILPushUdpHandler()
{
}

ILPushUdpHandler::~ILPushUdpHandler()
{
}

int ILPushUdpHandler::on_stfd_change(st_netfd_t /*fd*/)
{
    return ERROR_SUCCESS;
}

ILPushTcpHandler::ILPushTcpHandler()
{
}

ILPushTcpHandler::~ILPushTcpHandler()
{
}

LPushUdpListener::LPushUdpListener(ILPushUdpHandler* h, string i, int p)
{
    handler = h;
    ip = i;
    port = p;

    _fd = -1;
    _stfd = NULL;

    nb_buf = LPUSH_UDP_MAX_PACKET_SIZE;
    buf = new char[nb_buf];

    pthread = new LPushReusableThread("udp", this);
}

LPushUdpListener::~LPushUdpListener()
{
    // close the stfd to trigger thread to interrupted.
    lp_close_stfd(_stfd);

    pthread->stop();
    SafeDelete(pthread);
    
    // st does not close it sometimes, 
    // close it manually.
    close(_fd);

    SafeDeleteArray(buf);
}

int LPushUdpListener::fd()
{
    return _fd;
}

st_netfd_t LPushUdpListener::stfd()
{
    return _stfd;
}

int LPushUdpListener::listen()
{
    int ret = ERROR_SUCCESS;
    
    if ((_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        ret = ERROR_SOCKET_CREATE;
        lp_error("create linux socket error. ip=%s, port=%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    lp_verbose("create linux socket success. ip=%s, port=%d, fd=%d", ip.c_str(), port, _fd);
    
    int reuse_socket = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket, sizeof(int)) == -1) {
        ret = ERROR_SOCKET_SETREUSE;
        lp_error("setsockopt reuse-addr error. ip=%s, port=%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    lp_verbose("setsockopt reuse-addr success. ip=%s, port=%d, fd=%d", ip.c_str(), port, _fd);
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (bind(_fd, (const sockaddr*)&addr, sizeof(sockaddr_in)) == -1) {
        ret = ERROR_SOCKET_BIND;
        lp_error("bind socket error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    lp_verbose("bind socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);
    
    if ((_stfd = st_netfd_open_socket(_fd)) == NULL){
        ret = ERROR_ST_OPEN_SOCKET;
        lp_error("st_netfd_open_socket open socket failed. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    lp_verbose("st open socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);
    
    if ((ret = pthread->start()) != ERROR_SUCCESS) {
        lp_error("st_thread_create listen thread error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    lp_verbose("create st listen thread success, ep=%s:%d", ip.c_str(), port);

    return ret;
}

int LPushUdpListener::cycle()
{
    int ret = ERROR_SUCCESS;

    // TODO: FIXME: support ipv6, @see man 7 ipv6
    sockaddr_in from;
    int nb_from = sizeof(sockaddr_in);
    int nread = 0;

    if ((nread = st_recvfrom(_stfd, buf, nb_buf, (sockaddr*)&from, &nb_from, ST_UTIME_NO_TIMEOUT)) <= 0) {
        lp_warn("ignore recv udp packet failed, nread=%d", nread);
        return ret;
    }
    
    if ((ret = handler->on_udp_packet(&from, buf, nread)) != ERROR_SUCCESS) {
        lp_warn("handle udp packet failed. ret=%d", ret);
        return ret;
    }

    if (LPUSH_UDP_PACKET_RECV_CYCLE_INTERVAL_MS > 0) {
        st_usleep(LPUSH_UDP_PACKET_RECV_CYCLE_INTERVAL_MS * 1000);
    }

    return ret;
}

LPushTcpListener::LPushTcpListener(ILPushTcpHandler* h, std::string i, int p)
{
    handler = h;
    ip = i;
    port = p;

    _fd = -1;
    _stfd = NULL;

    pthread = new LPushReusableThread("tcp", this);
}

LPushTcpListener::~LPushTcpListener()
{
    // close the stfd to trigger thread to interrupted.
    lp_close_stfd(_stfd);

    pthread->stop();
    SafeDelete(pthread);
    
    // st does not close it sometimes, 
    // close it manually.
    close(_fd);
}

int LPushTcpListener::fd()
{
    return _fd;
}

int LPushTcpListener::listen()
{
    int ret = ERROR_SUCCESS;
    
    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ret = ERROR_SOCKET_CREATE;
        lp_error("create linux socket error. port=%d, ret=%d", port, ret);
        return ret;
    }
    lp_verbose("create linux socket success. port=%d, fd=%d", port, _fd);
    
    int reuse_socket = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket, sizeof(int)) == -1) {
        ret = ERROR_SOCKET_SETREUSE;
        lp_error("setsockopt reuse-addr error. port=%d, ret=%d", port, ret);
        return ret;
    }
    lp_verbose("setsockopt reuse-addr success. port=%d, fd=%d", port, _fd);
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (bind(_fd, (const sockaddr*)&addr, sizeof(sockaddr_in)) == -1) {
        ret = ERROR_SOCKET_BIND;
        lp_error("bind socket error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    lp_verbose("bind socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);
    
    if (::listen(_fd, SERVER_LISTEN_BACKLOG) == -1) {
        ret = ERROR_SOCKET_LISTEN;
        lp_error("listen socket error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    lp_verbose("listen socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);
    
    if ((_stfd = st_netfd_open_socket(_fd)) == NULL){
        ret = ERROR_ST_OPEN_SOCKET;
        lp_error("st_netfd_open_socket open socket failed. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    lp_verbose("st open socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);
    
    if ((ret = pthread->start()) != ERROR_SUCCESS) {
        lp_error("st_thread_create listen thread error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    lp_verbose("create st listen thread success, ep=%s:%d", ip.c_str(), port);
    
    return ret;
}

int LPushTcpListener::cycle()
{
    int ret = ERROR_SUCCESS;
    
    st_netfd_t client_stfd = st_accept(_stfd, NULL, NULL, ST_UTIME_NO_TIMEOUT);
    
    if(client_stfd == NULL){
        // ignore error.
        if (errno != EINTR) {
            lp_error("ignore accept thread stoppped for accept client error");
        }
        return ret;
    }
    lp_verbose("get a client. fd=%d", st_netfd_fileno(client_stfd));
    
    if ((ret = handler->on_tcp_client(client_stfd)) != ERROR_SUCCESS) {
        lp_warn("accept client error. ret=%d", ret);
        return ret;
    }
    
    return ret;
}
  
}