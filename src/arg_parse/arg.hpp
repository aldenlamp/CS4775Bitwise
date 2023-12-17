#ifndef SPARSE_ARG_PARSER_ARG_H_
#define SPARSE_ARG_PARSER_ARG_H_

#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "arg_parser.hpp"

// TODO: - Refactor handling of strings in this file

namespace argparse {

namespace ops {

void operator>>(const std::string& str, int* val);
void operator>>(const std::string& str, bool* val);
void operator>>(const std::string& str, float* val);
void operator>>(const std::string& str, char** val);
void operator>>(const std::string& str, std::string* val);
void operator>>(const std::string& str, std::vector<float>* val);

void operator<<(std::stringstream& ss, const std::vector<float>& val);

}  // namespace ops

}  // namespace argparse

using argparse::ops::operator>>;
using argparse::ops::operator<<;

namespace argparse {

constexpr int kHelpWidthLong = 20;
constexpr int kHelpWidthShort = 10;

// A base class for Arg to allow unified functionality considering templates
class BaseArg {
 public:
  virtual ~BaseArg() = default;
  virtual BaseArg* Clone() = 0;
  virtual void ParseArg(ArgParser* parser) = 0;
  virtual void ValidateRequired() = 0;
  friend std::ostream& operator<<(std::ostream& os, const BaseArg& arg) {
    return arg.GetHelpStr(os);
  }
  virtual std::string GetFlag() = 0;
  virtual std::string GetValueStr() = 0;
  virtual std::ostream& GetHelpStr(std::ostream& os) const = 0;
};

template <typename T>
class Arg : public BaseArg {
 public:
  /**
   * @brief Construct a new Arg object. If it is of type boolean, it will
   *   always assume the default value is "0".
   *
   * @param keyword the keyword corresponding to this arg
   * @param var a pointer to the variable to set
   * @param name the name of this argument
   * @param desc the string description of this arg
   * @param required whether or not this arg is required. Defaults to false
   * @param has_default A boolean representing if this arg will use a
   *    default value
   * @param default_val The default value of this argument
   */
  Arg(const std::string& keyword, T* var, const std::string& name,
      const std::string& desc, bool required = false, bool has_default = false,
      const std::string& default_val = "")
      : has_default_(has_default),
        did_set_(false),
        keyword_(keyword),
        required_(required),
        description_(desc),
        name_(name),
        var_(var) {
    if (IsBool() && !has_default) {
      has_default_ = true;
      std::string("0") >> var_;
      default_str_ = "0";
    }

    if (has_default && default_val != "") {
      default_val >> var_;
      default_str_ = default_val;
      has_default_ = true;
    }
  }

  /**
   * @brief Clones this Arg into a new class
   *
   * @return BaseArg*
   */
  BaseArg* Clone() { return new Arg<T>(*this); }

  /**
   * @brief Process this argument given a parser reference
   *
   * @param parser the ArgParser to process from
   */
  void ParseArg(ArgParser* parser) {
    if (parser->KeywordExists(keyword_)) {
      bool is_bool = std::is_same<T, bool>::value;
      bool is_vector = std::is_same<T, std::vector<float>>::value;
      const char* res = parser->GetKeywordValue(keyword_, is_bool, is_vector);
      std::string(res) >> var_;
      if (is_vector) {
        delete res;
      }
      did_set_ = true;
    }
  }

  /**
   * @brief Validates this argument by checking that if it is required it
   *   has been set
   *
   * @param required represents if this should be considered required during
   *   validation
   */
  void ValidateRequired() {
    if (required_ && !HasValue()) {
      std::cout << "No value set for argument: " << keyword_ << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  /**
   * @brief Determines if the value has ever been set either from a default
   *   value or from manually setting it
   *
   * @return true if it has been set
   * @return false if it has not been set
   */
  bool HasValue() const { return did_set_ || has_default_; }

  bool DidSet() { return did_set_; }

  /**
   * @brief Get the value string representing this object as a key value pair
   *
   * @return std::string
   */
  std::string GetValueStr() {
    std::stringstream value_ss;
    value_ss << std::setw(kHelpWidthLong) << std::left << keyword_
             << std::setw(kHelpWidthShort) << std::left << GetValueAsStr();
    return value_ss.str();
  }

  std::string GetFlag() { return keyword_; }

  /**
   * @brief Outputs the help string
   *
   * @param os the stream to output to
   * @return std::ostream&
   */
  std::ostream& GetHelpStr(std::ostream& os) const {
    os << std::setw(kHelpWidthLong) << std::left << name_
       << std::setw(kHelpWidthShort) << std::left << keyword_
       << std::setw(kHelpWidthShort) << std::left << GetType()
       << std::setw(kHelpWidthShort) << std::left << GetValueAsStr(true)
       << std::setw(kHelpWidthShort) << std::left
       << (required_ ? "True" : "False") << std::left << description_;

    return os;
  }

  bool operator==(T* var) { return var == var_; }

 private:
  std::string name_;
  std::string keyword_;
  std::string description_;

  std::string default_str_;

  bool has_default_;
  bool did_set_;

  bool required_;
  T* var_;

  /**
   * @brief Returns the type of this Arg in string format
   *
   * @return std::string
   */
  std::string GetType() const {
    if (std::is_same<T, bool>::value) return "bool";
    if (std::is_same<T, int>::value) return "int";
    if (std::is_same<T, float>::value) return "float";
    if (std::is_same<T, char*>::value) return "string";
    if (std::is_same<T, std::string>::value) return "string";
    if (std::is_same<T, std::vector<float>>::value) return "list[float]";
    return "unknown";
  }

  /**
   * @brief Get the value of this object as a string
   *
   * @param use_default whether to get the actual value or the default value
   * @return std::string
   */
  std::string GetValueAsStr(bool use_default = false) const {
    if (use_default && !has_default_) return "Na";
    if (!use_default && !HasValue()) return "Na";

    std::stringstream ss;
    if (use_default) {
      ss << (IsBool() ? (default_str_ == "1" ? "True" : "False")
                      : default_str_);
    } else {
      ss << *var_;
    }
    return ss.str();
  }

  bool IsBool() const { return std::is_same<T, bool>::value; }
};

}  // namespace argparse

#endif