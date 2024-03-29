
#pragma once

#include <optional>
#include "common/basic.h"
#include "common/simple_date.h"
#include "statistics/statistics.h"
//#include "tasks/charts/detailed/detailed_chart.h"

#include "wiki/parse.h"

stats::DetailedRanks CreateRanksForAfd(const GeneralParameters& parameters,
                                       const Date& start_date,
                                       const Date& end_date);

wiki::parse::Nominations CreateNominationsForAfd(
    const GeneralParameters& parameters, const Date& start_date,
    const Date& end_date);
