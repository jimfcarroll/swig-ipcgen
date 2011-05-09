
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "MessageChannel.h"

using namespace XBMCIPC;

int childwrap_func(MessageChannel& channel, int p_arg1);
void handleMessage(MessageChannel& mc);

int main(int argc, char** argv)
{
  pid_t childpid;

  MessageChannel channel;

  if (!channel.isOk())
  {
    perror("MessageChannel failed");
    exit(1);
  }

  if((childpid = fork()) < 0)
  {
    perror("Fork Failed");
    exit(1);
  }

  if(childpid == 0) // child process
  {
    childwrap_func(channel,27);
  }
  else // parent process
  {
    handleMessage(channel);
  }
}
