#include "FatFsDirCacheInputStream.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiOutputStream.h"
#include "FatFsSpiInputStream.h"

// #define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#define FILINFO_SIZE ((uint32_t)sizeof(FILINFO))

FatFsDirCacheInputStream::FatFsDirCacheInputStream(SdCardFatFsSpi* sdCard) :
  _sdCard(sdCard),
  _open(false),
  _is(0),
  _l(0),
  _i(0)
{
}

FatFsDirCacheInputStream::~FatFsDirCacheInputStream() {
  close();
}

bool FatFsDirCacheInputStream::open(
  const char* folder, 
  const char* filename
) {
  
  if (_open) close();
  
  DBG_PRINTF("FatFsDirCacheInputStream: opening '%s' in '%s'\n", filename, folder);
  
  std::string cname;
  cname.append(folder);
  cname.append("/");
  cname.append(filename);

  const char* cp = cname.c_str();
  DBG_PRINTF("FatFsDirCacheInputStream: opening cache file '%s'\n", cp);

  _is = new FatFsSpiInputStream(_sdCard, cp);

  if (_is == 0) {
    DBG_PRINTF("FatFsDirCacheInputStream: failed to allocate cache file '%s'\n", cp);
  }
  else if (_is->closed()) {
    DBG_PRINTF("FatFsDirCacheInputStream: failed to open cache file '%s'\n", cp);
    if (_is) delete _is;
    _is = 0;
  }
  else {
    _open = true;      
  }
  
  if (_open) {
    _i = 0;
    if (!readCacheSize()) {
      close();
    }
  }
  
  return _open;
}

void FatFsDirCacheInputStream::close() {
  if (_open) {
    DBG_PRINTF("FatFsDirCacheInputStream: closing'\n");
    if (_is) delete _is;
    _is = 0;
    _open = false;
    _l = 0;
    _i = 0;
  }
}

bool FatFsDirCacheInputStream::readCacheSize() {
  DBG_PRINTF("FatFsDirCacheInputStream: reading number of directory entries\n");
  _l = 0;
  if (_open) {
    _l = _is->size() / FILINFO_SIZE;
    DBG_PRINTF("FatFsDirCacheInputStream: read file size %ld (entries %ld)\n", _is->size(), _l);
    return true;
  }
  return false;
}

bool FatFsDirCacheInputStream::seek(uint32_t i) {
  if (_open) {
    if (_i == 0) return true;
    _i = i;
    uint32_t fi = i * FILINFO_SIZE;
    DBG_PRINTF("FatFsDirCacheInputStream: seek index %ld (position %ld)\n", i, fi);
    if (i >= _l) {
      DBG_PRINTF("FatFsDirCacheInputStream: seek index %ld (position %ld) is out of range\n", i, fi);
      return false;
    }
    return _is->seek(fi) >= 0;
  }
  else {
    return false;
  }
}

bool FatFsDirCacheInputStream::read(FILINFO* info) {
  if (_open) {
    DBG_PRINTF("FatFsDirCacheInputStream: reading entry at index %ld\n", _i);
    int32_t r = _is->read((uint8_t *)info, FILINFO_SIZE);
    if (r < -1) {
      close();
    }
    if (r < 0) return false;
    _i++;
    return true;
  }
  else {
    return false;
  }
}

bool FatFsDirCacheInputStream::read(uint32_t i, FILINFO* info) {
  return seek(i) && read(info);
}
