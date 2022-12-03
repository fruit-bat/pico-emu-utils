#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include "SdCardFatFsSpi.h"
#include "FatFsSpiInputStream.h"

class FatFsDirCache {
private:
  SdCardFatFsSpi* _sdCard;
  std::string _folder;
  void filename(std::string *s);
  bool _open;
  FatFsSpiInputStream *_is;
  uint32_t _l;
  bool readCacheSize();
public:
  FatFsDirCache(SdCardFatFsSpi* sdCard);
  void attach(const char *folder);
  void create();
  void remove();
  bool open();
  void close();
  bool seek(uint32_t i);
  
};
