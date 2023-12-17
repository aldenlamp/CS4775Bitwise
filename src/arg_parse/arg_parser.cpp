#include "arg_parser.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>

using std::string;
using std::stringstream;
using std::vector;

namespace argparse {

ArgParser::ArgParser(int argc, char const** argv) : argc_(argc), argv_(argv) {
  used_args_ = vector<bool>(argc, false);
  used_args_[0] = true;
}

bool ArgParser::KeywordExists(const string& keyword) {
  const char** itr = std::find(argv_, argv_ + argc_, keyword);
  bool found = itr != argv_ + argc_;
  return found;
}

// Helper function for marking args as used or not
bool IsFloat(const char* myString) {
  std::istringstream iss(myString);
  float f;
  iss >> f;
  return iss.eof() && !iss.fail();
}

// TODO (Alden) - Clean up this function
const char* ArgParser::GetKeywordValue(const string& keyword, bool is_bool,
                                       bool is_vector) {
  char const** itr = std::find(argv_, argv_ + argc_, keyword);

  int index = static_cast<int>(itr - argv_);
  used_args_[index] = true;

  if (is_bool) {
    return "1";
  }

  if (++itr == argv_ + argc_) {
    std::cout << "Could not find value for argument: " << keyword << std::endl;
    exit(EXIT_FAILURE);
  }

  index = static_cast<int>(itr - argv_);
  used_args_[index] = true;

  if (is_vector) {
    stringstream ss;
    ss << *itr;
    while (++itr != argc_ + argv_ &&
           !std::count(known_args_.begin(), known_args_.end(),
                       std::string(*itr)) &&
           IsFloat(*itr)) {
      ss << " " << *itr;
      index = static_cast<int>(itr - argv_);
      used_args_[index] = true;
    }

    int n = ss.str().length();
    char* char_array = new char[n + 1];
    strcpy(char_array, ss.str().c_str());
    return char_array;
  }

  return *itr;
}

void ArgParser::ValidateUnused() {
  for (vector<bool>::iterator it = used_args_.begin(), end = used_args_.end();
       it != end; ++it) {
    if (!*it) {
      int index = static_cast<int>(it - used_args_.begin());
      std::cout << "Argument never parsed: " << argv_[index] << std::endl;
    }
  }
}

void ArgParser::SetKnownArgs(const vector<string> known_args) {
  known_args_ = known_args;
}

}  // namespace argparse