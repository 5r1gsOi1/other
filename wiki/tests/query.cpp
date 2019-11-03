
#include "wiki/query.h"
#include <iostream>
#include "common/tests.h"
#include "network/query_via_curl.h"
#include "wiki/cache_fs.h"
#include "wiki/page_getter.h"
#include "wiki/query_via_network.h"

#include "chart/curves.h"
#include "chart/date_chart/chart.h"
#include "tasks/charts/dibot/dibot_chart.h"

#include <fmt/format.h>

/*
bool TimestampEqual(const wiki::Timestamp& timestamp, const int year,
                    const int month, const int day, const int hour,
                    const int minute, const int second) {
  return timestamp.year == year and timestamp.month == month and
         timestamp.day == day and timestamp.hour == hour and
         timestamp.minute == minute and timestamp.second == second;
}

bool Test_Timestamp_Empty() {
  wiki::Timestamp ts;
  return TimestampEqual(ts, 0, 0, 0, 0, 0, 0);
}

bool Test_Timestamp_FirstType() {
  wiki::Timestamp ts("2001-03-23T12:34:56Z");
  return TimestampEqual(ts, 2001, 3, 23, 12, 34, 56);
}

bool Test_Timestamp_SecondType() {
  wiki::Timestamp ts("20010323T123456Z");
  return TimestampEqual(ts, 2001, 3, 23, 12, 34, 56);
}

bool Test_Timestamp() {
  TEST_START;
  CALL_AND_PRINT(Test_Timestamp_FirstType);
  CALL_AND_PRINT(Test_Timestamp_SecondType);
  TEST_END;
}
*/



bool Test_PageGetter() {

    std::vector<int> v;
  std::cout << fmt::format("{1} and {0}", 123, "test") << std::endl;

  /*const Point<double> image_size{2000, 1125};
  const Date &start_date{2010, 9, 1};
  const Date &end_date{--Date().Today()};

  auto curves_creator{CreateDibotDateCurvesCreator()};
  auto dibot_curves{curves_creator->create(start_date, end_date)};
  const int smooth_number{3};
  chart::SmoothCurves(smooth_number, dibot_curves);

  DetailedSvgChart totals_chart(image_size, start_date, end_date,
                                "Количество номинаций");

  double curve_width = 4.;

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

  totals_chart.SaveToFile(".data/charts/dibot_totals.svg");*/

  return true;
}

bool Test_All() {
  TEST_START;
  CALL_AND_PRINT(Test_PageGetter);
  TEST_END;
}

MAIN_TEST(Test_All)
