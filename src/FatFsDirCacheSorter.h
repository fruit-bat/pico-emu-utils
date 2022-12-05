#pragma once

/**
 * Thanks to https://www.programiz.com/dsa/quick-sort 
 * 
 */

#include <stdio.h>
#include "ff.h"
#include <map>

class FatFsDirCache;

class FatFsDirCacheSorter {
private:
  FatFsDirCache* _dir;
  std::map<uint32_t, FILINFO> _deferred;
  uint32_t _deferredMax;
  
  bool read(uint32_t i, FILINFO *info);
  bool write(uint32_t i, FILINFO *info);
  int32_t partition(int32_t low, int32_t high);
  bool quickSort(int32_t low, int32_t high);
  bool flush();
public:  
  FatFsDirCacheSorter(FatFsDirCache* cache, uint32_t deferredMax);
  ~FatFsDirCacheSorter() {}
  bool sort();
};

