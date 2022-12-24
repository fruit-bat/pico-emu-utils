#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include "SdCardFatFsSpi.h"

class FatFsFilePath {
  std::vector<std::string> _elements;
  FatFsFilePath* _parent;

public:
  FatFsFilePath(const char *root);
  FatFsFilePath();
  FatFsFilePath(FatFsFilePath* parent);
  FatFsFilePath(FatFsFilePath* parent, const char* folder);
  ~FatFsFilePath();
  
  void appendTo(std::string &fname, bool abs = true);
  void push(const char *e);
  void pop();
  bool createFolders(SdCardFatFsSpi* sdCard, bool abs = true);
  uint32_t size() { return _elements.size(); }
  bool equals(const char *p);
};
