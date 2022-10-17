#include "InputStream.h"

int InputStream::readWord() {
  unsigned int i;
  int r = decodeLsbf(&i, 2);
  return r < 0 ? r : i;
}

int InputStream::read(unsigned char* buffer, const unsigned int length) {
  unsigned int i = 0;
  while (true) {
    if (i == length) return i;
    const int r = readByte();
    if (r == -1) return i;
    if (r < 0) return r;
    buffer[i++] = r;
  }
}

int InputStream::decodeLsbf(unsigned int* i, int n) {
  // The following assumes Little Endian
  *i = 0;
  const int r = read((unsigned char*)i, n);
  return r < 0 ? r : r < n ? -3 : n;
}

int InputStream::decodeLsbf(unsigned int* i, unsigned char* l, unsigned int n) {
  for (unsigned int p = 0; p < n; ++p) {
    int r = decodeLsbf(i + p, l[n]);
    if (r < 0) return r;
  }
  return 0;
}
