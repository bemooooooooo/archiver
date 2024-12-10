#include <gtest/gtest.h>
#include <lib/ArgParser/ArgParser.h>
#include <lib/HamArchiver/HamArchiver.h>
#include <random>

#include <runtime/runtime.cpp>

std::string GenerateText(uint32_t number_of_bytes) {
  std::string result;
  for (uint32_t i = 0; i < number_of_bytes; ++i) {
    result.push_back(static_cast<char>(rand()));
  }
  return result;
}

std::filesystem::path GenerateFile(const std::string& filename,uint32_t number_of_bytes) {
  std::ofstream input("C:\\Users\\PC\\Source\\Repos\\labwork6-bemooooooooo1\\test_files\\" + filename, std::ios::binary);
  input << GenerateText(number_of_bytes);
  input.close();
  return std::filesystem::path(
      "C:\\Users\\PC\\Source\\Repos\\labwork6-bemooooooooo1\\test_files\\" +
      filename);
}

bool operator==(std::filesystem::path& lhs, std::filesystem::path& rhs) {
  uintmax_t size = std::filesystem::file_size(lhs);
  if (size != std::filesystem::file_size(rhs)) {
    return false;
  }
  std::ifstream input_1(lhs, std::ios::binary);
  std::ifstream input_2(rhs, std::ios::binary);
  char input_temp_1;
  char input_temp_2;
  for (uintmax_t i = 0; i < size; ++i) {
    input_1.get(input_temp_1);
    input_2.get(input_temp_2);
    if (input_temp_1 != input_temp_2) {
      return false;
    }
  }
  return true;
}

std::vector<bool> SetBits(size_t number, uint8_t additional_bits,
                          uint8_t for_encode = 1) {
  uint64_t lenght = ((1 << additional_bits) - for_encode * additional_bits - 1);
  std::vector<bool> result(lenght, 0);

  for (uint64_t i = 0; i < lenght; ++i) {
    result[i] = ((number & (1 << i)) >> i);
  }
  return result;
}

uint64_t GetBits(std::vector<bool>& bits) {
  uint64_t result = 0;

  for (int64_t i = 0; i < bits.size() - 1; ++i) {
    result |= (bits[i] << i);
  }
  return result;
}

TEST(CreateDir, Init) {
  std::filesystem::path new_dir =
      "C:\\Users\\PC\\Source\\Repos\\labwork6-bemooooooooo1\\test_output";
  ASSERT_NO_THROW(std::filesystem::create_directories(new_dir));
}

TEST(ArgParserTest, TestParse_0) {
  Option options;
  char** argv = new char*[4];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[3]{"-c"};
  argv[2] = new char[3]{"-f"};
  argv[3] = new char[8]{"HAMARCH"};
  options.Parse(4, argv);
  ASSERT_EQ(options.GetArchName(), "HAMARCH");
}

TEST(ArgParserTest, TestParse_1) {
  Option options;
  char** argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"--create"};
  argv[2] = new char[15]{"--file=HAMARCH"};
  argv[3] = new char[10]{"File1.txt"};
  argv[4] = new char[10]{"File2.txt"};
  options.Parse(5, argv);
  std::vector<std::string> list{"File1.txt", "File2.txt"};
  ASSERT_EQ(options.GetListing(), list);
}

TEST(ArgParserTest, TestParse_2) {
  Option options;
  char** argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"-A"};
  argv[2] = new char[6]{"FILE1"};
  argv[3] = new char[6]{"FILE2"};
  argv[4] = new char[15]{"--file=HAMARCH"};
  options.Parse(5, argv);
  std::pair<std::string, std::string> list{"FILE1", "FILE2"};
  ASSERT_EQ(options.GetConcatenate(), list);
}

class ArchiveTest : public ::testing::Test {
 protected:
  void Encode(std::vector<bool>& code, uint8_t kAdditionalBits) {
    archiver.Encode(code, kAdditionalBits);
  }
  void Decode(std::vector<bool>& code, uint8_t kAdditionalBits) {
    archiver.Decode(code, kAdditionalBits);
  }
  void AppendToArchive(std::ofstream& arch, const std::string& encode_file_name,
                       uint8_t kAdditionalBits, const std::string& dir) {
    archiver.AppendToArchive(arch, encode_file_name, kAdditionalBits, dir);
  }
  void Extract(std::string& name_archive, std::string file,
               const std::string& dir) {
    archiver.Extract(name_archive, file, dir);
  }

 private:
  HamArchiver archiver;
};

TEST_F(ArchiveTest, TestEncodeHamming_0) {
  std::vector<bool> bits = SetBits(153, 4);
  Encode(bits, 4);
  ASSERT_EQ(GetBits(bits), 2373);
}

TEST_F(ArchiveTest, TestEncodeHamming_1) {
  std::vector<bool> bits = SetBits(4652, 5);
  Encode(bits, 5);
  ASSERT_EQ(GetBits(bits), 172651);
}

TEST_F(ArchiveTest, TestDecodeHamming_0) {
  std::vector<bool> bits = SetBits(153, 4, 0);
  Encode(bits, 4);
  Decode(bits, 4);
  ASSERT_EQ(GetBits(bits), 153);
}

TEST_F(ArchiveTest, TestDecodeHamming_1) {
  std::vector<bool> bits = SetBits(4652, 5, 0);
  // Îäíà îøèáêà
  Encode(bits, 5);
  bits[3] = !bits[3];
  Decode(bits, 5);
  ASSERT_EQ(GetBits(bits), 4652);
}

TEST_F(ArchiveTest, TestDecodeHamming_2) {
  HamArchiver archiver;
  std::vector<bool> bits = SetBits(235, 5, 0);
  // Äâå îøèáêè
  Encode(bits, 4);
  bits[4] = !bits[4];
  bits[7] = !bits[7];
  ASSERT_EXIT(Decode(bits, 4), ::testing::ExitedWithCode(1), "Error");
}

TEST_F(ArchiveTest, CreateTest) {
  char** argv = new char*[5];
  argv[0] = new char[]{"hamarc"};
  argv[1] = new char[]{"--create"};
  argv[2] = new char[]{"--file=HAMARCH"};
  argv[3] = new char[]{"file1.txt"};
  argv[4] = new char[]{"file2.txt"};
  ASSERT_NO_THROW(runtime(5, argv));
}

TEST_F(ArchiveTest, ListTest) {
  char** argv = new char*[3];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"-l"};
  argv[2] = new char[15]{"--file=HAMARCH"};

  ASSERT_NO_THROW(runtime(3, argv));
}
//
// TEST_F(ArchiveTest, CreateTest_Photo) {
//  char** argv = new char*[6];
//  argv[0] = new char[7]{"hamarc"};
//  argv[1] = new char[9]{"--create"};
//  argv[2] = new char[16]{"--file=HAMARCH5"};
//  argv[3] = new char[40]{"C:\\Users\\PC\\Desktop\\archive\\files\\1.jpg"};
//  argv[4] = new char[3]{"-b"};
//  argv[5] = new char[2]{"8"};
//  ASSERT_NO_THROW(runtime(4, argv));
//}

TEST_F(ArchiveTest, ExtractTest) {
  std::filesystem::path test_file_1 = GenerateFile("1.txt", 1563);
  std::filesystem::path test_file_2 = GenerateFile("2.txt", 1563 * 2);


  char** argv = new char*[8];
  argv[0] = new char[]{"hamarc"};
  argv[1] = new char[]{"-c"};
  argv[2] = new char[]{"--file=TEST"};
  argv[3] = new char[]{"1.txt"};
  argv[4] = new char[]{"2.txt"};
  argv[5] = new char[]{"-o"};
  argv[7] = new char[]{
      "C:\\Users\\PC\\Source\\Repos\\labwork6-bemooooooooo1\\test_files\\"};
  runtime(7, argv);

  argv = new char*[6];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[3]{"-x"};
  argv[2] = new char[15]{"--file=TEST"};
  argv[3] = new char[6]{"2.txt"};
  argv[4] = new char[]{"-o"};
  argv[5] = new char[]{
      "C:\\Users\\PC\\Source\\Repos\\labwork6-bemooooooooo1\\test_files\\"};
  runtime(6, argv);

  std::filesystem::path result =
      "C:\\Users\\PC\\Source\\Repos\\labwork6-bemooooooooo1\\test_files\\copy_"
      "2.txt";

  ASSERT_EQ(test_file_2, result);
}
//
// TEST_F(ArchiveTest, ExtractTest_Photo) {
//  char** argv = new char*[4];
//  argv[0] = new char[7]{"hamarc"};
//  argv[1] = new char[3]{"-x"};
//  argv[2] = new char[16]{"--file=HAMARCH5"};
//  argv[3] = new char[40]{"1.jpg"};
//  ASSERT_NO_THROW(runtime(4, argv));
//}

TEST_F(ArchiveTest, ConcatenateTest) {
  char** argv = new char*[4];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"--create"};
  argv[2] = new char[16]{"--file=HAMARCH1"};
  argv[3] = new char[44]{"C:\\Users\\PC\\Desktop\\archive\\files\\file1.txt"};
  runtime(4, argv);

  argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"--create"};
  argv[2] = new char[16]{"--file=HAMARCH2"};
  argv[3] = new char[44]{"C:\\Users\\PC\\Desktop\\archive\\files\\file2.txt"};
  runtime(4, argv);

  argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[3]{"-A"};
  argv[2] = new char[9]{"HAMARCH1"};
  argv[3] = new char[9]{"HAMARCH2"};
  argv[4] = new char[16]{"--file=HAMARCH3"};
  ASSERT_NO_THROW(runtime(5, argv));
}

TEST_F(ArchiveTest, DeleteTest) {
  char** argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"--create"};
  argv[2] = new char[16]{"--file=HAMARCH4"};
  argv[3] = new char[44]{"C:\\Users\\PC\\Desktop\\archive\\files\\file1.txt"};
  argv[4] = new char[44]{"C:\\Users\\PC\\Desktop\\archive\\files\\file2.txt"};
  runtime(5, argv);

  argv = new char*[4];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[16]{"--file=HAMARCH4"};
  argv[2] = new char[9]{"--delete"};
  argv[3] = new char[10]{"file1.txt"};
  runtime(4, argv);

  argv = new char*[4];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[3]{"-x"};
  argv[2] = new char[16]{"--file=HAMARCH4"};
  argv[3] = new char[10]{"file1.txt"};
  ASSERT_EXIT(runtime(4, argv), ::testing::ExitedWithCode(1),
              "Error: File is not in archive\n");
}
