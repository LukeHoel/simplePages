#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct html_element {
  std::string name;
  std::unordered_map<std::string, std::string> attributes;
  std::string content;
  std::vector<html_element> children;
};

html_element read_opening_tag(std::istream& is){
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
        
        std::string attribute;
        while(ss >> attribute){
                size_t equals_location = attribute.find_first_of('=');
                if(equals_location != std::string::npos){
						std::string attribute_name = attribute.substr(0, equals_location);
                }
                else{
                        new_element.attributes[attribute] = "";
                }
        }

        return new_element;
}

void read_closing_tag(std::string expected_end_tag_name,std::istream& is){
  char current;
  std::string end_tag_name;
  // Record content of end tag and check it against opening tag
  while (is >> current && current != '>') {
    end_tag_name += current;
  }
  if (expected_end_tag_name != end_tag_name) {
    std::cout << "Warning - Unexpected end tag '" << end_tag_name << "'"
              << std::endl;
    std::cout << "        - Expected end tag '" << expected_end_tag_name
              << "'" << std::endl;
  }
}

bool is_void_tag(std::string tag_name){
			return tag_name == "area" ||
			tag_name == "base" ||
			tag_name == "br" ||
			tag_name == "col" ||
			tag_name == "embed" ||
			tag_name == "hr" ||
			tag_name == "img" ||
			tag_name == "input" ||
			tag_name == "link" ||
			tag_name == "meta" ||
			tag_name == "param" ||
			tag_name == "source" ||
			tag_name == "track" ||
			tag_name == "wbr" ||
			// Obsolete
			tag_name == "command" ||
			tag_name == "keygen" ||
			tag_name == "menuitem";
}

void read_into_element(html_element &parent_element, std::istream &is) {
  char current;
  bool found_closing;
  std::string content;
  while (!found_closing && is >> current) {
    if (current == '<') {
      // Check if it's a closing tag
      char next;
      is >> next;
      if (next == '/') {
        found_closing = true;
        read_closing_tag(parent_element.name, is);
      } else {
        is.putback(next);
        html_element new_element = read_opening_tag(is);
  		
        if(!is_void_tag(new_element.name)){
              read_into_element(new_element, is);
        }
        parent_element.children.push_back(new_element);
      }
    } else {
      // Raw content
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
