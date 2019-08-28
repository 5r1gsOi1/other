
#include "afmo_detailed_chart.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <regex>
#include <string>
#include <vector>

#include "statistics/statistics.h"
#include "tasks/charts/detailed/detailed_chart.h"
#include "wiki/parse.h"

std::string RemoveVerticalBarsFromString(const std::string& string) {
  std::string copy = string;
  std::string::size_type bar_position;
  while ((bar_position = copy.find('|')) != std::string::npos) {
    copy.erase(bar_position, 1);
  }
  return copy;
}

bool TryToExtractDateFromSubHeader(const std::string& string, Date& date) {
  std::string string_copy = RemoveLeadingSpaces(string);
  if (string.empty()) {
    return false;
  }
  if (string.at(0) == '|') {
    string_copy = RemoveVerticalBarsFromString(string_copy);
    return date.FromString(string_copy);
  } else if (string.at(0) == '{' and
             std::string::npos != string.find("ереименование статей/День")) {
    auto first_bar = string_copy.find('|');
    if (first_bar != std::string::npos) {
      auto second_bar = string_copy.find('|', first_bar + 1);
      if (second_bar != std::string::npos) {
        auto date_string =
            string_copy.substr(first_bar + 1, second_bar - first_bar - 1);
        return date.FromString(date_string);
      }
    }
  }
  return false;
}

bool TryToExtractDate(const std::string& line, Date& date) {
  return (line.at(0) == '{' or line.at(0) == '|') and
         TryToExtractDateFromSubHeader(line, date);
}

// 2017-05-22 -- the first of new format
Pages ParseMovementMainPage(const Date& identifier,
                            const std::string& page_text) {
  std::ignore = identifier;
  Pages result;
  std::istringstream stream(page_text);
  std::string line, previous_line;
  Date date;
  while (std::getline(stream, line)) {
    previous_line = RemoveLeadingSpaces(previous_line);
    if (previous_line.empty() or TryToExtractDate(previous_line, date)) {
      previous_line = line;
      continue;
    }
    if (((previous_line.substr(0, 2) == "* " and line.substr(0, 2) != "**") or
         previous_line.substr(0, 2) == "**") and
        not PageIsStrokeAway(previous_line)) {
      result.push_back(std::make_pair(date, previous_line));
    }

    previous_line = line;
  }
  if (line.substr(0, 1) == "*" and not PageIsStrokeAway(line)) {
    result.push_back(std::make_pair(date, line));
  }
  return result;
}

Curves GetAfmoDetailedCurves(const GeneralParameters& parameters,
                             const Date& start_date, const Date& end_date,
                             const bool use_excluded_dates_list) {
  static const char kSubfolderName[] = "AFMO";
  static const char kServerPageName[] = "Википедия:К переименованию";
  DetailedCurvesCreator<
      CreateDetailedStatsFromParsingMainPage<ParseMovementMainPage>>
      creator(kServerPageName, kSubfolderName, parameters);
  return creator.create(start_date, end_date, use_excluded_dates_list);
}

void CreateSvgChartForDetailedAfmoStatistics(
    const GeneralParameters& parameters, const Point<double> image_size,
    const Date& start_date, const Date& end_date) {
  std::cout << start_date << " - " << end_date << std::endl;
  auto curves = GetAfmoDetailedCurves(parameters, start_date, end_date);
  int smooth_number = 5;
  SmoothCurves(smooth_number, curves);

  DetailedSvgChart chart(image_size, start_date, end_date,
                         "Количество номинаций КПМ");

  chart.AddCurve(curves[0],
                 svg::CreateStrokeAttributes("#000080", 5., "round"));
  chart.AddCurve(curves[2],
                 svg::CreateStrokeAttributes("#0000A0", 5., "round"));
  chart.AddCurve(curves[3],
                 svg::CreateStrokeAttributes("#0000C0", 5., "round"));
  chart.AddCurve(curves[4],
                 svg::CreateStrokeAttributes("#0000E0", 5., "round"));
  chart.AddCurve(curves[5],
                 svg::CreateStrokeAttributes("#0000FF", 5., "round"));

  auto file_name = parameters.charts_folder + "/afmo_detailed.svg";
  chart.SaveToFile(file_name);
}
