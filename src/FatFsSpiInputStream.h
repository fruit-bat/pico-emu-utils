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
  virtual int32_t readByte();
  virtual int32_t read(uint8_t* buffer, const uint32_t length); // returns actual length read, -1 for eof, -ve for error
  virtual void close();
  virtual bool closed();
  virtual bool end();
  virtual int32_t seek(const uint32_t pos);
  virtual int32_t rseek(const int32_t rpos);
  virtual uint32_t pos();
  uint32_t size();
  // TODO should be somewhere else
  int32_t write(uint8_t* buffer, const uint32_t length);
  FatFsSpiInputStream(SdCardFatFsSpi* sdCard, const char* name, uint32_t mode);
};
