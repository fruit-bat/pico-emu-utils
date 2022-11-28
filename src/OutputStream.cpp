#include "OutputStream.h"

int32_t OutputStream::writeByte(int32_t b) {
  uint8_t buf[1];
  buf[0] = b;
  return write((unsigned char *)&buf, 1);
}

int32_t OutputStream::writeWord(int32_t w) {
  uint8_t buf[2];
  buf[0] = w & 0xff;
  buf[1] = w >> 8;
  return write((uint8_t *)&buf, 2);
}
