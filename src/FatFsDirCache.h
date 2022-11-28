#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include "SdCardFatFsSpi.h"

class FatFsDirCache {
private:
  SdCardFatFsSpi* _sdCard;
  std::string _folder;

public:
  FatFsDirCache(SdCardFatFsSpi* sdCard);
  void create(char *folder);
  
};
