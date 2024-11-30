#include "FatFsDirCacheSorter.h"
#include <cstring>
#include <iterator>

// #define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

FatFsDirCacheSorter::FatFsDirCacheSorter(
    FatFsDirCacheInputStream* is,
    FatFsDirCacheOutputStream* os
) :
  _is(is),
  _os(os)
{
  uint32_t l = _is->size();
  _index.reserve(l);
  for (uint16_t i = 0; i < l; ++i) _index.push_back(i);
}

bool FatFsDirCacheSorter::sort() {
  DBG_PRINTF("FatFsDirCacheSorter: sort\n");

  bool r = 
      lampSort() &&
      flush();
    
  _is->close();
      
  return r;
}

bool FatFsDirCacheSorter::read(uint32_t i, FILINFO *info) {

  uint16_t xi = _index[i];

  bool r = _is->read(xi, info);
#ifdef DEBUG_FAT_SPI
  if (r) {
  //  DBG_PRINTF("FatFsDirCacheSorter: read element at %d '%s' \n", xi, info->fname);    
  }
  else {
    DBG_PRINTF("FatFsDirCacheSorter: error reading element at %d\n", xi);    
  }
#endif
  return r;
}

bool FatFsDirCacheSorter::flush() {
  uint32_t l = _is->size();
  FILINFO info;
  for (uint16_t i = 0; i < l; ++i) {
    if (!(read(i, &info) && _os->write(&info))) return false;
  }
  _os->close();
  return true;
}

void FatFsDirCacheSorter::swap(int16_t i, int16_t j) {
  uint16_t xi = _index[i];
  uint16_t xj = _index[j];
  _index[i] = xj;
  _index[j] = xi;
}

int FatFsDirCacheSorter::compare(FILINFO *a, FILINFO *b) {
  bool da = a->fattrib & AM_DIR;
  bool db = b->fattrib & AM_DIR;
  if (da && !db) return -1;
  if (!da && db) return 1;
  return strncasecmp(a->fname, b->fname, FF_LFN_BUF);
}

bool FatFsDirCacheSorter::push(int16_t i) {
  _stack.push_back(i);
  return true;
}

bool FatFsDirCacheSorter::pop(int16_t* i) {
  *i = _stack.back();
  _stack.pop_back();
  return true;
}

uint32_t FatFsDirCacheSorter::stackSize() {
  return _stack.size();
}

bool FatFsDirCacheSorter::lampSort() {
  
  int16_t low = 0;
  int16_t high = _is->size() - 1;
  
  if (!(push(low, high))) return false;
  
  while(stackSize()) {
      
    if (!pop(&low, &high)) return false;
    
    DBG_PRINTF("FatFsDirCacheSorter: span %d %d\n", low, high);    

    int16_t span = high - low;
    
    if (span >= 2) {
      
      int16_t pi = low;
      
      FILINFO pivot;
      
      if (!read(high, &pivot)) return false;

      for (int16_t j = low; j < high; j++) {
        
        FILINFO jth;

        if (!read(j, &jth)) return false;

        if (compare(&jth, &pivot) < 0) swap(pi++, j);
      }
     
      swap(pi, high);
      
      if (!(push(low, max(low, pi - 1)) && push(min(pi + 1, high), high))) return false;
    }
    else if (span == 1) {
      
      FILINFO low_info, high_info;
      
      if (!(read(low, &low_info) && read(high, &high_info))) return false;
     
      if (compare(&low_info, &high_info) > 0) swap(low, high);
    }
  }
  return true;
}
