#pragma once

#include "OutputStream.h"
#include "SdCardFatFsSpi.h"
#include "ff_util.h"

class FatFsSpiOutputStream : public OutputStream {
  SdCardFatFsSpi* _sdCard;
  FRESULT _fr;     /* Return value for file */
  FIL _fil;        /* File object */
  bool _open;
  void open(const char* name);
public:
  FatFsSpiOutputStream(SdCardFatFsSpi* sdCard, const char* name);
  FatFsSpiOutputStream(SdCardFatFsSpi* sdCard, const char* folder, const char* file);
  virtual ~FatFsSpiOutputStream() { close(); }
  virtual int32_t write(uint8_t* buffer, const uint32_t length);
  virtual void close();
  virtual bool closed();
};
