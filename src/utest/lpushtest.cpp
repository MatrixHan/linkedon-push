#include "lpushtest.h"

using namespace lpush;

LpushTest::LpushTest()
{
	userId = "10000";
	appId = "LOFFICIEL";
	screteKey = "654321";
////	
	clientFlag = "1";

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
	remote_addr.sin_addr.s_addr = inet_addr("172.16.104.21");
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
	printf("connect to server success!/n");		
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
	set_packet_header();
	
	len = send(client_sockfd, buf, datalen+14, 0);
	
	return RET_SUCCESS;
}

int LpushTest::set_handshake_message()
{
	string md5keystr = userId + appId + screteKey;
	md5Data = getmd5str(md5keystr);
	
	headjson.insert(pair<string, string>("appId", appId));
	headjson.insert(pair<string, string>("screteKey", screteKey));
	headjson.insert(pair<string ,string>("userId", userId));
	headjson.insert(pair<string, string>("clientFlag", clientFlag));
	headjson.insert(pair<string, string>("md5Data", md5Data));
	
	string msg = LPushConfig::mapToJsonStr(headjson);
	
	datalen = msg.size();	
	memcpy(p + 14, msg.c_str(), msg.size());
}

int LpushTest::set_packet_header()
{	
	memcpy(&stheader.flag, "LPUSH", 5);
	time_t timep; 
	time(&timep);
	stheader.timestamp = htonl(timep);
	stheader.datatype = 0x01;
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

