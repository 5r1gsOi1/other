
#ifndef SVG_CHART_H_
#define SVG_CHART_H_

#include <string>
#include "chart.h"
#include "curves.h"
#include "coordinates_transformer.h"
#include "svg.h"

struct NotReady : public std::exception {
  const char* what() const noexcept override {
    return "Some settings need to be set prior to this operation";
  }
};

struct NotOpened : public std::exception {
  const char* what() const noexcept override { return "File was not opened"; }
};

class SvgChart {
 public:
  struct Settings {
    std::string title;
    Point<double> image_size;
    PointArea margins;
    PointArea working_area;
    svg::Attributes background;
  };

  struct FontParameters {
    std::string family;
    int size;
    svg::Attributes color;
  };

  class LegendItem {
   public:
    LegendItem() = delete;
    LegendItem(const std::string& text, const FontParameters& font_parameters)
        : text_(text), font_parameters_(font_parameters) {}
    virtual ~LegendItem() = default;

    virtual void Draw(const Point<double>& position, svg::Tag& parent) const;
    virtual void DrawPicto(const PointArea& area, svg::Tag& parent) const = 0;
    virtual void DrawText(const PointArea& area, svg::Tag& parent) const;
    virtual Point<double> GetSize() const;

   protected:
    const double picto_width_px = 50, delimiter_width_px = 15;

    std::string text_;
    FontParameters font_parameters_;

    std::size_t TextSize() const;
  };

  class LegendItemLine : public SvgChart::LegendItem {
   public:
    LegendItemLine(const std::string& text,
                   const FontParameters& font_parameters,
                   const svg::Attributes& line_attributes)
        : LegendItem(text, font_parameters),
          line_attributes_(line_attributes) {}
    virtual ~LegendItemLine() override = default;
    virtual void DrawPicto(const PointArea& area,
                           svg::Tag& parent) const override;

   protected:
    svg::Attributes line_attributes_;
  };

  class LegendItemRect : public SvgChart::LegendItem {
   public:
    LegendItemRect(const std::string& text,
                   const FontParameters& font_parameters,
                   const svg::Attributes& rect_attributes)
        : LegendItem(text, font_parameters),
          rect_attributes_(rect_attributes) {}
    virtual ~LegendItemRect() override = default;
    virtual void DrawPicto(const PointArea& area,
                           svg::Tag& parent) const override;

   protected:
    svg::Attributes rect_attributes_;
  };

  SvgChart() = delete;
  SvgChart(const SvgChart::Settings& settings);
  SvgChart(const SvgChart& other) = delete;
  SvgChart(SvgChart&& other);

  void SaveToFile(const std::string& file_name) const;
  void AddFrame(const svg::Attributes& stroke);
  void AddGridX(const std::vector<double>& values,
                const svg::Attributes& stroke);
  void AddNumbersX(const std::vector<std::pair<double, std::string>>& values,
                   const svg::Attributes& attributes);
  void AddGridY(const std::vector<double>& values,
                const svg::Attributes& stroke);
  void AddNumbersY(const std::vector<std::pair<double, std::string>>& values,
                   const svg::Attributes& attributes);
  void AddCurve(const chart::Curve& curve, const svg::Attributes& stroke);
  void SetWorkingArea(const PointArea& working_area);
  void AddBackground(const svg::Attributes& fill);

  void AddLegend(const Point<double>& area, const svg::Attributes& background);

  void AddLegendItemLine(const std::string& name,
                         const SvgChart::FontParameters& font,
                         const svg::Attributes& stroke);
  void AddLegendItemRect(const std::string& name,
                         const SvgChart::FontParameters& font,
                         const svg::Attributes& stroke);

  void AddRect(const PointArea& area, const svg::Attributes& attributes);

 protected:
  std::unique_ptr<svg::Tag> root_;
  svg::Svg* inner_svg_;
  Point<double> inner_image_size_;
  SvgChart::Settings settings_;
  std::vector<LegendItem*> legend_;
  CoordinatesTransformer transformer_;
  bool is_ready_;
  bool to_create_legend_;

  void CreateInnerSvgIfNotExists();
};

#endif
