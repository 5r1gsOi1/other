
#ifndef AFI_SUMMARIES_CHART_H_
#define AFI_SUMMARIES_CHART_H_

#include "chart/chart.h"
#include "common/simple_date.h"

void CreateSvgChartForSummariesAfiStatistics(
    const GeneralParameters& parameters, const Point<double> image_size,
    const Date& start_date, const Date& end_date,
    const std::vector<std::pair<Date, Date>>& marathons_dates = {});

#endif
