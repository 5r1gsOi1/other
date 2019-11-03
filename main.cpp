
#include <iostream>
#include "chart/svg_chart.h"
#include "statistics/cache.h"
#include "statistics/statistics.h"
#include "tasks/analysis/analysis.h"
#include "tasks/charts/charts.h"
#include "tasks/ranks/afd.h"

int main() {
  std::cerr << "main start" << std::endl;
  std::string base_folder = "/media/sf_old_d/wiki";

  std::unique_ptr<Credentials> credentials{new Credentials{"", ""}};
  // credentials = ReadCredentialsFromFile(base_folder + "credentials.txt");

  ServerParameters server_parameters{"https://ru.wikipedia.org/w/api.php", ""};

  GeneralParameters parameters{
      std::move(credentials),
      ServerParameters{"https://ru.wikipedia.org/w/api.php", ""},
      {base_folder + "/cache/server", base_folder + "/cache/parser",
       base_folder + "/out/charts"}};
  EnsureAllfoldersExists(parameters.paths);

  // 2007, 9, 1 -- cache start
  // 2010, 8, 24 -- DiBot start
  const Date statistics_start(2007, 9, 1), differences_curves_start(2007, 9, 1),
      dibot_start(2010, 9, 1), marathons_statistics_start = Date(2014, 12, 31),
                               yesterday = --Date::Today(),
                               month_start{Date::Today() - 365 * 5};

  const Point<double> picture_size{2000, 1125};

  // CreateSvgChartForSummariesAfiStatistics(
  //    parameters, picture_size, marathons_statistics_start, yesterday);
  // CreateDetailedAfiSvgChartWithMarathons(parameters, picture_size,
  //                                       marathons_statistics_start,
  //                                       yesterday);
  //  CreateDetailedSvgCharts(parameters, picture_size, dibot_start,
  //  //statistics_start,
  //                          yesterday);
  //  CreateDifferencesSvgCharts(parameters, picture_size,
  //  differences_curves_start,
  //                             yesterday);
  CreateDibotSvgCharts(parameters, picture_size, dibot_start, yesterday);
  //CreateDibotSvgCharts(parameters, picture_size, month_start, yesterday,
  //                     "dibot_month.svg", 1);

  // CreateRanksForAfd(parameters, Date{2018, 12, 31}, Date{2018, 12, 31});

  // CheckAfiForDuplicates(parameters, yesterday);
  return 0;
}
