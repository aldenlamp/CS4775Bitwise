#ifndef SPARSE_ARG_PARSER_MAIN_CONFIG_H_
#define SPARSE_ARG_PARSER_MAIN_CONFIG_H_

#include <string>

#include "../arg.hpp"
#include "../config.hpp"

namespace argparse {

class MainConfig : public Config {
 protected:
  /**
   * @brief Adds all arguments to the TestConfig
   *
   */
  void AddAllArgs() {
    // Program Control Flow
    AddArg(new Arg<bool>("-var", &heritability_, "EstimateMutliVar",
                         "Run multi variance component using trace estimates"));

    AddArg(
        new Arg<bool>("-ld", &ld_score_, "LD Score", "Run LD Score analysis"));

    AddArg(new Arg<bool>("-grm", &grm_calc_, "GRM Calc", "Run GRM Calculator"));

    // Arg Parser Controller
    AddArg(new Arg<bool>("-h", &show_help_, "Help", "Show Help"));

    AddArg(new Arg<bool>("-l", &show_parsed_args_, "ListVals",
                         "List parsed argument values"));

    // Other
    AddArg(new Arg<bool>("-perf", &performance_gen_, "PerformanceGen",
                         "Generate outputs for performace tester program"));
  }

 public:
  /**
   * @brief Construct a new Config object
   *
   * @param argc the number of args to use
   * @param argv a pointer to a list of args to use
   */
  MainConfig() : Config() {
    config_name_ = "Main Config";
    AddAllArgs();
  }

  // Arg Parser Controller
  bool show_help_;
  bool show_parsed_args_;

  // Program Control Flow
  bool heritability_;
  bool ld_score_;
  bool grm_calc_;

  // Other
  bool performance_gen_;
};

}  // namespace argparse

#endif
