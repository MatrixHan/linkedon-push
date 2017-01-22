#pragma once

#include <lpushCommon.h>
#include <lpushLogger.h>
#define BUFLEN 255   

namespace lpush 
{
 
    long long getTimeDec();
 
  
  inline std::string FloatToString(float num)
  {
    std::stringstream ss;
    ss << num;
    std::string re;
    ss >> re;
    return re;
  }
  
  inline std::string IntToString(int num)
  {
    std::stringstream ss;
    ss << num;
    std::string re;
    ss >> re;
    return re;
  }
  
  inline std::string LongToString(long long num)
  {
    std::ostringstream ss;
    ss << num;
    std::string result;  
    std::istringstream is(ss.str());  
    is>>result; 
    return result;
  }
  
  inline int RandomInt(int from = 0, int to = 10)
  {
      int ran = rand() % (to - from +1) + from;
      return ran;
  }
  
  inline int StringToInt(const std::string &str)
  {
      return atoi(str.c_str());
  }
  
  inline float StringToFloat(const std::string &str)
  {
      return (float)atof(str.c_str());
  }
  
  inline bool EqualFloat(float l,float r)
  {
      return Abs(l-r)<=EPSILON_E6;
  }
  
  inline std::string getNameFromPath(const std::string &path)
  {
      std::size_t beg = path.find_last_of("\\/");
      std::size_t end = path.find_last_of(".");
      if(beg == std::string::npos)
      {
	
	beg = -1;
      }
      return path.substr(beg+1,end-beg-1);
  }
  
  inline std::string Trim(const std::string &msg)
  {
      const static std::string SPACE_CHAR = " \t\f\v\n\r";
      std::size_t beg = msg.find_first_not_of(SPACE_CHAR);
      
      if(beg > msg.length())
      {
	return std::string();
      }
      std::string result = msg.substr(beg);
      
      std::size_t end = result.find_last_not_of(SPACE_CHAR);
      
      if(end != std::string::npos)
	  end++;
      return result.substr(0,end);
    
  }
  
  std::string timeFormat(long long timedec);
  
  
  std::string now();
  
  long long getCurrentTime();
  
}