#include <pico/printf.h>
#include "FatFsSpiDirReader.h"
#include "ff.h"

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

FatFsSpiDirReader::FatFsSpiDirReader(SdCardFatFsSpi* sdCard, const char *folder) :
  _sdCard(sdCard),
  _folder(folder)
{
}

void FatFsSpiDirReader::foreach(std::function <void(const FILINFO* info)> cb) {
  if (!_sdCard->mounted()) {   
    if (!_sdCard->mount()) {
      DBG_PRINTF("Failed to mount SD card\n");
      return;
    }
  }
  DBG_PRINTF("reading folder %s\n", _folder.c_str());

  DIR dj;         /* Directory object */
  FILINFO fno;    /* File information */
  FRESULT dfr = f_findfirst(&dj, &fno, _folder.c_str(), "*.*");

  while (dfr == FR_OK && fno.fname[0]) {
    DBG_PRINTF("file %s\n", fno.fname);
    cb(&fno);
    dfr = f_findnext(&dj, &fno); // Search for next item
  }

  f_closedir(&dj);
}

void FatFsSpiDirReader::foreach(std::function <void(const char* name)> cb) {
  foreach([=](const FILINFO* info){ cb(info->fname); });
}
