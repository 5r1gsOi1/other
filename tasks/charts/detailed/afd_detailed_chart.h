
#ifndef AFD_CHART_H_
#define AFD_CHART_H_

#include "chart/chart.h"
#include "common/simple_date.h"

Curves GetAfdDetailedCurves(const GeneralParameters& parameters,
                            const Date& start_date, const Date& end_date,
                            const bool use_excluded_dates_list = true);

void CreateSvgChartForDetailedAfdStatistics(const GeneralParameters& parameters,
                                            const Point<double> image_size,
                                            const Date& start_date,
                                            const Date& end_date);

#endif
