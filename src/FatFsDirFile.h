#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include "SdCardFatFsSpi.h"
#include "FatFsSpiInputStream.h" // TODO Should be input/output stream

class FatFsDirFile {
private:
  SdCardFatFsSpi* _sdCard;
  bool _open;
  FatFsSpiInputStream *_is;
  uint32_t _l, _i;
  bool readCacheSize();
public:
  FatFsDirFile(SdCardFatFsSpi* sdCard);
  ~FatFsDirFile();
  
  bool create(
    const char* folder, 
    const char* filename,
    std::function<bool(const char *fname)> filter
  );
    
  bool open(
    uint32_t mode,
    const char* folder, 
    const char* filename
  );

  void close();
  void load();
  void reload();
  bool sort();
  bool seek(uint32_t i);
  bool read(FILINFO* info);
  bool write(const FILINFO* info);
  uint32_t size() { return _l; }
  uint32_t pos() { return _i; }
};
