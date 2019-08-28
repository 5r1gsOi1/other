
#include "afd_detailed_chart.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <regex>
#include <string>
#include <vector>
#include "statistics/statistics.h"
#include "tasks/charts/detailed/detailed_chart.h"

Curves GetAfdDetailedCurves(const GeneralParameters& parameters,
                            const Date& start_date, const Date& end_date,
                            const bool use_excluded_dates_list) {
  static const char kSubfolderName[] = "AFD";
  static const char kServerPageName[] = "Википедия:К удалению";
  static const char kParseRegex[] =
      R"(\{\{\s*(У|у)*даление\s*статей\s*\|\s*([0-9]{4}-[0-9]{1,2}-[0-9]{1,2})\s*\|(.*)\}\}\n)";
  DetailedCurvesCreator<
      CreateDetailedStatsFromParsingMainPage<ParseMainPage<kParseRegex>>>
      creator(kServerPageName, kSubfolderName, parameters);
  return creator.create(start_date, end_date, use_excluded_dates_list);
}

void CreateSvgChartForDetailedAfdStatistics(const GeneralParameters& parameters,
                                            const Point<double> image_size,
                                            const Date& start_date,
                                            const Date& end_date) {
  std::cout << start_date << " - " << end_date << std::endl;
  auto curves = GetAfdDetailedCurves(parameters, start_date, end_date);

  int smooth_number = 5;
  SmoothCurves(smooth_number, curves);

  DetailedSvgChart chart(image_size, start_date, end_date,
                         "Количество номинаций КУ");

  chart.AddCurve(curves[0],
                 svg::CreateStrokeAttributes("#BB0000", 4., "round"));
  // chart.AddCurve(curves[1],
  //               svg::CreateStrokeAttributes("#FF1919", 5., "round"));
  chart.AddCurve(curves[2],
                 svg::CreateStrokeAttributes("#DD0000", 4., "round"));
  chart.AddCurve(curves[3],
                 svg::CreateStrokeAttributes("#FF0000", 4., "round"));
  chart.AddCurve(curves[4],
                 svg::CreateStrokeAttributes("#FF5555", 4., "round"));
  chart.AddCurve(curves[5],
                 svg::CreateStrokeAttributes("#FF9999", 4., "round"));

  auto file_name = parameters.charts_folder + "/afd_detailed.svg";
  chart.SaveToFile(file_name);
}
