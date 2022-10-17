#pragma once

#include "InputStream.h"

class BufferedInputStream : public InputStream {
  InputStream* _delegate;
  uint8_t _buffer[1024];
  uint32_t _len;
  uint32_t _pos;
  void flood();
public:
  BufferedInputStream(InputStream* delegate);
  virtual int32_t readByte(); // read a single byte, -1 for eof, -ve for error
  virtual void close();
  virtual bool closed();
  virtual bool end();
};
