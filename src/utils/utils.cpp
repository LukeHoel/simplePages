#include "utils.h"
int hex_str_to_int(std::string hex_str) {
  int decimal_value = 0;
  try {
    std::stringstream ss;
    std::string decimal_str;
    ss << hex_str;
    ss >> std::hex >> decimal_value;
  } catch (std::invalid_argument e) {
    std::cout << "Failed to read hex value :" << e.what() << std::endl;
  }
  return decimal_value;
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
