
#pragma once

#include <string.h>
#include <string>

namespace XbmcIpc
{
  class MessageException
  {
    std::string message;

  public:
    MessageException(const char* message_) : message(message_) {}
  };

  /**
   * This class is based on the java java.nio.Buffer class however, it 
   *  does not implement the 'mark' functionality. It's meant to
   *  hold a message for sending over the MessageChannel.
   *
   * [ the following is borrowed from the javadocs for java.nio.Buffer 
   * where it applies to this class]:
   *
   * A buffer is a linear, finite sequence of elements of a unspecified types.
   * Aside from its content, the essential properties of a buffer are its capacity, 
   *   limit, and position:
   *    A buffer's capacity is the number of elements it contains. The capacity 
   *      of a buffer is never negative and never changes.
   *
   *    A buffer's limit is the index of the first element that should not be 
   *      read or written. A buffer's limit is never negative and is never greater 
   *      than its capacity.
   *
   *     A buffer's position is the index of the next element to be read or written. 
   *       A buffer's position is never negative and is never greater than its limit.
   *
   * Invariants:
   *
   * The following invariant holds for the mark, position, limit, and capacity values:
   *
   *     0 <= mark <= position <= limit <= capacity 
   *
   * A newly-created buffer always has a position of zero and a limit set to the 
   * capacity. The initial content of a buffer is, in general, undefined. 
   *
   * Example:
   *  Message message(1024);
   *  message.putInt(1).putString("hello there").putLongLong( ((long long)2)^40 );
   *  message.flip();
   *  std::cout << "message contents:" << message.getInt() << ", ";
   *  std::cout << message.getCharPointerDirect() << ", ";
   *  std::cout << message.getLongLong() << std::endl;
   *
   * Note: the 'gets' are sensitive to the order-of-operations. Therefore, while
   *  the above is correct, it would be wrong to chain the output as follows:
   *
   *  std::cout << "message contents:" << message.getInt() << ", " << std::cout
   *      << message.getCharPointerDirect() << ", " << message.getLongLong() 
   *      << std::endl;
   *
   * This would result in the get's executing from right to left and therefore would
   *  produce totally erroneous results. This is also a problem when the values are
   *  passed to a method as in:
   *
   * printf("message contents: %d, \"%s\", %ll\n", message.getInt(), 
   *         message.getCharPointerDirect(), message.getLongLong());
   *
   * This would also produce erroneous results as they get's will be evaluated
   *   from right to left in the parameter list of printf.
   */
  class Message
  {
    typedef unsigned char byte;

    byte* buffer;
    size_t mposition;
    size_t mcapacity;
    size_t mlimit;
    bool owner;

    inline void check(int count) const
    { 
      if ((mposition + count) > mlimit) 
        throw MessageException("Message buffer overflow: Cannot add more data to the Message's buffer.");
    }

  public:
    /**
     * Construct a message given an externally managed memory buffer.
     * The ownership of the buffer is assumed to be the code that called
     * this constructor, therefore the Message descrutor will not free it.
     */
    Message(void* buffer_, size_t bufferSize) : buffer((byte*)buffer_), mcapacity(bufferSize), owner(false) { clear(); }

    /**
     * Construct a message buffer using the size buffer provided. The
     * buffer will be internally managed and will be freed upon 
     * destruction.
     */
    Message(size_t bufferSize) : buffer(new byte[bufferSize]), mcapacity(bufferSize), owner(true) { clear(); }
    ~Message() { if (owner) delete [] buffer; }

    /**
     * Flips this buffer. The limit is set to the current position 
     *   and then the position is set to zero. 
     *
     * After a sequence of channel-read or put operations, invoke this 
     *   method to prepare for a sequence of channel-write or relative 
     *   get operations. For example:
     *
     * buf.put(magic);    // Prepend header
     * in.read(buf);      // Read data into rest of buffer
     * buf.flip();        // Flip buffer
     * out.write(buf);    // Write header + data to channel
     *
     * This is used to prepare the Message for reading from after
     *  it has been written to.
     */
     Message& flip() { mlimit = mposition; mposition = 0; return *this; }

    /**
     *Clears this buffer. The position is set to zero, the limit 
     *  is set to the capacity.
     *
     * Invoke this method before using a sequence of channel-read 
     *  or put operations to fill this buffer. For example:
     *
     *     buf.clear();     // Prepare buffer for reading
     *     in.read(buf);    // Read data
     *
     * This method does not actually erase the data in the buffer, 
     *  but it is named as if it did because it will most often be used 
     *  in situations in which that might as well be the case. 
     */
    Message& clear() { mlimit = mcapacity; mposition = 0; return *this; }

    /**
     * This method resets the position to the beginning of the buffer
     *  so that it can be either reread or written to all over again.
     */
    Message& rewind() { mposition = 0; return *this; }

    Message& put(const void* src, size_t bytes) throw(MessageException) { check(bytes); memcpy( buffer + mposition, src, bytes); mposition += bytes; return *this; }
    Message& get(void* dest, size_t bytes) throw(MessageException) { check(bytes); memcpy( dest, buffer + mposition, bytes); mposition += bytes; return *this; }

    const byte* array() const { return buffer; }

    size_t limit() const { return mlimit; }
    size_t capacity() const { return mcapacity; }
    size_t position() const { return mposition; }

#define DEFAULTMESSAGERELATIVERW(name,type) \
    inline Message& put##name(const type & val) throw(MessageException) { return put(&val, sizeof(type)); } \
    inline type get##name() throw(MessageException) { type ret; get(&ret, sizeof(type)); return ret; }

    DEFAULTMESSAGERELATIVERW(Int,int);
    DEFAULTMESSAGERELATIVERW(Char,char);
    DEFAULTMESSAGERELATIVERW(Long,long);
    DEFAULTMESSAGERELATIVERW(Float,float);
    DEFAULTMESSAGERELATIVERW(Double,double);
    DEFAULTMESSAGERELATIVERW(Pointer,void*);
    DEFAULTMESSAGERELATIVERW(LongLong,long long);
#undef DEFAULTMESSAGERELATIVERW

    inline Message& putString(const char* str) throw (MessageException) { size_t len = strlen(str) + 1; check(len); put(str, len); return (*this); }
    inline Message& putString(const std::string& str) throw (MessageException) { size_t len = str.length() + 1; check(len); put(str.c_str(), len); return (*this); }

    inline std::string getString() throw (MessageException) { std::string ret((const char*)(buffer + mposition)); size_t len = ret.length() + 1; check(len); mposition += len; return ret; }
    inline char* getCharPointerDirect() throw (MessageException) { char* ret = (char*)(buffer + mposition); size_t len = strlen(ret) + 1; check(len); mposition += len; return ret; }
  };

}

