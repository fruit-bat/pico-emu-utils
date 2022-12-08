#include "FatFsDirCacheSorter.h"
#include <cstring>
#include <iterator>
#include "FatFsDirCache.h"

// #define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

FatFsDirCacheSorter::FatFsDirCacheSorter(FatFsDirCache* cache, uint32_t deferredMax) :
  _dir(cache),
  _deferred(),
  _deferredMax(deferredMax)
{
}


bool FatFsDirCacheSorter::sort() {
  DBG_PRINTF("FatFsDirCache: sort in folder '%s'\n", _dir->folder());

 _dir->close();
  
  if(_dir->open(FA_OPEN_EXISTING|FA_READ|FA_WRITE)) {
    
    bool r = 
      quickSort(0, _dir->size() - 1) &&
      flush();
    
    _dir->close();
    
    _deferred.clear();
    
    return r;
  }
  else {
    DBG_PRINTF("FatFsDirCache: failed to open '%s' for sorting \n", _dir->folder());
    return false;    
  }
}

bool FatFsDirCacheSorter::read(uint32_t i, FILINFO *info) {
  // 1) if cached return from the cache
  // 2) if not cached read from the disk
  std::map<uint32_t, FILINFO>::iterator it = _deferred.find(i);
  if (it != _deferred.end()) {
    *info = it->second;
    DBG_PRINTF("FatFsDirCacheSorter: read deferred element at %ld '%s' \n", i, info->fname);    
    return true;
  }
  
  _dir->seek(i);
  bool r = _dir->read(info);
#ifdef DEBUG_FAT_SPI
  if (r) {
    DBG_PRINTF("FatFsDirCacheSorter: read element at %ld '%s' \n", i, info->fname);    
  }
  else {
    DBG_PRINTF("FatFsDirCacheSorter: error reading element at %ld\n", i);    
  }
#endif
  return r;
}

bool FatFsDirCacheSorter::write(uint32_t i, FILINFO *info) {
  // 1) if cached overwrite cached version
  // 2) if not cached write to the cache
  _deferred[i] = *info;
  
  return _deferred.size() < _deferredMax || flush();
}

bool FatFsDirCacheSorter::flush() {
  for(std::map<uint32_t, FILINFO>::iterator it = _deferred.begin(); it != _deferred.end(); ++it) {
    DBG_PRINTF("FatFsDirCache: flushing %ld '%s' \n", it->first, it->second.fname);
    if (!_dir->seek(it->first)) {
      DBG_PRINTF("FatFsDirCache: flush failed to seek %ld '%s' \n", it->first, it->second.fname);
      return false;
    }
    if (!_dir->write(&(it->second))) {
      DBG_PRINTF("FatFsDirCache: flush failed to write %ld '%s' \n", it->first, it->second.fname);
      return false;
    }
  }
  _deferred.clear();
  return true;
}

int32_t FatFsDirCacheSorter::partition(int16_t low, int16_t high) {
  DBG_PRINTF("FatFsDirCacheSorter: pivot low %d, high %d\n", low, high);    

  FILINFO pivot;
  
  // select the rightmost element as pivot
  if (!read(high, &pivot)) return -1; // TODO can I use -1 ?
  
  // pointer for greater element
  int32_t i = low - 1;

  // traverse each element of the array
  // compare them with the pivot
  for (int32_t j = low; j < high; j++) {
    
    FILINFO jth, ith;

    if (!read(j, &jth)) return -1;

    if (strncmp(jth.fname, pivot.fname, FF_LFN_BUF) <= 0) {
        
      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;
      
      // swap element at i with element at j
      if (!(
        read(i, &ith) &&
        write(i, &jth) &&
        write(j, &ith)
      )) return -1;
    }
  }
  
  // swap pivot with the greater element at i
  {
    uint32_t v = i + 1;
    uint32_t h = high;
    
    FILINFO vth, hth;
    if (!(
      read(v, &vth) &&
      read(h, &hth) &&
      write(v, &hth) &&
      write(h, &vth)
    )) return -1;
  }
  
  // return the partition point
  return i + 1;
}

bool FatFsDirCacheSorter::pushIndex(int16_t i) {
  _indexes.push_back(i);
  return true;
}

bool FatFsDirCacheSorter::popIndex(int16_t* i) {
  *i = _indexes.back();
  _indexes.pop_back();
  return true;
}

uint32_t FatFsDirCacheSorter::indexCount() {
  return _indexes.size();
}

bool FatFsDirCacheSorter::quickSort(int16_t low, int16_t high) {
  
  if (!(
    pushIndex(low) &&
    pushIndex(high)
  )) return false;
  
  while(indexCount()) {
      
    if (!(
      popIndex(&high) &&
      popIndex(&low)
    )) return false;
    
    // find the pivot element such that
    // elements smaller than pivot are on left of pivot
    // elements greater than pivot are on righ of pivot
    int32_t pi = partition(low, high);
    
    DBG_PRINTF("FatFsDirCacheSorter: new partition index of %ld\n", pi);    

    if (pi < 0) {
      DBG_PRINTF("FatFsDirCacheSorter: ERROR sorting (partition index of) %ld\n", pi);    
      return false;
    }
      
    // If there are elements on left side of pivot,
    // then push left side to stack
    if (pi - 1 > low) {
      if (!(
        pushIndex(low) &&
        pushIndex(pi - 1) 
      )) return false;
    }

    // If there are elements on right side of pivot,
    // then push right side to stack
    if (pi + 1 < high) {
      if (!(
        pushIndex(pi + 1) &&
        pushIndex(high)
      )) return false;
    }
  }
  
  return true;
}

