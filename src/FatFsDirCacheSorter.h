#pragma once

/**
 * Thanks to:
 * 
 * https://www.programiz.com/dsa/quick-sort 
 * https://iq.opengenus.org/quick-sort-using-stack/
 * https://alienryderflex.com/quicksort/
 * https://github.com/faf0/LampSort/blob/master/src/de/ffoerg/sort/LampSort.java
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
  bool lampSort();
  bool flush();
  bool push(int16_t i);
  bool pop(int16_t* i);
  bool push(int16_t i, int16_t j) { return push(i) && push(j); }
  bool pop(int16_t* i, int16_t* j) { return pop(j) && pop(i); }
  static int compare(FILINFO *a, FILINFO *b);
  uint32_t stackSize();
  static int16_t min(int16_t i, int16_t j) { return i < j ? i : j; }
  static int16_t max(int16_t i, int16_t j) { return i > j ? i : j; }
  
public:

  FatFsDirCacheSorter(
    FatFsDirCacheInputStream* is,
    FatFsDirCacheOutputStream* os
  );
  
  ~FatFsDirCacheSorter() {}
  
  bool sort();
};

