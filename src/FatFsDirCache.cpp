#include "FatFsDirCache.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiOutputStream.h"
#include "FatFsSpiInputStream.h"
#include "FatFsDirCacheSorter.h"

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#define FILINFO_SIZE ((uint32_t)sizeof(FILINFO))

FatFsDirCache::FatFsDirCache(SdCardFatFsSpi* sdCard) :
  _sdCard(sdCard),
  _folder("/"),
  _open(false),
  _is(0),
  _l(0),
  _i(0),
  _filter([](const char* fname) -> bool {
    return true;
  })
{
}

FatFsDirCache::~FatFsDirCache() {
  close();
}

void FatFsDirCache::filename(std::string *s) {
  s->append(_folder);
  s->append("/");
  s->append(".dcache");  
}

bool FatFsDirCache::open(uint32_t mode) {
  if (!_open) {
    DBG_PRINTF("FatFsDirCache: opening cache in folder '%s'\n", _folder.c_str());
    
    std::string cname;
    filename(&cname);
    const char* cp = cname.c_str();
    DBG_PRINTF("FatFsDirCache: opening cache file '%s'\n", cp);
  
    _is = new FatFsSpiInputStream(_sdCard, cp, mode);

    if (_is == 0) {
      DBG_PRINTF("FatFsDirCache: failed to allocate cache file '%s'\n", cp);
    }
    else if (_is->closed()) {
      DBG_PRINTF("FatFsDirCache: failed to open cache file '%s'\n", cp);
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

void FatFsDirCache::close() {
  if (_open) {
    DBG_PRINTF("FatFsDirCache: closing cache in folder '%s'\n", _folder.c_str());
    if (_is) delete _is;
    _is = 0;
    _open = false;
    _l = 0;
    _i = 0;
  }
}

void FatFsDirCache::attach(const char *folder) {
  
  close();
  
  _folder = folder;
  DBG_PRINTF("FatFsDirCache: attaching cache to folder '%s'\n", _folder.c_str());
}

void FatFsDirCache::filter(std::function<bool(const char *fname)> filter) {

  DBG_PRINTF("FatFsDirCache: attaching filter to folder '%s'\n", _folder.c_str());

  _filter = filter;
}

void FatFsDirCache::create() {

  DBG_PRINTF("FatFsDirCache: creating cache in folder '%s'\n", _folder.c_str());
  
  close();
  
  const char *folder = _folder.c_str();
  FatFsSpiDirReader dirReader(_sdCard, folder);
  FatFsSpiOutputStream os(_sdCard, folder, ".dcache");
  dirReader.foreach([&](const FILINFO* info) { 
    DBG_PRINTF("caching dir entry %s\n", info->fname);
    if (_filter(info->fname)) {
      os.write((uint8_t *)info, FILINFO_SIZE);
    }
  });
}

void FatFsDirCache::remove() {
  
  DBG_PRINTF("FatFsDirCache: removing cache in folder '%s'\n", _folder.c_str());

  close();
  
  std::string cname;
  filename(&cname);
  const char* cp = cname.c_str();
  
  DBG_PRINTF("FatFsDirCache: removing cache file '%s'\n", cp);

  if (!_sdCard->mounted()) {   
    if (!_sdCard->mount()) {
      DBG_PRINTF("FatFsDirCache: failed to mount SD card\n");
      return;
    }
  }
  
  if(f_unlink(cp) != FR_OK) {
    DBG_PRINTF("FatFsDirCache: failed to remove cache in folder '%s'\n", _folder.c_str());
  }
}

bool FatFsDirCache::readCacheSize() {
  DBG_PRINTF("FatFsDirCache: reading cache size for folder '%s'\n", _folder.c_str());
  
  _l = 0;
  
  if (_open) {
    _l = _is->size() / FILINFO_SIZE;
    DBG_PRINTF("FatFsDirCache: read cache size %ld (entries %ld) for folder '%s'\n", _is->size(), _l, _folder.c_str());
    return true;
  }

  return false;
}

bool FatFsDirCache::seek(uint32_t i) {
  if (_open) {
    _i = i;
    uint32_t fi = i * FILINFO_SIZE;
    DBG_PRINTF("FatFsDirCache: seek index %ld (position %ld) in folder '%s'\n", i, fi, _folder.c_str());
    if (i >= _l) {
      DBG_PRINTF("FatFsDirCache: seek index %ld (position %ld) in folder '%s' is out of range\n", i, fi, _folder.c_str());
      return false;
    }
    return _is->seek(fi) >= 0;
  }
  else {
    return false;
  }
}

bool FatFsDirCache::read(FILINFO* info) {
  if (_open) {
    DBG_PRINTF("FatFsDirCache: reading entry at index %ld in folder '%s'\n", _i, _folder.c_str());
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

bool FatFsDirCache::write(FILINFO* info) {
  if (_open) {
    DBG_PRINTF("FatFsDirCache: writing entry at index %ld in folder '%s'\n", _i, _folder.c_str());
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

void FatFsDirCache::reload() {
  remove();
  create();
  sort();
  open(FA_READ|FA_OPEN_EXISTING);
}

bool FatFsDirCache::sort() {
  DBG_PRINTF("FatFsDirCache: sort in folder '%s'\n", _folder.c_str());
  FatFsDirCacheSorter sorter(this, 20);
  return sorter.sort();
}
