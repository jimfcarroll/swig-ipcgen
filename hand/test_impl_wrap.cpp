
#include "test.h"

#include "MessageChannel.h"

using namespace XbmcIpc;

void implwrap_func(Message& message, Message& ret)
{
  int arg1;

  arg1 = message.getInt();

  std::string result;
  result = func(arg1);

  ret.putString(result);
}

void implwrap_func2(Message& message, Message& ret)
{
  std::string arg1;

  arg1 = message.getString();

  int result;
  result = func2(arg1.c_str());

  ret.putInt(result);
}


typedef void (*plainFunc)(Message&,Message&);
static plainFunc ftab[2] = { 
  (plainFunc)implwrap_func,
  (plainFunc)implwrap_func2 
};

void handleMessage(MessageChannel& calling, MessageChannel& returning)
{
  Message incomming(1024);
  Message outgoing(1024);
  calling.receive(incomming);
  incomming.flip();
  (*ftab[incomming.getInt()])(incomming,outgoing);

  outgoing.flip();
  returning.send(outgoing);
}




