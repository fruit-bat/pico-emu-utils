#include "FatFsDirCacheOutputStream.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiOutputStream.h"

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#define FILINFO_SIZE ((uint32_t)sizeof(FILINFO))

FatFsDirCacheOutputStream::FatFsDirCacheOutputStream(SdCardFatFsSpi* sdCard) :
  _sdCard(sdCard),
  _open(false),
  _os(0)
{
}

FatFsDirCacheOutputStream::~FatFsDirCacheOutputStream() {
  close();
}

bool FatFsDirCacheOutputStream::open(
  const char* folder, 
  const char* filename
) {
  
  if (!_open) {
    DBG_PRINTF("FatFsDirCacheOutputStream: opening '%s' in '%s'\n", filename, folder);
    
    std::string cname;
    cname.append(folder);
    cname.append("/");
    cname.append(filename);

    const char* cp = cname.c_str();
    DBG_PRINTF("FatFsDirCacheOutputStream: opening cache file '%s'\n", cp);
  
    _os = new FatFsSpiOutputStream(_sdCard, cp);

    if (_os == 0) {
      DBG_PRINTF("FatFsDirCacheOutputStream: failed to allocate cache file '%s'\n", cp);
    }
    else if (_os->closed()) {
      DBG_PRINTF("FatFsDirCacheOutputStream: failed to open cache file '%s'\n", cp);
      if (_os) delete _os;
      _os = 0;
    }
    else {
      _open = true;      
    }
  }
  
  return _open;
}

void FatFsDirCacheOutputStream::close() {
  if (_open) {
    DBG_PRINTF("FatFsDirCacheOutputStream: closing'\n");
    if (_os) delete _os;
    _os = 0;
    _open = false;
  }
}

bool FatFsDirCacheOutputStream::write(const FILINFO* info) {
  if (_open) {
    DBG_PRINTF("FatFsDirCacheOutputStream: writing entry\n");
    int32_t r = _os->write((uint8_t *)info, FILINFO_SIZE);
    if (r < -1) {
      close();
    }
    if (r < 0) return false;
    return true;
  }
  else {
    return false;
  }
}
