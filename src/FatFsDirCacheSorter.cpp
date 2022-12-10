#include "FatFsDirCacheSorter.h"
#include <cstring>
#include <iterator>

#define DEBUG_FAT_SPI

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
      quickSort(0, _is->size() - 1) &&
      flush();
    
  _is->close();
      
  return r;
}

bool FatFsDirCacheSorter::read(uint32_t i, FILINFO *info) {

  uint16_t xi = _index[i];

  bool r = _is->read(xi, info);
#ifdef DEBUG_FAT_SPI
  if (r) {
    DBG_PRINTF("FatFsDirCacheSorter: read element at %d '%s' \n", xi, info->fname);    
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
    uint16_t xi = _index[i];
    if (!(read(xi, &info) && _os->write(&info))) return false;
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

int16_t FatFsDirCacheSorter::partition(int16_t low, int16_t high) {
  DBG_PRINTF("FatFsDirCacheSorter: pivot low %d, high %d\n", low, high);    

  FILINFO pivot;
  
  // select the rightmost element as pivot
  if (!read(high, &pivot)) return -1; // TODO can I use -1 ?
  
  // pointer for greater element
  int16_t i = low - 1;

  // traverse each element of the array
  // compare them with the pivot
  for (int16_t j = low; j < high; j++) {
    
    FILINFO jth;

    if (!read(j, &jth)) return -1;

    if (strncmp(jth.fname, pivot.fname, FF_LFN_BUF) <= 0) {
        
      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;
      
      // swap element at i with element at j
      swap(i, j);
    }
  }
  
  // swap pivot with the greater element at i
  swap(i + 1, high);

  // return the partition point
  return i + 1;
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

bool FatFsDirCacheSorter::quickSort(int16_t low, int16_t high) {
  
  if (!(push(low) && push(high))) return false;
  
  while(stackSize()) {
      
    if (!(
      pop(&high) &&
      pop(&low)
    )) return false;
    
    // find the pivot element such that
    // elements smaller than pivot are on left of pivot
    // elements greater than pivot are on righ of pivot
    int16_t pi = partition(low, high);
    
    DBG_PRINTF("FatFsDirCacheSorter: new partition index of %d\n", pi);    

    if (pi < 0) {
      DBG_PRINTF("FatFsDirCacheSorter: ERROR sorting (partition index of) %d\n", pi);    
      return false;
    }
      
    // If there are elements on left side of pivot,
    // then push left side to stack
    if (pi - 1 > low) {
      if (!(
        push(low) &&
        push(pi - 1) 
      )) return false;
    }

    // If there are elements on right side of pivot,
    // then push right side to stack
    if (pi + 1 < high) {
      if (!(
        push(pi + 1) &&
        push(high)
      )) return false;
    }
  }
  
  return true;
}

