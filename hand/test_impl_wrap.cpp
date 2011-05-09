
#include "test.h"

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

void implwrap_func(struct func_params* params)
{
  int arg1;

  arg1 = params->arg1;

  func(arg1);
}

typedef void (*plainFunc)(void*);
static plainFunc ftab[1] = { (plainFunc)implwrap_func };

//void setupFuncs()
//{
//  key_t shmkey = MessageChannel::ChannelKey::makeShmKey(-1);
//  shmid = shmget(shmkey,1024,IPC_CREAT | 0666);
//  void* data = shmat(shmid,NULL,0);
//  memset(data,0,1024);
//  memcpy(data,&ftab, sizeof(plainFunc) * 1);
//}

void handleMessage(MessageChannel& mc)
{
  func_message* message = (func_message*)mc.receive();
  (*ftab[message->functionIndex])(&(message->params));
}




