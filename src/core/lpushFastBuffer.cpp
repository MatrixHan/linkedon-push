#include <lpushFastBuffer.h>

#include <lpushLogger.h>
#include <lpushSystemErrorDef.h>

// the default recv buffer size, 128KB.
#define LP_DEFAULT_RECV_BUFFER_SIZE 131072

// limit user-space buffer to 256KB, for 3Mbps stream delivery.
//      800*2000/8=200000B(about 195KB).
// @remark it's ok for higher stream, the buffer is ok for one chunk is 256KB.
#define LP_MAX_SOCKET_BUFFER 262144

namespace lpush 
{
  
LPushFastBuffer::LPushFastBuffer()
{
    nb_buffer = LP_DEFAULT_RECV_BUFFER_SIZE;
    start = (char*)malloc(nb_buffer);
    p = end = start;
}

LPushFastBuffer::~LPushFastBuffer()
{
    SafeDelete(start);
}

  
int LPushFastBuffer::size()
{
    return (int)(end-p);
}

char* LPushFastBuffer::bytes()
{
    return p;
}

void LPushFastBuffer::set_buffer(int buffer_size)
{
    if(buffer_size>LP_MAX_SOCKET_BUFFER)
    {
      lp_warn("limit the user-space buffer from %d to %d", 
            buffer_size, LP_MAX_SOCKET_BUFFER);
    }
    
    int nb_resize_buf = Min(buffer_size,LP_MAX_SOCKET_BUFFER);
    
    if (nb_resize_buf <= nb_buffer) {
        return;
    }
    
    int startn = (int)(p-start);
    int nb_bytes = (int)(end-p);
    start = (char*)realloc(start,nb_resize_buf);
    p = start+startn;
    end = p+nb_bytes;
}

char LPushFastBuffer::read_1byte()
{
    assert(end-p>=1);
    return *p++;
}
char* LPushFastBuffer::read_slice(int size)
{
    assert(size>0);
    assert(end-p>=size);
    assert(p+size>=start);
    char *ptr = p;
    p += size;
    return ptr;
}



void LPushFastBuffer::skip(int size)
{
    assert(end-p>=size);
    assert(p+size>=start);
    p += size;
}

int LPushFastBuffer::grow(ILPushBufferReader* skt, int required_size)
{
    int ret = ERROR_SUCCESS;
    
    if(end - p >= required_size)
    {
	return ret;
    }
    
    assert(required_size>0);
    
    int nb_free_space = (int)(start + nb_buffer -end);
    
    if(nb_free_space <required_size)
    {
	int nb_exists_bytes = (int)(end-p);
	assert(nb_exists_bytes>=0);
	lp_verbose("move fast buffer %d bytes",nb_exists_bytes);
	
	if(!nb_exists_bytes)
	{
	  p = end =start;
	  lp_verbose("all consumed, reset fast buffer");
	}else
	{
	    assert(nb_exists_bytes < nb_buffer);
            start = (char*)memmove(start, p, nb_exists_bytes);
            p = start;
            end = p + nb_exists_bytes;
	}
	
	// check whether enough free space in buffer.
        nb_free_space = (int)(start + nb_buffer - end);
        if (nb_free_space < required_size) {
            ret = ERROR_READER_BUFFER_OVERFLOW;
            lp_error("buffer overflow, required=%d, max=%d, left=%d, ret=%d", 
                required_size, nb_buffer, nb_free_space, ret);
            return ret;
        }
    }
    
    while (end - p < required_size) {
        ssize_t nread;
        if ((ret = skt->read(end, nb_free_space, &nread)) != ERROR_SUCCESS) {
            return ret;
        }
        // we just move the ptr to next.
        assert((int)nread > 0);
        end += nread;
        nb_free_space -= nread;
    }
    return ret;
    
}

  
  
}