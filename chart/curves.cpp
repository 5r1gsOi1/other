
#include "curves.h"

namespace chart {

void CalculateAndAddSmoothedElement(
    const int block_number, const double number_of_blocks,
    const std::vector<Point<double>>& block,
    std::vector<Point<double>>& smoothed_points) {
  if (block.size() >= 1) {
    Point<double> p;
    double block_length = (block.end() - 1)->x - block.begin()->x;
    p.x = block.begin()->x;
    if (block_length > 0.) {
      p.x +=
          block_length * static_cast<double>(block_number) / number_of_blocks;
    }
    p.y = 0.;
    for (auto& i : block) {
      p.y += i.y;
    }
    p.y /= block.size();
    smoothed_points.push_back(p);
  }
}

void SmoothCurve(const size_t smooth_number, Curve<double>& curve) {
  auto& points = curve.points;
  std::vector<Point<double>> smoothed_points;
  std::vector<Point<double>> block(smooth_number + 1);
  int block_number = 0, n = 0;
  double number_of_blocks =
      ceil(points.size() / static_cast<double>(smooth_number));
  for (auto& i : points) {
    block.push_back(i);
    ++n;
    if (n >= smooth_number) {
      CalculateAndAddSmoothedElement(block_number, number_of_blocks, block,
                                     smoothed_points);
      block.clear();
      n = 0;
      ++block_number;
    }
  }
  CalculateAndAddSmoothedElement(block_number, number_of_blocks, block,
                                 smoothed_points);
  curve.points = std::move(smoothed_points);
}

bool MiddlePointMayBeDroppedOut(const Point<double>& p1,
                                const Point<double>& p2,
                                const Point<double>& p3) {
  const double max_area{100.};
  double area{(p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y)};
  area = std::abs(area);
  return area < max_area;
}

void RemoveNonVisiblePointsFrom(Curve<double>& curve) {
  auto& points{curve.points};
  const auto curve_size{points.size()};
  std::vector<Point<double>> smoothed_points;
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

void SmoothCurves(const size_t smooth_number, Curves<double>& curves) {
  for (size_t i{}; i < smooth_number; ++i) {
    for (auto& curve : curves) {
      RemoveNonVisiblePointsFrom(curve);
    }
  }
}

}  // namespace chart
