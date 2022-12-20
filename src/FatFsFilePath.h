#pragma once
#include <stdio.h>
#include <string>
#include <vector>

class FatFsFilePath {
  std::vector<std::string> _elements;
  FatFsFilePath* _parent;

public:
  FatFsFilePath(const char *root);
  FatFsFilePath();
  FatFsFilePath(FatFsFilePath* parent);
  FatFsFilePath(FatFsFilePath* parent, const char* folder);
  ~FatFsFilePath();
  
  void appendTo(std::string &fname);
  void push(const char *e);
  void pop();
  uint32_t size() { return _elements.size(); }
};
