#include <lpushCommon.h>
#include <lpushUtils.h>
#include <lpushJson.h>
#include <lpushLogger.h>
#include <lpushServer.h>
#include <lpushSystemErrorDef.h>
using namespace std;
using namespace lpush;

LPushServer *server = new LPushServer();


static int errfd = STDERR_FILENO;
static void start_daemon(void);
int run();
int main(void)
{
  
	int ret = ERROR_SUCCESS;
	if (!checkProDir())
	{
	   lp_trace("place move project root direct!\n");
	   return 0;
	}	
	initConfig();
	InitLog(DEFAULT_LOG_FILE_NAME);
	if ((ret = server->initializer()) != ERROR_SUCCESS)
	{
	  return ret;
	}
	//start_daemon();
	run();
	CloseLog();
	return 0;
}

static void start_daemon(void)
{
  pid_t pid;

  /* Start forking */
  if ((pid = fork()) < 0)
    err_sys_quit(errfd, "ERROR: fork");
  if (pid > 0)
    exit(0);                  /* parent */

  /* First child process */
  setsid();                   /* become session leader */

  if ((pid = fork()) < 0)
    err_sys_quit(errfd, "ERROR: fork");
  if (pid > 0)                /* first child */
    exit(0);

  umask(022);
  if (chdir("./") < 0)
    err_sys_quit(errfd, "ERROR: can't change directory to %s: chdir", "./");
}

int run()
{
    int ret = ERROR_SUCCESS;
    lp_info("lpush master run !\n");
    lp_info("lpush server initializer_st!\n");
    if ((ret = server->initializer_st()) != ERROR_SUCCESS)
    {
       return ret;
    }
    lp_info("lpush server acquire_pid_file!\n");
    if ((ret = server->acquire_pid_file()) != ERROR_SUCCESS)
    {
       return ret;
    }
    lp_info("lpush server signal_init !\n");
    if ((ret = server->signal_init()) != ERROR_SUCCESS)
    {
       return ret;
    }
     lp_info("lpush server signal_register !\n");
    if ((ret = server->signal_register()) != ERROR_SUCCESS)
    {
       return ret;
    }
    lp_info("lpush server listen !\n");
    if ((ret = server->listen()) != ERROR_SUCCESS)
    {
       return ret;
    }
    lp_info("lpush server cycle !\n");
    if ((ret = server->cycle()) != ERROR_SUCCESS)
    {
       return ret;
    }
    
    return ret;
}