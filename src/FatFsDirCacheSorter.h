#pragma once
#include <stdio.h>
#include "ff.h"

class FatFsDirCache;

class FatFsDirCacheSorter {
private:
  FatFsDirCache* _dir;
  bool read(uint32_t i, FILINFO *info);
  bool write(uint32_t i, FILINFO *info);
public:  
  FatFsDirCacheSorter(FatFsDirCache* cache);
  ~FatFsDirCacheSorter() {}
  bool sort();
};

