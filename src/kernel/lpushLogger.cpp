#include <lpushLogger.h>
#include <lpushUtils.h>
#include <sys/time.h>
#include <st.h>

namespace lpush 
{
    std::ofstream *GLogStream = NULL;
    
bool InitLog(const std::string& filename)
{
      if (GLogStream == NULL)
      {
	GLogStream = new std::ofstream();
	GLogStream->open(filename.c_str());
	if(GLogStream->good())
	{
	  return true;
	}
	return false;
      }
      return true;
}

void Log(const char* string, ...)
{
    if(GLogStream == NULL)
       return ;
    char buffer[256];
    if (!string || !GLogStream)
      return ;
    
    va_list arglist;
    va_start(arglist,string);
    vsprintf(buffer,string,arglist);
    va_end(arglist);
    
    std::string info(buffer);
    *GLogStream <<now()<<":"<< info << std::endl;
    GLogStream->flush();
}

void LogBS(const std::string str)
{
  Log(str.c_str());
}


void CloseLog()
{
      GLogStream->close();
      SafeDelete(GLogStream);
}

ILogContext::ILogContext()
{
}

ILogContext::~ILogContext()
{
}



class LogContext : public ILogContext
{
private:
	class DateTime
	{
	private:
	    // %d-%02d-%02d %02d:%02d:%02d.%03d
	    #define DATE_LEN 24
	    char time_data[DATE_LEN];
	public:
	    DateTime();
	    virtual ~DateTime();
	public:
	    virtual const char* format_time();
	};
private:
    DateTime time;
    std::map<st_thread_t, int> cache;
public:
    LogContext();
    virtual ~LogContext();
public:
    virtual void generate_id();
    virtual int get_id();
public:
    virtual const char* format_time();
};

ILogContext* log_context = new LogContext();

LogContext::DateTime::DateTime()
{
    memset(time_data, 0, DATE_LEN);
}

LogContext::DateTime::~DateTime()
{
}

const char* LogContext::DateTime::format_time()
{
    // clock time
    timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return "";
    }
    // to calendar time
    struct tm* tm;
    if ((tm = localtime(&tv.tv_sec)) == NULL) {
        return "";
    }
    
    // log header, the time/pid/level of log
    // reserved 1bytes for the new line.
    snprintf(time_data, DATE_LEN, "%d-%02d-%02d %02d:%02d:%02d.%03d", 
        1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, 
        (int)(tv.tv_usec / 1000));
        
    return time_data;
}

LogContext::LogContext()
{
}

LogContext::~LogContext()
{
}

void LogContext::generate_id()
{
    static int id = 1;
    cache[st_thread_self()] = id++;
}

int LogContext::get_id()
{
    return cache[st_thread_self()];
}

const char* LogContext::format_time()
{
    return time.format_time();
}


/*
 * Simple error reporting functions.
 * Suggested in W. Richard Stevens' "Advanced Programming in UNIX
 * Environment".
 */

#define MAXLINE 4096  /* max line length */

static void err_doit(int, int, const char *, va_list);


/*
 * Nonfatal error related to a system call.
 * Print a message and return.
 */
void err_sys_report(int fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(fd, 1, fmt, ap);
  va_end(ap);
}


/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void err_sys_quit(int fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(fd, 1, fmt, ap);
  va_end(ap);
  exit(1);
}


/*
 * Fatal error related to a system call.
 * Print a message, dump core, and terminate.
 */
void err_sys_dump(int fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(fd, 1, fmt, ap);
  va_end(ap);
  abort();  /* dump core and terminate */
  exit(1);  /* shouldn't get here */
}


/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void err_report(int fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(fd, 0, fmt, ap);
  va_end(ap);
}


/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void err_quit(int fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(fd, 0, fmt, ap);
  va_end(ap);
  exit(1);
}


/*
 * Return a pointer to a string containing current time.
 */
char *err_tstamp(void)
{
  static char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  static char str[32];
  static time_t lastt = 0;
  struct tm *tmp;
  time_t currt = st_time();

  if (currt == lastt)
    return str;

  tmp = localtime(&currt);
  sprintf(str, "[%02d/%s/%d:%02d:%02d:%02d] ", tmp->tm_mday,
          months[tmp->tm_mon], 1900 + tmp->tm_year, tmp->tm_hour,
          tmp->tm_min, tmp->tm_sec);
  lastt = currt;

  return str;
}


/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
static void err_doit(int fd, int errnoflag, const char *fmt, va_list ap)
{
  int errno_save;
  char buf[MAXLINE];

  errno_save = errno;         /* value caller might want printed   */
  strcpy(buf, err_tstamp());  /* prepend a message with time stamp */
  vsprintf(buf + strlen(buf), fmt, ap);
  if (errnoflag)
    sprintf(buf + strlen(buf), ": %s\n", strerror(errno_save));
  else
    strcat(buf, "\n");
  write(fd, buf, strlen(buf));
  errno = errno_save;
}



}