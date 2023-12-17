#include "heritability_calculator.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

#include "../linalg/linear_system.hpp"

using argparse::VarCalcConfig;
using linalg::LinearSystemSolver;
using linalg::Tensor1;
using linalg::Tensor2;
using linalg::Tensor3;
using std::ceil;
using std::default_random_engine;
using std::ifstream;
using std::normal_distribution;
using std::ostream;
using std::pow;
using std::setw;
using std::sqrt;
using std::string;
using std::vector;

namespace estvarcomp {

// MARK: - Init functions

HeritabilityCalculator::HeritabilityCalculator(const VarCalcConfig* config)
    : AnnotationData(config), PhenotypeData(config), GenotypeData(config) {
  timer.Start("HCalc:Init");
  num_rand_vects_ = config->num_rand_vects_;
  num_jackknife_ = config->num_jackknife_;

  VerifyInputSizes();

  rand_vect_sums_.Init(num_rand_vects_, true);
  rand_vects_.Init(num_ind_, num_rand_vects_);
  rand_vects2_.Init(num_ind_, num_rand_vects_);

  GenerateRandomVectors(config->seed_);

  g_XXtb_.Init(num_jackknife_, num_annotations_, num_ind_, num_rand_vects_,
               true);
  g_composite_XXtb_.Init(num_annotations_, num_ind_, num_rand_vects_, true);
  g_counts_.Init(num_jackknife_, num_annotations_, true);
  g_total_counts_.Init(num_annotations_, true);

  p_ytXXty_.Init(num_jackknife_, num_annotations_, true);
  p_composite_ytXXty_.Init(num_annotations_, true);

  size_of_results_ = num_annotations_ * 3 + 1;
  last_res_ind_ = num_jackknife_ * size_of_results_;
  result_values_.Init(num_jackknife_, size_of_results_);
  composite_result_values_.Init(size_of_results_);
  result_means_.Init(size_of_results_, true);
  result_var_.Init(size_of_results_, true);

  snps_per_jn_ = ceil(float(num_snps_) / num_jackknife_);
  remainder_jn_snps_ = num_snps_ - (snps_per_jn_ * (num_jackknife_ - 1));

  std::cout << "Num snps: " << num_snps_ << std::endl;
  std::cout << "Num individuals: " << num_ind_ << std::endl;
  std::cout << "Num jackknife blocks: " << num_jackknife_ << std::endl;
  std::cout << "Num snps per block: " << snps_per_jn_ << std::endl;
  std::cout << "Remainder snps per block: " << remainder_jn_snps_ << std::endl;
  std::cout << "Num annotations: " << static_cast<int>(num_annotations_)
            << std::endl;
  std::cout << "Num random vectors: " << num_rand_vects_ << std::endl;
  timer.End("HCalc:Init");
}

void HeritabilityCalculator::VerifyInputSizes() {
  if (num_ind_ != pheno_num_ind_) {
    std::cout << "Num snps in fam and pheno file do not match: " << num_ind_
              << " vs " << pheno_num_ind_ << std::endl;
    exit(EXIT_FAILURE);
  }

  if (annot_num_snps_ != -1 && num_snps_ != annot_num_snps_) {
    std::cout << "Num snps in bim and annotation file do not match: "
              << num_snps_ << " vs " << annot_num_snps_ << std::endl;
    exit(EXIT_FAILURE);
  }
}

void HeritabilityCalculator::GenerateRandomVectors(int seed) {
  timer.Start("HCalc:Init:GenB");
  default_random_engine generator(seed);
  normal_distribution<float> distribution(0, 1);
  for (size_t i = 0; i < num_ind_; ++i) {
    for (size_t b = 0; b < num_rand_vects_; ++b) {
      float r = distribution(generator);
      rand_vect_sums_.Add(b, r);
      rand_vects_.Set(i, b, r);
      rand_vects2_.Set(i, b, r * 2);
    }
  }
  timer.End("HCalc:Init:GenB");
}

// MARK: - Block creation functions

void HeritabilityCalculator::GenerateBlocks() {
  timer.Start("HCalc:Calc:GenBlocks");
  size_t snp_ind = 0;

  // Preallocated arrays (they get reused to save allocation time)
  Tensor2<float> g_standardize_XXtb;  // dim: J x K x B
  g_standardize_XXtb.Init(num_annotations_, num_rand_vects_);

  Tensor1 minor_index;
  minor_index.Init(num_ind_);

  Tensor1 hetero_index;
  hetero_index.Init(num_ind_);

  Tensor1 rand_sum;
  rand_sum.Init(num_rand_vects_);

  Tensor1 rand_sum2;
  rand_sum2.Init(num_rand_vects_);

  size_t pheno_loop_it = 0;
  for (size_t j = 0; j < num_jackknife_; ++j) {
    size_t block_size = snps_per_jn_;
    if (j == num_jackknife_ - 1) block_size = remainder_jn_snps_;
    std::cout << "Calc block: " << j << "\tSize: " << block_size << std::endl;

    g_standardize_XXtb.Zero();

    // Loop through all the snps in current jackknife block
    for (size_t s = 0; s < block_size; ++s) {
      size_t num_minor = 0;
      size_t num_hetero = 0;

      size_t byte_ind = 0;
      size_t bit_ind = 0;

      unsigned char byte = GetNextSnpByte();

      float pheno_mult = 0;

      rand_sum.Zero();
      rand_sum2.Zero();

      // First multiplication Xtz
      for (size_t ind = 0; ind < num_ind_; ++ind) {
        char plink_value = byte & 3;

        if (plink_value == 0) {
          for (size_t b = 0; b < num_rand_vects_; ++b) {
            rand_sum2.Add(b, rand_vects2_.Get(ind, b));
          }

          minor_index.Set(num_minor, ind);
          ++num_minor;

          pheno_mult += GetDoubledPhenotype(ind);

        } else if (plink_value == 2) {
          for (size_t b = 0; b < num_rand_vects_; ++b) {
            rand_sum.Add(b, rand_vects_.Get(ind, b));
          }

          hetero_index.Set(num_hetero, ind);
          ++num_hetero;

          pheno_mult += GetPhenotype(ind);
        }

        byte >>= 2;
        ++bit_ind;
        if (bit_ind == 4 && ind != num_ind_ - 1) {
          bit_ind = 0;
          byte = GetNextSnpByte();
        }
      }
      unsigned char annot = GetAnnotation(snp_ind);

      g_counts_.Add(j, annot, 1);
      g_total_counts_.Add(annot, 1);

      rand_sum.Combine(rand_sum2.f);

      // Calculate mean and sd of snp
      float mean = float((2 * num_minor) + num_hetero) / num_ind_;
      float inv_sd_sqr =
          num_ind_ /
          (pow(2 - mean, 2) * num_minor + pow(1 - mean, 2) * num_hetero +
           pow(0 - mean, 2) * (num_ind_ - num_minor - num_hetero));

      // Calculate and standardize Xty
      float Xty =
          pow((pheno_mult - (mean * pheno_sum_)) * sqrt(inv_sd_sqr), 2.0);
      p_ytXXty_.f[pheno_loop_it + annot] += Xty;
      p_composite_ytXXty_.f[annot] += Xty;

      // Standardize First Multiplication (Xtz)
      for (size_t b = 0; b < num_rand_vects_; ++b) {
        rand_sum.Set(
            b, (rand_sum.Get(b) - mean * rand_vect_sums_.Get(b)) * inv_sd_sqr);
        rand_sum2.Set(b, rand_sum.Get(b) * 2);
      }

      // Second multiplication X(Xtz)
      for (size_t m_ind = 0; m_ind < num_minor; ++m_ind) {
        size_t min_ind = minor_index.Get(m_ind);
        for (size_t b = 0; b < num_rand_vects_; ++b) {
          g_XXtb_.Add(j, annot, min_ind, b, rand_sum2.Get(b));
        }
      }

      for (size_t h_ind = 0; h_ind < num_hetero; ++h_ind) {
        size_t het_ind = hetero_index.Get(h_ind);
        for (size_t b = 0; b < num_rand_vects_; ++b) {
          g_XXtb_.Add(j, annot, het_ind, b, rand_sum.Get(b));
        }
      }

      for (size_t b = 0; b < num_rand_vects_; ++b) {
        g_standardize_XXtb.Add(annot, b, mean * rand_sum.Get(b));
      }

      ++snp_ind;
    }

    // Standardize and add to composite

    const size_t jack_block_size =
        num_annotations_ * num_ind_ * num_rand_vects_;
    const size_t curr_g_XXtb_loop_it = jack_block_size * j;

    size_t loop_it = 0;
    size_t g_loop_it = num_annotations_ * num_ind_ * num_rand_vects_ * j;

    for (size_t k = 0; k < num_annotations_; ++k) {
      size_t std_start = k * num_rand_vects_;
      for (size_t i = 0; i < num_ind_; ++i) {
        for (size_t b = 0; b < num_rand_vects_; ++b) {
          g_XXtb_.f[g_loop_it] -= g_standardize_XXtb.f[std_start + b];
          g_composite_XXtb_.f[loop_it] += g_XXtb_.f[g_loop_it];

          ++loop_it;
          ++g_loop_it;
        }
      }
    }

    pheno_loop_it += num_annotations_;
  }
  // raw_data_.Delete();
  CloseGenotypeData();
  timer.End("HCalc:Calc:GenBlocks");
}

// MARK: - Block update functions

void HeritabilityCalculator::UpdateBlocks() {
  timer.Start("HCalc:Calc:UpdateBlocks");
  if (num_jackknife_ == 1) return;

  // Update the XXtb data
  const size_t comp_it_threshold =
      num_annotations_ * num_rand_vects_ * num_ind_;
  size_t comp_it = 0;
  for (size_t loop_it = 0; loop_it < comp_it_threshold * num_jackknife_;
       ++loop_it) {
    g_XXtb_.f[loop_it] = g_composite_XXtb_.f[comp_it] - g_XXtb_.f[loop_it];
    ++comp_it;
    if (comp_it == comp_it_threshold) {
      comp_it = 0;
    }
  }

  // Update the snp count data and ytXXty data
  size_t k = 0;
  for (size_t loop_it = 0; loop_it < num_jackknife_ * num_annotations_;
       ++loop_it) {
    g_counts_.f[loop_it] = g_total_counts_.f[k] - g_counts_.f[loop_it];
    p_ytXXty_.f[loop_it] = (p_composite_ytXXty_.f[k] - p_ytXXty_.f[loop_it]) /
                           g_counts_.f[loop_it];
    ++k;
    if (k == num_annotations_) {
      k = 0;
    }
  }
  for (size_t k = 0; k < num_annotations_; ++k) {
    p_composite_ytXXty_.f[k] /= g_total_counts_.f[k];
  }
  timer.End("HCalc:Calc:UpdateBlocks");
}

// MARK: - Trace calculator functions

void HeritabilityCalculator::CalculateTraces(
    vector<vector<float>>& trace_matrix, float* tensor, float* counts) {
  for (size_t k1 = 0; k1 < num_annotations_; ++k1) {
    for (size_t k2 = 0; k2 < num_annotations_; ++k2) {
      size_t k1_start = k1 * num_rand_vects_ * num_ind_;
      size_t k2_start = k2 * num_rand_vects_ * num_ind_;

      float sum = 0;
      for (size_t loop_it = 0; loop_it < num_ind_ * num_rand_vects_;
           ++loop_it) {
        sum += tensor[k1_start + loop_it] * tensor[k2_start + loop_it];
      }
      sum /= counts[k1];
      sum /= counts[k2];
      sum /= num_rand_vects_;
      trace_matrix[k1][k2] = sum;
      trace_matrix[k2][k1] = sum;
    }
  }
}

// MARK: - Sigma calculator functions

void HeritabilityCalculator::AddToResults(size_t j,
                                          const vector<float>& sigmas) {
  size_t res_start = j * size_of_results_;

  float* sig_res = result_values_.f + res_start;
  if (j == num_jackknife_) {
    sig_res = composite_result_values_.f;
  }
  float* h_res = sig_res + num_annotations_ + 1;
  float* e_res = h_res + num_annotations_;

  float sig_sum = 0;
  for (size_t i = 0; i < num_annotations_ + 1; ++i) {
    sig_sum += sigmas[i];
    sig_res[i] = sigmas[i];
  }

  float h_snp = (sig_sum - sigmas[num_annotations_]) / sig_sum;

  float m = 0;
  size_t m_start = j * num_annotations_;
  if (j != num_jackknife_) {
    for (size_t i = m_start; i < (j + 1) * num_annotations_; ++i) {
      m += g_counts_.f[i];
    }
  } else {
    m = num_snps_;
    m_start = 0;
  }

  for (size_t i = 0; i < num_annotations_; ++i) {
    float h = sigmas[i] / sig_sum;
    h_res[i] = h;

    float annot_count = 0;
    if (j != num_jackknife_) {
      annot_count = g_counts_.f[m_start + i];
    } else {
      annot_count = g_total_counts_.f[m_start + i];
    }

    e_res[i] = (h / h_snp) / (annot_count / m);
  }
}

void HeritabilityCalculator::CalcHeritability() {
  timer.Start("HCalc:Calc:CalcHer");
  vector<vector<float>> trace_matrix(
      num_annotations_ + 1, vector<float>(num_annotations_ + 1, num_ind_));

  vector<float> phenotypes(num_annotations_ + 1, yty_);

  for (size_t j = 0; j < num_jackknife_; ++j) {
    size_t count_start = j * num_annotations_;
    size_t tensor_start = j * num_annotations_ * num_rand_vects_ * num_ind_;

    CalculateTraces(trace_matrix, g_XXtb_.f + tensor_start,
                    g_counts_.f + count_start);

    for (size_t i = 0; i < num_annotations_; ++i) {
      phenotypes[i] = p_ytXXty_.Get(j, i);
    }

    vector<float> sigmas = LinearSystemSolver(trace_matrix, phenotypes);
    AddToResults(j, sigmas);
  }

  // Calculating using the composite data
  CalculateTraces(trace_matrix, g_composite_XXtb_.f, g_total_counts_.f);
  for (size_t i = 0; i < num_annotations_; ++i) {
    phenotypes[i] = p_composite_ytXXty_.Get(i);
  }

  vector<float> sigmas = LinearSystemSolver(trace_matrix, phenotypes);

  AddToResults(num_jackknife_, sigmas);

  timer.End("HCalc:Calc:CalcHer");
}

// MARK: - Jackknife calculations functions

void HeritabilityCalculator::CalculateJackMean() {
  timer.Start("HCalc:Calc:JackMean");
  float* sig_mean = result_means_.f;
  float* h_mean = sig_mean + num_annotations_ + 1;
  float* e_mean = h_mean + num_annotations_;

  Tensor1 total_counts;
  total_counts.Init(num_annotations_, true);

  for (size_t j = 0; j < num_jackknife_; ++j) {
    float* sig_curr = result_values_.f + (j * size_of_results_);
    float* h_curr = sig_curr + num_annotations_ + 1;
    float* e_curr = h_curr + num_annotations_;

    float* counts = g_counts_.f + (j * num_annotations_);

    for (size_t k = 0; k < num_annotations_; ++k) {
      sig_mean[k] += (sig_curr[k] * counts[k]);
      h_mean[k] += (h_curr[k] * counts[k]);
      e_mean[k] += (e_curr[k] * counts[k]);
      total_counts.f[k] += counts[k];
    }

    sig_mean[num_annotations_] += sig_curr[num_annotations_];
  }

  for (size_t k = 0; k < num_annotations_; ++k) {
    sig_mean[k] /= total_counts.f[k];
    h_mean[k] /= total_counts.f[k];
    e_mean[k] /= total_counts.f[k];
  }
  sig_mean[num_annotations_] /= (static_cast<float>(num_jackknife_));
  timer.End("HCalc:Calc:JackMean");
}

void HeritabilityCalculator::CalculateJackVar() {
  timer.Start("HCalc:Calc:JackVar");
  float* sig_mean = result_means_.f;
  float* h_mean = sig_mean + num_annotations_ + 1;
  float* e_mean = h_mean + num_annotations_;

  float* sig_var = result_var_.f;
  float* h_var = sig_var + num_annotations_ + 1;
  float* e_var = h_var + num_annotations_;

  for (size_t j = 0; j < num_jackknife_; ++j) {
    float* sig_curr = result_values_.f + (j * size_of_results_);
    float* h_curr = sig_curr + num_annotations_ + 1;
    float* e_curr = h_curr + num_annotations_;

    for (size_t k = 0; k < num_annotations_; ++k) {
      sig_var[k] += pow((sig_curr[k] - sig_mean[k]), 2.0);
      h_var[k] += pow((h_curr[k] - h_mean[k]), 2.0);
      e_var[k] += pow((e_curr[k] - e_mean[k]), 2.0);
    }
    sig_var[num_annotations_] +=
        pow((sig_curr[num_annotations_] - sig_mean[num_annotations_]), 2.0);
  }

  float divisor = float(num_jackknife_ - 1) / num_jackknife_;

  for (size_t k = 0; k < num_annotations_; ++k) {
    sig_var[k] /= divisor;
    e_var[k] /= divisor;
    h_var[k] /= divisor;
  }
  sig_var[num_annotations_] /= divisor;
  timer.End("HCalc:Calc:JackVar");
}

// MARK: - Other

void HeritabilityCalculator::OutputResults() {}

ostream& operator<<(ostream& os, const HeritabilityCalculator& calc) {
  constexpr int w1 = 25;
  constexpr int w2 = 15;
  constexpr int w3 = 5;
  constexpr int w4 = 15;
  constexpr int w5 = 10;
  constexpr int w6 = 15;

  os << "Variance Component Analysis results: " << std::endl;
  os << "VARIANCES: " << std::endl;
  size_t loop_itr = 0;
  for (size_t k = 0; k < calc.num_annotations_; ++k) {
    os << setw(w1) << std::left << "Sigma^2_" + std::to_string(k) + ": "
       << setw(w2) << std::left << calc.composite_result_values_.f[loop_itr]
       << setw(w5) << std::left << "JN avg:" << setw(w6) << std::left
       << calc.result_means_.f[loop_itr] << setw(w3) << std::left
       << "SE: " << setw(w4) << std::left << calc.result_var_.f[loop_itr]
       << std::endl;
    ++loop_itr;
  }
  os << setw(w1) << std::left << "Sigma^2_e: " << setw(w2) << std::left
     << calc.composite_result_values_.f[loop_itr] << setw(w5) << std::left
     << "JN avg:" << setw(w6) << std::left << calc.result_means_.f[loop_itr]
     << setw(w3) << std::left << "SE: " << setw(w4) << std::left
     << calc.result_var_.f[loop_itr] << std::endl;

  ++loop_itr;

  os << std::endl << "HERITABILITIES:" << std::endl;
  for (size_t k = 0; k < calc.num_annotations_; ++k) {
    os << setw(w1) << std::left << "h^2 of bin " + std::to_string(k) + ": "
       << setw(w2) << std::left << calc.composite_result_values_.f[loop_itr]
       << setw(w5) << std::left << "JN avg:" << setw(w6) << std::left
       << calc.result_means_.f[loop_itr] << setw(w3) << std::left
       << "SE: " << setw(w4) << std::left << calc.result_var_.f[loop_itr]
       << std::endl;
    ++loop_itr;
  }

  os << std::endl << "ENRICHMENTS:" << std::endl;
  for (size_t k = 0; k < calc.num_annotations_; ++k) {
    os << setw(w1) << std::left
       << "Enrichment of bin " + std::to_string(k) + ": " << setw(w2)
       << std::left << calc.composite_result_values_.f[loop_itr] << setw(w5)
       << std::left << "JN avg:" << setw(w6) << std::left
       << calc.result_means_.f[loop_itr] << setw(w3) << std::left
       << "SE: " << setw(w4) << std::left << calc.result_var_.f[loop_itr]
       << std::endl;
    ++loop_itr;
  }
  return os;
}

metadata::Timer HeritabilityCalculator::GetTimer() { return timer; }

void HeritabilityCalculator::RunCalculations() {
  timer.Start("HCalc:Calc");
  GenerateBlocks();
  UpdateBlocks();
  CalcHeritability();
  CalculateJackMean();
  CalculateJackVar();
  timer.Start("HCalc:Calc");
}

}  // namespace estvarcomp
