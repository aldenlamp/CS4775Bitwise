#ifndef SPARSE_GRM_CALC_GRM_CALCULATOR_H_
#define SPARSE_GRM_CALC_GRM_CALCULATOR_H_

#include <vector>

#include "../arg_parse/configs/grm_calc_config.hpp"
#include "../file_input/genotype_data.hpp"
#include "../metadata/timer.hpp"

namespace grmcalc {

class GRMCalculator : public fileinput::GenotypeData {
 private:

  // TODO: Compressed Row
  std::vector<std::vector<std::tuple<size_t, char>>> ind_snp_;
  std::vector<std::vector<std::tuple<size_t, char>>> snp_ind_;
  float* snp_means_;
  std::tuple<float, float, float>*
      snp_std_;  // represents the inverse std squared

  // For standardization
  float* m_sig_x_;
  float m_sig_m_;

  float** results_;

  metadata::Timer timer_;

  void Test();

  void InputData();

  void CalculateStandardizationVects();

 public:
  GRMCalculator(argparse::GRMCalcConfig* config);
  ~GRMCalculator();

  void RunCalculator();

  friend std::ostream& operator<<(std::ostream& os, GRMCalculator& grm_calc);
};

}  // namespace grmcalc

#endif