#pragma once 

#include <lpushCommon.h>


#define LP_LOG_LEVEL  0

#define LP_LOG     1



namespace lpush
{
  class LPushFastLog;
  
    static const int Verbose = 0x01;
    static const int Info = 0x02;
    static const int Trace = 0x03;
    static const int Warn = 0x04;
    static const int Error = 0x05;
    // specified the disabled level, no log, for utest.
    static const int Disabled = 0x06;
  
  
  bool InitLog(const std::string &filename);
  
  void CloseLog();

  
/**
* we use memory/disk cache and donot flush when write log.
* it's ok to use it without config, which will log to console, and default trace level.
* when you want to use different level, override this classs, set the protected _level.
*/
class LPushFastLog 
{
// for utest to override
protected:
    // defined in LPushLogLevel.
    int _level;
private:
    char* log_data;
    // log to file if specified lp_log_file
    int fd;
    // whether log to file tank
    bool log_to_file_tank;
    // whether use utc time.
    bool utc;
    
    std::string logdir;
    
    std::string logfilename;
public:
    LPushFastLog();
    virtual ~LPushFastLog();
public:
    virtual int initialize();
    virtual void verbose(const char* tag, int context_id, const char* fmt, ...);
    virtual void info(const char* tag, int context_id, const char* fmt, ...);
    virtual void trace(const char* tag, int context_id, const char* fmt, ...);
    virtual void warn(const char* tag, int context_id, const char* fmt, ...);
    virtual void error(const char* tag, int context_id, const char* fmt, ...);

private:
    virtual bool generate_header(bool error, const char* tag, int context_id, const char* level_name, int* header_size);
    virtual void write_log(int& fd, char* str_log, int size, int level);
    virtual void open_log_file();
};

  // the context for multiple clients.
class ILogContext
{
public:
    ILogContext();
    virtual ~ILogContext();
public:
    virtual void generate_id() = 0;
    virtual int get_id() = 0;
public:
    virtual const char* format_time() = 0;
};


extern LPushFastLog* _lpush_log;
// user must implements the LogContext and define a global instance.
extern ILogContext* log_context;

// donot print method
#if LP_LOG_LEVEL == 0
    #define lp_verbose(msg, ...) _lpush_log->verbose(NULL, log_context->get_id(), msg, ##__VA_ARGS__)
    #define lp_info(msg, ...)    _lpush_log->info(NULL, log_context->get_id(), msg, ##__VA_ARGS__)
    #define lp_trace(msg, ...)   _lpush_log->trace(NULL, log_context->get_id(), msg, ##__VA_ARGS__)
    #define lp_warn(msg, ...)    _lpush_log->warn(NULL, log_context->get_id(), msg, ##__VA_ARGS__)
    #define lp_error(msg, ...)   _lpush_log->error(NULL, log_context->get_id(), msg, ##__VA_ARGS__)
// use __FUNCTION__ to print c method
#elif LP_LOG_LEVEL == 1
    #define lp_verbose(msg, ...) printf("[%s][%d][verbs][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define lp_info(msg, ...)    printf("[%s][%d][infos][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    // #define lp_info(msg, ...)
    #define lp_trace(msg, ...)   printf("[%s][%d][trace][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define lp_warn(msg, ...)    printf("[%s][%d][warns][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
    #define lp_error(msg, ...)   printf("[%s][%d][error][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
// use __PRETTY_FUNCTION__ to print c++ class:method
#elif LP_LOG_LEVEL == 2
    #define lp_verbose(msg, ...) 
    #define lp_info(msg, ...)    
    #define lp_trace(msg, ...)   
    #define lp_warn(msg, ...)    
    #define lp_error(msg, ...)   
#elif LP_LOG_LEVEL == 3 
    #define lp_verbose(msg, ...)
    #define lp_info(msg, ...)    
    // #define lp_info(msg, ...)
    #define lp_trace(msg, ...)   printf("[%s][%d][trace][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define lp_warn(msg, ...)    
    #define lp_error(msg, ...)  
#else
    #define lp_verbose(msg, ...) printf("[%s][%d][verbs][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define lp_info(msg, ...)    printf("[%s][%d][infos][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define lp_trace(msg, ...)   printf("[%s][%d][trace][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define lp_warn(msg, ...)    printf("[%s][%d][warns][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
    #define lp_error(msg, ...)   printf("[%s][%d][error][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
#endif
  



extern void handle_session(long srv_socket_index, st_netfd_t cli_nfd);
extern void load_configs(void);
extern void logbuf_open(void);
extern void logbuf_flush(void);
extern void logbuf_close(void);

/* Error reporting functions defined in the error.c file */
extern void err_sys_report(int fd, const char *fmt, ...);
extern void err_sys_quit(int fd, const char *fmt, ...);
extern void err_sys_dump(int fd, const char *fmt, ...);
extern void err_report(int fd, const char *fmt, ...);
extern void err_quit(int fd, const char *fmt, ...);
}
