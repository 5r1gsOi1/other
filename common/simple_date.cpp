
#include "simple_date.h"

// 2012-02-01
// 2012-2-01
// 2012-02-1
// 2012-2-1
// 12-02-01
// 12-2-01
// 12-02-1
// 12-2-1
// 012-02-01
// 012-2-01
// 012-02-1
// 012-2-1

Date ParseDate(const std::string& string) {
  Date date;
  auto first_dash = string.find("-", 0);
  if (first_dash == std::string::npos) {
    throw errors::NotParsedFromString();
  }
  auto second_dash = string.find("-", first_dash + 1);
  if (second_dash == std::string::npos) {
    throw errors::NotParsedFromString();
  }
  std::string year = string.substr(0, first_dash);
  std::string month =
      string.substr(first_dash + 1, second_dash - first_dash - 1);
  std::string day =
      string.substr(second_dash + 1, string.size() - second_dash - 1);
  if (year.empty() or month.empty() or day.empty()) {
    throw errors::NotParsedFromString();
  }
  // std::cout << "Date ParseDate(const std::string& string)\n" << std::flush;
  // std::cout << "\"" << string << "\"" << std::endl << std::flush;
  // std::cout << "\"" << year << "\"" << std::endl << std::flush;
  date.year = std::stoi(year);
  // std::cout << "\"" << month << "\"" << std::endl << std::flush;
  date.month = std::stoi(month);
  // std::cout << "\"" << day << "\"" << std::endl << std::flush;
  date.day = std::stoi(day);
  // std::cout << date << std::endl;
  return date;
}

std::ostream& operator<<(std::ostream& stream, const Date& date) {
  stream << date.ToString();
  return stream;
}

std::istream& operator>>(std::istream& stream, Date& date) {
  std::string line;
  std::getline(stream, line);
  if (line.empty() or not date.FromString(line)) {
    throw errors::NotParsedFromString();
  }
  return stream;
}
