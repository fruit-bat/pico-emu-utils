#pragma once
#include "InputStream.h"

class ArrayInputStream : public InputStream {
  const uint8_t* _buf;
  uint32_t _length;
  uint32_t _i;
public:
  ArrayInputStream(
    const uint8_t* buf,
    uint32_t length
  ) :
    _buf(buf),
    _length(length),
    _i(0)
  {}
  
  virtual int32_t readByte() { 
    return _i >= _length ? -1 : _buf[_i++];
  }
  
  virtual void close() {}

  virtual bool closed() { return false; }
  virtual bool end() { return _i >= _length; }
};
