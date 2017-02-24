#include <lpushMath.h>
#include <openssl/md5.h>
#include <lpushLogger.h>

namespace lpush 
{
  
int getDecimal(float src)
{
    float t = src - (int)src; //小数点前变成0了
    while ( t > 0 )
    {
	t *= 10;
	std::cout << (int)t;
	t = t - (int)t;
    }
    return t;
}

long getDecimal(double src)
{
    double t = src - (long)src; //小数点前变成0了
    while ( t > 0 )
    {
	t *= 10;
	std::cout << (long)t;
	t = t - (long)t;
    }
    return t;
}

int getInteger(float src)
{
    return (int)src;
}

long getInteger(double src)
{
    return (long)src;
}


std::string md5Encoder(std::string src)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    const char *str = src.c_str();
    
    MD5((unsigned char*)str,strlen(str),(unsigned char*)&digest);
    
    char mdString[33];
    for(int i = 0; i < 16; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
 
    lp_info("md5 digest: %s\n", mdString);
    
    return std::string(mdString);
}

bool empty(int data)
{
    return data <= 0;
}

bool empty(std::string data)
{
    return data.empty();
}

std::string uint8To2Char(uint8_t src)
{
    char bufa[2];
    static char buf[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    bufa[0] = buf[(src>>4) & 0x0f];
    bufa[1] = buf[src & 0x0f];
    bufa[2] = '\0';
    //std::cout << bufa[0] <<":" <<bufa[1] << std::endl;
    
    return std::string(bufa);
}

bool isIntergeStr(std::string src)
{
    static char buf[16] = {'0','1','2','3','4','5','6','7','8','9'};
    const char *a = src.c_str();
    int len = src.length();
    int plen = 0;
    for(int i=0;i<src.size();i++)
    {
	for(int j=0;j<10;j++)
	{
	    if(a[i]==buf[j])
	    {
	        plen++;
	    }
	}
    }
    if(len == plen)
	return true;
    else
	return false;
}

  
}