
#include "basic.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <random>

std::string RemoveLeadingSpaces(const std::string& string,
                                const std::string& whitespace) {
  const auto begin = string.find_first_not_of(whitespace);
  if (begin == std::string::npos) {
    return std::string("");
  }
  const auto end = string.find_last_not_of(whitespace);
  const auto range = end - begin + 1;
  return string.substr(begin, range);
}

std::string RemoveRepeatingSpaces_OldStyle(
    const std::string& string, const std::string& fill = " ",
    const std::string& whitespace = " \t") {
  auto result = RemoveLeadingSpaces(string, whitespace);
  auto beginSpace = result.find_first_of(whitespace);
  while (beginSpace != std::string::npos) {
    const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
    const auto range = endSpace - beginSpace;
    result.replace(beginSpace, range, fill);
    const auto newStart = beginSpace + fill.length();
    beginSpace = result.find_first_of(whitespace, newStart);
  }
  return result;
}

std::string ReplaceMultiSpaceWithSingleSpace(const std::string& string) {
  char* dest = new char[string.size() + 1];
  if (not dest) {
    throw std::runtime_error("memory allocation failed");
  }
  std::size_t size = 0;
  char *pos = const_cast<char*>(string.c_str()), *dest_pos = dest;

  while (*pos != 0 and *pos == ' ') {
    ++pos;
  }
  while (*pos != 0) {
    while (*pos == ' ' and *(pos + 1) == ' ') {
      ++pos;
    }
    *dest_pos++ = *pos++;
    ++size;
  }
  *dest_pos = 0;
  if (*--dest_pos == ' ') {
    --size;
  }
  std::string result(dest, size);
  delete[] dest;
  return result;
}

std::string RemoveRepeatingSpaces(const std::string& string) {
  return ReplaceMultiSpaceWithSingleSpace(string);
  /*
    auto copy = RemoveLeadingSpaces(string, " ");
    auto BothAreSpaces = [](char lhs, char rhs) -> bool {
      return (lhs == rhs) and (lhs == ' ');
    };
    auto new_end = std::unique(copy.begin(), copy.end(), BothAreSpaces);
    copy.erase(new_end, copy.end());
    return copy;//*/
}

std::string UnescapeString(const std::string& string) {
  std::string result;
  result.reserve(string.size());
  std::string::const_iterator it = string.begin();
  while (it != string.end()) {
    char c = *it++;
    if (c == '\\' && it != string.end()) {
      switch (*it++) {
        case '\\':
          c = '\\';
          break;
        case 'n':
          c = '\n';
          break;
        case 't':
          c = '\t';
          break;
        // all other escapes
        default:
          // invalid escape sequence - skip it. alternatively you can copy it as
          // is, throw an exception...
          result += std::string("\\") + c;
          continue;
      }
    }
    result += c;
  }
  return result;
}

void ReplaceSpacesWithUnderscores(std::string& string) {
  std::string::iterator it = string.begin();
  while (it != string.end()) {
    char& c = *it++;
    if (c == ' ') {
      c = '_';
    }
  }
}

std::string ReplaceSpacesWithHtmlCode(const std::string& string) {
  std::string copy;
  copy.reserve(string.size() + 3 * static_cast<unsigned long>(std::count(
                                       string.begin(), string.end(), ' ')));
  for (auto i = 0ul; i < string.size(); ++i) {
    if (string[i] == ' ') {
      copy += "%20";
    } else {
      copy += string[i];
    }
  }
  return copy;
}

std::string WindowsFileNameEncode(const std::string& string) {
  std::string copy;
  copy.reserve(string.size() + 3 * string.size());
  for (auto c : string) {
    switch (c) {
      case '<':
        copy += "%3C";
        break;
      case '>':
        copy += "%3E";
        break;
      case ':':
        copy += "%3A";
        break;
      case '"':
        copy += "%22";
        break;
      case '/':
        copy += "%2F";
        break;
      case '\\':
        copy += "%5C";
        break;
      case '|':
        copy += "%7C";
        break;
      case '?':
        copy += "%3F";
        break;
      case '*':
        copy += "%2A";
        break;
      default:
        copy += c;
    };
  }
  copy.shrink_to_fit();
  return copy;
}

std::string UrlEncode(const std::string& string) {
  std::ostringstream encoded;
  encoded.fill('0');
  encoded << std::hex;
  for (auto c : string) {
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded << c;
    } else {
      encoded << std::uppercase;
      encoded << '%' << std::setw(2)
              << static_cast<int>(static_cast<unsigned char>(c));
      encoded << std::nouppercase;
    }
  }
  return encoded.str();
}

double CeilToMultiple(const double value, const int multiple) {
  return static_cast<int>(ceil(value / multiple) * multiple);
}

double GeneralRoundToMultiple(
    const double value, const double multiple,
    const std::function<double(double)>& round_function) {
  return round_function(value / multiple) * multiple;
}

double RoundToMultiple(const double value, const double multiple) {
  return GeneralRoundToMultiple(value, multiple, round);
}

std::optional<std::string> LoadFileFromDisk(const std::string& file_name) {
  std::ifstream stream(file_name);
  if (not stream.is_open()) {
    return std::nullopt;
  }
  std::stringstream sstr;
  sstr << stream.rdbuf();
  return sstr.str();
}

void SkipLines(const std::size_t number_of_lines, std::istream& stream) {
  std::string temp;
  for (auto i = 0u; i < number_of_lines; ++i) {
    std::getline(stream, temp);
  }
}

std::size_t NumberOfSymbolsInUtf8String(const std::string& string) {
  std::size_t length = 0;
  auto ptr = string.c_str();
  while (*ptr) {
    length += (*ptr++ & 0b1100'0000) != 0b1000'0000;
  }
  return length;
}

Credentials ReadCredentialsFromFile(const std::string& file_name) {
  std::ifstream file(file_name);
  if (not file.is_open()) {
    throw errors::StreamNotOpened();
  }
  Credentials credentials;
  file >> credentials;
  return credentials;
}

std::istream& operator>>(std::istream& stream, Credentials& credentials) {
  SafeGetline(stream, credentials.username);
  SafeGetline(stream, credentials.password);
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const Credentials& credentials) {
  stream << credentials.username << std::endl
         << credentials.password << std::endl;
  return stream;
}

std::istream& SafeGetline(std::istream& stream, std::string& string) {
  string.clear();

  // The characters in the stream are read one-by-one using a std::streambuf.
  // That is faster than reading them one-by-one using the std::istream.
  // Code that uses streambuf this way must be guarded by a sentry object.
  // The sentry object performs various tasks,
  // such as thread synchronization and updating the stream state.

  std::istream::sentry se(stream, true);
  std::streambuf* stream_buf = stream.rdbuf();

  for (;;) {
    int c = stream_buf->sbumpc();
    switch (c) {
      case '\n':
        return stream;
      case '\r':
        if (stream_buf->sgetc() == '\n') {
          stream_buf->sbumpc();
        }
        return stream;
      case std::streambuf::traits_type::eof():
        // Also handle the case when the last line has no line ending
        if (string.empty()) {
          stream.setstate(std::ios::eofbit);
        }
        return stream;
      default:
        string += static_cast<char>(c);
    }
  }
}

template <typename Out>
void split(const std::string& s, char delim, Out result) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    *(result++) = item;
  }
}

std::vector<std::string> SplitString(const std::string& string,
                                     const char delimiter) {
  if (string.empty()) {
    return std::vector<std::string>{""};
  }
  std::vector<std::string> result;
  std::string field;
  std::string::size_type position = 0, old_position = 0;
  while (std::string::npos !=
         (position = string.find(delimiter, old_position))) {
    result.push_back(string.substr(old_position, position - old_position));
    old_position = position + 1;
  }
  if (old_position < string.size()) {
    result.push_back(string.substr(old_position));
  } else if (string.size() > 0 and string.at(string.size() - 1) == delimiter) {
    result.push_back("");
  }
  return result;
}

void CreateDirectoryIfNotExists(const std::string& directory_name) {
  namespace fs = std::filesystem;
  if (not fs::is_directory(directory_name) or not fs::exists(directory_name)) {
    fs::create_directories(directory_name);
  }
}

unsigned int CutToPrecision(const unsigned int number, const int precision) {
  unsigned int return_number = number;
SecondCheck:
  int decimal_digits = static_cast<int>(1. + std::log10(return_number));
  int number_of_divisions =
      decimal_digits > precision ? decimal_digits - precision : 0;
  int last_digit =
      static_cast<int>(return_number / std::pow(10, number_of_divisions - 1)) -
      static_cast<int>(return_number / std::pow(10, number_of_divisions)) * 10;
  bool second_check_is_needed = last_digit >= 5;
  for (int i = 0; i < number_of_divisions; ++i) {
    return_number /= 10;
  }
  if (second_check_is_needed) {
    ++return_number;
    goto SecondCheck;
  }
  return return_number;
}

std::string ConvertToPercentEncoding(const std::string& string) {
  std::ostringstream out;
  for (auto c : string) {
    out << '%' << std::hex << std::uppercase << (int)(unsigned char)c;
  }
  return out.str();
}

const std::string CHARS =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

std::string GenerateUUID() {
  std::random_device random_device;
  std::default_random_engine engine{random_device()};
  std::uniform_int_distribution<int> random_uniform{};

  std::string uuid = std::string(36, ' ');
  int rnd = 0;
  int r = 0;

  uuid[8] = '-';
  uuid[13] = '-';
  uuid[18] = '-';
  uuid[23] = '-';

  uuid[14] = '4';

  for (int i = 0; i < 36; ++i) {
    if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
      if (rnd <= 0x02) {
        rnd = random_uniform(random_device);  // 0x2000000 + (std::rand() * 0x1000000) | 0;
      }
      rnd >>= 4;
      uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
    }
  }
  return uuid;
}


void SaveFileToDisk(const std::string& file_name, const std::string& content) {
  std::ofstream stream(file_name);
  if (not stream.is_open()) {
    throw errors::StreamNotOpened();
  }
  stream << content;
}

void EnsureAllfoldersExists(const FileSystemPaths& paths) {
  CreateDirectoryIfNotExists(paths.out_charts_folder);
  CreateDirectoryIfNotExists(paths.server_cache_folder);
  CreateDirectoryIfNotExists(paths.parser_cache_folder);
}
