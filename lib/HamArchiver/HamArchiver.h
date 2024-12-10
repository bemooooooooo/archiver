#pragma once
#include "fstream"
#include "iostream"
#include "string"
#include "vector"
#include "filesystem"
#include "math.h"

class HamArchiver {
 public:
  void Create(const std::string& arch, std::vector<std::string>& files,
              const std::string& dir, uint8_t kAdditionalBits);
  void Append(const std::string& name, std::vector<std::string>& files,
              const std::string& dir,
              uint8_t kAdditionalBits);
  void ListArchive(std::string name, const std::string& dir);
  void ConcatenateArch(const std::string& archive,
                       const std::string& name_first,
                       const std::string& name_second,
                       const std::string& dir);
  void ExtractFiles(std::string& name_archive, std::vector<std::string>& files,
                    const std::string& dir);
  void DeleteFiles(const std::string& archive,
                   const std::vector<std::string>& files,
                   const std::string& dir);

 private:
  static const uint8_t kSizeInfoBits = 5;
  static const uint8_t kNameInfoBits = 4;
  static const uint8_t kSizeBits = 3;
  static const uint8_t kNameBits = 3;
  static const uint8_t kFileBits = 3;

  struct FileInfo {
    std::string name;
    uintmax_t size;
    uint8_t kAdditionalBits;

    FileInfo() {
      name = "";
      size = 0;
      kAdditionalBits = 3;
    }

    FileInfo(std::string new_name, uint64_t new_size,
             uint8_t new_kAdditionalBits) {
      name = new_name;
      size = new_size;
      kAdditionalBits = new_kAdditionalBits;
    }
  };

  bool SetAdditionalBits(std::vector<bool>& code, uint8_t index, uint64_t step);

  void WriteFile(std::ofstream& output, std::vector<bool>& code);

  void Encode(std::vector<bool>& code, uint8_t kAdditionalBits);

  void AppendToArchive(std::ofstream& output, const std::string& FileInfo,
                       int kAdditionalBits, const std::string& dir);

  void Extract(std::string& name_archive, std::string file,
               const std::string& dir);

  void Delete(const std::string& archive, const std::string& try_del,
              const std::string& dir);

  void EncodeHeader(const std::string& FileInfo, std::ofstream& output,
                  size_t file_size, uint8_t kAdditionalBits);

  void Decode(std::vector<bool>& code, uint8_t kAdditionalBits);

  FileInfo DecodeHeader(std::ifstream& archive);
  friend class ArchiveTest;
};
