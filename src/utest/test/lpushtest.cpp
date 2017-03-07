#include "lpushtest.h"

using namespace lpush;

LpushTest::LpushTest()
{
	userId = "10001";
	appId = "48947381";
	screteKey = "83EB8022CEF82135EA5AE3D627D18026";
////	
	clientFlag = "1";
	identity = "12121";
	devices = "ac:fd:cc:o3:4u";
	
	conn_ip = "106.3.138.173";

	datalen = 0;
	memset(buf, 0x0, sizeof(buf));
	
	pp = buf;	
}

LpushTest::~LpushTest()
{

	
}

int LpushTest::connection()
{
	struct sockaddr_in remote_addr;
	memset(&remote_addr, 0x0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(conn_ip.c_str()); //106.3.138.173 | 172.16.104.21  port  9732
	remote_addr.sin_port = htons(9732);
	
	if ((client_sockfd = socket(PF_INET, SOCK_STREAM, 0))< 0)
	{
		err_sys_report("socket");
		return RET_ERROR;
	}
	int i;
	if ( connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0)
	{
		err_sys_report("ERROR: connect");
		return RET_ERROR;
	}
	err_report("connect to server success!");		
	return RET_SUCCESS;
}

int LpushTest::init_message()
{
	memset(buf, 0x0, sizeof(buf));
	pp = buf;
	datalen = 0;

	return 0;
}


int LpushTest::send_handshake_message()
{
	int len = -1;
	
	set_handshake_message();
	char datatype = 0x01;
	set_packet_header(datatype, buf);
	
	//send shake msg	
	len = send(client_sockfd, buf, datalen+14, 0);
	if (len < 0)
	{
		err_sys_report("ERROR: send shake message fail...");
		return RET_ERROR;
	}
	
	//recv response shake msg
	init_message();
	len = recv(client_sockfd, buf, sizeof(buf), 0);
	datatype = buf[9];
	if (len < 0)
	{
		err_sys_report("ERROR: client handshake fail...");
		return RET_ERROR;
	}
	//send create connection msg
	init_message();
	datatype = 0x04;
	datalen = 1;
	set_packet_header(datatype, buf);
	
	buf[14] = 'c';
	
	len = send(client_sockfd, buf, 15, 0);
	if (len < 0)
	{
		err_sys_report("ERROR: client  send connect msg failed...");
		return RET_ERROR;
	}
	init_message();
	len = recv(client_sockfd, buf, 15, 0);
	
	if (len > 0 &&buf[9] == 0x04 && buf[14] == 0x01)
	{
		err_report("client  create connection success...");
	} else {
		err_sys_report("ERROR: client  create connection fail...");
		return RET_ERROR;
	}
		
	return RET_SUCCESS;
}

int LpushTest::set_handshake_message()
{
	pp[14] = 0x06;

	string md5keystr = userId + appId + screteKey;
	md5Data = getmd5str(md5keystr);
	
	headjson.insert(pair<string, string>("appId", appId));
	headjson.insert(pair<string, string>("screteKey", screteKey));
	headjson.insert(pair<string ,string>("userId", userId));
	headjson.insert(pair<string, string>("clientFlag", clientFlag));
	headjson.insert(pair<string, string>("md5Data", md5Data));
	headjson.insert(pair<string, string>("identity", identity));
	headjson.insert(pair<string, string>("devices", devices));	
	
	string msg = LPushConfig::mapToJsonStr(headjson);
	Trim(msg);	
	
	datalen = msg.size() + 5;
	unsigned int jsonlen = htonl(msg.size());
	memcpy(pp + 15, &jsonlen, 4);
	
	memcpy(pp + 19, msg.c_str(), msg.size());
	
	return 0;
}

int LpushTest::set_packet_header(unsigned char datatype, unsigned char *buff)
{	
	memcpy(&stheader.flag, "LPUSH", 5);
	time_t timep; 
	time(&timep);
	stheader.timestamp = htonl(timep);
	stheader.datatype = datatype;
	stheader.datalen = htonl(datalen);
	unsigned char *p = buff;	
	memcpy(p, &stheader.flag, 5);
	p += 5;
	memcpy(p, &(stheader.timestamp), 4);
	p += 4;
	memcpy(p, &(stheader.datatype), 1);
	p += 1;
	memcpy(p, &(stheader.datalen), 4);
	
	return 0;
}

int LpushTest::set_packet_body(const char *data)
{
	memcpy(pp, data, strlen(data));
	pp += strlen(data);
	
	return 0;
}

int LpushTest::recv_message()
{
	datalen = recv(client_sockfd, buf, sizeof(buf), 0);
	return datalen;
	
	return 0;
}


string LpushTest::getmd5str(std::string src)
{
    unsigned char digest[16];
    const char *str = src.c_str();
    
    MD5((unsigned char*)str,strlen(str),(unsigned char*)&digest);
    
    char mdString[33];
    for(int i = 0; i < 16; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    
    return std::string(mdString);
}

char * LpushTest::err_tstamp(void)
{
  static char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  static char str[32];
  static time_t lastt = 0;
  struct tm *tmp;
  time_t currt = st_time();

  if (currt == lastt)
    return str;

  tmp = localtime(&currt);
  sprintf(str, "[%02d/%s/%d:%02d:%02d:%02d] ", tmp->tm_mday,
          months[tmp->tm_mon], 1900 + tmp->tm_year, tmp->tm_hour,
          tmp->tm_min, tmp->tm_sec);
  lastt = currt;

  return str;
}

void LpushTest::err_doit(int errnoflag, const char *fmt, va_list ap)
{
	int errno_save;
	char sbuf[4096];

	errno_save = errno;         /* value caller might want printed   */
	strcpy(sbuf, err_tstamp());  /* prepend a message with time stamp */
	vsprintf(sbuf + strlen(sbuf), fmt, ap);
	if (errnoflag)
		sprintf(sbuf + strlen(sbuf), ": %s", strerror(errno_save));
	errno = errno_save;
	//fprintf(stderr, "%s", sbuf);
	cout << sbuf << endl;
}

void LpushTest::err_sys_report(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);	
}

void LpushTest::err_report(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(0, fmt, ap);
  va_end(ap);
}

int LpushTest::set_socket_nonblock(int sockfd)
{
	int block_flag = fcntl(sockfd, F_GETFL, 0);
	if (block_flag < 0) {
		err_sys_report("get socket fd flag error");
		return -1;
	}
	if (fcntl(sockfd, F_SETFL, block_flag | O_NONBLOCK) < 0) {
		err_sys_report("set socket fd non block error");
		return -1;
	} 
	
	return 0;
}

/** 
  *@return  
  * 0 success， 
  *-1 failure,errno==EPIPE, 对端进程fd被关掉 
  *-2 failure,网络断开或者对端超时未再发送数据 
  *-3 failure,other errno 
**/  

int LpushTest::socket_nonblock_send(int fd, unsigned char* buffer,  unsigned int length, unsigned long timeout)
{
	if (length == 0 || buffer == NULL) {
		err_report("buffer point is NULL or length is zero");
		return 0;
	}	
	unsigned int bytes_left;
	bytes_left = length;
	long long written_bytes;
	unsigned char *ptr;
	ptr = buffer;
	fd_set writefds;
	struct timeval tv;
	int ret = 0;
	while (bytes_left > 0) {
		written_bytes = send(fd, ptr, bytes_left, MSG_NOSIGNAL);
		if (written_bytes < 0) {
			if (errno == EINTR) { //信号中断，没有写入数据
				written_bytes = 0;
			} else if (errno == EWOULDBLOCK || errno == EAGAIN) { //即EＡＧＡＩＮ, SOCKET内核缓冲区满或者网络断开
				FD_ZERO(&writefds);
				FD_SET(fd, &writefds);
				tv.tv_sec = timeout/1000000;
				tv.tv_usec = timeout%1000000;
				ret = select(fd+1, NULL, &writefds, NULL, &tv); //阻塞，　ｅｒｒ: 0 timeout
				if (ret == 0) {
					//err_sys_report("send select error");
					return -2;
				} else if (ret < 0 && errno != EINTR) {
					//err_sys_report("send select error");
					return -2;
				}
				written_bytes = 0; //未超时，判定为ｓｏｃｋｅｔ缓冲区满导致的网络阻塞				
			} else if (errno == EPIPE) { //对端ｆｄ被关掉
				err_sys_report("write socket error %d: ", errno);
				return -1;
			} else { //其他错误
				err_sys_report("write socke error %d: ", errno);
				return -1;
			}
		}
		bytes_left -= written_bytes;
		ptr += written_bytes;
	}
	
	return 0;
}

/** 
  *@return  
    the number of bytes read success, 
    -1 failure, 对端进程fd被关掉或者对端超时未再发送数据 
    -2 failure,网络断开或者对端超时未再发送数据 
    -3 failure,other errno 
**/  

long long LpushTest::socket_nonblock_recv(int fd, unsigned char* buffer, unsigned int length, unsigned long timeout)
{
	unsigned int bytes_left;  
	long long read_bytes;  
	unsigned char* ptr;  
	ptr = buffer;  
	bytes_left = length;  
	fd_set readfds;  
	int ret = 0;  
	struct timeval tv;  
	
	while (bytes_left > 0) {
		read_bytes = recv(fd, ptr, bytes_left, 0);
		if (read_bytes < 0) {
			if (errno == EINTR) { //信号中断
				read_bytes = 0;
			} else if (errno == EAGAIN) { //EAGAIN 没有可读写数据,缓冲区无数据
				if (length > bytes_left) { //说明上一循环把缓冲区数据读完，继续读返回-1，应返回已读取的长度  
					return (length - bytes_left);
				} else {  //length == bytes_left,说明第一次调用该函数就无数据可读，可能是对端无数据发来，可能是对端网线断了 
					FD_ZERO(&readfds);
					FD_SET(fd, &readfds);
					tv.tv_sec = timeout/1000000;
					tv.tv_usec = timeout%1000000;
					ret = select(fd+1, &readfds, NULL, NULL, &tv); //阻塞,err:0 timeout err:-1
					if (ret == 0 || errno == EAGAIN) { //超时，判定为网线断开 
						//err_sys_report("recv select error");
						return -2;
					} else if (ret < 0 && errno != EINTR) {
						//err_sys_report("recv select error");
						return -2;
					}
					//未超时，有数据到来
					continue;
				}
			} else {
				err_sys_report("read socket buf error");
				return -3;
			}
		} else if (read_bytes == 0) { //缓冲区数据读完，对端fd 关闭或对端没有发数据了，超时1s后判定为连接已断
			FD_ZERO(&readfds);
			FD_SET(fd, &readfds);
			tv.tv_sec = timeout/1000000;
			tv.tv_usec = timeout/1000000;
			ret = select(fd+1, &readfds, NULL, NULL, &tv); //阻塞, err: 0
			if (ret == 0 || errno == EAGAIN) {
				//err_report("recv select error");
				return  -1;
			} else if (ret < 0 && errno != EINTR) {
				//err_sys_report("recv select error");
				return -1;
			}
			continue;
		}
		bytes_left -= read_bytes;
		ptr += read_bytes;
	}
	
	return (length - bytes_left);
}

