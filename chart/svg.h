
#ifndef SVG_H_
#define SVG_H_

#include <common/any.h>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include "common/any.h"
#include "common/basic.h"

namespace svg {
namespace attributes {

struct Color {
  unsigned char r, g, b;
  Color(const unsigned char r_, const unsigned char g_, const unsigned char b_)
      : r(r_), g(g_), b(b_) {}
  Color() : r(0), g(0), b(0) {}
  operator std::string() const {
    std::stringstream ss;
    ss << "#" << std::setfill('0') << std::setw(2) << std::hex << int(r)
       << std::setw(2) << int(g) << std::setw(2) << int(b);
    return ss.str();
  }
};

using FixedPointNumber = ::FixedPointNumber<>;

template <auto MeasureSuffix>
struct MeasuredNumber : public FixedPointNumber {
  MeasuredNumber() : FixedPointNumber() {}
  MeasuredNumber(const long double number) : FixedPointNumber(number) {}
  operator std::string() const {
    return FixedPointNumber::operator std::string() +
           std::string(MeasureSuffix);
  }
};

static constexpr char kPixelsSuffix[] = "px";
using Pixels = MeasuredNumber<kPixelsSuffix>;

static constexpr char kExSuffix[] = "ex";
using Ex = MeasuredNumber<kExSuffix>;

static constexpr char kEmSuffix[] = "em";
using Em = MeasuredNumber<kEmSuffix>;

struct ViewBox {
  FixedPointNumber p1, p2, p3, p4;
  ViewBox(const long double p1_, const long double p2_, const long double p3_,
          const long double p4_)
      : p1(p1_), p2(p2_), p3(p3_), p4(p4_) {}
  operator std::string() const {
    return p1.operator std::string() + " " + p2.operator std::string() + " " +
           p3.operator std::string() + " " + p4.operator std::string();
  }
  bool operator==(const ViewBox& other) const {
    return p1 == other.p1 and p2 == other.p2 and p3 == other.p3 and
           p4 == other.p4;
  }
};

struct Url {
  std::string name;
  Url(const std::string& name_) : name(name_) {}
  operator std::string() const { return "url(#" + name + ")"; }
  bool operator==(const Url& other) const { return name == other.name; }
};

}  // namespace attributes

using AttributeName = std::string;
using AttributeValue = Any;
using Attributes = std::map<AttributeName, AttributeValue>;

struct Base {
  virtual ~Base() = default;
  virtual void OutToStream(std::ostream& s, const int indent = 0) const = 0;
  virtual bool IsOnASeparateLine() const { return true; }
  static constexpr int indent_delta = 2;
};

struct PlainText : public Base {
  PlainText(const std::string& text);
  virtual ~PlainText() override;
  virtual bool IsOnASeparateLine() const override;
  virtual void OutToStream(std::ostream& s,
                           const int indent = 0) const override;

 private:
  std::string text_;
};

struct Tag : public Base {
  std::string name;
  Attributes attributes;
  std::list<std::unique_ptr<Base>> children;

  Tag() = default;
  Tag(const std::string& name_, const svg::Attributes& attributes_ = {},
      std::list<std::unique_ptr<Base>>&& children_ = {})
      : name(name_), attributes(attributes_), children(std::move(children_)) {}
  virtual ~Tag() override = default;

  virtual void OutputAttributes(std::ostream& s) const;
  virtual void OutputChildren(std::ostream& s, const int indent = 0) const;
  virtual void OutToStream(std::ostream& s,
                           const int indent = 0) const override;
  void AddChild(Base* child);
};

const std::string kHeader =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
    "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";

struct Svg : public Tag {
  Svg(const Attributes& attributes_) : Tag("svg", attributes_) {}
};

struct Group : public Tag {
  Group(const Attributes& attributes_) : Tag("g", attributes_) {}
};

Attributes CreateSvgAttributes(const Point<double> position,
                               const Point<double> size);

Attributes CreateStandardSvgAttributes(const Point<double> image_size);

Attributes CreateFillAttributes(const std::string& color,
                                const std::string& rule = std::string(),
                                const std::string& opacity = std::string());

Attributes CreateStrokeAttributes(
    const std::string& color, const double width,
    const std::string& linecap = std::string(),
    const std::string& dash = std::string(),
    const std::string& opacity = std::string(),
    const std::string& linejoin = std::string("round"),
    const std::string& vector_effect = std::string());

Attributes CreateFontAttributes(const std::string& family, const double size);

Attributes CreateTransformAttributes(const Point<double> scale,
                                     const Point<double> translate);

struct Title : public Tag {
  Title(const std::string& title);
};

struct Path : public Tag {
  Path(const std::vector<Point<double>>& points, const Attributes& attributes_);
  virtual void OutputAttributes(std::ostream& s) const override;

 private:
  std::vector<Point<double>> points_;
};

struct Rect : public Tag {
  Rect(const Point<double> p, const Point<double> size,
       const Attributes& attributes_);
  Rect(const PointArea area, const Attributes& attributes_);
  virtual void OutputAttributes(std::ostream& s) const override;

 private:
  Point<double> p_, size_;
};

struct Line : public Tag {
  Line(const Point<double>& p1, const Point<double>& p2,
       const Attributes& stroke);
  virtual void OutputAttributes(std::ostream& s) const override;

 private:
  Point<double> p1_, p2_;
};

struct Text : public Tag {
  Text(const std::string& text, const Point<double>& p, const Point<double>& d,
       const Attributes& font, const Attributes& fill);
  virtual void OutputAttributes(std::ostream& s) const override;

 private:
  std::string text_;
  Point<double> p_, d_;
};
}  // namespace svg

svg::Attributes operator+(const svg::Attributes& a1, const svg::Attributes& a2);
// svg::Attributes operator+(svg::Attributes&& a1, const svg::Attributes& a2);
// svg::Attributes operator+(const svg::Attributes& a1, svg::Attributes&& a2);

#endif
