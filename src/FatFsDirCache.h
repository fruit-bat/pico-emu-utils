#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include "SdCardFatFsSpi.h"
#include "FatFsSpiInputStream.h"

class FatFsDirCache {
private:
  SdCardFatFsSpi* _sdCard;
  std::string _folder;
  void filename(std::string *s);
  bool _open;
  FatFsSpiInputStream *_is;
  uint32_t _l, _i;
  std::function<bool(const char *fname)> _filter;
  bool readCacheSize();
public:
  FatFsDirCache(SdCardFatFsSpi* sdCard);
  ~FatFsDirCache();
  void attach(const char *folder);
  void filter(std::function<bool(const char *fname)> filter);
  void create();
  void remove();
  bool open();
  void close();
  void reload();  
  bool seek(uint32_t i);
  bool read(FILINFO* info);
  uint32_t size() { return _l; }
  uint32_t pos() { return _i; }
  const char* folder() { return _folder.c_str(); }
};
