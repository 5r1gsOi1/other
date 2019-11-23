
#include "svg.h"

svg::Text::Text(const std::string &text, const Point<double> &p,
                const Point<double> &d, const svg::Attributes &font,
                const svg::Attributes &fill)
    : text_(text), p_(p), d_(d) {
  name = "text";
  attributes.insert(font.begin(), font.end());
  attributes.insert(fill.begin(), fill.end());
  children.push_back(std::make_unique<PlainText>(text));
}

void svg::Text::OutputAttributes(std::ostream &s) const {
  Tag::OutputAttributes(s);
  s << " x=\"" << p_.x << "\" y=\"" << p_.y << "\"";
  if (d_.x != 0.) {
    s << " dx=\"" << d_.x << "\"";
  }
  if (d_.y != 0.) {
    s << "dy =\"" << d_.y << "\"";
  }
}

svg::Line::Line(const Point<double> &p1, const Point<double> &p2,
                const svg::Attributes &stroke)
    : p1_(p1), p2_(p2) {
  name = "line";
  attributes.insert(stroke.begin(), stroke.end());
}

void svg::Line::OutputAttributes(std::ostream &s) const {
  Tag::OutputAttributes(s);
  s << " x1=\"" << p1_.x << "\" y1=\"" << p1_.y << "\"";
  s << " x2=\"" << p2_.x << "\" y2=\"" << p2_.y << "\"";
}

svg::Rect::Rect(const Point<double> p, const Point<double> size,
                const svg::Attributes &attributes_)
    : p_(p), size_(size) {
  name = "rect";
  attributes = attributes_;
}

svg::Rect::Rect(const PointArea area, const svg::Attributes &attributes_)
    : svg::Rect::Rect(area.min, {area.width(), area.height()}, attributes_) {}

void svg::Rect::OutputAttributes(std::ostream &s) const {
  Tag::OutputAttributes(s);
  s << " x=\"" << p_.x << "\" y=\"" << p_.y << "\"";
  s << " width=\"" << size_.x << "\" height=\"" << size_.y << "\"";
}

svg::Path::Path(const std::vector<std::pair<Point<double>, char>> &points,
                const svg::Attributes &attributes_)
    : points_(points) {
  Path::name = "path";
  attributes = attributes_;
}

svg::Path::Path(const std::vector<Point<double>> &points,
                const svg::Attributes &attributes_) {
  points_.reserve(points.size());
  for (auto &p : points) {
    points_.push_back(std::make_pair(p, '\0'));
  }
  if (not points_.empty()) {
    points_.begin()->second = 'M';
  }
  Path::name = "path";
  attributes = attributes_;
}

void svg::Path::OutputAttributes(std::ostream &s) const {
  Tag::OutputAttributes(s);
  if (not points_.empty()) {
    std::ios init(nullptr);
    init.copyfmt(s);
    constexpr const int p{1};
    s << " d=\"";
    for (auto it = points_.begin(); it != points_.end(); ++it) {
      if (it->second != '\0') {
        s << it->second << " ";
      }
      s << std::setprecision(p) << std::fixed << it->first.x << ","
        << std::setprecision(p) << std::fixed << it->first.y << " ";
    }
    s << "\"";
    s.copyfmt(init);
  }
}

svg::Title::Title(const std::string &title) {
  name = "title";
  children.push_back(std::make_unique<PlainText>(title));
}

void svg::Tag::OutputAttributes(std::ostream &s) const {
  for (auto &i : attributes) {
    s << " " << i.first << "=\"" << static_cast<std::string>(i.second) << "\"";
  }
}

void svg::Tag::OutputChildren(std::ostream &s, const int indent) const {
  for (auto &i : children) {
    if (i->IsOnASeparateLine()) {
      i->OutToStream(s, indent);
    } else {
      i->OutToStream(s);
    }
  }
}

void svg::Tag::OutToStream(std::ostream &s, const int indent) const {
  s << std::string(indent, ' ') << "<" << name;
  OutputAttributes(s);
  if (not children.empty()) {
    s << ">";
    bool need_line_break = false;
    for (auto &i : children) {
      if (i->IsOnASeparateLine()) {
        need_line_break = true;
      }
    }
    if (need_line_break) {
      s << std::endl;
    }
    OutputChildren(s, indent + indent_delta);
    if (need_line_break) {
      s << std::string(indent, ' ');
    }
    s << "</" << name << ">\n";
  } else {
    s << "/>\n";
  }
}

void svg::Tag::AddChild(svg::Base *child) {
  children.push_back(std::unique_ptr<Base>(child));
}

svg::PlainText::PlainText(const std::string &text) : text_(text) {}

bool svg::PlainText::IsOnASeparateLine() const { return false; }

void svg::PlainText::OutToStream(std::ostream &s, const int indent) const {
  std::ignore = indent;
  s << text_;
}

svg::PlainText::~PlainText() = default;

svg::Attributes svg::CreateFillAttributes(const std::string &color,
                                          const std::string &rule,
                                          const std::string &opacity) {
  Attributes attributes;
  if (not color.empty()) {
    attributes.insert({"fill", color});
  }
  if (not rule.empty()) {
    attributes.insert({"fill-rule", rule});
  }
  if (not opacity.empty()) {
    attributes.insert({"fill-opacity", opacity});
  }
  return attributes;
}

svg::Attributes svg::CreateStrokeAttributes(
    const std::string &color, const double width, const std::string &linecap,
    const std::string &dash, const std::string &opacity,
    const std::string &linejoin, const std::string &vector_effect) {
  Attributes attributes;
  if (not color.empty()) {
    attributes.insert({"stroke", color});
  }
  attributes.insert({"stroke-width", attributes::FixedPointNumber(width)});
  if (not linecap.empty()) {
    attributes.insert({"stroke-linecap", linecap});
  }
  if (not dash.empty()) {
    attributes.insert({"stroke-dasharray", dash});
  }
  if (not opacity.empty()) {
    attributes.insert({"stroke-opacity", opacity});
  }
  if (not linejoin.empty()) {
    attributes.insert({"stroke-linejoin", linejoin});
  }
  if (not vector_effect.empty()) {
    attributes.insert({"vector-effect", vector_effect});
  }  // "non-scaling-stroke"

  return attributes;
}

svg::Attributes svg::CreateFontAttributes(const std::string &family,
                                          const double size) {
  return svg::Attributes{{"font-family", family},
                         {"font-size", attributes::FixedPointNumber(size)}};
}

svg::Attributes svg::CreateTransformAttributes(const Point<double> scale,
                                               const Point<double> translate) {
  return svg::Attributes{
      {"transform", "translate(" + std::to_string(translate.x) + " " +
                        std::to_string(translate.y) + ") scale(" +
                        std::to_string(scale.x) + " " +
                        std::to_string(scale.y) + ")"}};
}

svg::Attributes svg::CreateStandardSvgAttributes(
    const Point<double> image_size) {
  return Attributes{
      {"width", attributes::Pixels(image_size.x)},
      {"height", attributes::Pixels(image_size.y)},
      {"viewBox", attributes::ViewBox(0, 0, image_size.x, image_size.y)},
      {"xmlns", std::string("http://www.w3.org/2000/svg")},
      {"version", attributes::FixedPointNumber(1.1)}};
}

svg::Attributes svg::CreateSvgAttributes(const Point<double> position,
                                         const Point<double> size) {
  return Attributes{
      {"width", attributes::Pixels(size.x)},
      {"height", attributes::Pixels(size.y)},
      {"x", attributes::Pixels(position.x)},
      {"y", attributes::Pixels(position.y)},
  };
}

svg::Attributes operator+(const svg::Attributes &a1,
                          const svg::Attributes &a2) {
  svg::Attributes r = a1;
  r.insert(a2.begin(), a2.end());
  return r;
}

svg::Attributes operator+(svg::Attributes &&a1, const svg::Attributes &a2) {
  svg::Attributes r = std::move(a1);
  r.insert(a2.begin(), a2.end());
  return r;
}

svg::Attributes operator+(const svg::Attributes &a1, svg::Attributes &&a2) {
  svg::Attributes r = std::move(a2);
  r.insert(a1.begin(), a1.end());
  return r;
}
