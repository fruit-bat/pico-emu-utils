#include "FatFsFilePath.h"

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

FatFsFilePath::FatFsFilePath() 
{
  
}

FatFsFilePath::~FatFsFilePath() {
  
}

void FatFsFilePath::appendTo(std::string &fname) {
  for(auto e : _elements) {
    fname.append("/");
    fname.append(e);
  }
}

void FatFsFilePath::push(char *e) {
  _elements.push_back(e);
}

void FatFsFilePath::pop() {
  _elements.pop_back();
}

