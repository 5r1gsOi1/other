
#ifndef BASIC_H_
#define BASIC_H_

#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace errors {
class General : public std::exception {};

class CredentialsAreAbsent : public General {};
class InvalidArguments : public General {};

class Cache : public General {};
class MoreThanOneDataForOneIdentifier : public Cache {};
class NoCacheForThisIdentifier : public Cache {};

class NoDataForThisIdentifier : public General {};

class FileNotFound : public General {};
class StreamNotOpened : public General {};
class NotAFileName : public General {};

// Is thrown when string pasring failed
class NotParsedFromString : public General {};
}  // namespace errors

template <typename T>
struct Point {
  T x, y;
  Point() : x{T{}}, y{T{}} {}
  Point(const T& x_, const T& y_) : x(x_), y(y_) {}
  Point<T> operator-(const Point<T>& other) const {
    return Point<T>(x - other.x, y - other.y);
  }
  Point<T> operator+(const Point<T>& other) const {
    return Point<T>(x + other.x, y + other.y);
  }
  template <class Other>
  Point<T> operator*(const Other& multiplier) const {
    return Point<T>(x * multiplier, y * multiplier);
  }
};

struct PointArea {
  Point<double> min, max;
  double height() const noexcept { return max.y - min.y; }
  double width() const noexcept { return max.x - min.x; }
};

std::string RemoveLeadingSpaces(const std::string& string,
                                const std::string& whitespace = " \t");

std::string RemoveRepeatingSpaces(const std::string& string);

std::string UnescapeString(const std::string& string);
void ReplaceSpacesWithUnderscores(std::string& string);
std::string ReplaceSpacesWithHtmlCode(const std::string& string);
std::string UrlEncode(const std::string& string);
std::string WindowsFileNameEncode(const std::string& string);
double CeilToMultiple(const double value, const int multiple);
double RoundToMultiple(const double value, const double multiple);
std::optional<std::string> LoadFileFromDisk(const std::string& file_name);
std::vector<std::string> SplitString(const std::string& string,
                                     const char delimiter);
void CreateDirectoryIfNotExists(const std::string& directory_name);
unsigned int CutToPrecision(const unsigned int number, const int precision);
std::string GenerateUUID();
void SaveFileToDisk(const std::string& file_name, const std::string& content);

template <class Data>
void PutDataToFileOnDisk(const std::string& file_name, const Data& data,
                         const bool overwrite = true) {
  if (not overwrite and std::ifstream(file_name).is_open()) {
    throw errors::FileNotFound();
  }
  auto directory_name = file_name.substr(0, file_name.find_last_of('/'));
  CreateDirectoryIfNotExists(directory_name);
  std::ofstream stream(file_name);
  if (not stream.is_open()) {
    throw errors::StreamNotOpened();
  }
  stream << data;
}

template <int MaxPrecision = 5>
class FixedPointNumber {
 public:
  FixedPointNumber() : integer_(0), fraction_(0) {}
  FixedPointNumber(const int integer, const unsigned int fraction)
      : integer_(integer), fraction_(fraction) {
    fraction_ = RemoveTrailingZeros(CutToPrecision(fraction_, MaxPrecision));
  }
  FixedPointNumber(const long double number) {
    if (number >= std::numeric_limits<int>::max() or
        number <= std::numeric_limits<int>::min()) {
      throw std::invalid_argument("number is out of limits");
    }
    integer_ = static_cast<int>(number);
    fraction_ =
        lround(std::abs(number - integer_) *
               std::pow(10, std::numeric_limits<unsigned int>::digits10));
    fraction_ = RemoveTrailingZeros(CutToPrecision(fraction_, MaxPrecision));
  }
  int integer() const { return integer_; }
  int fraction() const { return fraction_; }

  bool operator==(const FixedPointNumber& other) const {
    return this->integer_ == other.integer_ and
           this->fraction_ == other.fraction_;
  }
  bool operator!=(const FixedPointNumber& other) const {
    return not this->operator==(other);
  }
  bool operator==(const int integer) const {
    return this->integer() == integer and this->fraction() == 0;
  }
  bool operator!=(const int integer) const {
    return not this->operator==(integer);
  }

  operator std::string() const {
    return std::to_string(integer_) +
           (fraction_ > 0 ? "." + std::to_string(fraction_) : "");
  }

 private:
  int integer_;
  unsigned int fraction_;

  static unsigned int RemoveTrailingZeros(const unsigned int number) {
    auto result{number};
    while (result % 10 == 0 and result != 0) {
      result /= 10;
    }
    return result;
  }
};

struct Credentials {
  std::string username;
  std::string password;
};

std::ostream& operator<<(std::ostream& stream, const Credentials& credentials);
std::istream& operator>>(std::istream& stream, Credentials& credentials);

Credentials ReadCredentialsFromFile(const std::string& file_name);

struct ServerParameters {
  const std::string api_address;
  const std::string pages_address;
};

struct FileSystemPaths {
  std::string server_cache_folder;
  std::string parser_cache_folder;
  std::string out_charts_folder;
};

struct GeneralParameters {
  std::unique_ptr<Credentials> credentials;
  ServerParameters server;
  FileSystemPaths paths;
};

void SkipLines(const std::size_t number_of_lines, std::istream& stream);
std::size_t NumberOfSymbolsInUtf8String(const std::string& string);
std::istream& SafeGetline(std::istream& stream, std::string& string);

void EnsureAllfoldersExists(const FileSystemPaths& paths);

#endif
