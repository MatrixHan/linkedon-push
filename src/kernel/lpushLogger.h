#pragma once 

#include <lpushCommon.h>


#define LP_LOG_LEVEL  3



namespace lpush
{
  extern std::ofstream *GLogStream;
  
  bool InitLog(const std::string &filename);
  
  void Log(const char * string , ...);
  
  void LogBS(const std::string str);
  
  void CloseLog();
  
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

// user must implements the LogContext and define a global instance.
extern ILogContext* log_context;

// donot print method
#if LP_LOG_LEVEL == 0
    #define brs_verbose(msg, ...) printf("[%s][%d][verbs] ", log_context->format_time(), log_context->get_id());printf(msg, ##__VA_ARGS__);printf("\n")
    #define brs_info(msg, ...)    printf("[%s][%d][infos] ", log_context->format_time(), log_context->get_id());printf(msg, ##__VA_ARGS__);printf("\n")
    #define brs_trace(msg, ...)   printf("[%s][%d][trace] ", log_context->format_time(), log_context->get_id());printf(msg, ##__VA_ARGS__);printf("\n")
    #define brs_warn(msg, ...)    printf("[%s][%d][warns] ", log_context->format_time(), log_context->get_id());printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
    #define brs_error(msg, ...)   printf("[%s][%d][error] ", log_context->format_time(), log_context->get_id());printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
// use __FUNCTION__ to print c method
#elif LP_LOG_LEVEL == 1
    #define brs_verbose(msg, ...) printf("[%s][%d][verbs][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define brs_info(msg, ...)    printf("[%s][%d][infos][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    // #define brs_info(msg, ...)
    #define brs_trace(msg, ...)   printf("[%s][%d][trace][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define brs_warn(msg, ...)    printf("[%s][%d][warns][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
    #define brs_error(msg, ...)   printf("[%s][%d][error][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
// use __PRETTY_FUNCTION__ to print c++ class:method
#elif LP_LOG_LEVEL == 2
    #define brs_verbose(msg, ...) 
    #define brs_info(msg, ...)    
    #define brs_trace(msg, ...)   
    #define brs_warn(msg, ...)    
    #define brs_error(msg, ...)   
#elif LP_LOG_LEVEL == 3 
    #define brs_verbose(msg, ...)
    #define brs_info(msg, ...)    
    // #define brs_info(msg, ...)
    #define brs_trace(msg, ...)   printf("[%s][%d][trace][%s] ", log_context->format_time(), log_context->get_id(), __FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define brs_warn(msg, ...)    
    #define brs_error(msg, ...)  
#else
    #define brs_verbose(msg, ...) printf("[%s][%d][verbs][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define brs_info(msg, ...)    printf("[%s][%d][infos][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define brs_trace(msg, ...)   printf("[%s][%d][trace][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf("\n")
    #define brs_warn(msg, ...)    printf("[%s][%d][warns][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
    #define brs_error(msg, ...)   printf("[%s][%d][error][%s] ", log_context->format_time(), log_context->get_id(), __PRETTY_FUNCTION__);printf(msg, ##__VA_ARGS__);printf(" errno=%d(%s)", errno, strerror(errno));printf("\n")
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
