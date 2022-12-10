#pragma once

/**
 * Thanks to https://www.programiz.com/dsa/quick-sort 
 * 
 */

#include <stdio.h>
#include "ff.h"
#include <map>
#include <vector>
#include "FatFsDirCacheInputStream.h"
#include "FatFsDirCacheOutputStream.h"

class FatFsDirCache;

class FatFsDirCacheSorter {
private:

  FatFsDirCacheInputStream* _is;
  FatFsDirCacheOutputStream* _os;
  
  std::vector<int16_t> _stack;
  std::vector<int16_t> _index;

  bool read(uint32_t i, FILINFO *info);
  void swap(int16_t low, int16_t high);
  int16_t partition(int16_t low, int16_t high);
  bool quickSort(int16_t low, int16_t high);
  bool flush();
  bool push(int16_t i);
  bool pop(int16_t* i);
  uint32_t stackSize();
  
public:

  FatFsDirCacheSorter(
    FatFsDirCacheInputStream* is,
    FatFsDirCacheOutputStream* os
  );
  
  ~FatFsDirCacheSorter() {}
  
  bool sort();
};

