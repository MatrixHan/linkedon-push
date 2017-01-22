#include <lpushCommon.h>
#include <lpushLogger.h>

using namespace lpush;
static int errfd        = STDERR_FILENO;
static void start_daemon(void);

int main(void)
{
	printf(" Hello world lpush!\n");
	
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

  if (chdir(DEFAULT_LOG_FILE_NAME) < 0)
    err_sys_quit(errfd, "ERROR: can't change directory to %s: chdir", DEFAULT_LOG_FILE_NAME);
}