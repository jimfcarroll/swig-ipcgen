
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "MessageChannel.h"

namespace XBMCIPC
{

  unsigned char MessageChannel::sequence = 0;

  MessageChannel::MessageChannel() : status(0), ckey(sequence++)
  {
    memset(&op,0,sizeof(sembuf));
    semval = 0;

    shmid = shmget(ckey.shmKey,1024,IPC_CREAT | 0666);

    if (shmid < 0)
    {
      status |= XSHM_FAILED;
      return;
    }

    semid = semget(ckey.semKey,1,IPC_CREAT | 0777);

    if (semid < 0)
    {
      status |= XSEM_FAILED;
      return;
    }

    if (semctl( semid, 1, SETALL, &semval) < 0)
    {
      status |= XSEM_FAILED;
      return;
    }

    data = shmat(shmid,NULL,0);
  }

  static key_t makeKey(unsigned char channelNumber, char c3)
  {
    union 
    {
      char c[sizeof(key_t)];
      key_t k;
    } myKey;

    memset(&myKey,0,sizeof(key_t));
    myKey.c[0] = (char)channelNumber;
    myKey.c[1] = 'X';
    myKey.c[2] = 'B';
    myKey.c[3] = c3;
    if (sizeof(key_t) > 4)
      myKey.c[4] = 'C';

    return myKey.k;
  }

  key_t MessageChannel::ChannelKey::makeShmKey(unsigned char channelNumber) { return makeKey(channelNumber,'M'); }
  key_t MessageChannel::ChannelKey::makeSemKey(unsigned char channelNumber) { return makeKey(channelNumber,'S'); }

  void* MessageChannel::receive()
  {
    status = 0;
    op.sem_num = 0;
    op.sem_op = -1;

    if (semop(semid,&op,1) < 0)
    {
      status |= XSEM_FAILED;
      return NULL;
    }

    return data;
  }

  void MessageChannel::send(const void* message, int len)
  {
    status = 0;
    op.sem_num = 0;
    op.sem_op = 1;
    if (semop(semid,&op,1) < 0)
      status |= XSEM_FAILED;
    else
      memcpy(data,message,len);
  }

}
