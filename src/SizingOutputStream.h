#pragma once

#include "OutputStream.h"

class SizingOutputStream : public OutputStream {
  uint32_t _l;
public:
  SizingOutputStream() : _l(0) {}
  virtual int32_t writeByte(int32_t b) { _l++; return 1; }
  virtual int32_t writeWord(int32_t w) { _l += 2; return 2; }
  virtual int32_t write(uint8_t* buffer, const uint32_t length) { _l += length; return length;}
  uint32_t length() { return _l; }
  virtual void close() {}
};
