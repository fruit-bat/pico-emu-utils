#pragma once

#include "InputStream.h"
#include "SdCardFatFsSpi.h"
#include "ff_util.h"

class FatFsSpiInputStream : public InputStream {
  SdCardFatFsSpi* _sdCard;
  FRESULT _fr;     /* Return value for file */
  FIL _fil;        /* File object */
  bool _eof;
  bool _open;
public:
  FatFsSpiInputStream(SdCardFatFsSpi* sdCard, const char* name);
  virtual ~FatFsSpiInputStream() { close(); }
  virtual int readByte();
  virtual int read(unsigned char* buffer, const unsigned int length); // returns actual length read, -1 for eof, -ve for error
  virtual void close();
  virtual bool closed();
  virtual bool end();
  virtual int seek(const unsigned int pos);
  virtual int rseek(const int rpos);
};
