#include <lpushProtocolStack.h>

#include <lpushLogger.h>
#include <lpushSocket.h>
#include <lpushFastBuffer.h>

namespace lpush 
{
LPushProtocol::LPushProtocol(ILPushProtocolReaderWriter* skt)
{
    lst=skt;
    fast_buffer = new LPushFastBuffer();
}
LPushProtocol::~LPushProtocol()
{
    
}


  
}