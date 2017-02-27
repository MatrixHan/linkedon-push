#include "lpushtest.h"

using namespace lpush;

LpushTest::LpushTest()
{
	userId = "10000";
	appId = "LOFFICIEL";
	screteKey = "123456";
////	
	clientFlag = "1";
	identity = "12121";
	devices = "ac:fd:cc:o3:4u";

	datalen = 0;
	memset(buf, 0x0, sizeof(buf));

	p = buf;	
}

LpushTest::~LpushTest()
{

	
}

int LpushTest::connection()
{
	struct sockaddr_in remote_addr;
	memset(&remote_addr, 0x0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr("106.3.138.173"); //106.3.138.173 | 172.16.104.21  port  9732
	remote_addr.sin_port = htons(9732);
	
	if ((client_sockfd = socket(PF_INET, SOCK_STREAM, 0))< 0)
	{
		perror("socket");
		return RET_ERROR;
	}
	int i;
	if ( connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0)
	{
		perror("connect");
		return RET_ERROR;
	}
	printf("connect to server success!\n");		
	return RET_SUCCESS;
}

int LpushTest::init_message()
{
	memset(buf, 0x0, sizeof(buf));
	p = buf;
	datalen = 0;	
}


int LpushTest::send_handshake_message()
{
	int len = -1;
	
	set_handshake_message();
	char datatype = 0x01;
	set_packet_header(datatype);
	
	//send shake msg	
	len = send(client_sockfd, buf, datalen+14, 0);
	if (len < 0)
	{
		cout << "send shake message fail..." << endl;
		return RET_ERROR;
	}
	
	//recv response shake msg
	init_message();
	len = recv(client_sockfd, buf, sizeof(buf), 0);
	datatype = buf[9];
	if (len < 0)
	{
		cout << "client handshake fail..." << endl;
		return RET_ERROR;
	}
	cout << "client handshake success..." << endl;	
	//send create connection msg
	init_message();
	datatype = 0x04;
	datalen = 1;
	set_packet_header(datatype);
	
	buf[14] = 'c';
	
	len = send(client_sockfd, buf, 15, 0);
	if (len > 0)
	{
		cout << "client  send connect msg success..." << endl;
	}
	init_message();
	len = recv(client_sockfd, buf, 15, 0);
	
	if (len > 0 &&buf[9] == 0x04 && buf[14] == 0x01)
	{
		cout << "client  create connection success..." << endl;
	}
		
	return RET_SUCCESS;
}

int LpushTest::set_handshake_message()
{
	p[14] = 0x06;

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
	memcpy(p + 15, &jsonlen, 4);
	
	memcpy(p + 19, msg.c_str(), msg.size());
}

int LpushTest::set_packet_header(unsigned char datatype)
{	
	memcpy(&stheader.flag, "LPUSH", 5);
	time_t timep; 
	time(&timep);
	stheader.timestamp = htonl(timep);
	stheader.datatype = datatype;
	stheader.datalen = htonl(datalen);
		
	memcpy(p, &stheader.flag, 5);
	p += 5;
	memcpy(p, &(stheader.timestamp), 4);
	p += 4;
	memcpy(p, &(stheader.datatype), 1);
	p += 1;
	memcpy(p, &(stheader.datalen), 4);
}

int LpushTest::set_packet_body(const char *data)
{
	memcpy(p, data, strlen(data));
	p += strlen(data);
}

int LpushTest::recv_message()
{
	datalen = recv(client_sockfd, buf, sizeof(buf), 0);
	return datalen;
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

