#ifndef UTIL_H
#define UTIL_H
#include <sstream>
#include <string>
int hex_str_to_int(std::string hex_str);
void trim(std::string &str);
std::string to_lowercase(std::string input);
#endif