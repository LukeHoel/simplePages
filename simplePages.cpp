#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct html_element {
  std::string name;
  std::string content;
  std::vector<html_element> children;
  char test[10][1000];
};

struct html_attribute {
  html_element *element;
  std::string name;
  std::string content;
};

std::vector<html_element> attributes;

void read_into_element(html_element &parent_element, std::istream &is) {
  char current;
  bool found_closing;
  std::string content;
  while (!found_closing && is >> current) {
    // Get tag open
    if (current == '<') {
      // Check if it's a closing tag
      char next;
      is >> next;
      if (next == '/') {
        found_closing = true;
        std::string end_tag_name;
        // Record content of end tag and check it against opening tag
        while (is >> current && current != '>') {
          end_tag_name += current;
        }
        if (parent_element.name != end_tag_name) {
          std::cout << "Warning - Unexpected end tag '" << end_tag_name << "'"
                    << std::endl;
          std::cout << "        - Expected end tag '" << parent_element.name
                    << "'" << std::endl;
        }
      } else {
        // If it's an opening tag, read it as normal
        is.putback(next);
        html_element new_element;
        // Construct initial tag
        std::stringstream ss;
        while (is >> current && current != '>') {
          ss << current;
        }
        // At this point we could read the rest of the values into an
        // unordered_map to store attributes
        ss >> new_element.name;
        /*
        std::string attribute;
        while(ss >> attribute){
                size_t equals_location = attribute.find_first_of('=');
                if(equals_location != std::string::npos){

                }
                else{
                        new_element.attributes[attribute] = "";
                }
        }
        */
        // Read element content
        read_into_element(new_element, is);
        parent_element.children.push_back(new_element);
      }
    } else {
      parent_element.content += current;
    }
  }
  // Trim the raw content
  const char *chars = "\t\n\v\f\r ";
  parent_element.content.erase(0,
                               parent_element.content.find_first_not_of(chars));
  parent_element.content.erase(parent_element.content.find_last_not_of(chars) +
                               1);
}

void print_element(html_element element, int depth_level = 1) {
  for (html_element &element : element.children) {
    for (int i = 0; i < depth_level; i++) {
      std::cout << "-";
    }
    std::cout << "|" << element.name << " ~ '" << element.content << "'"
              << std::endl;
    print_element(element, depth_level + 1);
  }
}

int main() {

  html_element root_element;
  root_element.name = "Root Element";

  std::cin.unsetf(std::ios_base::skipws);
  read_into_element(root_element, std::cin);

  print_element(root_element);

  std::cout << std::endl;

  return 0;
}
