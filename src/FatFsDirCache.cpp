#include "FatFsDirCache.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiOutputStream.h"
#include "FatFsSpiInputStream.h"
#include "FatFsDirCacheSorter.h"

// #define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#define FILINFO_SIZE ((uint32_t)sizeof(FILINFO))

FatFsDirCache::FatFsDirCache(SdCardFatFsSpi* sdCard) :
  _sdCard(sdCard),
  _folder("/"),
  _is(0),
  _filter([](const char* fname) -> bool {
    return true;
  })
{
}

FatFsDirCache::~FatFsDirCache() {
  close();
}

bool FatFsDirCache::open() {
  DBG_PRINTF("FatFsDirCache: opening cache in folder '%s'\n", _folder.c_str());
  _is.open(_folder.c_str(), ".dcache");
  return _is.isOpen();
}

void FatFsDirCache::close() {
  DBG_PRINTF("FatFsDirCache: closing cache in folder '%s'\n", _folder.c_str());
  _is.close();
}

bool FatFsDirCache::read(uint32_t i, FILINFO* info) {
  return _is.read(i, info);
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

bool FatFsDirCache::create() {

  DBG_PRINTF("FatFsDirCache: creating cache in folder '%s'\n", _folder.c_str());
  
  const char *folder = _folder.c_str();
  FatFsSpiDirReader dirReader(_sdCard, folder);
  FatFsDirCacheOutputStream os(_sdCard);
  
  if (!os.open(folder, ".dcache.tmp")) {
    DBG_PRINTF("FatFsDirCache: failed to open '.dcache.tmp' in folder '%s'\n", _folder.c_str());
    return false;
  }
  
  bool r = dirReader.foreach([&](const FILINFO* info) { 
    DBG_PRINTF("FatFsDirCache: writing dir entry %s\n", info->fname);
    if (_filter(info->fname)) {
      if (!os.write(info)) {
        DBG_PRINTF("FatFsDirCache: failed to write '.dcache.tmp' in folder '%s'\n", _folder.c_str());
        return false;
      }
    }
    return true;
  });
  
  os.close();
 
  return r;
}

void FatFsDirCache::remove(const char *name) {
  
  DBG_PRINTF("FatFsDirCache: removing '%s' in folder '%s'\n", name, _folder.c_str());

  close();
  
  std::string cname;
  cname.append(_folder);
  cname.append("/");
  cname.append(name);  
    
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

void FatFsDirCache::reload() {
  close();
  remove(".dcache");
  create();
  sort();
  remove(".dcache.tmp");
  open();
}

void FatFsDirCache::load() {
  if (open()) return;
  reload();
}

bool FatFsDirCache::sort() {
  DBG_PRINTF("FatFsDirCache: sort in folder '%s'\n", _folder.c_str());
  FatFsDirCacheInputStream is(_sdCard);
  FatFsDirCacheOutputStream os(_sdCard);
  is.open(_folder.c_str(), ".dcache.tmp");
  os.open(_folder.c_str(), ".dcache");
  FatFsDirCacheSorter sorter(&is, &os);
  return sorter.sort();
}

uint32_t FatFsDirCache::size() {
  return _is.isOpen() ? _is.size() : 0;
}
