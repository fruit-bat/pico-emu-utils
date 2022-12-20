#include "FatFsFilePath.h"

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#include <iterator>

FatFsFilePath::FatFsFilePath() :
  _parent(0)
{
  
}

FatFsFilePath::FatFsFilePath(const char *root) :
  _parent(0)
{
  push(root);
}

FatFsFilePath::FatFsFilePath(FatFsFilePath* parent, const char* folder) :
  _parent(parent)
{
  push(folder);
}

FatFsFilePath::FatFsFilePath(FatFsFilePath* parent) :
  _parent(parent)
{
  
}

FatFsFilePath::~FatFsFilePath() {
  
}

void FatFsFilePath::appendTo(std::string &fname) {
  if (_parent) {
    _parent->appendTo(fname);
    fname.append("/");
  }

  for(auto it = _elements.begin(); it != _elements.end(); it++) {
    fname.append(*it);
    if (it != _elements.end()) fname.append("/");
  }
}

void FatFsFilePath::push(const char *e) {
  _elements.push_back(e);
}

void FatFsFilePath::pop() {
  if (_elements.size()) _elements.pop_back();
}

