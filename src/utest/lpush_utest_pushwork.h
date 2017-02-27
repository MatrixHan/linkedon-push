#pragma once
#include <lpushCommon.h>
#include <uuid.h>

namespace lpush {

class LPushWork{
public:
int pushWork(int loopNum);
int pushWork(std::string user);

void testJson();

std::string uuidinit();
  
};

}