#include "ArgParser.h"

Option::Option() {
  dir = "C:\\Users\\PC\\Source\\Repos\\labwork6-bemooooooooo1\\test_files\\";
  archname = "";
  create = false;
  list = false;
  extract = false;
  append = false;
  remove = false;
  concatenate = std::pair<std::string, std::string>();
  additional_bits = 3;
}

std::string Option::GetArchName() { return archname; }

std::string Option::GetDir() { return dir; }

bool Option::GetCreate() { return create; }

bool Option::GetExtract() { return extract; }

bool Option::GetAppend() { return append; }

bool Option::GetRemove() { return remove; }

uint8_t Option::GetAdditionalBits() { return additional_bits; }

bool Option::GetList() { return list; }

std::vector<std::string> Option::GetListing() { return files; }

std::pair<std::string, std::string> Option::GetConcatenate() {
  return concatenate;
}

void Option::Parse(int argc, char** argv) {
  bool flag = true;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-c" || arg == "--create") {
      create = true;
      flag = false;
    } else if ((arg == "-l" || arg == "--list") && flag) {
      list = true;
      flag = false;
    } else if ((arg == "-x" || arg == "--extract") && flag) {
      extract = true;
      flag = false;
    } else if ((arg == "-a" || arg == "--append") && flag) {
      append = true;
      flag = false;
    } else if ((arg == "-d" || arg == "--delete") && flag) {
      remove = true;
      flag = false;
    } else if (arg == "-f") {
      ++i;
      archname = argv[i];
    } else if (arg.substr(0, arg.find_first_of('=')+1) == "--file=") {
      archname = arg.substr(arg.find_first_of('=')+1);	 
	} else if ((arg == "-o" || arg == "--output") && flag) {
      ++i;
      dir = argv[i];
    } else if ((arg == "-A" || arg == "--concatenate") && flag) {
      ++i;
      concatenate.first = argv[i];
      ++i;
      concatenate.second = argv[i];
    } else if (arg == "-b" || arg == "--bits") {
      ++i;
      additional_bits = std::stoi(argv[i]);
    } else {
      files.push_back(arg);
    }
  }
}
