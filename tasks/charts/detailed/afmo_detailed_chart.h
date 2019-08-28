
#ifndef AFMO_CHART_H_
#define AFMO_CHART_H_

#include "chart/chart.h"
#include "common/simple_date.h"
#include "statistics/statistics.h"

Curves GetAfmoDetailedCurves(const GeneralParameters& parameters,
                             const Date& start_date, const Date& end_date,
                             const bool use_excluded_dates_list = true);

void CreateSvgChartForDetailedAfmoStatistics(
    const GeneralParameters& parameters, const Point<double> image_size,
    const Date& start_date, const Date& end_date);

Pages ParseMovementMainPage(const Date& identifier,
                            const std::string& page_text);

#endif
