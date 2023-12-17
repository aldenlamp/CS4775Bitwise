#ifndef SPARSE_ARG_PARSER_CONFIGS_OUTPUT_FILE_CONFIG_H_
#define SPARSE_ARG_PARSER_CONFIGS_OUTPUT_FILE_CONFIG_H_

#include <string>

#include "../arg.hpp"
#include "../config.hpp"

namespace argparse {

class OutputFileConfig : public virtual Config {
 private:
  Arg<std::string>* outfile_arg_;

 protected:
  /**
   * @brief Adds all arguments to the TestConfig
   *
   */
  void AddAllArgs() {
    outfile_arg_ = new Arg<std::string>("-o", &output_file_, "OutputFile",
                                        "The path to the output file", false);
    AddArg(outfile_arg_);
  }

 public:
  OutputFileConfig() {
    config_name_ = "Output File Config";
    AddAllArgs();
  }

  bool DidSetOuput() const { return outfile_arg_->DidSet(); }

  std::string output_file_;
};

}  // namespace argparse

#endif
