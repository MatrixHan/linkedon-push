#include <lpushUtils.h>


namespace lpush 
{
  long long int getTimeDec()
  {
	  register uint32_t lo, hi;
	  register unsigned long long o;
      __asm__ __volatile__ (
	  "rdtscp" : "=a"(lo), "=d"(hi)
	  );
	  o = hi;
	  o <<= 32;
	  return (o | lo);
  }

  std::string timeFormat(long long int timedec)
  {
	time_t tTime = time(NULL) ;   
	struct tm *tmTime;   //定义tm类型指针
	tmTime = localtime(&tTime);  //获取时间
	
	char tmpBuf[BUFLEN];   
	strftime(tmpBuf, BUFLEN, "%F %T", tmTime); //format date and time. 
	return std::string(tmpBuf);
	
  }



  std::string now()
  {
      return timeFormat(getTimeDec());
  }
  
  long long getCurrentTime()
  {
    time_t tTime = time(NULL) ; 
    return tTime;
  }

  std::string getCurrentDirect()
  {
    char filePath[260];
    getcwd(filePath,260);
    std::string rpath(filePath);
    return rpath;
  }

  bool checkProDir()
  {
    std::string proSuffix = LPUSH_PROJECT_NAME;
    std::string currentDir = getCurrentDirect();
    int currentNameSize = currentDir.size();
    int proNameSize = proSuffix.size();
    std::size_t index;
    if ((index = currentDir.find(proSuffix))!=std::string::npos)
    {
	if (index>0)
	{
	  if (currentDir.find(proSuffix,index+1)==std::string::npos)
	  if (currentNameSize-proNameSize-index==0)
	  {
	    return true;
	  }
	}
    }
    return false;
  }
  
}