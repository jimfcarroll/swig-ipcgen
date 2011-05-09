#include "MessageChannel.h"

using namespace XBMCIPC;

struct func_params
{
  int arg1;
};

struct func_message
{
  int functionIndex;
  struct func_params params;
};

int childwrap_func(MessageChannel& channel, int p_arg1)
{
  func_message message;

  message.functionIndex = 0;
  message.params.arg1 = p_arg1;

  channel.send(&message,sizeof(func_message));
}

