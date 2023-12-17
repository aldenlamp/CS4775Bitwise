#include <cmath>
#include <cstdlib>
#include <filesystem>  // TODO: Update project to use filesystem
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "src/arg_parse/arg_parser_controller.hpp"
#include "src/est_var_comp/heritability_calculator.hpp"
#include "src/grm_calc/grm_calculator.hpp"
#include "src/ldscore/ldscore_calculator.hpp"
#include "src/metadata/timer.hpp"

using argparse::ArgParserController;
using argparse::GRMCalcConfig;
using argparse::LDScoreConfig;
// using argparse::MainConfig;
using argparse::VarCalcConfig;

using estvarcomp::HeritabilityCalculator;
using grmcalc::GRMCalculator;
using ldscore::LDScoreCalculator;

using metadata::Timer;

using std::ofstream;
using std::setw;
using std::string;
using std::string_view;
using std::stringstream;
using std::vector;

int main(int argc, const char** argv) {
  Timer main_timer;
  main_timer.Start("Main");
  Timer timer;

  ArgParserController arg_controller(argc, argv);
  arg_controller.parseArgs();

  // MARK: - Estimated Heritability Calculator

  if (arg_controller.shouldRunVarianceCompAnalysis()) {
    timer.Start("MultiVar");

    // VarCalcConfig var_config(parser, argparse::VarCalcType::EstimateMulti);
    VarCalcConfig* var_config = arg_controller.getVarCalConfig();

    HeritabilityCalculator calc(var_config);
    calc.RunCalculations();

    std::cout << calc << std::endl;

    if (arg_controller.isPerf()) {
      calc.GetTimer().OutputPartition(std::cout, "Main:MultiVar");
    }

    timer.End("MultiVar");
  }

  // MARK: - LD Score

  if (arg_controller.shouldRunLDScore()) {
    // LdScoreConfig ld_config(parser);
    // std::cout << "LD SCORE" << std::endl;

    LDScoreConfig* ld_config = arg_controller.getLDScoreConfig();

    LDScoreCalculator calc(ld_config);
    calc.ProcessLDScore();

    if (ld_config->DidSetOuput()) {
      // TODO: Convert more file managing system to use filesystem
      std::filesystem::path out_file_path(ld_config->output_file_);

      if (out_file_path.has_parent_path() &&
          !std::filesystem::exists(out_file_path.parent_path())) {
        std::filesystem::create_directories(out_file_path.parent_path());
      }

      ofstream output_file;
      output_file.open(out_file_path);
      output_file << calc << std::endl;
      output_file.close();
    } else {
      std::cout << calc << std::endl;
    }
  }

  // MARK: - GRM Calc

  if (arg_controller.shouldRunGRMCalc()) {
    GRMCalcConfig* grm_calc_config = arg_controller.getGRMCalcConfig();
    GRMCalculator grm_calc(grm_calc_config);
    grm_calc.RunCalculator();

    if (grm_calc_config->DidSetOuput()) {
      std::filesystem::path out_file_path(grm_calc_config->output_file_);

      if (out_file_path.has_parent_path() &&
          !std::filesystem::exists(out_file_path.parent_path())) {
        std::filesystem::create_directories(out_file_path.parent_path());
      }

      ofstream output_file;
      output_file.open(out_file_path);
      output_file << grm_calc << std::endl;
      output_file.close();
    } else {
      std::cout << grm_calc << std::endl;
    }
  }

  // Output
  main_timer.End("Main");
  if (arg_controller.isPerf()) {
    main_timer.OutputPartition(std::cout, "");
    timer.OutputPartition(std::cout, "Main");
  }

  std::cout << "TOTAL TIME: " << main_timer.GetTime("Main") << std::endl;

  return EXIT_SUCCESS;
}