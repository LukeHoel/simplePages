#ifndef CSS_H
#define CSS_H
#include "../utils/utils.h"
#include <string>
class css_color {
  bool isSet = true;

public:
  int red = 0;
  int green = 0;
  int blue = 0;
  css_color() : isSet(false) {}
  css_color(std::string color_string) : isSet(true) {
    // If it's a hex color, turn it into rgb
    if (color_string[0] == '#') {
      red = hex_str_to_int(color_string.substr(1, 2));
      green = hex_str_to_int(color_string.substr(3, 2));
      blue = hex_str_to_int(color_string.substr(5, 2));
    }
  }
  bool is_set() { return isSet; }
};

class css_style {
protected:
  bool isSet = false;

public:
  bool is_set() { return isSet; }
  void un_set() { isSet = false; }
};

class css_style_int : public css_style {
  int inner_value = 0;

public:
  css_style_int operator=(const int value) {
    isSet = true;
    inner_value = value;
    return *this;
  }
  operator int() { return inner_value; }
  bool operator<(const int value) { return inner_value < value; }
  bool operator>(const int value) { return inner_value > value; }
  bool operator==(const int value) { return inner_value == value; }
};

enum css_display { none, block };

struct css_styles {
  css_style_int width;
  css_style_int height;
  css_color background_color;
  css_color color;
  css_display display = css_display::block;
};
#endif