#include "html.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

html_element read_opening_tag(std::istream &is) {
  html_element new_element;
  char current;
  // Construct initial tag
  std::stringstream ss;
  while (is >> current && current != '>') {
    ss << current;
  }
  ss >> new_element.name;
  ss.unsetf(std::ios_base::skipws);

  std::string attribute_name;
  std::string attribute_value;
  while (ss >> current) {
    // Handle case where there isn't any value
    if (!attribute_name.empty() && current == ' ') {
      new_element.attributes.push_back(
          html_attribute(attribute_name, attribute_value));
      attribute_name = attribute_value = "";
      // Go until hitting equals, then go into value reading mode
    } else if (current != '=') {
      attribute_name += current;
    } else {
      ss >> current;
      // Treat either " or ' as the same here. Subject to change
      if (current == '"' || current == '\'') {
        while (ss >> current && current != '"' && current != '\'') {
          attribute_value += current;
        }
        new_element.attributes.push_back(
            html_attribute(attribute_name, attribute_value));
        attribute_name = attribute_value = "";
      }
    }
  }
  if (!attribute_name.empty()) {
    new_element.attributes.push_back(html_attribute(
        attribute_name, attribute_value.empty() ? "" : attribute_value));
  }

  return new_element;
}

void read_closing_tag(std::string &expected_end_tag_name, std::istream &is) {
  char current;
  std::string end_tag_name;
  // Record content of end tag and check it against opening tag
  while (is >> current && current != '>') {
    end_tag_name += current;
  }
  if (expected_end_tag_name != end_tag_name) {
    std::cout << "Warning - Unexpected end tag '" << end_tag_name << "'"
              << std::endl;
    std::cout << "        - Expected end tag '" << expected_end_tag_name << "'"
              << std::endl;
  }
}

bool is_void_tag(std::string &tag_name) {
  return tag_name == "area" || tag_name == "base" || tag_name == "br" ||
         tag_name == "col" || tag_name == "embed" || tag_name == "hr" ||
         tag_name == "img" || tag_name == "input" || tag_name == "link" ||
         tag_name == "meta" || tag_name == "param" || tag_name == "source" ||
         tag_name == "track" || tag_name == "wbr" ||
         // Obsolete
         tag_name == "command" || tag_name == "keygen" ||
         tag_name == "menuitem";
}

void read_parsed_style(css_style_int &parsed_value, std::string &raw_value) {
  try {
    // Just cut off units 'px' 'em' whatever for now
    parsed_value = std::stoi(raw_value.substr(0, raw_value.size() - 2));
  } catch (std::invalid_argument e) {
    std::cout << "Failed to parse value: " << raw_value << std::endl;
  }
}

void read_raw_style(css_styles &style, std::string &style_name,
                    std::string &style_value) {
  trim(style_name);
  trim(style_value);
  if (!style_name.empty() && !style_value.empty()) {
    if (style_name == "width") {
      read_parsed_style(style.width, style_value);
    } else if (style_name == "height") {
      read_parsed_style(style.height, style_value);
    } else if (style_name == "color") {
      style.color = css_color(style_value);
    } else if (style_name == "background-color") {
      style.background_color = css_color(style_value);
    } else if (style_name == "display") {
      if (style_value == "none") {
        style.display = css_display::none;
      } else {
        style.display = css_display::block;
	  }
    }
  }
  // Clear values after getting
  style_name = style_value = "";
}

void read_element_styles(html_element &element) {
  std::stringstream ss;
  char current;
  ss.unsetf(std::ios_base::skipws);
  for (const html_attribute &attribute : element.attributes) {
    std::string lower_name = to_lowercase(attribute.name);
    trim(lower_name);
    if (lower_name == "style") {
      ss.clear();
      ss << attribute.value;
      std::string style_name;
      std::string style_value;
      // Css styles are seperated by semicolons. values and labels are seperated
      // by colons
      while (ss >> current) {
        if (current == ':') {
          while (ss >> current && current != ';') {
            style_value += current;
          }
          read_raw_style(element.style, style_name, style_value);
        } else {
          style_name += current;
        }
      }
      // Only allow for one style tag
      break;
    }
  }
}

void read_into_element(html_element &parent_element, std::istream &is) {
  char current;
  bool found_closing;
  std::string text;
  while (!found_closing && is >> current) {
    if (current == '<') {
      // Trim the raw content
      trim(text);
      if (!text.empty()) {
	  	html_element text_element = html_element(text);
        parent_element.children.push_back(text_element);
      }
      text = "";
      // Check if it's a closing tag
      char next;
      is >> next;
      if (next == '/') {
        found_closing = true;
        read_closing_tag(parent_element.name, is);
      } else {
        is.putback(next);
        html_element new_element = read_opening_tag(is);
        read_element_styles(new_element);

        // Full width block elements
        if (!new_element.style.width.is_set() ||
            new_element.style.display == css_display::_inline) {
          new_element.calculated_width = parent_element.calculated_width;
        }

        if (!is_void_tag(new_element.name)) {
          read_into_element(new_element, is);
        }

        parent_element.children.push_back(new_element);
      }
    } else {
      // Raw content
      text += current;
    }
  }
  // Elements with set width
  for (html_element &child : parent_element.children) {
    if (!parent_element.style.width.is_set()) {
      parent_element.calculated_width += child.calculated_width;
    }
    if (!parent_element.style.height.is_set()) {
      parent_element.calculated_height += child.calculated_height;
    }
  }
  if (parent_element.calculated_width < parent_element.style.width) {
    parent_element.calculated_width = parent_element.style.width;
  }
  if (parent_element.calculated_height < parent_element.style.height) {
    parent_element.calculated_height = parent_element.style.height;
  }
}

void print_element(html_element element, int depth_level) {
  for (html_element &element : element.children) {
    for (int i = 0; i < depth_level; i++) {
      std::cout << "-";
    }
    std::cout << "|" << element.name;
    for (html_attribute attribute : element.attributes) {
      std::cout << " " << attribute.name << "='" << attribute.value << "'";
    }
    if (!element.text.empty()) {
      std::cout << " Content: '" << element.text << "'";
    }
    if (element.style.width != 0) {
      std::cout << " Width: " << element.style.width;
    }
    if (element.style.height != 0) {
      std::cout << " Height: " << element.style.height;
    }
    std::cout << " Red: " << element.style.background_color.red;
    std::cout << " Green: " << element.style.background_color.green;
    std::cout << " Blue: " << element.style.background_color.blue;
    std::cout << std::endl;
    print_element(element, depth_level + 1);
  }
}
