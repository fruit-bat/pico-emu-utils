#include "InputStream.h"

int32_t InputStream::readWord() {
  uint32_t i;
  int32_t r = decodeLsbf(&i, 2);
  return r < 0 ? r : i;
}

int32_t InputStream::read(uint8_t* buffer, const uint32_t length) {
  uint32_t i = 0;
  while (true) {
    if (i == length) return i;
    const int32_t r = readByte();
    if (r == -1) return i;
    if (r < 0) return r;
    buffer[i++] = r;
  }
}

int32_t InputStream::skip(const uint32_t length) {
  uint32_t i = 0;
  while (true) {
    if (i == length) return i;
    const int32_t r = readByte();
    if (r == -1) return i;
    if (r < 0) return r;
  }
}

int32_t InputStream::decodeLsbf(uint32_t* i, uint8_t n) {
  // The following assumes Little Endian
  *i = 0;
  const int32_t r = read((unsigned char*)i, n);
  return r < 0 ? r : r < n ? -3 : n;
}

int32_t InputStream::decodeLsbf(uint32_t* i, const int8_t* l, uint32_t n) {
  for (uint32_t p = 0; p < n; ++p) {
    int8_t ln = l[n];
    int32_t r = ln >= 0 ? decodeLsbf(i + p, ln) : skip(-ln);
    if (r < 0) return r;
  }
  return 0;
}
