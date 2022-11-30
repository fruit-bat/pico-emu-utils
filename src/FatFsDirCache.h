#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include "SdCardFatFsSpi.h"

class FatFsDirCache {
private:
  SdCardFatFsSpi* _sdCard;
  std::string _folder;
  uint16_t _folderDate;

public:
  FatFsDirCache(SdCardFatFsSpi* sdCard);
  void create(const char *folder);
  
};
