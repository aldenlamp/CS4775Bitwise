#ifndef SPARSE_ARG_PARSER_LD_SCORE_CONFIG_H_
#define SPARSE_ARG_PARSER_LD_SCORE_CONFIG_H_

#include <string>

#include "../arg.hpp"
#include "../config.hpp"
#include "annotation_file_config.hpp"
#include "genotype_file_config.hpp"
#include "output_file_config.hpp"

namespace argparse {

class LDScoreConfig : public GenotypeFileConfig,
                      public AnnotationFileConfig,
                      public OutputFileConfig {
 private:
  /**
   * @brief Adds all arguments to the TestConfig
   *
   */
  void AddAllArgs() {
    AddArg(new Arg<int>("-w", &window_, "Window",
                        "The size of the window to use", false, true, "-1"));
  }

 public:
  /**
   * @brief Construct a new Config object
   *
   * @param argc the number of args to use
   * @param argv a pointer to a list of args to use
   */
  LDScoreConfig() {
    config_name_ = "LD Score Config";
    AddAllArgs();
  }

  int window_;
};

}  // namespace argparse

#endif
