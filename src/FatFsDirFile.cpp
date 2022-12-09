#include "FatFsDirFile.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiOutputStream.h"
#include "FatFsSpiInputStream.h"

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#define FILINFO_SIZE ((uint32_t)sizeof(FILINFO))

FatFsDirFile::FatFsDirFile(SdCardFatFsSpi* sdCard) :
  _sdCard(sdCard),
  _open(false),
  _is(0),
  _l(0),
  _i(0)
{
}

FatFsDirFile::~FatFsDirFile() {
  close();
}

bool FatFsDirFile::open(
  uint32_t mode,
  const char* folder, 
  const char* filename
) {
  
  if (!_open) {
    DBG_PRINTF("FatFsDirFile: opening '%s' in '%s'\n", filename, folder);
    
    std::string cname;
    cname.append(folder);
    cname.append("/");
    cname.append(filename);

    const char* cp = cname.c_str();
    DBG_PRINTF("FatFsDirFile: opening cache file '%s'\n", cp);
  
    _is = new FatFsSpiInputStream(_sdCard, cp, mode);

    if (_is == 0) {
      DBG_PRINTF("FatFsDirFile: failed to allocate cache file '%s'\n", cp);
    }
    else if (_is->closed()) {
      DBG_PRINTF("FatFsDirFile: failed to open cache file '%s'\n", cp);
      if (_is) delete _is;
      _is = 0;
    }
    else {
      _open = true;      
    }
  }
  
  if (_open) {
    _i = 0;
    if (!readCacheSize()) {
      close();
    }
  }
  
  return _open;
}

void FatFsDirFile::close() {
  if (_open) {
    DBG_PRINTF("FatFsDirFile: closing'\n");
    if (_is) delete _is;
    _is = 0;
    _open = false;
    _l = 0;
    _i = 0;
  }
}

bool FatFsDirFile::create(
  const char* folder, 
  const char* filename,
  std::function<bool(const char *fname)> filter
) {

  DBG_PRINTF("FatFsDirFile: creating directory file '%s' in folder '%s'\n", filename, folder);
  
  close();
  
  FatFsSpiDirReader dirReader(_sdCard, folder);
  
  if (!open(
    FA_WRITE|FA_OPEN_ALWAYS,
    folder,
    filename)
  ) return false;

  dirReader.foreach([&](const FILINFO* info) { 
    DBG_PRINTF("FatFsDirFile: writing dir entry %s\n", info->fname);
    if (filter(info->fname)) {
      write(info); // TODO Handle failed write
    }
  });
  
  close();
  
  return true;
}

bool FatFsDirFile::readCacheSize() {
  DBG_PRINTF("FatFsDirFile: reading number of directory entries\n");
  _l = 0;
  if (_open) {
    _l = _is->size() / FILINFO_SIZE;
    DBG_PRINTF("FatFsDirFile: read file size %ld (entries %ld)\n", _is->size(), _l);
    return true;
  }
  return false;
}

bool FatFsDirFile::seek(uint32_t i) {
  if (_open) {
    _i = i;
    uint32_t fi = i * FILINFO_SIZE;
    DBG_PRINTF("FatFsDirFile: seek index %ld (position %ld) in folder '%s'\n", i, fi, _folder.c_str());
    if (i >= _l) {
      DBG_PRINTF("FatFsDirFile: seek index %ld (position %ld) in folder '%s' is out of range\n", i, fi, _folder.c_str());
      return false;
    }
    return _is->seek(fi) >= 0;
  }
  else {
    return false;
  }
}

bool FatFsDirFile::read(FILINFO* info) {
  if (_open) {
    DBG_PRINTF("FatFsDirFile: reading entry at index %ld\n", _i);
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

bool FatFsDirFile::write(const FILINFO* info) {
  if (_open) {
    DBG_PRINTF("FatFsDirFile: writing entry at index %ld\n", _i);
    int32_t r = _is->write((uint8_t *)info, FILINFO_SIZE);
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
