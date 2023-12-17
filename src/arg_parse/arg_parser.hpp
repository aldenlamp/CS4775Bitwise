#ifndef SPARSE_ARG_PARSER_ARG_PARSER_H_
#define SPARSE_ARG_PARSER_ARG_PARSER_H_

#include <string>
#include <vector>

namespace argparse {

class ArgParser {
 private:
  int argc_;
  char const** argv_;
  std::vector<bool> used_args_;
  std::vector<std::string> known_args_;

 public:
  /**
   * @brief Determine if the keyword exists
   *
   * @param keyword the keyword to search for
   * @return true if the keyword is found
   * @return false if the keyword is not found
   */
  bool KeywordExists(const std::string& keyword);

  /**
   * @brief Get the next value after keyword
   *
   * @param keyword the keyword to search for
   * @param is_bool represents whether the argument being parsed is a bool
   * @return char*
   */
  const char* GetKeywordValue(const std::string& keyword, bool is_bool,
                              bool is_vector);

  /**
   * @brief Validates the unused args. If there is an argument that is not
   *    used, it will print that to std::out
   *
   */
  void ValidateUnused();

  /**
   * @brief Set the known args to be able to process lists
   *
   * @param known_args a vector of known args
   */
  void SetKnownArgs(const std::vector<std::string> known_args);

  /**
   * @brief Construct a new Arg Parser object
   *
   * @param argc the number of arguments
   * @param argv a pointer to a list of arguments
   */
  ArgParser(int argc, char const** argv);
};

}  // namespace argparse

#endif