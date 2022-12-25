#include "FatFsFilePath.h"

// #define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#include <iterator>
#include "ff.h"

FatFsFilePath::FatFsFilePath() :
  _parent(0)
{
}

bool FatFsFilePath::createFolders(SdCardFatFsSpi* sdCard) {
  
  if (!sdCard->mounted()) {
    if (!sdCard->mount()) {
      DBG_PRINTF("FatFsFilePath: Failed to mount SD card\n");
      return false;
    }
  }

  if (_parent) {
    if (!_parent->createFolders(sdCard)) return false;
  }

  std::string fname;
  if (_parent) {
    _parent->appendTo(fname);
    fname.append("/");
  }

  for(auto it = _elements.begin(); it != _elements.end(); ) {
    fname.append(*it++);
    DBG_PRINTF("FatFsFilePath: Creating folder '%s'\n", fname.c_str());
    f_mkdir(fname.c_str());
    if (it != _elements.end()) fname.append("/");
  }
  
  return true;
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

  for(auto it = _elements.begin(); it != _elements.end(); ) {
    fname.append(*it++);
    if (it != _elements.end()) fname.append("/");
  }
}

void FatFsFilePath::push(const char *e) {
  _elements.push_back(e);
}

void FatFsFilePath::pop() {
  if (_elements.size()) _elements.pop_back();
}

bool FatFsFilePath::equals(const char *p) {
  std::string fp;
  appendTo(fp);
  return fp == p;
}
