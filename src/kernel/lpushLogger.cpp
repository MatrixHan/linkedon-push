#include <lpushLogger.h>
#include <lpushUtils.h>
#include <lpushSystemErrorDef.h>
#include <sys/time.h>
#include <st.h>

namespace lpush 
{
  LPushFastLog *_lpush_log = NULL;
    
bool InitLog(const std::string& filename)
{
	if (!_lpush_log) {
		_lpush_log = new LPushFastLog();
		_lpush_log->initialize();
	}
	return true;
}


void CloseLog()
{
	SafeDelete(_lpush_log);
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


// the max size of a line of log.
#define LOG_MAX_SIZE 4096

// the tail append to each log.
#define LOG_TAIL '\n'
// reserved for the end of log data, it must be strlen(LOG_TAIL)
#define LOG_TAIL_SIZE 1

LPushFastLog::LPushFastLog()
{
    _level = Error;
    log_data = new char[LOG_MAX_SIZE];

    fd = -1;
    log_to_file_tank = false;
    utc = false;
}

LPushFastLog::~LPushFastLog()
{
    SafeDelete(log_data);

    if (fd > 0) {
        ::close(fd);
        fd = -1;
    }

}

int LPushFastLog::initialize()
{
    int ret = ERROR_SUCCESS;
    log_to_file_tank = true;
    
    
    return ret;
}

void LPushFastLog::verbose(const char* tag, int context_id, const char* fmt, ...)
{
    if (_level > Verbose) {
        return;
    }
    
    int size = 0;
    if (!generate_header(false, tag, context_id, "verb", &size)) {
        return;
    }
    
    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(log_data + size, LOG_MAX_SIZE - size, fmt, ap);
    va_end(ap);

    write_log(fd, log_data, size, Verbose);
}

void LPushFastLog::info(const char* tag, int context_id, const char* fmt, ...)
{
    if (_level > Info) {
        return;
    }
    
    int size = 0;
    if (!generate_header(false, tag, context_id, "debug", &size)) {
        return;
    }
    
    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(log_data + size, LOG_MAX_SIZE - size, fmt, ap);
    va_end(ap);

    write_log(fd, log_data, size, Info);
}

void LPushFastLog::trace(const char* tag, int context_id, const char* fmt, ...)
{
    if (_level > Trace) {
        return;
    }
    
    int size = 0;
    if (!generate_header(false, tag, context_id, "trace", &size)) {
        return;
    }
    
    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(log_data + size, LOG_MAX_SIZE - size, fmt, ap);
    va_end(ap);

    write_log(fd, log_data, size, Trace);
}

void LPushFastLog::warn(const char* tag, int context_id, const char* fmt, ...)
{
    if (_level > Warn) {
        return;
    }
    
    int size = 0;
    if (!generate_header(true, tag, context_id, "warn", &size)) {
        return;
    }
    
    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(log_data + size, LOG_MAX_SIZE - size, fmt, ap);
    va_end(ap);

    write_log(fd, log_data, size, Warn);
}

void LPushFastLog::error(const char* tag, int context_id, const char* fmt, ...)
{
    if (_level > Error) {
        return;
    }
    
    int size = 0;
    if (!generate_header(true, tag, context_id, "error", &size)) {
        return;
    }
    
    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(log_data + size, LOG_MAX_SIZE - size, fmt, ap);
    va_end(ap);

    // add strerror() to error msg.
    if (errno != 0) {
        size += snprintf(log_data + size, LOG_MAX_SIZE - size, "(%s)", strerror(errno));
    }

    write_log(fd, log_data, size, Error);
}



bool LPushFastLog::generate_header(bool error, const char* tag, int context_id, const char* level_name, int* header_size)
{
    // clock time
    timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return false;
    }
    
    // to calendar time
    struct tm* tm;
    if (utc) {
        if ((tm = gmtime(&tv.tv_sec)) == NULL) {
            return false;
        }
    } else {
        if ((tm = localtime(&tv.tv_sec)) == NULL) {
            return false;
        }
    }
    
    // write log header
    int log_header_size = -1;
    
    if (error) {
        if (tag) {
            log_header_size = snprintf(log_data, LOG_MAX_SIZE, 
                "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%s][%d][%d][%d] ", 
                1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000), 
                level_name, tag, getpid(), context_id, errno);
        } else {
            log_header_size = snprintf(log_data, LOG_MAX_SIZE, 
                "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%d][%d] ", 
                1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000), 
                level_name, getpid(), context_id, errno);
        }
    } else {
        if (tag) {
            log_header_size = snprintf(log_data, LOG_MAX_SIZE, 
                "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%s][%d][%d] ", 
                1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000), 
                level_name, tag, getpid(), context_id);
        } else {
            log_header_size = snprintf(log_data, LOG_MAX_SIZE, 
                "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%d] ", 
                1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000), 
                level_name, getpid(), context_id);
        }
    }

    if (log_header_size == -1) {
        return false;
    }
    
    // write the header size.
    *header_size = Min(LOG_MAX_SIZE - 1, log_header_size);
    
    return true;
}

void LPushFastLog::write_log(int& fd, char *str_log, int size, int level)
{
    // ensure the tail and EOF of string
    //      LOG_TAIL_SIZE for the TAIL char.
    //      1 for the last char(0).
    size = Min(LOG_MAX_SIZE - 1 - LOG_TAIL_SIZE, size);
    
    // add some to the end of char.
    str_log[size++] = LOG_TAIL;
    
    // if not to file, to console and return.
    if (!log_to_file_tank) {
        // if is error msg, then print color msg.
        // \033[31m : red text code in shell
        // \033[32m : green text code in shell
        // \033[33m : yellow text code in shell
        // \033[0m : normal text code
        if (level <= Trace) {
            printf("%.*s", size, str_log);
        } else if (level == Warn) {
            printf("\033[33m%.*s\033[0m", size, str_log);
        } else{
            printf("\033[31m%.*s\033[0m", size, str_log);
        }

        return;
    }
    
    // open log file. if specified
    if (fd < 0) {
        open_log_file();
    }
    
    // write log to file.
    if (fd > 0) {
        write(fd, str_log, size);
    }
}

void LPushFastLog::open_log_file()
{
    
    std::string filename = std::string(DEFAULT_LOG_FILE_NAME);
    
    if (filename.empty()) {
        return;
    }
    
    fd = ::open(filename.c_str(), O_RDWR | O_APPEND);
    
    if(fd == -1 && errno == ENOENT) {
        fd = open(filename.c_str(), 
            O_RDWR | O_CREAT | O_TRUNC, 
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
        );
    }
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