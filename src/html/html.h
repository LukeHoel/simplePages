
#ifndef HTML_H
#define HTML_H
#include "../css/css.h"
#include <string>
#include <vector>

struct html_attribute {
  std::string name;
  std::string value;
  html_attribute(std::string name, std::string value)
      : name(name), value(value) {}
};

class html_element {
public:
  const bool isTextOnly;
  std::string name;
  std::vector<html_attribute> attributes;
  css_styles style;
  // Elements should not have both
  std::string text;
  std::vector<html_element> children;
  html_element() : isTextOnly(false), text(""){};
  html_element(std::string text) : isTextOnly(true) { this->text = text; };
  int calculated_width = 0;
  int calculated_height = 0;
};

html_element read_opening_tag(std::istream &is);
void read_closing_tag(std::string &expected_end_tag_name, std::istream &is);
bool is_void_tag(std::string &tag_name);
void read_parsed_style(css_style_int &parsed_value, std::string &raw_value);
void read_raw_style(css_styles &style, std::string &style_name,
                    std::string &style_value);
void read_element_styles(html_element &element);
void read_into_element(html_element &parent_element, std::istream &is);
void print_element(html_element element, int depth_level = 1);
#endif
