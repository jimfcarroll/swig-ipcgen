#include "MessageChannel.h"

using namespace XbmcIpc;

std::string childwrap_func(MessageChannel& calling, MessageChannel& returning, int p_arg1)
{
  Message message(1024);

  message.putInt(0);// function index
  message.putInt(p_arg1);

  message.flip(); // prepare for reading
  calling.send(message);

  std::string result;
  message.clear(); // prepare to read the results
  returning.receive(message);
  message.flip();
  result = message.getString();
  return result;
}

int childwrap_func2(MessageChannel& calling, MessageChannel& returning, const char* p_arg1)
{
  Message message(1024);

  message.putInt(1);// function index
  message.putString(p_arg1);

  message.flip();
  calling.send(message);

  int result;

  message.clear();
  returning.receive(message);
  message.flip();
  result = message.getInt();

  return result;
}

