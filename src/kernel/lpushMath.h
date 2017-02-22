#pragma once

#include <lpushCommon.h>

namespace lpush 
{
  
  extern long getDecimal(double src);
  
  extern int getDecimal(float src);
  
  extern long getInteger(double src);
  
  extern int getInteger(float src);
  
  extern std::string md5Encoder(std::string src);
  
  extern bool empty(std::string data);
  
  extern bool empty(int data);
  
  extern std::string uint8To2Char(uint8_t src);
  
  
}