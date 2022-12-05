#pragma once
#include <stdio.h>
#include "ff.h"
#include <map>

class FatFsDirCache;

class FatFsDirCacheSorter {
private:
  FatFsDirCache* _dir;
  std::map<uint32_t, FILINFO> _deferred;
  
  bool read(uint32_t i, FILINFO *info);
  bool write(uint32_t i, FILINFO *info);
  int32_t partition(int32_t low, int32_t high);
  bool swap(int32_t a, int32_t b);
  bool quickSort(int32_t low, int32_t high);
public:  
  FatFsDirCacheSorter(FatFsDirCache* cache);
  ~FatFsDirCacheSorter() {}
  bool sort();
};

