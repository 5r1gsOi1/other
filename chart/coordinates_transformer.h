
#pragma once

#include "common/basic.h"

#include <string>

class CoordinatesTransformer {
 public:
  CoordinatesTransformer() = default;
  CoordinatesTransformer(const Point<double>& min1, const Point<double>& max1,
                         const Point<double>& min2, const Point<double>& max2,
                         bool invert_x, bool invert_y)
      : min1(min1),
        max1(max1),
        min2(min2),
        max2(max2),
        invert_x_(invert_x),
        invert_y_(invert_y) {
    delta_x_ = (max1.x - min1.x) / (max2.x - min2.x);
    delta_y_ = (max1.y - min1.y) / (max2.y - min2.y);
  }

  double x(const double x) const {
    return (invert_x_ ? (max1.x - x) : (x - min1.x)) / delta_x_ + min2.x;
  }

  double y(const double y) const {
    return (invert_y_ ? (max1.y - y) : (y - min1.y)) / delta_y_ + min2.y;
  }

  bool visible_x(const double x) const {
    double x2 = this->x(x);
    return x >= min1.x && x <= max1.x && x2 >= min2.x && x2 <= max2.x;
  }

  bool visible_y(const double y) const {
    double y2 = this->y(y);
    return y >= min1.y && y <= max1.y && y2 >= min2.y && y2 <= max2.y;
  }

 private:
  Point<double> min1, max1, min2, max2;
  bool invert_x_, invert_y_;
  double delta_x_, delta_y_;
};
