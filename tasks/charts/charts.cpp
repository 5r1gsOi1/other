
#include "tasks/charts/charts.h"
#include "chart/date_chart/chart.h"
//#include "tasks/charts/detailed/afd_detailed_chart.h"
//#include "tasks/charts/detailed/afi_detailed_chart.h"
//#include "tasks/charts/detailed/afmo_detailed_chart.h"
//#include "tasks/charts/detailed/detailed_chart.h"
#include "tasks/charts/dibot/dibot_chart.h"

static const std::vector<std::pair<Date, Date>> afi_marathons{
    {Date{2015, 05, 20}, Date{2015, 06, 20}},
    {Date{2015, 12, 01}, Date{2015, 12, 31}},
    {Date{2016, 04, 15}, Date{2016, 05, 15}},
    {Date{2016, 12, 01}, Date{2016, 12, 31}},
    {Date{2017, 05, 22}, Date{2017, 06, 22}},
    {Date{2017, 10, 16}, Date{2017, 11, 01}},
    {Date{2018, 05, 01}, Date{2018, 06, 01}},
    {Date{2018, 12, 04}, Date{2019, 01, 04}},
};

#if 0
void CreateDetailedAfiSvgChartWithMarathons(const GeneralParameters &parameters,
                                            const Point<double> image_size,
                                            const Date &start_date,
                                            const Date &end_date) {
  std::cout << __func__ << " : " << start_date << " - " << end_date
            << std::endl;
  auto afi_curves = GetAfiDetailedCurves(parameters, start_date, end_date);
  int smooth_number = 5;
  SmoothCurves(smooth_number, afi_curves);

  DetailedSvgChart afi_chart(image_size, start_date, end_date,
                             "Количество номинаций КУЛ с марафонами");

  afi_chart.AddDatesRects("Марафоны", afi_marathons,
                          svg::CreateFillAttributes("#DDDDDD"));

  double curve_width = 4.;

  afi_chart.AddCurve(afi_curves[0], svg::CreateStrokeAttributes(
                                        "#008000", curve_width, "round"));
  afi_chart.AddCurve(afi_curves[2], svg::CreateStrokeAttributes(
                                        "#00A000", curve_width, "round"));
  afi_chart.AddCurve(afi_curves[3], svg::CreateStrokeAttributes(
                                        "#00C000", curve_width, "round"));
  afi_chart.AddCurve(afi_curves[4], svg::CreateStrokeAttributes(
                                        "#00E000", curve_width, "round"));
  afi_chart.AddCurve(afi_curves[5], svg::CreateStrokeAttributes(
                                        "#00FF00", curve_width, "round"));

  afi_chart.SaveToFile(parameters.charts_folder +
                       "/afi_detailed_with_marathons.svg");
}

Curves CreateDifferencesCurves(const Curves &curves) {
  Curves differences;
  differences.resize(curves.size());
  for (auto i = 0ul; i < curves.at(0).points.size(); ++i) {
    for (auto j = 0ul; j < curves.size() - 1; ++j) {
      differences.at(j).points.push_back(Point<double>(
          curves.at(j).points.at(i).x,
          curves.at(j).points.at(i).y - curves.at(j + 1).points.at(i).y));
    }
    differences.at(curves.size() - 1)
        .points.push_back(curves.at(curves.size() - 1).points.at(i));
  }
  return differences;
}

void CreateDifferencesSvgCharts(const GeneralParameters &parameters,
                                const Point<double> image_size,
                                const Date &start_date, const Date &end_date) {
  std::cout << __func__ << " : " << start_date << " - " << end_date
            << std::endl;
  auto afd_curves = GetAfdDetailedCurves(parameters, start_date, end_date);
  auto afi_curves = GetAfiDetailedCurves(parameters, start_date, end_date);
  auto afmo_curves = GetAfmoDetailedCurves(parameters, start_date, end_date);

  int smooth_number = 5;

  DetailedSvgChart afd_difference_chart(image_size, start_date, end_date,
                                        "Количество номинаций КУ");
  DetailedSvgChart afi_difference_chart(image_size, start_date, end_date,
                                        "Количество номинаций КУЛ");
  DetailedSvgChart afmo_difference_chart(image_size, start_date, end_date,
                                         "Количество номинаций КПМ");

  afmo_difference_chart.SetVertialSteps(500, 500, 500);

  auto afd_difference_curves = CreateDifferencesCurves(afd_curves);
  SmoothCurves(smooth_number, afd_difference_curves);

  double curve_width = 4.;
  afd_difference_chart.AddCurve(
      afd_difference_curves[0],
      svg::CreateStrokeAttributes("#BB0000", curve_width, "round"),
      std::string("[0, 8)"));
  afd_difference_chart.AddCurve(
      afd_difference_curves[1],
      svg::CreateStrokeAttributes("#FF0000", curve_width, "round"),
      std::string("[8, 30)"));
  afd_difference_chart.AddCurve(
      afd_difference_curves[2],
      svg::CreateStrokeAttributes("#DD8800", curve_width, "round"),
      std::string("[30, 90)"));
  afd_difference_chart.AddCurve(
      afd_difference_curves[3],
      svg::CreateStrokeAttributes("#00ddDD", curve_width, "round"),
      std::string("[90, 183)"));
  afd_difference_chart.AddCurve(
      afd_difference_curves[4],
      svg::CreateStrokeAttributes("#dd00dd", curve_width, "round"),
      std::string("[183, 365)"));
  afd_difference_chart.AddCurve(
      afd_difference_curves[5],
      svg::CreateStrokeAttributes("#0088FF", curve_width, "round"),
      std::string("[365, 730)"));
  afd_difference_chart.AddCurve(
      afd_difference_curves[6],
      svg::CreateStrokeAttributes("#888888", curve_width, "round"),
      std::string("[730, +∞)"));

  auto afi_difference_curves = CreateDifferencesCurves(afi_curves);
  SmoothCurves(smooth_number, afi_difference_curves);

  afi_difference_chart.AddCurve(
      afi_difference_curves[0],
      svg::CreateStrokeAttributes("#BB0000", curve_width, "round"),
      std::string("[0, 8)"));
  afi_difference_chart.AddCurve(
      afi_difference_curves[1],
      svg::CreateStrokeAttributes("#FF0000", curve_width, "round"),
      std::string("[8, 30)"));
  afi_difference_chart.AddCurve(
      afi_difference_curves[2],
      svg::CreateStrokeAttributes("#DD8800", curve_width, "round"),
      std::string("[30, 90)"));
  afi_difference_chart.AddCurve(
      afi_difference_curves[3],
      svg::CreateStrokeAttributes("#00ddDD", curve_width, "round"),
      std::string("[90, 183)"));
  afi_difference_chart.AddCurve(
      afi_difference_curves[4],
      svg::CreateStrokeAttributes("#dd00dd", curve_width, "round"),
      std::string("[183, 365)"));
  afi_difference_chart.AddCurve(
      afi_difference_curves[5],
      svg::CreateStrokeAttributes("#0088FF", curve_width, "round"),
      std::string("[365, 730)"));
  /*
  afi_difference_chart.AddCurve(
      afi_difference_curves[6],
      svg::CreateStrokeAttributes("#888888", curve_width, "round"),
      std::string("[730, +∞)"));//*/

  afi_difference_chart.AddDatesRects("Марафоны", afi_marathons,
                                     svg::CreateFillAttributes("#DDDDDD"));

  auto afmo_difference_curves = CreateDifferencesCurves(afmo_curves);
  SmoothCurves(smooth_number, afmo_difference_curves);

  afmo_difference_chart.AddCurve(
      afmo_difference_curves[0],
      svg::CreateStrokeAttributes("#BB0000", curve_width, "round"),
      std::string("[0, 8)"));
  afmo_difference_chart.AddCurve(
      afmo_difference_curves[1],
      svg::CreateStrokeAttributes("#FF0000", curve_width, "round"),
      std::string("[8, 30)"));
  afmo_difference_chart.AddCurve(
      afmo_difference_curves[2],
      svg::CreateStrokeAttributes("#DD8800", curve_width, "round"),
      std::string("[30, 90)"));
  afmo_difference_chart.AddCurve(
      afmo_difference_curves[3],
      svg::CreateStrokeAttributes("#00ddDD", curve_width, "round"),
      std::string("[90, 183)"));
  afmo_difference_chart.AddCurve(
      afmo_difference_curves[4],
      svg::CreateStrokeAttributes("#dd00dd", curve_width, "round"),
      std::string("[183, 365)"));
  afmo_difference_chart.AddCurve(
      afmo_difference_curves[5],
      svg::CreateStrokeAttributes("#0088FF", curve_width, "round"),
      std::string("[365, 730)"));
  afmo_difference_chart.AddCurve(
      afmo_difference_curves[6],
      svg::CreateStrokeAttributes("#888888", curve_width, "round"),
      std::string("[730, +∞)"));  //*/

  afd_difference_chart.SaveToFile(parameters.charts_folder +
                                  "/afd_detailed_differences.svg");
  afi_difference_chart.SaveToFile(parameters.charts_folder +
                                  "/afi_detailed_differences.svg");
  afmo_difference_chart.SaveToFile(parameters.charts_folder +
                                   "/afmo_detailed_differences.svg");
}

void CreateDetailedSvgCharts(const GeneralParameters &parameters,
                             const Point<double> image_size,
                             const Date &start_date, const Date &end_date) {
  std::cout << __func__ << " : " << start_date << " - " << end_date
            << std::endl;
  auto afd_curves = GetAfdDetailedCurves(parameters, start_date, end_date);
  auto afi_curves = GetAfiDetailedCurves(parameters, start_date, end_date);
  auto afmo_curves = GetAfmoDetailedCurves(parameters, start_date, end_date);

  int smooth_number = 3;
  SmoothCurves(smooth_number, afd_curves);
  SmoothCurves(smooth_number, afi_curves);
  SmoothCurves(smooth_number, afmo_curves);

  DetailedSvgChart afd_chart(image_size, start_date, end_date,
                             "Количество номинаций КУ");
  DetailedSvgChart afi_chart(image_size, start_date, end_date,
                             "Количество номинаций КУЛ");
  DetailedSvgChart afmo_chart(image_size, start_date, end_date,
                              "Количество номинаций КПМ");
  DetailedSvgChart totals_chart(image_size, start_date, end_date,
                                "Количество номинаций в разных проектах");

  double curve_width = 3.;

  afd_chart.AddCurve(afd_curves[0], svg::CreateStrokeAttributes(
                                        "#BB0000", curve_width, "round"));
  afd_chart.AddCurve(afd_curves[2], svg::CreateStrokeAttributes(
                                        "#DD0000", curve_width, "round"));
  afd_chart.AddCurve(afd_curves[3], svg::CreateStrokeAttributes(
                                        "#FF0000", curve_width, "round"));
  afd_chart.AddCurve(afd_curves[4], svg::CreateStrokeAttributes(
                                        "#FF5555", curve_width, "round"));
  afd_chart.AddCurve(afd_curves[5], svg::CreateStrokeAttributes(
                                        "#FF9999", curve_width, "round"));

  afi_chart.AddCurve(afi_curves[0], svg::CreateStrokeAttributes(
                                        "#008000", curve_width, "round"));
  afi_chart.AddCurve(afi_curves[2], svg::CreateStrokeAttributes(
                                        "#00A000", curve_width, "round"));
  afi_chart.AddCurve(afi_curves[3], svg::CreateStrokeAttributes(
                                        "#00C000", curve_width, "round"));
  afi_chart.AddCurve(afi_curves[4], svg::CreateStrokeAttributes(
                                        "#00E000", curve_width, "round"));
  afi_chart.AddCurve(afi_curves[5], svg::CreateStrokeAttributes(
                                        "#00FF00", curve_width, "round"));

  afmo_chart.AddCurve(afmo_curves[0], svg::CreateStrokeAttributes(
                                          "#0000BB", curve_width, "round"));
  afmo_chart.AddCurve(afmo_curves[2], svg::CreateStrokeAttributes(
                                          "#0000DD", curve_width, "round"));
  afmo_chart.AddCurve(afmo_curves[3], svg::CreateStrokeAttributes(
                                          "#0000FF", curve_width, "round"));
  afmo_chart.AddCurve(afmo_curves[4], svg::CreateStrokeAttributes(
                                          "#5555FF", curve_width, "round"));
  afmo_chart.AddCurve(afmo_curves[5], svg::CreateStrokeAttributes(
                                          "#9999FF", curve_width, "round"));
  afmo_chart.AddCurve(afmo_curves[6], svg::CreateStrokeAttributes(
                                          "#BBBBFF", curve_width, "round"));

  totals_chart.AddCurve(
      afd_curves[0],
      svg::CreateStrokeAttributes("#BB0000", curve_width, "round"),
      std::string("КУ"));
  totals_chart.AddCurve(
      afi_curves[0],
      svg::CreateStrokeAttributes("#008000", curve_width, "round"),
      std::string("КУЛ"));
  totals_chart.AddCurve(
      afmo_curves[0],
      svg::CreateStrokeAttributes("#0000BB", curve_width, "round"),
      std::string("КПМ"));

  afd_chart.SaveToFile(parameters.charts_folder + "/afd_detailed.svg");
  afi_chart.SaveToFile(parameters.charts_folder + "/afi_detailed.svg");
  afmo_chart.SaveToFile(parameters.charts_folder + "/afmo_detailed.svg");
  totals_chart.SaveToFile(parameters.charts_folder + "/totals.svg");
}

#endif

void CreateDibotSvgCharts(const GeneralParameters &parameters,
                          const Point<double> image_size,
                          const Date &start_date, const Date &end_date,
                          const std::string &user_provided_file_name,
                          const size_t smooth_number) {
  std::cout << __func__ << " : " << start_date << " - " << end_date
            << std::endl;

  auto curves_creator{CreateDibotDateCurvesCreator(parameters)};
  auto dibot_curves{curves_creator->create(start_date, end_date)};
  chart::SmoothCurves(smooth_number, dibot_curves);

  DetailedSvgChart totals_chart{image_size, start_date, end_date,
                                "Количество номинаций"};

  const double curve_width{3.};

  totals_chart.AddCurve(dibot_curves[0], svg::CreateStrokeAttributes(
                                             "green", curve_width, "round"));
  totals_chart.AddCurve(dibot_curves[7], svg::CreateStrokeAttributes(
                                             "red", curve_width, "round"));
  totals_chart.AddCurve(
      dibot_curves[9],
      svg::CreateStrokeAttributes("royalblue", curve_width, "round"));
  totals_chart.AddCurve(dibot_curves[11], svg::CreateStrokeAttributes(
                                              "#9932CC", curve_width, "round"));
  totals_chart.AddCurve(dibot_curves[13], svg::CreateStrokeAttributes(
                                              "maroon", curve_width, "round"));
  totals_chart.AddCurve(dibot_curves[15], svg::CreateStrokeAttributes(
                                              "orange", curve_width, "round"));

  const std::string file_name{user_provided_file_name.empty()
                                  ? "dibot_totals.svg"
                                  : user_provided_file_name};
  totals_chart.SaveToFile(parameters.paths.out_charts_folder + "/" + file_name);
}
