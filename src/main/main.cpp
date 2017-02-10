#include <lpushCommon.h>
#include <lpushUtils.h>
#include <lpushJson.h>
#include <lpushLogger.h>
#include <lpushProtocolStack.h>
using namespace std;
using namespace lpush;
static int errfd        = STDERR_FILENO;
static void start_daemon(void);
void run();
int main(void)
{
	if(!checkProDir())
	{
	   lp_trace("place move project root direct!\n");
	   return 0;
	}
	
	initConfig();
	InitLog(DEFAULT_LOG_FILE_NAME);

	start_daemon();
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

void run()
{
    LogBS(" Hello world lpush!\n");
    
}