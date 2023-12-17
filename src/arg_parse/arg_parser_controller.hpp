#ifndef SPARSE_ARG_PARSER_ARG_PARSER_CONTROLLER_H_
#define SPARSE_ARG_PARSER_ARG_PARSER_CONTROLLER_H_

#include <string>
#include <vector>

#include "arg_parser.hpp"
#include "configs/grm_calc_config.hpp"
#include "configs/ld_score_config.hpp"
#include "configs/main_config.hpp"
#include "configs/var_calc_config.hpp"

namespace argparse {

class ArgParserController {
 public:
  // MARK: - Constructors

  // Note: This class should never need to be copied or assigned
  ArgParserController(int argc, const char** argv);
  ~ArgParserController();

  // MARK: - Getters and Setters

  bool shouldRunVarianceCompAnalysis();
  VarCalcConfig* getVarCalConfig();

  bool shouldRunLDScore();
  LDScoreConfig* getLDScoreConfig();

  bool shouldRunGRMCalc();
  GRMCalcConfig* getGRMCalcConfig();

  bool isPerf();

  // MARK: - Control Functions

  void parseArgs();

 private:
  ArgParser parser_;
  MainConfig main_config_;
  VarCalcConfig* var_calc_config_;
  LDScoreConfig* ld_score_config_;
  GRMCalcConfig* grm_calc_config_;

  void outputHelp();

  void outputProcessedArgs();

  void validateArgs();
};

}  // namespace argparse

#endif