#include "lib/ArgParser/ArgParser.h"
#include "lib/HamArchiver/HamArchiver.h"
#include "fstream"
#include "iostream"
#include "vector"

int runtime(int argc, char** argv) {
  HamArchiver arch;
  Option options;
  options.Parse(argc, argv);
  std::string dir = options.GetDir();
  std::string FileInfo = options.GetArchName();
  uint8_t additional_bits = options.GetAdditionalBits();
  std::vector<std::string> files = options.GetListing();
  if (options.GetCreate()) {
    arch.Create(FileInfo, files, dir, additional_bits);
  }
  if (options.GetList()) {
    arch.ListArchive(FileInfo, dir);
  } else if (options.GetAppend()) {
    arch.Append(FileInfo, files, dir, additional_bits);
  } else if (options.GetConcatenate() !=
             std::pair<std::string, std::string>()) {
    arch.ConcatenateArch(FileInfo, options.GetConcatenate().first,
                    options.GetConcatenate().second, dir);
  } else if (options.GetRemove()) {
    arch.DeleteFiles(FileInfo, options.GetListing(), dir);
  } else if (options.GetExtract()) {
    arch.ExtractFiles(FileInfo, files, dir);
  }
  return 0;
}
