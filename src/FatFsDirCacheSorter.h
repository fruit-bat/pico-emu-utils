#pragma once

/**
 * Thanks to https://www.programiz.com/dsa/quick-sort 
 * 
 */

#include <stdio.h>
#include "ff.h"
#include <map>
#include <vector>

class FatFsDirCache;

class FatFsDirCacheSorter {
private:
  FatFsDirCache* _dir;
  std::map<uint32_t, FILINFO> _deferred;
  std::vector<int16_t> _indexes;
  
  uint32_t _deferredMax;
  
  bool read(uint32_t i, FILINFO *info);
  bool write(uint32_t i, FILINFO *info);
  int32_t partition(int16_t low, int16_t high);
  bool quickSort(int16_t low, int16_t high);
  bool flush();
  bool pushIndex(int16_t i);
  bool popIndex(int16_t* i);
  uint32_t indexCount();
public:  
  FatFsDirCacheSorter(FatFsDirCache* cache, uint32_t deferredMax);
  ~FatFsDirCacheSorter() {}
  bool sort();
};

