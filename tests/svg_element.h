#pragma once

#include <cmath>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>

namespace SVG {

struct SVGPoint {
  double x;
  double y;
  bool is_higher_than(const SVGPoint &other) const;
  bool is_lower_than(const SVGPoint &other) const;
  bool is_more_left_than(const SVGPoint &other) const;
  bool is_more_right_than(const SVGPoint &other) const;
};

struct SVGRect {
  double x;
  double y;
  double width;
  double height;
  SVGPoint center() const;
  void extend(const SVGPoint &point);
  void extend(const SVGRect &other);
};

struct SVGMatrix {
  double a;
  double b;
  double c;
  double d;
  double e;
  double f;
};

enum class SVGElementType {
  Circle,
  Ellipse,
  Group,
  Line,
  Path,
  Polygon,
  Polyline,
  Rect,
  Svg,
  Text,
  Title,
};

std::string_view tag(SVG::SVGElementType type);

struct SVGAttributes {
  std::string class_;
  double cx;
  double cy;
  std::string fill;
  std::string font_family;
  double font_size;
  double height;
  std::string id;
  std::vector<SVGPoint> points;
  double rx;
  double ry;
  std::string stroke;
  double stroke_opacity = 1;
  double stroke_width = 1;
  std::string text_anchor;
  std::optional<SVGMatrix> transform;
  SVGRect viewBox;
  double width;
  double x;
  double y;
};

/**
 * @brief The SVGElement class represents an SVG element
 */

class SVGElement {
public:
  SVGElement() = delete;
  explicit SVGElement(SVG::SVGElementType type);

  /// \brief Return the value of an attribute retrieved from the element and its
  /// children
  ///
  /// @param attribute_ptr pointer to a member of the `SVGAttributes` class.
  /// @param predicate pointer to a member function of the `SVGElement` class
  /// which returns `true` for elements on which the attribute exists.
  /// @param default_value the value to return if the attribute does not exist
  /// on any of the elements.
  ///
  /// Returns the value of the attribute if it has the same value on all
  /// elements on which it exists and throws an exception if the value is not
  /// consistent on those elements. Returns the default value if the attribute
  /// does not exist on any of the elements.
  template <typename T>
  T attribute_from_subtree(T SVGAttributes::*attribute_ptr,
                           bool (SVGElement::*predicate)() const,
                           T default_value) const {
    std::optional<T> attribute;
    if ((this->*predicate)()) {
      attribute = this->attributes.*attribute_ptr;
    }
    for (const auto &child : children) {
      if (!(child.*predicate)()) {
        continue;
      }
      const auto child_attribute = child.attributes.*attribute_ptr;
      if (!attribute.has_value()) {
        attribute = child_attribute;
        continue;
      }
      if (attribute.value() != child_attribute) {
        throw std::runtime_error{fmt::format(
            "Inconsistent value of attribute: current {}: {}, child {}: {}",
            tag(type), attribute.value(), tag(child.type), child_attribute)};
      }
    }
    return attribute.value_or(default_value);
  }
  /// Return the bounding box of the element and its children. The bounding box
  /// is calculated and stored the first time this function is called and later
  /// calls will return the already calculated value. If this function is called
  /// for an SVG element for which the bounding box is not defined, it will
  /// throw an exception unless the `throw_if_bbox_not_defined` parameter is
  /// `false`.
  SVG::SVGRect bbox(bool throw_if_bbox_not_defined = true);
  bool is_shape_element() const;
  std::string to_string(std::size_t indent_size) const;

  SVGAttributes attributes;
  /// The Graphviz build date
  std::string graphviz_build_date;
  std::vector<SVGElement> children;
  /// The `graph_id`, `node_id` or `edgeop` according to the DOT language
  /// specification. Note that this is not the same as the `id` attribute of the
  /// SVG element
  std::string graphviz_id;
  /// The Graphviz release version
  std::string graphviz_version;
  /// The points given by the `d` attribute of a path element
  std::vector<SVGPoint> path_points;
  /// The SVG element text node contents. Not to be confused with an SVG `text`
  /// element
  std::string text;
  /// The type of SVG element
  const SVGElementType type;

private:
  /// append a string possibly containing an attribute to another string,
  /// handling space separation
  void append_attribute(std::string &output,
                        const std::string &attribute) const;
  std::string id_attribute_to_string() const;
  std::string fill_attribute_to_string() const;
  std::string points_attribute_to_string() const;
  std::string stroke_attribute_to_string() const;
  std::string stroke_opacity_attribute_to_string() const;
  std::string stroke_width_attribute_to_string() const;
  std::string stroke_to_graphviz_color(const std::string &color) const;
  SVG::SVGRect text_bbox() const;
  void to_string_impl(std::string &output, std::size_t indent_size,
                      std::size_t current_indent) const;

  /// The bounding box of the element and its children. Stored the first time
  /// it's computed
  std::optional<SVG::SVGRect> m_bbox;
};

} // namespace SVG
