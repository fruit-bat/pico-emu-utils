#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include "SdCardFatFsSpi.h"
#include "FatFsSpiInputStream.h"

class FatFsDirCacheInputStream {
private:

  SdCardFatFsSpi* _sdCard;
  bool _open;
  FatFsSpiInputStream *_is;
  uint32_t _l, _i;
  bool readCacheSize();
  
public:

  FatFsDirCacheInputStream(SdCardFatFsSpi* sdCard);
  
  ~FatFsDirCacheInputStream();
    
  bool open(
    const char* folder, 
    const char* filename
  );

  void close();
  bool seek(uint32_t i);
  bool read(FILINFO* info);
  bool read(uint32_t i, FILINFO *info);
  uint32_t size() { return _l; }
  uint32_t pos() { return _i; }
};
