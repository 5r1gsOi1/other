
#include "chart.h"

PointArea CalculateExtremumValues(const chart::Curves& curves) {
  PointArea extremums{
      {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()},
      {std::numeric_limits<double>::min(), std::numeric_limits<double>::min()}};
  for (const auto& curve : curves) {
    auto minmax_x = std::minmax_element(
        curve.points.begin(), curve.points.end(),
        [](const Point<double>& p1, const Point<double>& p2) {
          return p1.x < p2.x;
        });
    auto minmax_y = std::minmax_element(
        curve.points.begin(), curve.points.end(),
        [](const Point<double>& p1, const Point<double>& p2) {
          return p1.y < p2.y;
        });
    extremums.min.x = std::min(extremums.min.x, minmax_x.first->x);
    extremums.max.x = std::max(extremums.max.x, minmax_x.second->x);
    extremums.min.y = std::min(extremums.min.y, minmax_y.first->y);
    extremums.max.y = std::max(extremums.max.y, minmax_y.second->y);
  }
  return extremums;
}

template <class X, class Predicate>
std::vector<double> CreateGridPoints(const X& start, const X& end,
                                     Predicate filter) {
  std::vector<double> points;
  for (auto i = start; i <= end; ++i) {
    if (filter(i)) {
      points.push_back(i - start);
    }
  }
  return points;
}

std::vector<double> CreateMonthGridPoints(const Date& start, const Date& end) {
  std::vector<double> points;
  int month = start.month, year = start.year;
  for (Date i = start; i <= end; ++i) {
    if (month != i.month) {
      if (year != i.year) {
        year = i.year;
      } else {
        points.push_back(i - start);
      }
      month = i.month;
    }
  }
  return points;
}

std::vector<double> CreateYearGridPoints(const Date& start, const Date& end) {
  std::vector<double> points;
  int year = start.year;
  for (Date i = start; i <= end; ++i) {
    if (year != i.year) {
      points.push_back(i - start);
      year = i.year;
    }
  }
  return points;
}

std::vector<double> CreateValueGridPoints(const int max_y, const int step) {
  std::vector<double> points;
  for (int i = step; i <= max_y; i += step) {
    points.push_back(i);
  }
  return points;
}

std::vector<std::pair<double, std::string>> CreateYearGridNumbers(
    const Date& start, const Date& end) {
  std::vector<std::pair<double, std::string>> points;
  int year = start.year;
  for (Date i = start; i <= end; ++i) {
    if (year != i.year) {
      points.push_back(
          std::pair<double, std::string>(i - start, std::to_string(i.year)));
      year = i.year;
    }
  }
  return points;
}

std::vector<std::pair<double, std::string>> CreateValueGridNumbers(
    const int max_y, const int step) {
  std::vector<std::pair<double, std::string>> points;
  for (int i = step; i <= max_y; i += step) {
    points.push_back(std::pair<double, std::string>(i, std::to_string(i)));
  }
  return points;
}

DetailedSvgChart::DetailedSvgChart(const Point<double> image_size,
                                   const Date& start_date, const Date& end_date,
                                   const std::string& title,
                                   const int vertical_numbers_step,
                                   const int vertical_grid_step,
                                   const int vertical_rounding_step)
    : SvgChart(SvgChart::Settings{
          title, image_size, PointArea{{95., 20.}, {20., 60.}},
          PointArea{{0., 0.}, {0., 0.}}, svg::CreateFillAttributes("white")}),
      start_date_(start_date),
      end_date_(end_date),
      vertical_numbers_step_(vertical_numbers_step),
      vertical_grid_step_(vertical_grid_step),
      vertical_rounding_step_(vertical_rounding_step) {}

void DetailedSvgChart::SetVertialSteps(const int grid_step,
                                       const int numbers_step,
                                       const int rounding_step) {
  vertical_grid_step_ = grid_step;
  vertical_numbers_step_ = numbers_step;
  vertical_rounding_step_ = rounding_step;
}

void DetailedSvgChart::AddCurve(const chart::Curve& curve,
                                const svg::Attributes& attributes,
                                const bool add_to_legend) {
  curves_.push_back(curve);
  attributes_.push_back(attributes);
  add_to_legend_.push_back(add_to_legend);
}

void DetailedSvgChart::AddCurve(const chart::Curve& curve,
                                const svg::Attributes& attributes,
                                const std::string& override_name) {
  chart::Curve curve_copy{curve};
  curve_copy.name = override_name;
  curves_.push_back(std::move(curve_copy));
  attributes_.push_back(attributes);
  add_to_legend_.push_back(true);
}

void DetailedSvgChart::AddDatesRects(
    const std::string& legend_name,
    const std::vector<std::pair<Date, Date>>& dates,
    const svg::Attributes& fill) {
  dates_rects_ = dates;
  dates_rects_fill_ = fill;
  dates_rects_name_ = legend_name;
}

void DetailedSvgChart::AddDatesRects() {
  PointArea area{{0., 0.}, {0., 0.}};
  for (const auto& i : dates_rects_) {
    area.min.x = i.first - start_date_;
    area.max.x = i.second - start_date_;
    area.min.y = extremums_.min.y;
    area.max.y = extremums_.max.y;
    if (area.min.x < extremums_.min.x) {
      if (area.max.x >= extremums_.min.x) {
        if (area.max.x <= extremums_.max.x) {
          area.min.x = extremums_.min.x;
        } else {
          area.min.x = extremums_.min.x;
          area.max.x = extremums_.max.x;
        }
        AddRect(area, dates_rects_fill_);
      }
    } else if (area.min.x >= extremums_.min.x and
               area.min.x <= extremums_.max.x) {
      if (area.max.x > extremums_.max.x) {
        area.max.x = extremums_.max.x;
      }
      AddRect(area, dates_rects_fill_);
    }
  }
}

void DetailedSvgChart::AddDatesRectsToLegend() {
  if (not dates_rects_.empty()) {
    AddLegendItemRect(dates_rects_name_,
                      SvgChart::FontParameters{
                          "Verdana", 30, svg::CreateFillAttributes("gray")},
                      dates_rects_fill_);
  }
}

void DetailedSvgChart::CalculateCurvesSpecificData() {
  extremums_ = CalculateExtremumValues(curves_);
  extremums_.max.y = CeilToMultiple(extremums_.max.y, vertical_rounding_step_);
  extremums_.min.y = 0;
  SvgChart::SetWorkingArea(extremums_);
}

void DetailedSvgChart::AddClipPath() {
  auto clip_path_tag = new svg::Tag();
  clip_path_tag->name = "clipPath";
  clip_path_tag->attributes.insert({"id", std::string("clipRect")});
  clip_path_tag->AddChild(
      new svg::Rect(Point<double>{0., 0.}, SvgChart::inner_image_size_,
                    svg::Attributes{{"rx", svg::attributes::Pixels(20)}}));
  SvgChart::root_->AddChild(clip_path_tag);
}

void DetailedSvgChart::AddGrid() {
  const double small_grid_width = 1.5;
  const double big_grid_width = 1.5;
  const std::string small_grid_color = "lightgray";
  const std::string big_grid_color = "darkgray";
  SvgChart::AddGridX(CreateMonthGridPoints(start_date_, end_date_),
                     svg::CreateStrokeAttributes(
                         small_grid_color, small_grid_width, "round", "5,5"));

  SvgChart::AddGridY(
      CreateGridPoints(0., extremums_.max.y,
                       [this](const double x) {
                         return int(x) % vertical_grid_step_ == 0 and
                                int(x) % vertical_numbers_step_ != 0;
                       }),
      svg::CreateStrokeAttributes(small_grid_color, small_grid_width, "round",
                                  "5,5"));
  /*
    SvgChart::AddGridY(
        CreateValueGridPoints(static_cast<int>(extremums_.max.y),
                              vertical_grid_step_),
        svg::CreateStrokeAttributes("lightgray", 2., "round", "5,5"));*/
  SvgChart::AddGridX(CreateYearGridPoints(start_date_, end_date_),
                     svg::CreateStrokeAttributes(big_grid_color, big_grid_width,
                                                 "round", "5,5"));
  SvgChart::AddGridY(
      CreateGridPoints(0., extremums_.max.y,
                       [this](const double x) {
                         return int(x) % vertical_numbers_step_ == 0;
                       }),
      svg::CreateStrokeAttributes(big_grid_color, big_grid_width, "round",
                                  "5,5"));
}

void DetailedSvgChart::AddNumbers() {
  SvgChart::AddNumbersX(CreateYearGridNumbers(start_date_, end_date_),
                        svg::CreateFontAttributes("Verdana", 30) +
                            svg::CreateFillAttributes("#333333"));
  SvgChart::AddNumbersY(
      CreateValueGridNumbers(static_cast<int>(extremums_.max.y),
                             vertical_numbers_step_),
      svg::CreateFontAttributes("Verdana", 30) +
          svg::CreateFillAttributes("#333333"));
}

void DetailedSvgChart::AddCurves() {
  auto curves_size = curves_.size();
  for (auto i = 1ul; i <= curves_size; ++i) {
    SvgChart::AddCurve(curves_.at(curves_size - i),
                       attributes_.at(curves_size - i));
  }
}

void DetailedSvgChart::AddInnerSvg() {
  SvgChart::CreateInnerSvgIfNotExists();
  /*SvgChart::inner_svg_->attributes.insert(
      {"clip-path", svg::attributes::Url("clipRect")});//*/
}

void DetailedSvgChart::AddFrame() {
  auto attributes = svg::CreateStrokeAttributes("#111111", 1.5);
  // attributes.insert({"rx", svg::attributes::Pixels(20)});
  SvgChart::AddFrame(attributes);
}

void DetailedSvgChart::AddCurvesToLegend() {
  for (auto curve_number = 0ul; curve_number < curves_.size(); ++curve_number) {
    if (add_to_legend_.at(curve_number)) {
      SvgChart::AddLegendItemLine(
          curves_.at(curve_number).name,
          SvgChart::FontParameters{"Verdana", 30,
                                   svg::CreateFillAttributes("#333333")},
          attributes_.at(curve_number));
    }
  }
}

void DetailedSvgChart::AddLegend() {
  auto attributes = svg::CreateFillAttributes("white", "", "1.");
  // attributes.insert({"rx", svg::attributes::Pixels(20)});
  auto stroke_attributes = svg::CreateStrokeAttributes("#333333", 1.5);
  attributes.insert(stroke_attributes.begin(), stroke_attributes.end());
  SvgChart::AddLegend(Point<double>{200., 56.}, attributes);
}

void DetailedSvgChart::AddBackground() {
  SvgChart::AddBackground(svg::CreateFillAttributes("white"));
}

void DetailedSvgChart::SaveToFile(const std::string& file_name) {
  CalculateCurvesSpecificData();
  // AddClipPath();
  AddBackground();
  AddInnerSvg();
  AddDatesRects();
  AddGrid();
  AddCurves();
  AddFrame();
  AddNumbers();
  AddCurvesToLegend();
  AddDatesRectsToLegend();
  AddLegend();
  SvgChart::SaveToFile(file_name);
}
