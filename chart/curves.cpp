
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

void SmoothCurve(const int smooth_number, Curve& curve) {
  auto points = curve.points;
  std::vector<Point<double>> smoothed_points;
  std::vector<Point<double>> block;
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

void SmoothCurves(const int smooth_number, Curves& curves) {
  for (auto& curve : curves) {
    SmoothCurve(smooth_number, curve);
  }
}

}  // namespace chart
