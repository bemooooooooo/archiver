#include "HamArchiver.h"
#include "iostream"

void HamArchiver::Create(const std::string& arch,
                         std::vector<std::string>& files,
                         const std::string& dir,
                         uint8_t kAdditionalBits) {
  std::ofstream archive("C:\\Users\\PC\\Desktop\\archive\\" + arch + ".haf",
                        std::ios::binary);
  archive.close();
  Append(arch, files, dir, kAdditionalBits);
}

void HamArchiver::Append(const std::string& name,
                         std::vector<std::string>& files,
                         const std::string& dir,
            uint8_t kAdditionalBits) {
  std::ofstream output;
  output.open("C:\\Users\\PC\\Desktop\\archive\\" +
          name + ".haf",
      std::ios::binary | std::ios::app);
  for (size_t i = 0; i < files.size(); ++i) {
    AppendToArchive(output, files[i], kAdditionalBits, dir);
  }
  output.close();
}

void HamArchiver::ListArchive(std::string name, const std::string& dir) {
  std::ifstream input;
  input.open("C:\\Users\\PC\\Desktop\\archive\\" + name + ".haf",
             std::ios::binary);
  if (!input.is_open()) {
    std::cerr << "File dont open!\n";
  }
  char temp;
  while (input.get(temp)) {
    input.seekg(-1, std::ios::cur);
    FileInfo res = DecodeHeader(input);
    if (input.eof()) {
      break;
    }
    std::cout << res.name << "\n";
    input.seekg(input.tellg()+std::streampos(res.size));
  }
  input.close();
}

void HamArchiver::ConcatenateArch(const std::string& archive,
                                  const std::string& name_first,
                                  const std::string& name_second,
                                  const std::string& dir) {
  std::ifstream input_first(dir + name_first +
                                ".haf", std::ios::binary);
  std::ofstream output(dir + archive + ".haf",
                       std::ios::binary);
  char ch;
  while (input_first.get(ch)) {
    output << ch;
  }
  std::ifstream input_second(dir + name_second + ".haf",
      std::ios::binary);
  while (input_second.get(ch)) {
    output << ch;
  }
  output.close();
}

bool HamArchiver::SetAdditionalBits(std::vector<bool>& code, uint8_t index,
                                   uint64_t step) {
  bool result = false;
  for (size_t i = index; i < code.size(); i += (1 << step)) {
    for (size_t j = i; (j < i + (1 << (step - 1))) && j < code.size(); ++j) {
      result ^= code[j];
    }
  }
  return result;
}

void HamArchiver::Encode(std::vector<bool>& code,
                                 uint8_t kAdditionalBits) {
  bool partial_bit = false;
  for (size_t i = 0; i < kAdditionalBits; ++i) {
    code.insert(code.begin() + (1 << i) - 1, 0);
  }
  for (size_t i = 0; i < kAdditionalBits; ++i) {
    code[(1 << i) - 1] = SetAdditionalBits(code, (1 << i) - 1, i + 1);
  }
  for (size_t i = 0; i < code.size(); i++) {
    partial_bit ^= code[i];
  }
  code.push_back(partial_bit);
}

void HamArchiver::WriteFile(std::ofstream& output, std::vector<bool>& code) {
  char res = 0;
  for (int i = 0; i < code.size() / CHAR_BIT; ++i) {
    res = 0;
    for (int j = 0; j < CHAR_BIT; ++j) {
      res |= (code[i * CHAR_BIT + j] << j);
    }
    output << res;
  }
  code.clear();
}

void HamArchiver::AppendToArchive(std::ofstream& output,
                               const std::string& FileInfo,
                                  int kAdditionalBits, const std::string& dir) {
  std::ifstream input(dir + FileInfo, std::ios::binary);
  uint8_t block = (1 << kAdditionalBits) - kAdditionalBits - 1;
  size_t file_size = std::filesystem::file_size((dir + FileInfo));
  EncodeHeader(FileInfo, output, file_size, kAdditionalBits);
  std::vector<bool> code;
  char ch;
  while (input.get(ch)) {
    for (int i = 0; i < CHAR_BIT; ++i) {
      code.push_back((ch >> i) & 1);
      if (code.size() == block) {
        Encode(code, kAdditionalBits);
        WriteFile(output, code);
      }
    }
  }
  input.close();
}

void HamArchiver::ExtractFiles(std::string& name_archive,
                               std::vector<std::string>& files,
                               const std::string& dir) {
  for (size_t i = 0; i < files.size(); ++i) {
    Extract(name_archive, files[i], dir);
  }
}

void HamArchiver::Extract(std::string& name_archive, std::string file,
                          const std::string& dir) {
  std::ifstream input;
  input.open(dir + name_archive + ".haf", std::ios::binary);
  std::ofstream output(dir + "clone_" + file, std::ios::binary);
  if (!input.is_open()) {
    std::cerr << "File dont open!\n";
  }
  char temp;
  std::vector<bool> code;
  FileInfo inf = DecodeHeader(input);
  while (inf.name != file && input.get(temp)) {
    input.seekg(-1, std::ios::cur);
    input.seekg(input.tellg() + std::streampos(inf.size));
    inf = DecodeHeader(input);
    }
  for (int i = 0; i < inf.size / 2; ++i) {
    char temp_char_name = 0;
    for (int k = 0; k < 2; ++k) {
      input >> temp;
      for (int j = 0; j < CHAR_BIT; ++j) {
        code.push_back((temp >> j) & 1);
      }
      Decode(code, kFileBits);
      for (int j = 0; j < CHAR_BIT / 2; ++j) {
        temp_char_name |= (code[j] << (j + 4 * k));
      }
      code.clear();
    }
    output << temp_char_name;
  }
  input.close();
  output.close();
}

void HamArchiver::Delete(const std::string& archive, const std::string& try_del,
                         const std::string& dir) {
  std::ifstream input;
  input.open(dir + archive + ".haf", std::ios::binary);
  std::ofstream output(dir + archive + "_clone" + ".haf", std::ios::binary);
  if (!input.is_open()) {
    std::cerr << "File dont open!\n";
  }
  char temp;
  while (input) {
    FileInfo res = DecodeHeader(input);
    if (try_del == res.name) {
      input.seekg(res.size, std::ios::cur);
    } else {
      EncodeHeader(res.name, output, res.size, res.kAdditionalBits);
      for (int i = 0; i < res.size; ++i) {
        input >> temp;
        output << temp;
      }
    }
  }
  input.close();
  output.close();
}

void HamArchiver::DeleteFiles(const std::string& archive,
                              const std::vector<std::string>& files,
                              const std::string& dir) {
  for (int i = 0; i < files.size(); ++i) {
    Delete(archive, files[i], dir);
  }
}

void HamArchiver::EncodeHeader(const std::string& FileInfo, std::ofstream& output,
                size_t file_size, uint8_t kAdditionalBits) {
  std::vector<bool> code;
  uint8_t length_code = (CHAR_BIT / ((1 << kSizeBits) - kSizeBits) - 1);
  size_t len_blocks_name =
      FileInfo.size() * length_code;
  for (int i = 0; i < (1 << kNameInfoBits) - kNameInfoBits - 1; ++i) {
    code.push_back((len_blocks_name >> i) & 1);
  }
  Encode(code, kNameInfoBits);
  WriteFile(output, code);
  for (int i = 0; i < FileInfo.size(); ++i) {
    for (int j = 0; j < CHAR_BIT; ++j) {
      code.push_back((FileInfo[i] >> j) & 1);
      if (code.size() == length_code) {
        Encode(code, 3);
        WriteFile(output, code);
      }
    }
  }
  for (int i = 0; i < length_code; ++i) {
    code.push_back((kAdditionalBits >> i) & 1);
  }
  Encode(code, kSizeBits);
  WriteFile(output, code);
  size_t len_blocks_size = std::ceil(std::log2(file_size)/(length_code));
  for (int i = 0; i < (1 << kSizeInfoBits) - kSizeInfoBits - 1; ++i) {
    code.push_back((len_blocks_size >> i) & 1);
  }
  Encode(code, kSizeInfoBits);
  WriteFile(output, code);
  for (int i = 0; i < len_blocks_size; ++i) {
    for (int j = 0; j < length_code; ++j) {
      code.push_back((file_size >> (j+length_code*i)) & 1);
      if (code.size() == length_code) {
        Encode(code, kSizeBits);
        WriteFile(output, code);
      }
    }
  }
}

void HamArchiver::Decode(std::vector<bool>& code, uint8_t kAdditionalBits) {
  std::vector<bool> error_bit;
  bool double_error_bit = code.back();
  code.pop_back();
  for (size_t i = 0; i < kAdditionalBits; ++i) {
    error_bit.push_back(SetAdditionalBits(code, (1 << i) - 1, i + 1));
  }
  uint32_t error_number = 0;
  for (int32_t i = 0; i < error_bit.size() - 1; ++i) {
    error_number |= (error_bit[i] << i);
  }
  if (!double_error_bit && error_number != 0) {
    std::cerr << "Error" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (error_number != 0) {
    code[error_number - 1] = (code[error_number - 1] + 1) % 2;
  }
  std::vector<bool> result;
  uint8_t current_pow = 1;
  for (size_t i = 0; i < code.size(); i++) {
    if (i == current_pow - 1) {
      current_pow *= 2;
      continue;
    }
    result.push_back(code[i]);
  }
  code = result;
}

HamArchiver::FileInfo HamArchiver::DecodeHeader(std::ifstream& archive) {
  uint64_t len_size_text_bit = 0;
  uint64_t size_text_bit = 0;
  uint64_t size_name = 0;
  std::string full_name = "";
  char temp_char_first, temp_char_second;
  std::vector<bool> code;
  archive >> temp_char_first >> temp_char_second;
  for (int j = 0; j < CHAR_BIT; ++j) {
    code.push_back((temp_char_first >> j) & 1);
  }
  for (int j = 0; j < CHAR_BIT; ++j) {
    code.push_back((temp_char_second >> j) & 1);
  }
  Decode(code, 4);
  for (uint64_t i = 0; i < (1 << kNameInfoBits) - kNameInfoBits - 1; ++i) {
    size_name |= (code[i] << i);
  }
  code.clear();
  
  uint8_t length_code = (1 << kNameBits) - kNameBits - 1;
  for (uint64_t i = 0; i < size_name / 2; ++i) {
    archive >> temp_char_first >> temp_char_second;
    for (int j = 0; j < CHAR_BIT; ++j) {
      code.push_back((temp_char_first >> j) & 1);
    }
    Decode(code, kNameBits);
    char temp_char_name = 0;
    for (uint64_t j = 0; j < length_code; ++j) {
      temp_char_name |= (code[j] << j);
    }
    code.clear();
    for (int j = 0; j < CHAR_BIT; ++j) {
      code.push_back((temp_char_second >> j) & 1);
    }
    Decode(code, kNameBits);
    for (uint64_t j = 0; j < length_code; ++j) {
      temp_char_name |= (code[j] << (j + 4));
    }
    code.clear();
    full_name += static_cast<char>(temp_char_name);
  }
  
  length_code = (1 << kSizeBits) - kSizeBits - 1;
  uint8_t kDopBit = 0;
  archive >> temp_char_first;
  for (int j = 0; j < CHAR_BIT; ++j) {
    code.push_back((temp_char_first >> j) & 1);
  }
  Decode(code, kSizeBits);
  for (uint64_t j = 0; j < length_code; ++j) {
    kDopBit |= (code[j] << j);
  }
  code.clear();
  
  char temp_char_third, temp_char_fourth;
  archive >> temp_char_first >> temp_char_second >> temp_char_third >>
      temp_char_fourth;
  for (int j = 0; j < CHAR_BIT; ++j) {
    code.push_back((temp_char_first >> j) & 1);
  }
  for (int j = 0; j < CHAR_BIT; ++j) {
    code.push_back((temp_char_second >> j) & 1);
  }
  for (int j = 0; j < CHAR_BIT; ++j) {
    code.push_back((temp_char_third >> j) & 1);
  }
  for (int j = 0; j < CHAR_BIT; ++j) {
    code.push_back((temp_char_fourth >> j) & 1);
  }
  Decode(code, kSizeInfoBits);
  for (uint64_t i = 0; i < (1 << kSizeInfoBits) - kSizeInfoBits - 1; ++i) {
    len_size_text_bit |= (code[i] << i);
  }
  code.clear();
  
  std::vector<bool> temp_char_size;
  for (uint64_t i = 0; i < len_size_text_bit; ++i) {
    archive >> temp_char_first;
    for (int j = 0; j < CHAR_BIT; ++j) {
      code.push_back((temp_char_first >> j) & 1);
    }
    Decode(code, kSizeBits);
    for (int8_t k = 0; k < length_code; ++k) {
      temp_char_size.push_back(code[k]);
    }
    code.clear();
  }
  for (uint64_t i = 0; i < len_size_text_bit*length_code; ++i) {
    size_text_bit |= (temp_char_size[i] << i); 
  }
  FileInfo res = FileInfo(full_name, size_text_bit * (CHAR_BIT/((1<<kSizeBits)-kSizeBits-1)),
      kDopBit);
  return res;
}
