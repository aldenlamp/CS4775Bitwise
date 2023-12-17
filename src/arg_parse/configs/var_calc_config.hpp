#ifndef SPARSE_ARG_PARSER_VAR_CALC_CONFIG_H_
#define SPARSE_ARG_PARSER_VAR_CALC_CONFIG_H_

#include <string>

#include "../arg.hpp"
#include "../config.hpp"
#include "annotation_file_config.hpp"
#include "genotype_file_config.hpp"
#include "phenotype_file_config.hpp"

namespace argparse {

class VarCalcConfig : public GenotypeFileConfig,
                      public PhenotypeFileConfig,
                      public AnnotationFileConfig {
 private:
  /**
   * @brief Adds all arguments to the TestConfig
   *
   */
  void AddAllArgs() {
    AddArg(new Arg<int>("-j", &num_jackknife_, "NumJackknife",
                        "The number of jackknife blocks to use", false, true,
                        "100"));

    AddArg(new Arg<int>("-b", &num_rand_vects_, "NumRandVect",
                        "The number of random vectors to use", false, true,
                        "20"));

    AddArg(new Arg<int>("-s", &seed_, "Seed", "The seed for the random vectors",
                        false, true, "0"));
  }

 public:
  /**
   * @brief Construct a new Config object
   *
   * @param argc the number of args to use
   * @param argv a pointer to a list of args to use
   */
  VarCalcConfig() {
    config_name_ = "Heritability Calculator Config";
    AddAllArgs();
    // ParseArgs();
    // HandleOutputHelp();
    // ValidateArgs();
    // HandleListArgs();
  }

  // void Validate(VarCalcType config_type) {
  //   switch (config_type) {
  //     case EstimateMulti:
  //       Config::Validate(&annotation_file_);
  //     case ExactMulti:
  //       Config::Validate(&annotation_file_);
  //     default:
  //       break;
  //   }
  // }

  int num_rand_vects_;
  int num_jackknife_;

  // VarCalcType var_calc_type_;

  int seed_;
};

}  // namespace argparse

#endif
