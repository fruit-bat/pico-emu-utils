#pragma once
#include <stdio.h>
#include <string>
#include <vector>

class FatFsFilePath {
  std::vector<std::string> _elements;

public:
  FatFsFilePath();
  ~FatFsFilePath();
  
  void appendTo(std::string &fname);
  void push(char *e);
  void pop();
};
