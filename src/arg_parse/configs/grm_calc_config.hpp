#ifndef SPARSE_ARG_PARSER_GRM_CALC_CONFIG_H_
#define SPARSE_ARG_PARSER_GRM_CALC_CONFIG_H_

#include <string>

#include "../arg.hpp"
#include "../config.hpp"
#include "genotype_file_config.hpp"
#include "output_file_config.hpp"

namespace argparse {

class GRMCalcConfig : public GenotypeFileConfig, public OutputFileConfig {
 private:
  /**
   * @brief Adds all arguments to the TestConfig
   *
   */
  void AddAllArgs() {}

 public:
  /**
   * @brief Construct a new Config object
   *
   * @param argc the number of args to use
   * @param argv a pointer to a list of args to use
   */
  GRMCalcConfig() {
    config_name_ = "GRM Calc Config";
    AddAllArgs();
  }
};

}  // namespace argparse

#endif
