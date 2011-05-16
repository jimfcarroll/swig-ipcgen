
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
    Message(void* buffer_, size_t bufferSize) : buffer((byte*)buffer_), mcapacity(bufferSize), owner(false) { clear(); }
    Message(size_t bufferSize) : buffer(new byte[bufferSize]), mcapacity(bufferSize), owner(true) { clear(); }
    ~Message() { if (owner) delete [] buffer; }

    Message& flip() { mlimit = mposition; mposition = 0; return *this; }
    Message& clear() { mlimit = mcapacity; mposition = 0; return *this; }
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

    inline Message& putString(const char* str) throw (MessageException) { size_t len = strlen(str) + 1; check(len); put(str, len); }
    inline Message& putString(const std::string& str) throw (MessageException) { size_t len = str.length() + 1; check(len); put(str.c_str(), len); }

    inline std::string getString() throw (MessageException) { std::string ret((const char*)(buffer + mposition)); size_t len = ret.length() + 1; check(len); mposition += len; return ret; }
    inline char* getCharPointerDirect() throw (MessageException) { char* ret = (char*)(buffer + mposition); size_t len = strlen(ret) + 1; check(len); mposition += len; return ret; }
  };

}

