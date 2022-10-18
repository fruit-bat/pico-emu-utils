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
  printf("InputStream::read r=%ld\n", r);
    if (r == -1) return i;
    if (r < 0) return r;
    buffer[i++] = r;
  }
}

int32_t InputStream::skip(const uint32_t length) {
  uint32_t i = 0;
  while (true) {
    if (i++ == length) return i;
    const int32_t r = readByte();
  printf("InputStream::skip r=%ld\n", r);
    if (r == -1) return i;
    if (r < 0) return r;
  }
}

int32_t InputStream::decodeLsbf(uint32_t* i, uint8_t n) {
  // The following assumes Little Endian
  *i = 0;
  const int32_t r = read((uint8_t*)i, n);
  printf("InputStream::decodeLsbf *i=%08lX, l=%d\n", *i, n);
  return r < 0 ? r : r < n ? -3 : n;
}

int32_t InputStream::decodeLsbf(uint32_t* i, const int8_t* l, uint32_t n) {
  uint32_t j = 0;
  for (uint32_t p = 0; p < n; ++p) {
    int8_t lp = l[p];
    int32_t r = (lp >= 0 ? decodeLsbf(&i[j++], lp) : skip(-lp));
    if (r < 0) return r;
  }
  return 0;
}
