#include "FatFsSpiInputStream.h"
#include <pico/printf.h>

// #define DEBUG_FAT_IS

#ifdef DEBUG_FAT_IS
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

FatFsSpiInputStream::FatFsSpiInputStream(SdCardFatFsSpi* sdCard, const char* name) :
  FatFsSpiInputStream(sdCard, name, (uint32_t)FA_READ|FA_OPEN_EXISTING)
{
}

FatFsSpiInputStream::FatFsSpiInputStream(SdCardFatFsSpi* sdCard, const char* name, uint32_t mode) :
  _sdCard(sdCard),
  _eof(false),
  _open(false)
{
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) {
      DBG_PRINTF("Failed to mount SD card\n");
      return;
    }
  }
  
  DBG_PRINTF("openning file %s for read\n", name);
  _fr = f_open(&_fil, name, mode);
  if (FR_OK != _fr && FR_EXIST != _fr) {
    DBG_PRINTF("f_open(%s) error: %s (%d)\n", name, FRESULT_str(_fr), _fr);
  }
  else {
    DBG_PRINTF("openned file %s ok!\n", name);
    _open = true;
    _fr = FR_OK;
  }
}

int32_t FatFsSpiInputStream::readByte() {
  uint8_t b;
  int32_t r = read(&b, 1);
  return r < 0 ? r : b;
}

int32_t FatFsSpiInputStream::read(uint8_t* buffer, const uint32_t length) {
  if (_eof || !_open) return -1;
  if (FR_OK != _fr) return -2; 
  UINT br = 0;
  _fr = f_read(&_fil, buffer, length, &br);
  _eof = br == 0;
  if (_eof) {
    DBG_PRINTF("eof\n");
    return -1;
  }
  if (_fr != FR_OK) {
    DBG_PRINTF("f_read(%s) error: (%d)\n", FRESULT_str(_fr), _fr);
    return -2;
  }
  return br;
}

void FatFsSpiInputStream::close() {
  if (_open) {
    f_close(&_fil);
    _open = false;
  }
}

bool FatFsSpiInputStream::closed() {
  return !_open;
}

bool FatFsSpiInputStream::end() {
  return _eof;
}

int32_t FatFsSpiInputStream::seek(const uint32_t pos) {
  DBG_PRINTF("seek pos (%ld)\n", pos);
  if (!_open) return -1;
  if (FR_OK != _fr) return -2;
  _fr = f_lseek(&_fil, pos);
  if (_fr != FR_OK) {
    DBG_PRINTF("f_seek(%s) error: (%d)\n", FRESULT_str(_fr), _fr);
    return -2;
  }
  _eof = false;
  return 0;
}

int32_t FatFsSpiInputStream::rseek(const int32_t rpos) {
  DBG_PRINTF("rseek rpos (%ld)\n", rpos);
  if (!_open) return -1;
  if (FR_OK != _fr) return -2;
  return seek(rpos + f_tell(&_fil));
}

uint32_t FatFsSpiInputStream::pos() {
  if (!_open) return 0;
  if (FR_OK != _fr) return 0;
  return f_tell(&_fil);
}

uint32_t FatFsSpiInputStream::size() {
  if (!_open) return 0;
  if (FR_OK != _fr) return 0;
  return f_size(&_fil);
}

int32_t FatFsSpiInputStream::write(uint8_t* buffer, const uint32_t length) {
  if (!_open) return -1;
  
  // TODO Handle errors with separate codes
  if (FR_OK != _fr) return -1;

  UINT bw = 0;
  _fr = f_write(&_fil, buffer, length, &bw);
  if (_fr != FR_OK) {
    DBG_PRINTF("f_write(%s) error: (%d)\n", FRESULT_str(_fr), _fr);
    return -2;
  }
  if (bw < length) {
    DBG_PRINTF("f_write error: wrote %d of %ld\n", bw, length);
    return -3; // failed to write
  }
  return bw;
}
