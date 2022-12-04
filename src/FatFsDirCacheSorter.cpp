#include "FatFsDirCacheSorter.h"
#include <cstring>
#include "FatFsDirCache.h"

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

FatFsDirCacheSorter::FatFsDirCacheSorter(FatFsDirCache* cache) :
  _dir(cache)
{
}


bool FatFsDirCacheSorter::sort() {
  DBG_PRINTF("FatFsDirCache: sort in folder '%s'\n", _dir->folder());

 _dir->close();
  
  if(_dir->open(FA_OPEN_EXISTING|FA_READ|FA_WRITE)) {
    
    // Read a few entries as a test
    FILINFO info;
    if (_dir->read(&info)) {
      DBG_PRINTF("FatFsDirCache: read '%s' for sorting \n", info.fname);    
    }
    else {
      DBG_PRINTF("FatFsDirCache: error reading '%s' for sorting \n", _dir->folder());
    }
    
    _dir->seek(0);
    strcpy(info.fname, "FISH!!");
    
    
    if (_dir->write(&info)) {
      DBG_PRINTF("FatFsDirCache: wrote '%s' for sorting \n", info.fname);    
    }
    else {
      DBG_PRINTF("FatFsDirCache: error writing '%s' for sorting \n", _dir->folder());
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
  
  _dir->seek(i);
  return _dir->read(info);
}

bool FatFsDirCacheSorter::write(uint32_t i, FILINFO *info) {
  // 1) if cached overwrite cached version
  // 2) if not cached write to the cache
  return false;
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
