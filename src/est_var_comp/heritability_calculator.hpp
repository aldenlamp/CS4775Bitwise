#ifndef SPARSE_EST_VAR_COMP_HERITABILITY_CALCULATOR_H_
#define SPARSE_EST_VAR_COMP_HERITABILITY_CALCULATOR_H_

#include <string>
#include <vector>

#include "../arg_parse/configs/var_calc_config.hpp"
#include "../file_input/annotation_data.hpp"
#include "../file_input/genotype_data.hpp"
#include "../file_input/phenotype_data.hpp"
#include "../linalg/tensor.hpp"
#include "../metadata/timer.hpp"

namespace estvarcomp {

class HeritabilityCalculator : public fileinput::AnnotationData,
                               public fileinput::PhenotypeData,
                               public fileinput::GenotypeData {
 private:
  metadata::Timer timer;

  // General variables;
  // size_t num_snps_; --> Implemented in GenotypeData
  // size_t num_ind_; --> Implemented in GenotypeData
  // int num_annotations_; --> Implemented in AnnotationData
  size_t num_rand_vects_;
  size_t num_jackknife_;

  size_t snps_per_jn_;
  size_t remainder_jn_snps_;

  linalg::Tensor2<char> raw_data_;

  // Random Vectors
  linalg::Tensor1 rand_vect_sums_;
  linalg::Tensor2<float> rand_vects_;
  linalg::Tensor2<float> rand_vects2_;

  // Genotype Data
  linalg::Tensor4 g_XXtb_;            // dim: J x K x N x B
  linalg::Tensor3 g_composite_XXtb_;  // dim: K x N x B
  linalg::Tensor2<float> g_counts_;   // dim: J x K
  linalg::Tensor1 g_total_counts_;    // dim: K

  // Phenotype Data
  linalg::Tensor2<float> p_ytXXty_;     // dim: J x K
  linalg::Tensor1 p_composite_ytXXty_;  // dim: K

  // Results
  int last_res_ind_;
  int size_of_results_;
  linalg::Tensor2<float> result_values_;     // dim: (J + 1) x (3K + 1)
  linalg::Tensor1 composite_result_values_;  // dim: (3K + 1)
  linalg::Tensor1 result_means_;             // dim: (3K + 1)
  linalg::Tensor1 result_var_;               // dim: (3K + 1)

  // MARK: - Init functions

  void VerifyInputSizes();

  void GenerateRandomVectors(int seed);

  // MARK: - Block creation functions

  void GenerateBlocks();

  // MARK: - Block update functions

  void UpdateBlocks();

  // MARK: - Trace calculator functions

  void CalculateTraces(std::vector<std::vector<float> >& trace_matrix,
                       float* tensor, float* counts);

  // MARK: - Sigma calculator functions

  void AddToResults(size_t j, const std::vector<float>& sigmas);

  void CalcHeritability();

  // MARK: - Jackknife calculations functions

  void CalculateJackMean();

  void CalculateJackVar();

  // MARK: - Ohter

  void OutputResults();

  friend std::ostream& operator<<(std::ostream& os,
                                  const HeritabilityCalculator& calc);

 public:
  HeritabilityCalculator(const argparse::VarCalcConfig* config);

  void RunCalculations();

  metadata::Timer GetTimer();
};

}  // namespace estvarcomp

#endif