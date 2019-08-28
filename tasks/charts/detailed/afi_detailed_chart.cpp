
#include "afi_detailed_chart.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <regex>
#include <string>
#include <vector>
#include "statistics/statistics.h"
#include "tasks/charts/detailed/detailed_chart.h"

Pages ParseImproveMainPage(const Date& identifier,
                           const std::string& page_text) {
  if (identifier < Date(2017, 05, 23)) {
    static const char kParseRegex[] =
        R"(\{\{\s*(У|у)*лучшение\s*статей\s*/\s*День\s*\|)"
        R"(\s*([0-9]{4}-[0-9]{1,2}-[0-9]{1,2})\s*\|(.*)\}\}\n)";
    return ParseMainPage<kParseRegex>(identifier, page_text);
  } else {
    static const char kParseRegex[] =
        R"((\|)([0-9]{4}-[0-9]{1,2}-[0-9]{1,2})\s*\|\s*\n(.*)\n)";
    return ParseMainPage<kParseRegex>(identifier, page_text);
  }
}

Curves GetAfiDetailedCurves(const GeneralParameters& parameters,
                            const Date& start_date, const Date& end_date,
                            const bool use_excluded_dates_list) {
  static const char kSubfolderName[] = "AFI";
  static const char kServerPageName[] = "Википедия:К улучшению";
  DetailedCurvesCreator<
      CreateDetailedStatsFromParsingMainPage<ParseImproveMainPage>>
      creator(kServerPageName, kSubfolderName, parameters);
  return creator.create(start_date, end_date, use_excluded_dates_list);
}

void CreateSvgChartForDetailedAfiStatistics(const GeneralParameters& parameters,
                                            const Point<double> image_size,
                                            const Date& start_date,
                                            const Date& end_date) {
  std::cout << start_date << " - " << end_date << std::endl;
  auto curves = GetAfiDetailedCurves(parameters, start_date, end_date);
  int smooth_number = 5;
  SmoothCurves(smooth_number, curves);

  DetailedSvgChart chart(image_size, start_date, end_date,
                         "Количество номинаций КУЛ");

  chart.AddCurve(curves[0],
                 svg::CreateStrokeAttributes("#008000", 5., "round"));
  chart.AddCurve(curves[2],
                 svg::CreateStrokeAttributes("#00A000", 5., "round"));
  chart.AddCurve(curves[3],
                 svg::CreateStrokeAttributes("#00C000", 5., "round"));
  chart.AddCurve(curves[4],
                 svg::CreateStrokeAttributes("#00E000", 5., "round"));
  chart.AddCurve(curves[5],
                 svg::CreateStrokeAttributes("#00FF00", 5., "round"));
  /*
    chart.AddCurve(curves[1],
                   svg::CreateStrokeAttributes("#0098FF", 2., "round", "7,3"),
                   false);*/

  auto file_name = parameters.charts_folder + "/afi_detailed.svg";
  chart.SaveToFile(file_name);
}
