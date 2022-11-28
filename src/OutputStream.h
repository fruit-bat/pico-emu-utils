#pragma once
#include <stdio.h>

class OutputStream {
  
public:
  virtual ~OutputStream() { close(); }
  virtual int32_t writeByte(int32_t b); // write a single byte, return -ve for error
  virtual int32_t writeWord(int32_t w); // write 2 bytes LSB first, return  -ve for error
  virtual int32_t write(uint8_t* buffer, const uint32_t length); // return -ve for error
  virtual void close() {};
  virtual bool closed() { return true; };
};
