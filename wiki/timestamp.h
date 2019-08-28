
#pragma once

#include "wiki.h"

#include <iomanip>

namespace wiki {
struct Timestamp {
  int year{}, month{}, day{}, hour{}, minute{}, second{};

  Timestamp() = default;
  Timestamp(const int year_, const int month_, const int day_,
            const int hour_ = 0, const int minute_ = 0, const int second_ = 0)
      : year(year_),
        month(month_),
        day(day_),
        hour(hour_),
        minute(minute_),
        second(second_) {}
  Timestamp(const std::string& string) { *this = FromString(string); }

  bool operator<(const Timestamp& other) const {
    return this->year < other.year and this->month < other.month and
           this->day < other.day and this->hour < other.hour and
           this->minute < other.minute and this->second < other.second;
  }
  bool operator==(const Timestamp& other) const {
    return this->year == other.year and this->month == other.month and
           this->day == other.day and this->hour == other.hour and
           this->minute == other.minute and this->second == other.second;
  }
  bool operator!=(const Timestamp& other) const { return !(*this == other); }
  bool operator<=(const Timestamp& other) const {
    return *this < other || *this == other;
  }
  bool operator>(const Timestamp& other) const { return !operator<=(other); }
  bool operator>=(const Timestamp& other) const { return !(*this < other); }

  bool IsNull() const {
    return year == 0 and month == 0 and day == 0 and hour == 0 and
           minute == 0 and second == 0;
  }

  // 2018-06-10T05:56:38Z
  explicit operator std::string() const {
    std::stringstream stream;
    stream << std::setw(4) << std::setfill('0') << year << "-" << std::setw(2)
           << month << "-" << std::setw(2) << day << "T" << std::setw(2) << hour
           << ":" << std::setw(2) << minute << ":" << std::setw(2) << second
           << "Z";
    return stream.str();
  }

  static Timestamp FromString(const std::string& string) {
    return FromString(string.c_str());
  }

  static Timestamp FromString(const char* string) {
    int year{}, month{}, day{}, hour{}, minute{}, second{};
    Timestamp ts{};
    bool parsed = 6 == std::sscanf(string, "%4d-%2d-%2dT%2d:%2d:%2dZ", &year,
                                   &month, &day, &hour, &minute, &second) or
                  6 == std::sscanf(string, "%4d%2d%2dT%2d%2d%2dZ", &year,
                                   &month, &day, &hour, &minute, &second) or
                  false;
    if (not parsed) {
      char month_string[17];  // 23:25, 31 декабря 2018 (UTC)
      int parsed_tokens{std::sscanf(string, "%2d:%2d, %2d %16s %4d (UTC)",
                                    &hour, &minute, &day, month_string, &year)};
      month = GetMonthNumberFromStringGenitiveRussian(month_string);
      parsed = (5 == parsed_tokens and 0 != month);
      if (not parsed) {
        // 29 июля 2019 в 15:19 (UTC)
        int parsed_tokens{std::sscanf(string, "%2d %16s %4d в %2d:%2d (UTC)",
                                      &day, month_string, &year, &hour,
                                      &minute)};
        month = GetMonthNumberFromStringGenitiveRussian(month_string);
        parsed = (5 == parsed_tokens and 0 != month);
      }
    }

    if (parsed) {
      ts.year = year;
      ts.month = month;
      ts.day = day;
      ts.hour = hour;
      ts.minute = minute;
      ts.second = second;
    }
    return ts;
  }

  Timestamp& Now() {
    time_t now = time(nullptr);
    struct tm* time_info;
    time_info = localtime(&now);
    this->year = 1900 + time_info->tm_year;
    this->month = 1 + time_info->tm_mon;
    this->day = time_info->tm_mday;
    this->hour = time_info->tm_hour;
    this->minute = time_info->tm_min;
    this->second = time_info->tm_sec;
    return *this;
  }

 protected:
  static int GetMonthNumberFromStringGenitiveRussian(
      const std::string& string) {
    for (int i = 0; i <= 12; ++i) {
      if (string == GetMonthGenitiveNameInRussian(i)) {
        return i;
      }
    }
    return 0;
  }

  static std::string GetMonthGenitiveNameInRussian(const int month) {
    static const std::string months[]{
        "января", "февраля", "марта",    "апреля",  "мая",    "июня",
        "июля",   "августа", "сентября", "октября", "ноября", "декабря"};
    return (month > 0 and month <= 12) ? months[month - 1] : "UNKNOWN";
  }
};  // namespace wiki
}  // namespace wiki
