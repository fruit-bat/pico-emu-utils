#include "FatFsDirCache.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiOutputStream.h"

FatFsDirCache::FatFsDirCache(SdCardFatFsSpi* sdCard) :
  _sdCard(sdCard),
  _folder()
{
}

void FatFsDirCache::create(char *folder) {
  _folder = folder;
  FatFsSpiDirReader dirReader(_sdCard, folder);
  FatFsSpiOutputStream os(_sdCard, folder, ".dcache");
  dirReader.foreach([&](const FILINFO* info) { 
    os.write((uint8_t *)info, (uint32_t)sizeof(FILINFO));
  });
}

