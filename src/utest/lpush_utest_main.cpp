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
		cout << "send heartbeat begin..." << endl;
		pthread_mutex_lock(&mut);
		char datatype = 0x05;
		test.init_message();
		test.datalen = 1;
		test.buf[14] = 0x01;
		test.set_packet_header(datatype);
		int len = send(test.client_sockfd, test.buf, 15, 0);
		cout << "send heartbeat end..." << endl;
		pthread_mutex_unlock(&mut);
		
		sleep(10);		
	}
	
}

void *thread_recv(void *)
{
	while (1)
	{
		pthread_mutex_lock(&mut);
		cout << "thread recv..." << endl;
		test.init_message();
		if (test.recv_message() > 0)
		{	
			char datatype =test.buf[9];
			cout << "datatype: " << datatype << endl;
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
				string msg;
				memcpy((void *)msg.c_str(), &(test.buf[14]), k);
				cout << "recv message: " << msg << endl;				
			}			
		}
		pthread_mutex_unlock(&mut);
		sleep(1);
	}	
	
	
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
		printf("");
	}
	//wait recv thread
 	if(thread[1] !=0) 
 	{
 		pthread_join(thread[1],NULL);
 		printf("");
 	}
}



int main(int argc, char **argv)
{
	test.connection();
	
	//handshake
	test.send_handshake_message();
	test.init_message();			
	pthread_mutex_init(&mut, NULL);
	//set socket nonblock
	int flags = fcntl(test.client_sockfd, F_GETFL, 0);
	fcntl(test.client_sockfd, F_SETFL, flags|O_NONBLOCK);
	//start thread
	thread_create();
	//wait end thread
	thread_wait();
	
	return 0;
}