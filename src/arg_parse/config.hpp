#ifndef SPARSE_ARG_PARSER_CONFIG_H_
#define SPARSE_ARG_PARSER_CONFIG_H_

#include <string>
#include <vector>

#include "arg.hpp"
#include "arg_parser.hpp"

namespace argparse {

class Config {
 public:
  // MARK: - Constructors

  Config() {}
  ~Config();
  Config(const Config& other);
  const Config& operator=(const Config& other);

  /**
   * @brief Parses all args using the ArgParser
   *
   */
  void ParseArgs(ArgParser* parser);

  /**
   * @brief Handles outputting help if requested
   *
   */
  void OutputHelp();

  /**
   * @brief Handles listing the arguments and their values
   *
   */
  void ListArgs();

  /**
   * @brief Validates that all the required args have set values
   *
   */
  void ValidateRequired();

 protected:
  std::string config_name_;
  std::vector<BaseArg*> args_;

  /**
   * @brief Add an argument to the config
   *
   * @tparam T the data type of the argument to add
   * @param arg the argument to add
   */
  template <typename T>
  void AddArg(Arg<T>* arg) {
    args_.push_back(arg);
  }

  /**
   * @brief A function to add all the arguments using AddArgs derived classes
   *   should class Config::AddAllArgs to add overall args
   *
   */
  virtual void AddAllArgs() = 0;

  /**
   * @brief Get a vector of all the arguments' flags
   *
   * @return std::vector<std::string>
   */
  std::vector<std::string> GetArgFlags();

 private:
  /**
   * @brief Copies another config into this one
   *
   * @param other the Config to copy from
   */
  void Copy(const Config& other);

  /**
   * @brief Deletes all allocated data in the Config
   *
   */
  void Delete();
};

}  // namespace argparse

#endif
