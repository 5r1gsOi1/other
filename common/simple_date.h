
#ifndef SIMPLE_DATE_H_
#define SIMPLE_DATE_H_

#include "basic.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Date;

Date ParseDate(const std::string& string);

struct Date {
  int year, month, day;

  Date() : year(0), month(0), day(0) {}

  Date(const int year_, const int month_, const int day_)
      : year(year_), month(month_), day(day_) {}

  Date(const int year_, const int month_)
      : year(year_), month(month_), day(1) {}

  Date(const std::string& string) { FromString(string); }

  //
  // Operators
  //

  Date& operator++() {
    ++day;
    if (day > MonthMaxDays(year, month)) {
      day = 1;
      IncrementMonth();
    }
    return *this;
  }

  Date& operator--() {
    --day;
    if (day <= 0) {
      --month;
      if (month <= 0) {
        if (year == 0) {
          month = 0;
          day = 0;
          return *this;
        }
        --year;
        month = 12;
      }
      day = MonthMaxDays(year, month);
    }
    return *this;
  }

  std::tm ToTm() const {
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    return tm;
  }

  std::size_t operator-(const Date& other) const {
    const Date& min_date = (*this) < other ? *this : other;
    const Date& max_date = other == min_date ? *this : other;

    std::tm tm_max = max_date.ToTm(), tm_min = min_date.ToTm();
    const std::size_t duration_in_seconds =
        static_cast<std::size_t>(std::mktime(&tm_max) - std::mktime(&tm_min));
    constexpr auto seconds_in_a_day = 60 * 60 * 24;
    return duration_in_seconds / seconds_in_a_day;
  }

  Date operator-(const int number_of_days) const {
    Date result{*this};
    for (int i{}; i < number_of_days; ++i) {
      --result;
    }
    return result;
  }

  bool operator<(const Date& other) const {
    long d1 = day + (month << 5) + (year << 9);
    long d2 = other.day + (other.month << 5) + (other.year << 9);
    return d1 < d2;
  }

  bool operator==(const Date& other) const {
    return this->year == other.year and this->month == other.month and
           this->day == other.day;
  }

  bool operator!=(const Date& other) const { return not(*this == other); }

  bool operator<=(const Date& other) const {
    return *this < other or *this == other;
  }

  bool operator>(const Date& other) const { return not operator<=(other); }

  bool operator>=(const Date& other) const { return not(*this < other); }

  operator std::string() const { return this->ToString(); }

  //
  //
  //

  static Date Today() {
    time_t now = time(nullptr);
    struct tm* time_info;
    time_info = localtime(&now);
    int year = 1900 + time_info->tm_year;
    int month = 1 + time_info->tm_mon;
    int day = time_info->tm_mday;
    return Date{year, month, day};
  }

  static Date ThisMonthStart() {
    auto date{Date::Today()};
    date.day = 1;
    return date;
  }

  void SetToToday() { *this = Today(); }

  //
  //
  //

  std::string GetGenitiveStringInRussian() const {
    return std::to_string(day) + " " + GetMonthGenitiveNameInRussian(month) +
           " " + std::to_string(year);
  }

  bool FromStringInRussian(const std::string& string) {
    std::string date_string = RemoveRepeatingSpaces(string);
    std::istringstream stream(string);
    int day = 0, month = 0, year = 0;
    std::string month_string;
    stream >> day >> month_string >> year;
    month = GetMonthNumberFromStringGenitiveRussian(month_string);
    return SafeAssign(day, month, year);
  }

  bool FromStringYearMonthDayWithDashes(const std::string& string) {
    std::string date_string = RemoveRepeatingSpaces(string);
    try {
      *this = ParseDate(string);
      return true;
    } catch (const errors::NotParsedFromString&) {
      return false;
    }
  }

  bool FromString(const std::string& string) {
    return FromStringYearMonthDayWithDashes(string) or
           FromStringInRussian(string) or false;
  }

  std::string ToString() const {
    std::ostringstream stream;
    stream << std::setw(4) << std::setfill('0') << year << "-" << std::setw(2)
           << month << "-" << std::setw(2) << day;
    return stream.str();
  }

  std::string ToStringInRussian() const {
    std::ostringstream stream;
    stream << day << " " << GetMonthGenitiveNameInRussian(month) << " " << year;
    return stream.str();
  }

  std::string ToStringYearMonth() const {
    std::ostringstream stream;
    stream << std::setw(4) << std::setfill('0') << year << std::setw(2)
           << month;
    return stream.str();
  }

  static int MonthMaxDays(const int year, const int month) {
    if (month == 2 and YearIsLeap(year)) {
      return 29;
    }
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return (month >= 1 and month <= 12) ? days[month - 1] : 0;
  }

 private:
  bool SafeAssign(const int day, const int month, const int year) {
    if (year > 0 and month >= 1 and month <= 12 and day >= 1 and
        day <= MonthMaxDays(year, month)) {
      this->year = year;
      this->month = month;
      this->day = day;
      return true;
    }
    return false;
  }

  int GetMonthNumberFromStringGenitiveRussian(const std::string& string) {
    for (int i = 0; i <= 12; ++i) {
      if (string == GetMonthGenitiveNameInRussian(i)) {
        return i;
      }
    }
    return 0;
  }

  Date& IncrementMonth() {
    ++month;
    if (month > 12) {
      month = 1;
      ++year;
    }
    return *this;
  }

  std::string GetMonthGenitiveNameInRussian(const int month) const {
    static const std::string months[]{
        "января", "февраля", "марта",    "апреля",  "мая",    "июня",
        "июля",   "августа", "сентября", "октября", "ноября", "декабря"};
    return (month > 0 and month <= 12) ? months[month - 1] : "UNKNOWN";
  }

  static bool YearIsLeap(const int year) {
    return year % 400 == 0 or (year % 100 != 0 and year % 4 == 0);
  }
};

std::ostream& operator<<(std::ostream& stream, const Date& date);
std::istream& operator>>(std::istream& stream, Date& date);

#endif

/*
// test

#include <chrono>

using days = std::chrono::duration<std::chrono::hours::rep, std::ratio<86400>>;
using years =
    std::chrono::duration<std::chrono::hours::rep, std::ratio<31556952>>;

days operator""_days(const unsigned long long days) { return ::days{days}; }
days operator""_years(const unsigned long long years) { return ::years{years}; }

*/
