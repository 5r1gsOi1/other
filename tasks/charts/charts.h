
#ifndef CHARTS_H_
#define CHARTS_H_

#include <algorithm>
#include <limits>
#include <string>
#include <vector>
#include "chart/svg_chart.h"
#include "statistics/statistics.h"

void CreateDetailedSvgCharts(const GeneralParameters& parameters,
                             const Point<double> image_size,
                             const Date& start_date, const Date& end_date);

void CreateDetailedAfiSvgChartWithMarathons(const GeneralParameters& parameters,
                                            const Point<double> image_size,
                                            const Date& start_date,
                                            const Date& end_date);

void CreateDifferencesSvgCharts(const GeneralParameters& parameters,
                                const Point<double> image_size,
                                const Date& start_date, const Date& end_date);

void CreateDibotSvgCharts(const GeneralParameters& parameters,
                          const Point<double> image_size,
                          const Date& start_date, const Date& end_date);

#endif
