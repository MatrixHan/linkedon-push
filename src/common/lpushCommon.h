#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
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
#include <assert.h>
#include <getopt.h>

#include <st.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdarg.h>
#include <map>
#include <vector>
#include <algorithm>

#define CONFIG_DEFAULT_FILE_NAME "conf/lpush.conf"
#define DEFAULT_LOG_FILE_NAME "logs/lpush.log"
#define DEFAULT_PID_FILE_NAME "objs/lpush.pid"

#define DEFAULT_PROCESS_MAX	1000
  
#define Abs(a) ((a) > 0 ? (a) : -(a))

#define STRLENGTH(str) (sizeof(str))
  
#define SafeDelete(p) if(p) {delete p; p=0x0;}
  
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

	
#define LPUSH_PROJECT_NAME "linkedon-push"

#define LPUSH_PROJECT_VERSION  1.0


	
	

namespace lpush
{
    template<class T>
    const T& Min(const T& a,const T& b)
    {
      
      return (a	< b) ? a : b;
    }
    
    template<class T>
    const T& Max(const T & a,const T& b)
    {
      return (a < b) ? b : a;
    }
  
    template<class T>
    const T& Clamp(const T& val,const T& minV,const T& maxV)
    {
      return Min(Max(val,maxV),minV);
    }
  

}