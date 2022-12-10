#pragma once

#include "SdCardFatFsSpi.h"
#include <string>
#include <functional>

class FatFsSpiDirReader {
private:
  SdCardFatFsSpi* _sdCard;
  std::string _folder;
public:
  FatFsSpiDirReader(SdCardFatFsSpi* sdCard, const char *folder);
  bool foreach(std::function <bool(const FILINFO* info)> cb);
  bool foreach(std::function <bool(const char* name)> cb);
};
