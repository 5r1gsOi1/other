
#pragma once

#include "common/file_index.h"
#include "common/simple_date.h"

namespace chart {

struct Curve {
  std::string name;
  std::vector<Point<double>> points;

  Curve() = default;
  Curve(const std::string& name_) : name(name_) {}
};

using Curves = std::vector<Curve>;

void SmoothCurve(const int smooth_number, Curve& curve);
void SmoothCurves(const int smooth_number, Curves& curves);

}  // namespace chart
