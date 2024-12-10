#pragma once
#include <iostream>
#include <string>
#include <vector>


class Option {
 private:
  std::string dir;
  std::vector<std::string> files;
  std::string archname;
  bool create;
  bool list;
  bool extract;
  bool append;
  bool remove;
  std::pair<std::string, std::string> concatenate;
  uint8_t additional_bits;
 public:
  Option();
  ~Option() {}

  std::string GetArchName();
  std::string GetDir();
  bool GetCreate();
  bool GetExtract();
  bool GetAppend();
  bool GetRemove();
  uint8_t GetAdditionalBits();
  bool GetList();
  std::vector<std::string> GetListing();
  std::pair<std::string, std::string> GetConcatenate();
  void Parse(int argc, char** argv);
};
