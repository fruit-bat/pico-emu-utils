#include "FatFsDirCache.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiOutputStream.h"

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

FatFsDirCache::FatFsDirCache(SdCardFatFsSpi* sdCard) :
  _sdCard(sdCard),
  _folder()
{
}

void FatFsDirCache::create(const char *folder) {
  FatFsSpiDirReader dirReader(_sdCard, folder);
  FatFsSpiOutputStream os(_sdCard, folder, ".dcache");
  dirReader.foreach([&](const FILINFO* info) { 
    DBG_PRINTF("caching dir entry %s\n", info->fname);
    os.write((uint8_t *)info, (uint32_t)sizeof(FILINFO));
  });
}

void FatFsDirCache::remove(const char *folder) {
  std::string cname(folder);
  cname.append("/");
  cname.append(".dcache");
  const char* cp = cname.c_str();
  DBG_PRINTF("FatFsDirCache: removing cache in folder '%s'\n", folder);
  
  if (!_sdCard->mounted()) {   
    if (!_sdCard->mount()) {
      DBG_PRINTF("FatFsDirCache: failed to mount SD card\n");
      return;
    }
  }
  
  if(f_unlink(cp) != FR_OK) {
    DBG_PRINTF("FatFsDirCache: failed to remove cache in folder '%s'\n", folder);
  }
}
