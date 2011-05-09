
#pragma once

#include <sys/types.h>
#include <sys/sem.h>

namespace XBMCIPC
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
      static key_t makeShmKey(unsigned char channelNumber);
      static key_t makeSemKey(unsigned char channelNumber);

      ChannelKey(unsigned char channelNumber) : semKey(makeSemKey(channelNumber)), shmKey(makeShmKey(channelNumber)) {}
    };

    ChannelKey ckey;

    MessageChannel();
    bool isOk() { return status == 0; }
    void* receive();
    void send(const void* data, int len);
  };
}
