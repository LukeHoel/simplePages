#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct css_styles {
  int width = 0;
  int height = 0;
  int background_color_red = 0;
  int background_color_green = 0;
  int background_color_blue = 0;
  int background_color_alpha = -1;
};

struct html_attribute {
  std::string name;
  std::string value;
  html_attribute(std::string name, std::string value)
      : name(name), value(value) {}
};

struct html_element {
  std::string name;
  std::vector<html_attribute> attributes;
  css_styles style;
  // Elements should not have both
  const bool isTextOnly;
  std::string text;
  std::vector<html_element> children;
  html_element() : isTextOnly(false), text(""){};
  html_element(std::string text) : isTextOnly(true) { this->text = text; };
};

html_element read_opening_tag(std::istream &is) {
  html_element new_element;
  char current;
  // Construct initial tag
  std::stringstream ss;
  while (is >> current && current != '>') {
    ss << current;
  }
  // At this point we could read the rest of the values into an
  // unordered_map to store attributes
  ss >> new_element.name;
  ss.unsetf(std::ios_base::skipws);

  std::string attribute_name;
  std::string attribute_value;
  while (ss >> current) {
    if (current != '=') {
      attribute_name += current;
    } else {
      ss >> current;
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

void read_closing_tag(std::string expected_end_tag_name, std::istream &is) {
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

bool is_void_tag(std::string tag_name) {
  return tag_name == "area" || tag_name == "base" || tag_name == "br" ||
         tag_name == "col" || tag_name == "embed" || tag_name == "hr" ||
         tag_name == "img" || tag_name == "input" || tag_name == "link" ||
         tag_name == "meta" || tag_name == "param" || tag_name == "source" ||
         tag_name == "track" || tag_name == "wbr" ||
         // Obsolete
         tag_name == "command" || tag_name == "keygen" ||
         tag_name == "menuitem";
}

void trim(std::string &str) {
  const char *chars = "\t\n\v\f\r ";
  str.erase(0, str.find_first_not_of(chars));
  str.erase(str.find_last_not_of(chars) + 1);
}

std::string to_lowercase(std::string input) {
  std::string output(input);
  for (int i = 0; i < output.size(); i++) {
    if (output[i] >= 'A' && output[i] <= 'Z') {
      output[i] += 32;
    }
  }
  return output;
}

void read_element_styles(html_element element) {
  for (const html_attribute &attribute : element.attributes) {
    std::string lower_name = to_lowercase(attribute.name);
    if (lower_name == "style") {
      std::cout << element.name << "has a style" << std::endl;
      element.style.width = 5;
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
        parent_element.children.push_back(html_element(text));
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
}

void print_element(html_element element, int depth_level = 1) {
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
    std::cout << std::endl;
    print_element(element, depth_level + 1);
  }
}

int main(int argc, char **argv) {

  html_element root_element;
  root_element.name = "Root Element";

  if (argc > 1) {
    std::ifstream file(argv[1]);
    file.unsetf(std::ios_base::skipws);
    read_into_element(root_element, file);
  } else {
    std::cin.unsetf(std::ios_base::skipws);
    read_into_element(root_element, std::cin);
  }
  print_element(root_element);

  return 0;
}
