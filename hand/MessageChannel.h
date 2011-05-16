
#pragma once

#include <sys/types.h>
#include <sys/sem.h>

#include "Message.h"

namespace XbmcIpc
{

  /**
   * This class provides an abstraction for a message channel between 
   *  two processes. Internally it's built on two IPC mechanisms:
   *  shared memory, and ipc semaphores.
   */
  class MessageChannel
  {
    int shmid;
    int semid;
    int status;

    void* data;
    sembuf op;
    short semval;

    void send(const void* data, int len);
  public:
    static const int XSHM_FAILED = 0x01;
    static const int XSEM_FAILED = 0x02;
    static unsigned char sequence;

    class ChannelKey
    {
      key_t semKey;
      key_t shmKey;

      friend class MessageChannel;

    public:
      static key_t makeKey(unsigned char channelNumber,char c3);

      ChannelKey(unsigned char channelNumber) : semKey(makeKey(channelNumber,'S')),
                                                shmKey(makeKey(channelNumber,'M')) {}
    };

    ChannelKey ckey;

    MessageChannel();
    bool isOk() { return status == 0; }
    void receive(Message& messageToFill);
    void send(const Message& message) { send(message.array(), message.limit());  }
  };
}
