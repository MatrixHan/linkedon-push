#include "lpushCommon.h"
#include "lpushtest.h"


using namespace std;
using namespace lpush;


#include <pthread.h>


pthread_t thread[2];
pthread_mutex_t mut;

LpushTest test;

void *send_heart(void *)
{
	while (1)
	{		
		pthread_mutex_lock(&mut);
		//cout << "**************************************send heartbeat begin*******************************" << endl;
		char datatype = 0x05;
		test.init_message();
		test.datalen = 1;
		test.buf[14] = 0x01;
		test.set_packet_header(datatype);
		int len = send(test.client_sockfd, test.buf, 15, 0);
		if (len > 0)
			cout << "thread id:[" << pthread_self() << "] send heartbeat success..." << len << endl;
		//cout << "**************************************send heartbeat end***********************************" << endl;
		pthread_mutex_unlock(&mut);
		
		sleep(1);		
	}
	pthread_exit(NULL);
	
}

void *thread_recv(void *)
{
	while (1)
	{
		pthread_mutex_lock(&mut);
		//cout << "thread id:[" << pthread_self() << "] thread recv........" << endl;
		test.init_message();
		int recv_len = test.recv_message();
		if (recv_len > 0)
		{	
			cout << "userId: " << test.userId << endl;
			unsigned char buff[204800];
			char datatype =test.buf[9];																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																
			printf("datatype = %0x\n", datatype);
			if (datatype == 0x07 || datatype == 0x08)
			{
				char flags[6] = {'\0'};
				memcpy(flags, test.buf, 5);
				cout << "flags: " << flags << endl;
				unsigned int k;
				memcpy(&k, &(test.buf[5]), 4);
				k = ntohl(k);
				cout << "time: " << k << endl;
				memcpy(&k, &(test.buf[10]), 4);
				k = ntohl(k);
				cout << "datalen: " << k << endl;
				char msgtype = test.buf[14];
				printf("msgtype = %0x\n", msgtype);
				
				unsigned int msg_len;
				memcpy(&msg_len, &(test.buf[15]), 4);
				msg_len = ntohl(msg_len);
				cout << "msg_len: " << msg_len << endl;
							
				memset(buff, 0x0, sizeof(buff));
				memcpy(buff, &(test.buf[19]), k-5);
				printf("recv message: %s", buff);				
			}

			map<string, string> json_valuemap, msg_mapto_json;			
			LPushFMT::decodeJson(test.buf+14, json_valuemap);
			string msg = json_valuemap["TaskId"];						
			//msg_mapto_json.insert(pair<string, string>("TaskId", taskId));
			//string msg = LPushConfig::mapToJsonStr(msg_mapto_json);
			//Trim(msg);			
 			test.init_message();

			//string msg = LPushFMT::encodeString(taskId);
			test.buf[14] = 0x01;			
 			test.datalen = msg.size() + 5;
			unsigned int mslen = htonl(msg.size()); 
			memcpy(&test.buf[15], &mslen, 4);
 			test.set_packet_header(datatype);
			memcpy(test.buf + 19, msg.c_str(), msg.size());
 			int slen = send(test.client_sockfd, test.buf, test.datalen+14, 0);
			if (slen > 0)
			{
				cout << test.userId << " send ok recv message success..." << slen << endl;
			}
// 			else
// 			{
// 				cout << "send recv failed ..." << endl;
// 			}
			
		}
// 		else
// 		{				
// 			perror("recv");
// 		}
		pthread_mutex_unlock(&mut);
		usleep(5*1000);
	}	
	pthread_exit(NULL);	
}

void thread_create()
{
	memset(&thread, 0, sizeof(thread));
	
 	if (pthread_create(&thread[0], NULL, send_heart, NULL) != 0)
 	{
 		printf("thread heart create fail!\n");
 	}
	
 	if (pthread_create(&thread[1], NULL, thread_recv, NULL) != 0)
 	{
 		printf("thread recv create fail!\n");
 	}
	printf("thread create success.....\n");
}

void thread_wait(void)
{
	//wati heart thread
	if(thread[0] !=0)
	{ 
		pthread_join(thread[0],NULL);
		printf("heartbeat thread exit...\n");
	}
	//wait recv thread
 	if(thread[1] !=0) 
 	{
 		pthread_join(thread[1],NULL);
 		printf("recv thread exit...\n");
 	}
}


int test1(void)
{	
	int ret = test.connection();
	if (ret == RET_ERROR)
	{
		exit(-1);
	}

	
	//handshake
	ret = test.send_handshake_message();
	if (ret == RET_ERROR)
	{
		exit(-1);
	}
	
	//set socket nonblock
	int flags = fcntl(test.client_sockfd, F_GETFL, 0);
	fcntl(test.client_sockfd, F_SETFL, flags|O_NONBLOCK);
	
	test.init_message();			
	pthread_mutex_init(&mut, NULL);

	//start thread
	thread_create();
	//wait end thread
	thread_wait();
	while (1)
	{
		sleep(10);
		
	}	
	return 0;
}

int process_signal()
{
// 	struct sigaction sa;
// 	sa.sa_handler = SIG_IGN;
// 	sigaction( SIGPIPE, &sa, 0 );
	
	
//	signal(SIGPIPE, SIG_IGN);
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	sigprocmask(SIG_BLOCK, &set, NULL);		
}

int main(int argc, char **argv)
{
	test.userId = argv[1];
	cout << "userId: " << test.userId << endl;
	process_signal();	
	test1();
	return 0;
}

