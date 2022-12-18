#include "FatFsFilePath.h"

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#include <iterator>

FatFsFilePath::FatFsFilePath() 
{
  
}

FatFsFilePath::~FatFsFilePath() {
  
}

void FatFsFilePath::appendTo(std::string &fname) {
  for(auto it = _elements.begin(); it != _elements.end(); it++) {
    fname.append(*it);
    if (it != _elements.end()) fname.append("/");
  }
}

void FatFsFilePath::push(char *e) {
  _elements.push_back(e);
}

void FatFsFilePath::pop() {
  _elements.pop_back();
}

