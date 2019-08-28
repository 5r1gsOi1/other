
#include "summaries_chart.h"
//#include "tasks/charts/detailed/afi_detailed_chart.h"
//#include "tasks/charts/summaries/summaries_chart.h"
/*
Curves GetAfiSummariesCurves(const GeneralParameters& parameters,
                             const Date& start_date, const Date& end_date) {
  SummariesMainPageParser<ParseImproveMainPage> main_page_parser;
  SummariesCurvesCreator creator(parameters, "Википедия:К улучшению", "AFI",
                                 main_page_parser);

  return creator.create(start_date, end_date);
}

void CreateSvgChartForSummariesAfiStatistics(
    const GeneralParameters& parameters, const Point<double> image_size,
    const Date& start_date, const Date& end_date,
    const std::vector<std::pair<Date, Date> >& marathons_dates) {
  std::cout << start_date << " - " << end_date << std::endl;
  auto curves = GetAfiSummariesCurves(parameters, start_date, end_date);
  // int smooth_number = = 1;
  // SmoothCurves(smooth_number =, curves);

  DetailedSvgChart chart(image_size, start_date, end_date,
                         "Количество снимаемых страниц", 5, 5);

  chart.AddDatesRects("Марафоны", marathons_dates,
                      svg::CreateFillAttributes("orange"));

  chart.AddCurve(curves[0],
                 svg::CreateStrokeAttributes("#000080", 5., "round"));
  chart.AddCurve(curves[1],
                 svg::CreateStrokeAttributes("#8000A0", 5., "round"));
  chart.AddCurve(curves[2],
                 svg::CreateStrokeAttributes("#0080C0", 5., "round"));

  auto file_name = parameters.charts_folder + "/summaries_afi.svg";
  chart.SaveToFile(file_name);
}//*/
