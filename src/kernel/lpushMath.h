#pragma once

#include <lpushCommon.h>

namespace lpush 
{
  
  extern long getDecimal(double src);
  
  extern int getDecimal(float src);
  
  extern long getInteger(double src);
  
  extern int getInteger(float src);
  
  extern std::string md5Encoder(std::string src);
  
}