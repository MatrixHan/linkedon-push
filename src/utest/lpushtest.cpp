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
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	remote_addr.sin_port = htons(8888);
	
	if (client_sockfd = socket(PF_INET, SOCK_STREAM, 0) < 0)
	{
		perror("socket");
		return RET_ERROR;
	}
	
	if (connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr) < 0))
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
	set_packet_header();
	set_handshake_message();
	
	len = send(client_sockfd, buf, datalen, 0);
	
	return RET_SUCCESS;
}

int LpushTest::set_handshake_message()
{
	string md5keystr = userId + appId + screteKey;
	md5Data = md5Encoder(md5keystr);
	
	headjson.insert(pair<string, string>("appId", appId));
	headjson.insert(pair<string, string>("screteKey", screteKey));
	headjson.insert(pair<string ,string>("userId", userId));
	headjson.insert(pair<string, string>("clientFlag", clientFlag));
	headjson.insert(pair<string, string>("md5Data", md5Data));
	
	string msg = LPushConfig::mapToJsonStr(headjson);
	
	datalen = strlen(msg.c_str());	
	memcpy(p, msg.c_str(), datalen);
	datalen = p - buf;
}

int LpushTest::set_packet_header()
{	
	memcpy(&stheader.flag, "LPUSH", 5);
	time_t timep; 
	time(&timep);
	stheader.timestamp = timep;
	stheader.datatype = 0x01;
	stheader.datalen = datalen;
	memcpy(p, &stheader, sizeof(stheader));
	p += sizeof(stheader);
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



