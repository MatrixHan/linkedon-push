#pragma once
#include <lpushCommon.h>
#include <lpushSocket.h>


namespace lpush 
{

class LPushFastBuffer
{
private:
  // *start buffer begin point 
  // *end buffer max size end point
  // *p current point 
  char *start,*end,*p;
  
  //the size of start
  int nb_buffer;
  
public:
  LPushFastBuffer();
  virtual ~LPushFastBuffer();
public:
  
  virtual int size();
  
  virtual char * bytes();
  
  virtual void set_buffer(int buffer_size);
  
  virtual char read_1byte();
  
  virtual char * read_slice(int size);
  
  virtual void skip(int size);
  
  virtual int grow(ILPushBufferReader *skt,int requited_size);
};

  
}
