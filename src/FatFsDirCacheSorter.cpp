#include "FatFsDirCacheSorter.h"
#include <cstring>
#include <iterator>
#include "FatFsDirCache.h"

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

FatFsDirCacheSorter::FatFsDirCacheSorter(FatFsDirCache* cache) :
  _dir(cache),
  _deferred()
{
}


bool FatFsDirCacheSorter::sort() {
  DBG_PRINTF("FatFsDirCache: sort in folder '%s'\n", _dir->folder());

 _dir->close();
  
  if(_dir->open(FA_OPEN_EXISTING|FA_READ|FA_WRITE)) {
    
    quickSort(0, _dir->size() - 1);
    
    
    {
      for(std::map<uint32_t, FILINFO>::iterator it = _deferred.begin(); it != _deferred.end(); ++it) {
        DBG_PRINTF("FatFsDirCache: Sorted %ld '%s' \n", it->first, it->second.fname);
      }
    }
    
    
    _dir->close();
  }
  else {
    DBG_PRINTF("FatFsDirCache: failed to open '%s' for sorting \n", _dir->folder());
    
  }
  
  return false;
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
  
  return true;
}

int32_t FatFsDirCacheSorter::partition(int32_t low, int32_t high) {
  DBG_PRINTF("FatFsDirCacheSorter: pivot low %ld, high %ld\n", low, high);    

  FILINFO pivot;
  
  // select the rightmost element as pivot
  if (!read(high, &pivot)) return -1; // TODO can I use -1 ?
  
  // pointer for greater element
  int32_t i = (low - 1);

  // traverse each element of the array
  // compare them with the pivot
  for (int32_t j = low; j < high; j++) {
    
    FILINFO jth, ith;

    if (!read(j, &jth)) return -1; // TODO can I use -1 ?

    if (strncmp(jth.fname, pivot.fname, FF_LFN_BUF) <= 0) {
        
      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;
      
      // swap element at i with element at j
      if (!read(i, &ith)) return -1; // TODO can I use -1 ?
      write(i, &jth);
      write(j, &ith);
    }
  }
  
  // swap pivot with the greater element at i
  {
    uint32_t v = i + 1;
    uint32_t h = high;
    
    FILINFO vth, hth;
    if (!read(v, &vth)) return -1; // TODO can I use -1 ?
    if (!read(h, &hth)) return -1; // TODO can I use -1 ?
   
    write(v, &hth);
    write(h, &vth);
  }   
  // return the partition point
  return i + 1;
}

bool FatFsDirCacheSorter::swap(int32_t a, int32_t b) {
  DBG_PRINTF("FatFsDirCacheSorter: swap %ld, %ld\n", a, b);    

  return false;
}

bool FatFsDirCacheSorter::quickSort(int32_t low, int32_t high) {
  if (low < high) {
      
    // find the pivot element such that
    // elements smaller than pivot are on left of pivot
    // elements greater than pivot are on righ of pivot
    int32_t pi = partition(low, high);
    DBG_PRINTF("FatFsDirCacheSorter: new partition index of %ld\n", pi);    

    if (pi < 0) return false;
    
    // recursive call on the left of pivot
    if (!quickSort(low, pi - 1)) return false;

    // recursive call on the right of pivot
    if (!quickSort(pi + 1, high)) return false;
  }
  
  return true;
}

/*
// function to swap elements
void FatFsDirCacheSorter::swap(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

// function to rearrange array (find the partition point)
int FatFsDirCacheSorter::partition(int array[], int low, int high) {
    
  // select the rightmost element as pivot
  int pivot = array[high];
  
  // pointer for greater element
  int i = (low - 1);

  // traverse each element of the array
  // compare them with the pivot
  for (int j = low; j < high; j++) {
    if (array[j] <= pivot) {
        
      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;
      
      // swap element at i with element at j
      swap(&array[i], &array[j]);
    }
  }
  
  // swap pivot with the greater element at i
  swap(&array[i + 1], &array[high]);
  
  // return the partition point
  return (i + 1);
}

void FatFsDirCacheSorter::quickSort(int array[], int low, int high) {
  if (low < high) {
      
    // find the pivot element such that
    // elements smaller than pivot are on left of pivot
    // elements greater than pivot are on righ of pivot
    int pi = partition(array, low, high);

    // recursive call on the left of pivot
    quickSort(array, low, pi - 1);

    // recursive call on the right of pivot
    quickSort(array, pi + 1, high);
  }
}


*/
