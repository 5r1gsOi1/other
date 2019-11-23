
#pragma once

#include "common/file_index.h"
#include "common/simple_date.h"

namespace chart {

template <class NumberType>
struct Curve {
  std::string name;
  std::vector<Point<NumberType>> points;

  Curve() = default;
  Curve(const std::string& name_) : name(name_) {}
};

template <class NumberType>
using Curves = std::vector<Curve<NumberType>>;

template <class NumberType>
bool MiddlePointMayBeDroppedOut(const Point<NumberType>& p1,
                                const Point<NumberType>& p2,
                                const Point<NumberType>& p3) {
  const double max_area{100.};
  double area{(static_cast<double>(p1.x) - static_cast<double>(p3.x)) *
                  (static_cast<double>(p2.y) - static_cast<double>(p3.y)) -
              (static_cast<double>(p2.x) - static_cast<double>(p3.x)) *
                  (static_cast<double>(p1.y) - static_cast<double>(p3.y))};
  area = std::abs(area);
  return area < max_area;
}

template <class NumberType>
void RemoveNonVisiblePointsFrom(Curve<NumberType>& curve) {
  auto& points{curve.points};
  const auto curve_size{points.size()};
  std::vector<Point<NumberType>> smoothed_points;
  smoothed_points.reserve(curve_size);
  const size_t untouchables_at_end{curve_size > 10 ? 10u : 0u};

  smoothed_points.push_back(points[0]);
  for (size_t i{}; i + 2 + untouchables_at_end <= curve_size; i += 2) {
    if (not MiddlePointMayBeDroppedOut(points[i], points[i + 1],
                                       points[i + 2])) {
      smoothed_points.push_back(points[i + 1]);
    }
    smoothed_points.push_back(points[i + 2]);
  }
  for (size_t i{curve_size - untouchables_at_end}; i < curve_size; ++i) {
    smoothed_points.push_back(points[i]);
  }
  curve.points = std::move(smoothed_points);
}

// void SmoothCurve(const size_t smooth_number, Curve& curve);
void SmoothCurves(const size_t smooth_number, Curves<double>& curves);

}  // namespace chart
