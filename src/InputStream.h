#pragma once
#include <stdio.h>

class InputStream {
  
public:
  virtual ~InputStream() { close(); }
  virtual int32_t readByte(); // read a single byte, -1 for eof, -ve for error
  virtual int32_t readWord(); // read 2 bytes LSB first, -1 for eof, -ve for error
  virtual int32_t read(uint8_t* buffer, const uint32_t length); // returns actual length read, -1 for eof, -ve for error
  virtual void close() {}
  virtual bool closed();
  virtual bool end();
  virtual int32_t seek(const uint32_t pos) { return -2; }
  virtual int32_t rseek(const int32_t rpos) { return -2; }
  virtual uint32_t pos() { return 0; }
  int32_t decodeLsbf(uint32_t* i, uint8_t n);
  int32_t decodeLsbf(uint32_t* i, const uint8_t* l, uint32_t n);
};
