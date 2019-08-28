
#ifndef CHART_H_
#define CHART_H_

#include <map>
#include <string>
#include <vector>
#include "common/basic.h"



/*
using Color = unsigned long;
constexpr Color kBlack = 0;
constexpr Color kWhite = 0xffffff;
constexpr Color kRed = 0xff0000;

enum class StrokeStyle {

};

enum class FillStyle {

};

struct Stroke {
  Color color;
  StrokeStyle style;
  double width;
};

struct Fill {
  Color color;
  FillStyle style;
};

class Chart {
 public:
  class Settings {};

  Chart() = delete;
  Chart(const Settings& settings) { std::ignore = settings; }
  virtual ~Chart() = default;

  virtual void SaveToFile(const std::string& file_name) const = 0;

  virtual void AddFrame(const Stroke& stroke) = 0;

  virtual void AddGridX(const std::vector<double>& values,
                        const Stroke& stroke) = 0;
  virtual void AddNumbersX(
      const std::vector<std::pair<double, std::string>>& values,
      const Attributes& font, const Attributes& fill) = 0;
  virtual void AddGridY(const std::vector<double>& values,
                        const Attributes& stroke) = 0;
  virtual void AddNumbersY(
      const std::vector<std::pair<double, std::string>>& values,
      const Attributes& font, const Attributes& fill) = 0;
  virtual void AddCurve(const std::vector<Point<double>>& points,
                        const Attributes& stroke,
                        const std::string& name = std::string()) = 0;
  virtual void AddLegend(const std::string& font_name, const double font_size,
                         const Attributes& font_fill,
                         const Attributes& back_fill) = 0;
  virtual void AddLegend(const Point<double>& p, const Attributes& font,
                         const Attributes& font_fill,
                         const Attributes& back_fill, const int font_size) = 0;

 protected:
  bool is_ready_;
};//*/

#endif
