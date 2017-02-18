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
		char *hearstr = "keep alive";
		send(test.client_sockfd, hearstr, strlen(hearstr), 0);
		pthread_mutex_unlock(&mut);
		sleep(60);		
	}
	
}

void *thread_recv(void *)
{
	while (1)
	{
		pthread_mutex_lock(&mut);
		if (test.recv_message() > 0)
		{	
			test.buf[test.datalen] = 0x0;
			printf("%s\n", test.buf);
			test.init_message();
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

int main(int argc, char **argv)
{
	
	test.connection();
	test.set_packet_header();
	test.set_handshake_message();
	//handshake
	test.send_handshake_message();
	test.init_message();
			
	pthread_mutex_init(&mut, NULL);
	thread_create();
	
  
	return 0;
}