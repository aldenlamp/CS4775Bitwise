
#include "arg_parser_controller.hpp"

namespace argparse {

ArgParserController::ArgParserController(int argc, const char** argv)
    : parser_(argc, argv) {
  if (argc <= 1) {
    outputHelp();
  }

  var_calc_config_ = new VarCalcConfig();
  ld_score_config_ = new LDScoreConfig();
  grm_calc_config_ = new GRMCalcConfig();
}

ArgParserController::~ArgParserController() {
  delete var_calc_config_;
  delete ld_score_config_;
  delete grm_calc_config_;
}

// MARK: - Getters and Setters

bool ArgParserController::shouldRunVarianceCompAnalysis() {
  return main_config_.heritability_;
}

VarCalcConfig* ArgParserController::getVarCalConfig() {
  return var_calc_config_;
}

bool ArgParserController::shouldRunLDScore() { return main_config_.ld_score_; }

LDScoreConfig* ArgParserController::getLDScoreConfig() {
  return ld_score_config_;
}

bool ArgParserController::shouldRunGRMCalc() { return main_config_.grm_calc_; }

GRMCalcConfig* ArgParserController::getGRMCalcConfig() {
  return grm_calc_config_;
}

bool ArgParserController::isPerf() { return main_config_.performance_gen_; }

// MARK: - Control Functions

void ArgParserController::parseArgs() {
  main_config_.ParseArgs(&parser_);
  var_calc_config_->ParseArgs(&parser_);
  ld_score_config_->ParseArgs(&parser_);
  grm_calc_config_->ParseArgs(&parser_);

  if (main_config_.show_parsed_args_) {
    outputProcessedArgs();
  }

  if (main_config_.show_help_) {
    outputHelp();
  }

  validateArgs();
}

void ArgParserController::outputHelp() {
  if (main_config_.heritability_) {
    var_calc_config_->OutputHelp();
  }
  if (main_config_.ld_score_) {
    ld_score_config_->OutputHelp();
  }
  if (main_config_.grm_calc_) {
    grm_calc_config_->OutputHelp();
  }
  if (!main_config_.ld_score_ && !main_config_.heritability_ &&
      !main_config_.grm_calc_) {
    main_config_.OutputHelp();
  }
  exit(EXIT_SUCCESS);
}

void ArgParserController::outputProcessedArgs() {
  main_config_.ListArgs();
  if (main_config_.heritability_) {
    var_calc_config_->ListArgs();
  }
  if (main_config_.ld_score_) {
    ld_score_config_->ListArgs();
  }

  if (main_config_.grm_calc_) {
    grm_calc_config_->ListArgs();
  }
}

void ArgParserController::validateArgs() {
  if (main_config_.heritability_) {
    var_calc_config_->ValidateRequired();
  }
  if (main_config_.ld_score_) {
    ld_score_config_->ValidateRequired();
  }
  if (main_config_.grm_calc_) {
    grm_calc_config_->ValidateRequired();
  }
}

}  // namespace argparse
