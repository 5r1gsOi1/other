
#include "svg_chart.h"
#include <fstream>
#include <iomanip>

SvgChart::SvgChart(const SvgChart::Settings& settings) : settings_(settings) {
  root_ = std::make_unique<svg::Svg>(
      svg::CreateStandardSvgAttributes(settings_.image_size));
  root_->children.push_back(std::make_unique<svg::Title>(settings_.title));

  inner_image_size_ =
      settings_.image_size - settings_.margins.max - settings.margins.min;
  inner_svg_ = nullptr;
  SetWorkingArea(settings_.working_area);
  is_ready_ = true;
}

SvgChart::SvgChart(SvgChart&& other)
    : root_(std::move(other.root_)),
      inner_svg_(other.inner_svg_),
      inner_image_size_(other.inner_image_size_),
      settings_(other.settings_),
      legend_(std::move(other.legend_)),
      transformer_(other.transformer_) {
  other.inner_svg_ = nullptr;
  other.legend_.empty();
}

void SvgChart::SaveToFile(const std::string& file_name) const {
  if (not is_ready_) {
    throw NotReady();
  }
  std::ofstream file(file_name, std::ios_base::out);
  if (not file.is_open()) {
    throw NotOpened();
  }

  root_->OutToStream(file);
}

void SvgChart::AddFrame(const svg::Attributes& stroke_attributes) {
  root_->AddChild(new svg::Rect(
      settings_.margins.min,
      settings_.image_size - settings_.margins.max - settings_.margins.min,
      stroke_attributes + svg::CreateFillAttributes("none")));
}

void SvgChart::CreateInnerSvgIfNotExists() {
  if (inner_svg_ == nullptr) {
    inner_svg_ = new svg::Svg(
        svg::CreateSvgAttributes(settings_.margins.min, inner_image_size_));
    root_->children.push_back(std::unique_ptr<svg::Svg>(inner_svg_));
  }
}

void SvgChart::AddGridX(const std::vector<double>& values,
                        const svg::Attributes& stroke) {
  CreateInnerSvgIfNotExists();
  std::vector<std::pair<Point<double>, char>> points;
  for (auto& i : values) {
    if (i > settings_.working_area.min.x and i < settings_.working_area.max.x) {
      points.push_back(std::make_pair(
          Point<double>{transformer_.x(i),
                        transformer_.y(settings_.working_area.min.y)},
          'M'));
      points.push_back(std::make_pair(
          Point<double>{transformer_.x(i),
                        transformer_.y(settings_.working_area.max.y)},
          '\0'));
    }
  }
  svg::Path* path = new svg::Path{points, stroke};
  inner_svg_->AddChild(path);
}

void SvgChart::AddNumbersX(
    const std::vector<std::pair<double, std::string>>& values,
    const svg::Attributes& attributes) {
  ;
  svg::Group* group = new svg::Group(
      attributes +
      svg::Attributes{{"text-anchor", std::string("middle")},
                      {"alignment-baseline", std::string("baseline")}});
  root_->AddChild(group);
  svg::Attributes individual_attributes{{"dy", svg::attributes::Ex(2.5)}};

  for (auto& i : values) {
    if (i.first > settings_.working_area.min.x and
        i.first < settings_.working_area.max.x) {
      if (not i.second.empty()) {
        group->AddChild(new svg::Text(
            i.second,
            Point<double>{settings_.margins.min.x + transformer_.x(i.first),
                          settings_.margins.min.y +
                              transformer_.y(settings_.working_area.min.y)},
            Point<double>{0., 0.}, individual_attributes, {}));
      }
    }
  }
}

void SvgChart::AddGridY(const std::vector<double>& values,
                        const svg::Attributes& stroke) {
  CreateInnerSvgIfNotExists();
  std::vector<std::pair<Point<double>, char>> points;
  for (auto& i : values) {
    if (i > settings_.working_area.min.y and i < settings_.working_area.max.y) {
      points.push_back(std::make_pair(
          Point<double>{transformer_.x(settings_.working_area.min.x),
                        transformer_.y(i)},
          'M'));
      points.push_back(std::make_pair(
          Point<double>{transformer_.x(settings_.working_area.max.x),
                        transformer_.y(i)},
          '\0'));
    }
  }
  svg::Path* path = new svg::Path{points, stroke};
  inner_svg_->AddChild(path);
}

void SvgChart::AddNumbersY(
    const std::vector<std::pair<double, std::string>>& values,
    const svg::Attributes& attributes) {
  svg::Group* group = new svg::Group(
      attributes +
      svg::Attributes{{"text-anchor", std::string("start")},
                      {"alignment-baseline", std::string("baseline")}});
  root_->AddChild(group);
  svg::Attributes individual_attributes{{"dx", svg::attributes::Em(0.25)},
                                        {"dy", svg::attributes::Ex(0.7)}};
  for (auto& i : values) {
    if (i.first >= settings_.working_area.min.y and
        i.first <= settings_.working_area.max.y) {
      if (not i.second.empty()) {
        group->AddChild(new svg::Text(
            i.second,
            Point<double>{0.,
                          settings_.margins.min.y + transformer_.y(i.first)},
            Point<double>{0., 0.}, individual_attributes, {}));
      }
    }
  }
}

void SvgChart::AddLegendItemLine(const std::string& name,
                                 const SvgChart::FontParameters& font,
                                 const svg::Attributes& stroke) {
  legend_.push_back(new SvgChart::LegendItemLine(name, font, stroke));
}

void SvgChart::AddLegendItemRect(const std::string& name,
                                 const SvgChart::FontParameters& font,
                                 const svg::Attributes& fill) {
  legend_.push_back(new SvgChart::LegendItemRect(name, font, fill));
}

void SvgChart::AddRect(const PointArea& area,
                       const svg::Attributes& attributes) {
  PointArea transformed_area{
      {transformer_.x(area.min.x), transformer_.y(area.max.y)},
      {transformer_.x(area.max.x), transformer_.y(area.min.y)}};
  CreateInnerSvgIfNotExists();
  inner_svg_->AddChild(new svg::Rect(transformed_area, attributes));
}

void SvgChart::AddCurve(const chart::Curve<double>& curve,
                        const svg::Attributes& attributes) {
  std::vector<Point<double>> transformed_points;
  for (auto& p : curve.points) {
    if (p.x >= settings_.working_area.min.x and
        p.x <= settings_.working_area.max.x) {
      transformed_points.push_back(
          Point<double>(transformer_.x(p.x), transformer_.y(p.y)));
    }
  }
  CreateInnerSvgIfNotExists();
  inner_svg_->AddChild(new svg::Path(
      transformed_points, attributes + svg::CreateFillAttributes("none") +
                              svg::Attributes{{"name", curve.name}}));
}

// 100, 56

void SvgChart::SetWorkingArea(const PointArea& working_area) {
  settings_.working_area = working_area;
  transformer_ = CoordinatesTransformer(working_area.min, working_area.max,
                                        Point<double>{0., 0.},
                                        inner_image_size_, false, true);
}

void SvgChart::AddBackground(const svg::Attributes& fill_attributes) {
  root_->AddChild(
      new svg::Rect(Point<double>{0., 0.}, settings_.image_size,
                    svg::CreateStrokeAttributes("none", 0.) + fill_attributes));
}

void SvgChart::AddLegend(const Point<double>& position,
                         const svg::Attributes& background_attributes) {
  const Point<double> margins{20, 20};
  const double interline = 10;

  if (legend_.empty()) {
    return;
  }

  Point<double> items_area_size{0., 0.};
  std::vector<Point<double>> items_positions;
  Point<double> current_position = position + margins;

  for (const auto& i : legend_) {
    auto item_size = i->GetSize();
    items_area_size.x = std::max(items_area_size.x, item_size.x);
    items_area_size.y += item_size.y + interline;
    items_positions.push_back(current_position);
    current_position.y += item_size.y + interline;
  }

  //  items_area_size.x += margins.x;
  items_area_size.y -= interline;

  root_->AddChild(new svg::Rect(position, items_area_size + margins * 2,
                                background_attributes));

  for (auto i = 0ul; i < legend_.size(); ++i) {
    legend_.at(i)->Draw(items_positions.at(i), *root_);
  }

  // double max_text_length = GetMaxLegendTextLength();
  // CalculateItemArea(max_text_length, font_size);

  /*
      Point<double> total_size{0., 0.};

      Point<double> size = {
          local_margins.min.x + local_margins.max.x + line_margin +
              max_text_length * parameters.font_size / 1.33,
          local_margins.min.y + local_margins.max.y +
              (legend_.size() - 1) * (line_margin + parameters.font_size)};

      root_->AddChild(new svg::Rect(parameters.position, size,
                                    svg::CreateStrokeAttributes("none", 0.),
                                    parameters.back_fill));
      Point<double> p1{0., 0.}, p2{0., 0.}, p_text{0., 0.}, d_text{0., 0.};
      int n = 0;

      for (auto& i : legend_) {
        p1.x = parameters.position.x + local_margins.min.x;
        p1.y = parameters.position.y + local_margins.min.y +
               n * (parameters.font_size + line_margin);
        p2 = p1;
        p2.x += line_length;
        // p1.y += font_size / 4.;
        // p2.y += font_size / 4.;
        root_->AddChild(new svg::Line(p1, p2, i.second));
        ++n;
      }
      svg::Attributes attributes{
          {"text-anchor", [](std::ostream& s) { s << "start"; }},
          {"alignment-baseline", [](std::ostream& s) { s << "baseline"; }}};
      svg::Group* group =
          new svg::Group({parameters.font, parameters.font_fill, attributes});
      root_->AddChild(group);

      svg::Attributes individual_attributes{
          {"dx", [](std::ostream& s) { s << "0.4em"; }},
          {"dy", [](std::ostream& s) { s << "0.7ex"; }}};
      n = 0;
      for (auto& i : legend_) {
        p1.x = parameters.position.x + local_margins.min.x;
        p1.y = parameters.position.y + local_margins.min.y +
               n * (parameters.font_size + line_margin);
        p2 = p1;
        p2.x += line_length;
        p_text = p2;
        // p_text.y += font_size / 4.;
        group->AddChild(
            new svg::Text(i.first, p_text, d_text, individual_attributes, {}));
        ++n;
      }//*/
}

void SvgChart::LegendItem::Draw(const Point<double>& position,
                                svg::Tag& parent) const {
  auto size = GetSize();
  PointArea area{position, position + size};
  PointArea picto_area{{area.min.x, area.min.y},
                       {area.min.x + picto_width_px, area.max.y}};
  DrawPicto(picto_area, parent);
  PointArea text_area{
      {area.min.x + picto_area.width() + delimiter_width_px, area.min.y},
      {area.max.x, area.max.y}};
  DrawText(text_area, parent);
}

void SvgChart::LegendItem::DrawText(const PointArea& area,
                                    svg::Tag& parent) const {
  Point<double> p{area.min.x, area.max.y};
  Point<double> d{0., 0.};
  p.y -= area.height() / 10.;
  parent.AddChild(new svg::Text(
      text_, p, d,
      svg::CreateFontAttributes(font_parameters_.family, font_parameters_.size),
      font_parameters_.color));
}

Point<double> SvgChart::LegendItem::GetSize() const {
  auto text_size = TextSize();
  return Point<double>{
      picto_width_px + delimiter_width_px +
          text_size * static_cast<double>(font_parameters_.size) * 0.7,
      font_parameters_.size};
}

std::size_t SvgChart::LegendItem::TextSize() const {
  return NumberOfSymbolsInUtf8String(text_);
}

void SvgChart::LegendItemLine::DrawPicto(const PointArea& area,
                                         svg::Tag& parent) const {
  const double x = area.min.x, y = area.min.y + area.height() / 2.;
  PointArea pp{{x, y}, {x + area.width(), y}};
  parent.AddChild(new svg::Line(pp.min, pp.max, line_attributes_));
}

void SvgChart::LegendItemRect::DrawPicto(const PointArea& area,
                                         svg::Tag& parent) const {
  auto custom_area = area;
  custom_area.min.y += area.height() * 0.1;
  custom_area.max.y -= area.height() * 0.1;
  parent.AddChild(new svg::Rect(custom_area, rect_attributes_));
}
