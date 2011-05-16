
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "MessageChannel.h"

// 64-bit = worst case word align
#define WORDALIGN 8

#define BLOCK_DECR(semnum) \
    op.sem_num = semnum; \
    op.sem_op = -1; \
    if (semop(semid,&op,1) < 0) \
      status |= XSEM_FAILED

#define INCR(semnum) \
    op.sem_num = semnum; \
    op.sem_op = 1; \
    if (semop(semid,&op,1) < 0) \
      status |= XSEM_FAILED

#define WAIT_FOR_NONE(semnum) \
    op.sem_num = 0; \
    op.sem_op = 0; \
    if (semop(semid,&op,1) < 0) \
      status |= XSEM_FAILED

#define SHM_MUTEX 1

namespace XbmcIpc
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

    // try to get the sem ... see if it already exists
    semid = semget(ckey.semKey,0,0);

    if (semid != -1)
      semctl(semid,0,IPC_RMID);

    semid = semget(ckey.semKey,2,IPC_CREAT | 0777);

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

  key_t MessageChannel::ChannelKey::makeKey(unsigned char channelNumber, char c3)
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

  void MessageChannel::receive(Message& messageToFill)
  {
    status = 0;

    BLOCK_DECR(0); // wait for a message
    BLOCK_DECR(SHM_MUTEX); // grab access to the shm

    int len;
    memcpy(&len, data, sizeof(int));
//    printf("receive: read %i from len\n",len);

    if (len > 0)
      messageToFill.put( ((unsigned char*)data) + WORDALIGN, len);

//    printf("receive: writing 0 to len\n");
    memset(data, 0, sizeof(int)); // clear the len

    // OK, reset the read sem
    INCR(SHM_MUTEX); // release access to the shm
  }

  void MessageChannel::send(const void* message, int len)
  {
    status = 0;

    BLOCK_DECR(SHM_MUTEX); // grab access to the read

    // I have access now so I can go ahead and set the message
    // write data
//    printf("send: writing %i to len\n",len);
    memcpy(data,&len,sizeof(int));
    memcpy(((unsigned char*)data) + WORDALIGN,message,len);

    INCR(0); // flag a message available
    INCR(SHM_MUTEX); // release the access
  }
}
