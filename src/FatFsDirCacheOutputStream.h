#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include "SdCardFatFsSpi.h"
#include "FatFsSpiOutputStream.h"

class FatFsDirCacheOutputStream {
private:

  SdCardFatFsSpi* _sdCard;
  bool _open;
  FatFsSpiOutputStream *_os;
  
public:

  FatFsDirCacheOutputStream(SdCardFatFsSpi* sdCard);
  
  ~FatFsDirCacheOutputStream();
     
  bool open(
    const char* folder, 
    const char* filename
  );

  void close();
  
  bool write(const FILINFO* info);
};
