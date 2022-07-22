#pragma once

#include "OutputStream.h"
#include "SdCardFatFsSpi.h"
#include "ff_util.h"

class FatFsSpiOutputStream : public OutputStream {
  SdCardFatFsSpi* _sdCard;
  FRESULT _fr;     /* Return value for file */
  FIL _fil;        /* File object */
  bool _open;
public:
  FatFsSpiOutputStream(SdCardFatFsSpi* sdCard, const char* name);
  virtual ~FatFsSpiOutputStream() { close(); }
  virtual int write(unsigned char* buffer, const unsigned int length);
  virtual void close();
  virtual bool closed();
};
