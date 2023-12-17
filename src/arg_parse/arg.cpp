
#include "arg.hpp"

using std::istringstream;
using std::vector;

namespace argparse {

namespace ops {

void operator>>(const std::string& str, int* val) { *val = std::stoi(str); }

void operator>>(const std::string& str, bool* val) { *val = str == "1"; }

void operator>>(const std::string& str, float* val) { *val = std::stof(str); }

void operator>>(const std::string& ss, char** val) {
  char* c = new char[100];
  std::copy(ss.begin(), ss.end(), c);
  delete *val;
  *val = c;
}

void operator>>(const std::string& ss, std::string* val) { *val = ss; }

void operator>>(const std::string& ss, std::vector<float>* val) {
  *val = vector<float>();

  istringstream iss(ss);

  std::copy(std::istream_iterator<float>(iss), std::istream_iterator<float>(),
            std::back_inserter(*val));
}

void operator<<(std::stringstream& ss, const std::vector<float>& val) {
  for (vector<float>::const_iterator it = val.begin(); it != val.end(); ++it) {
    ss << *it << "  ";
  }
}

}  // namespace ops

}  // namespace argparse