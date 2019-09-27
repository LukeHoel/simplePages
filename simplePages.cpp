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
        read_closing_tag(parent_element.name, is);
      } else {
        // If it's an opening tag, read it as normal
        is.putback(next);
        html_element new_element = read_opening_tag(is);
  		
        // Read element content unless it is a void element (List found on 
		if(
			new_element.name != "area" &&
			new_element.name != "base" &&
			new_element.name != "br" &&
			new_element.name != "col" &&
			new_element.name != "embed" &&
			new_element.name != "hr" &&
			new_element.name != "img" &&
			new_element.name != "input" &&
			new_element.name != "link" &&
			new_element.name != "meta" &&
			new_element.name != "param" &&
			new_element.name != "source" &&
			new_element.name != "track" &&
			new_element.name != "wbr" &&
			// Obsolete
			new_element.name != "command" &&
			new_element.name != "keygen" &&
			new_element.name != "menuitem"
			){
        	read_into_element(new_element, is);
		}
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
