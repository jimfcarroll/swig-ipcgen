
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

#include "MessageChannel.h"

using namespace XbmcIpc;

std::string childwrap_func(MessageChannel& channel, MessageChannel& returning, int p_arg1);
int childwrap_func2(MessageChannel& channel, MessageChannel& returning, const char* p_arg1);
void handleMessage(MessageChannel& mc, MessageChannel& returning);

int main(int argc, char** argv)
{
  pid_t childpid;

  MessageChannel calling;

  if (!calling.isOk())
  {
    perror("MessageChannel calling failed");
    exit(1);
  }

  MessageChannel returning;

  if (!returning.isOk())
  {
    perror("MessageChannel returning failed");
    exit(1);
  }

  if((childpid = fork()) < 0)
  {
    perror("Fork Failed");
    exit(1);
  }

  if(childpid != 0) // child process
  {
    std::string rets = childwrap_func(calling,returning,27);
    printf("func returned %s\n", rets.c_str());

    int reti = childwrap_func2(calling,returning,"my string.");
    printf("func2 returned %i\n", reti);
  }
  else // parent process
  {
    handleMessage(calling,returning);
    handleMessage(calling,returning);
  }
}
