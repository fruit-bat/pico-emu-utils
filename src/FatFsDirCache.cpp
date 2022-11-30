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
  DBG_PRINTF("creating cache in folder '%s'\n", folder);
  _folder = folder;
  FatFsSpiDirReader dirReader(_sdCard, folder);
  FatFsSpiOutputStream os(_sdCard, folder, ".dcache");
  dirReader.foreach([&](const FILINFO* info) { 
    DBG_PRINTF("caching dir entry %s\n", info->fname);
    os.write((uint8_t *)info, (uint32_t)sizeof(FILINFO));
  });
}

