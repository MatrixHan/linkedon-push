#include "lpushCommon.h"
#include "lpushtest.h"


using namespace std;
using namespace lpush;


#include <pthread.h>

static int print_message(unsigned char *pbuf);
static void start_daemon(void);



pthread_t thread[2];
pthread_mutex_t mut;

LpushTest test;

#define TIME_OUT 1*1000*1000LL

void *send_heart(void *)
{
	while (1) {
		pthread_mutex_lock(&mut);	
		char datatype = 0x05;
		test.init_message();
		test.datalen = 1;
		test.buf[14] = 0x01;
		test.set_packet_header(datatype, test.buf);
		int len = test.socket_nonblock_send(test.client_sockfd, test.buf, 15, TIME_OUT);
		if (len < 0) {
			test.err_sys_report("ERROR: heat send fail...\n");
			//break;
		}
		//test.err_sys_report("send heartbeat success...");
		pthread_mutex_unlock(&mut);
		sleep(30);
	}
	pthread_mutex_unlock(&mut);
	pthread_exit(NULL);		
}


void *thread_recv(void *)
{
	
	unsigned char buff[10240];
	while (1) {
		pthread_mutex_lock(&mut);	
		test.init_message();
		int recv_len = test.socket_nonblock_recv(test.client_sockfd, test.buf, sizeof(test.buf), TIME_OUT);
		if (recv_len < 0 && errno != EAGAIN) {
			test.err_sys_report("ERROR: data recv fail...");
			//break;
		}
		
		while (recv_len > 0) {
			
			int k = print_message(test.buf);		
			map<string, string> json_valuemap, msg_mapto_json;			
			LPushFMT::decodeJson(test.buf+14, json_valuemap);
			string taskId = json_valuemap["TaskId"];
			string msg = json_valuemap["MsgId"];
			char datatype = test.buf[9];
			
			memset(buff, 0x0, sizeof(buff));
			
			buff[14] = 0x01;			
			
			unsigned int mslen = htonl(taskId.size()); 
			memcpy(&buff[15], &mslen, 4);
			
			memcpy(buff + 19, taskId.c_str(), taskId.size());
			
			int buflen = 19 + taskId.size();
			
			buff[buflen] = 0x01;
			mslen = htonl(msg.size());
			
			
			memcpy(&buff[buflen+1], &mslen, 4);
			memcpy(&buff[buflen+5], msg.c_str(), msg.size());
			
			test.datalen = taskId.size() + 5 + msg.size() + 5;
			test.set_packet_header(datatype, buff);
			
			int slen = test.socket_nonblock_send(test.client_sockfd, buff, test.datalen+14, TIME_OUT);
			if (slen < 0) {
				test.err_sys_report("ERROR: data send fail...");
				//break;
			}
			recv_len = recv_len - k- 14;
			if (recv_len)
				memmove(test.buf, test.buf + k + 14, recv_len);
			else
				break;			
		}
		
		pthread_mutex_unlock(&mut);
		usleep(500*1000);
	}
	pthread_mutex_unlock(&mut);
	pthread_exit(NULL);		
}


void *send_heart1(void *)
{
	while (1)
	{		
		pthread_mutex_lock(&mut);
		//cout << "**************************************send heartbeat begin*******************************" << endl;
		char datatype = 0x05;
		test.init_message();
		test.datalen = 1;
		test.buf[14] = 0x01;
		test.set_packet_header(datatype, test.buf);
		int len = send(test.client_sockfd, test.buf, 15, 0);
		if (len > 0)
			//cout << "thread id:[" << pthread_self() << "] send heartbeat success..." << len << endl;
			;
		else  {
			if (errno == EAGAIN) {
				
			} else {	
				perror("heart_send: ");
			}	
		}	
	    //cout << "**************************************send heartbeat end***********************************" << endl;
		pthread_mutex_unlock(&mut);
		
		sleep(30);		
	}
	pthread_exit(NULL);	
}


static int print_message(unsigned char *pbuf)
{
	unsigned int msg_len = 0;
	unsigned char buff[1024];
	unsigned int k = 0;	
	char datatype = pbuf[9];																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																
	printf("datatype = %0x\n", datatype);
	if (datatype == 0x07 || datatype == 0x08)
	{
		char flags[6] = {'\0'};
		memcpy(flags, pbuf, 5);
		cout << "flags: " << flags << endl;
		
		memcpy(&k, &(pbuf[5]), 4);
		k = ntohl(k);
		cout << "time: " << k << endl;
		memcpy(&k, &(pbuf[10]), 4);
		k = ntohl(k);
		cout << "datalen: " << k << endl;
		char msgtype = pbuf[14];
		printf("msgtype = %0x\n", msgtype);
							
		memcpy(&msg_len, &(pbuf[15]), 4);
		msg_len = ntohl(msg_len);
		cout << "msg_len: " << msg_len << endl;
					
		memset(buff, 0x0, sizeof(buff));
		memcpy(buff, &(pbuf[19]), k-5);
		printf("recv message: %s", buff);				
	}
	
	return k;
}


void *thread_recv1(void *)
{
	
	while (1)
	{	
		pthread_mutex_lock(&mut);
		//cout << "thread id:[" << pthread_self() << "] thread recv........" << endl;
		test.init_message();
		int recv_len = test.recv_message();		
		if (recv_len > 0)			
		{	
			cout << "recv_len: " << recv_len << endl;
			int tmp_len = recv_len;
			unsigned int msg_len = 0;
			unsigned char buff[10240];
			unsigned int k = 0;
			while (recv_len > 0) {
				cout << "userId: " << test.userId << endl;				
				//print message				
				char datatype =test.buf[9];																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																
				printf("datatype = %0x\n", datatype);
				if (datatype == 0x07 || datatype == 0x08)
				{
					char flags[6] = {'\0'};
					memcpy(flags, test.buf, 5);
					cout << "flags: " << flags << endl;
					
					memcpy(&k, &(test.buf[5]), 4);
					k = ntohl(k);
					cout << "time: " << k << endl;
					memcpy(&k, &(test.buf[10]), 4);
					k = ntohl(k);
					cout << "datalen: " << k << endl;
					char msgtype = test.buf[14];
					printf("msgtype = %0x\n", msgtype);
										
					memcpy(&msg_len, &(test.buf[15]), 4);
					msg_len = ntohl(msg_len);
					cout << "msg_len: " << msg_len << endl;
								
					memset(buff, 0x0, sizeof(buff));
					memcpy(buff, &(test.buf[19]), k-5);
					printf("recv message: %s", buff);				
				}
				
				//
				
				map<string, string> json_valuemap, msg_mapto_json;			
				LPushFMT::decodeJson(test.buf+14, json_valuemap);
				string taskId = json_valuemap["TaskId"];
				string msg = json_valuemap["MsgId"];
				
				//msg_mapto_json.insert(pair<string, string>("TaskId", taskId));
				//string msg = LPushConfig::mapToJsonStr(msg_mapto_json);
				//Trim(msg);			

				//string msg = LPushFMT::encodeString(taskId);
				memset(buff, 0x0, sizeof(buff));
				
				buff[14] = 0x01;			
				
				unsigned int mslen = htonl(taskId.size()); 
				memcpy(&buff[15], &mslen, 4);
				
				memcpy(buff + 19, taskId.c_str(), taskId.size());
				
				int buflen = 19 + taskId.size();
				
				buff[buflen] = 0x01;
				mslen = htonl(msg.size());
				
				
				memcpy(&buff[buflen+1], &mslen, 4);
				memcpy(&buff[buflen+5], msg.c_str(), msg.size());
				
				test.datalen = taskId.size() + 5 + msg.size() + 5;
				test.set_packet_header(datatype, buff);
				int slen = send(test.client_sockfd, buff, test.datalen+14, 0);
				if (slen > 0)
				{
					cout << test.userId << " send ok recv message success..." << slen << endl;
// 					for (int i=0;  i<buflen; i++) {
// 						printf("i =%d %0x\n", i, buff[i]);
// 						cout << endl;
// 					}	
				}
	 			else
	// 			{
					if (errno == EINTR ) 
						usleep(30*1000);
					else {
						perror("answer_send: ");
						break;
					}	
	// 				cout << "send recv failed ..." << endl;
	// 			}
				recv_len = recv_len - k- 14;
				if (recv_len)
					memmove(test.buf, test.buf + k + 14, recv_len);
				else
					break;
			}	
				
		}
  	else
  	{	
		if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
 
		} else {
			perror("data_recv: ");
		}	
  	}

	pthread_mutex_unlock(&mut);
	usleep(500*1000);
	}	
	pthread_exit(NULL);	
}

void thread_create()
{
	memset(&thread, 0, sizeof(thread));
	
 	if (pthread_create(&thread[0], NULL, send_heart, NULL) != 0)
 	{
 		test.err_sys_report("thread heart create fail!");
 	}
	
 	if (pthread_create(&thread[1], NULL, thread_recv, NULL) != 0)
 	{
 		test.err_sys_report("thread recv create fail!");
 	}
	//test.err_sys_report("thread create success.....");
}

void thread_wait(void)
{
	//wati heart thread
	if(thread[0] !=0)
	{ 
		pthread_join(thread[0],NULL);
		test.err_sys_report("heartbeat thread exit...");
	}
	//wait recv thread
 	if(thread[1] !=0) 
 	{
 		pthread_join(thread[1],NULL);
 		test.err_sys_report("recv thread exit...");
 	}
}


int test1(void)
{	
	int ret = test.connection();
	if (ret == RET_ERROR)
	{
		cout << "ERROR： connection fail..." << endl;
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

int start()
{
	int ret = test.connection();
	if (ret == RET_ERROR)
	{
		test.err_sys_report("ERROR: connection fail...");
	}
	
	//handshake
	ret = test.send_handshake_message();
	if (ret == RET_ERROR)
	{
		exit(-1);
	}
	
	test.set_socket_nonblock(test.client_sockfd);	
	test.init_message();	
	thread_create();	
	thread_wait();
	
	while (1)
	{
		sleep(10);		
	}	
	return 0;
}

static void start_daemon(void)
{
  pid_t pid;

  /* Start forking */
  if ((pid = fork()) < 0)
    test.err_sys_report("ERROR: fork");
  if (pid > 0)
    exit(0);                  /* parent */

  /* First child process */
  setsid();                   /* become session leader */

  if ((pid = fork()) < 0)
     test.err_sys_report("ERROR: fork");
  if (pid > 0)                /* first child */
    exit(0);

  umask(022);

  if (chdir("/") < 0)
    test.err_report("ERROR: can't change directory to root directory: chdir");
}

int main(int argc, char **argv)
{
	if (argc > 1)
		test.userId = argv[1];
	if (argc > 2)
		test.conn_ip = argv[2];
	test.err_sys_report("userId: %s", test.userId.c_str());
	test.err_sys_report("connection ip: %s", test.conn_ip.c_str());
	start_daemon();
	process_signal();	
	//test1();
	start();
	
	return 0;
}

