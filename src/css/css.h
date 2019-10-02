#ifndef CSS_H
#define CSS_H
#include "../utils/utils.h"
#include <string>
struct css_color {
  int red = 0;
  int green = 0;
  int blue = 0;
  css_color() {}
  css_color(std::string color_string) {
    if (color_string[0] == '#') {
      red = hex_str_to_int(color_string.substr(1, 2));
      green = hex_str_to_int(color_string.substr(3, 2));
      blue = hex_str_to_int(color_string.substr(5, 2));
    }
  }
};

enum css_display { none, block };

struct css_styles {
  int width = 0;
  int height = 0;
  css_color background_color;
  css_display display = css_display::block;
};
#endif