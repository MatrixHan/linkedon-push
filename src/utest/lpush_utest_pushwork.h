#pragma once
#include <lpushCommon.h>
#include <uuid/uuid.h>

namespace lpush {

class LPushWork{
public:
int pushWork(int loopNum);
int pushWork(std::string user);

std::string uuidinit();
  
};

}