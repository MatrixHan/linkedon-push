#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <pwd.h>
#include <st.h>

#include <assert.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdarg.h>
#define CONFIG_DEFAULT_FILE_NAME "conf/lpush.conf"
#define DEFAULT_LOG_FILE_NAME "logs/lpush.log"
  
#define Abs(a) ((a) > 0 ? (a) : -(a))

#define STRLENGTH(str) (sizeof(str))
  
#define SafeDelete(p) if(p) {delete p;p=0x0;}
  
#define SafeDeleteArray(p) if(p) {delete [] p;p=0x0;}
  
#define COUT(a) std::cout<< #a <<" : "<< (a) <<std:endl;
  
#define BCOLOR_16BIT(r,g,b) ( ((r) & 0xff)<<16 |  ((g) & 0xff)<<8  | ((b) & 0xff) )
  
#define COLOR255(rgb) ((rgb)>255?255:(rgb))
  
#define EPSILON_E6 (float)(1E-6)
  
#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)


namespace lpush
{
  

}