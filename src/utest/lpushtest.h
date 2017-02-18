#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#include "lpushJson.h"
#include "lpushMath.h"

using namespace std;
using namespace lpush;

#define RET_ERROR -1
#define RET_SUCCESS 0

#define BUFFER_SIZE 256*1024


typedef struct lpush_header
{
	unsigned char flag[5];
	unsigned long int  timestamp;
	unsigned char datatype;
	unsigned  int datalen;		
} push_header;

namespace lpush {
	
	
	class LpushTest
	{
	public:
		int	client_sockfd;
		int datalen;
		string appId;
		string screteKey;
		string userId;
		string clientFlag;
		string md5Data;
		
		unsigned char *p;
		unsigned char buf[BUFFER_SIZE];
		push_header stheader;
		map<string, string> headjson;
	public:	
		LpushTest();
		~LpushTest();

		int set_packet_header();
		int set_packet_body(const char *data);
		int packet_data();
		int connection();
		int set_handshake_message();
		int send_handshake_message();
		int recv_message();
		int init_message();
		string getmd5str(string src);
	};

	void *fun_thrReceiveHandler(void *socketCon);

}
