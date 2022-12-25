#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include "SdCardFatFsSpi.h"
#include "FatFsDirCacheInputStream.h"
#include "FatFsDirCacheOutputStream.h"
#include "FatFsFilePath.h"

class FatFsDirCache {
private:

  SdCardFatFsSpi* _sdCard;
  std::string _folder;
  FatFsDirCacheInputStream _is;
  std::function<bool(const char *fname)> _filter;

public:

  FatFsDirCache(SdCardFatFsSpi* sdCard);
  ~FatFsDirCache();
  void attach(const char *folder);
  void attach(FatFsFilePath *path);
  void filter(std::function<bool(const char *fname)> filter);
  bool create();
  void remove(const char *name);
  bool open();
  void close();
  void load();
  void reload();
  bool sort();
  bool read(uint32_t i, FILINFO* info);
  const char* folder() { return _folder.c_str(); }
  uint32_t size();
};
