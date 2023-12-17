
#include "grm_calculator.hpp"

#include <cmath>

using fileinput::GenotypeData;
using std::tuple;
using std::vector;

namespace grmcalc {

GRMCalculator::GRMCalculator(argparse::GRMCalcConfig* config)
    : GenotypeData(config) {
  ind_snp_ = vector<vector<tuple<size_t, char>>>(num_ind_,
                                                 vector<tuple<size_t, char>>());
  snp_ind_ = vector<vector<tuple<size_t, char>>>(num_snps_,
                                                 vector<tuple<size_t, char>>());

  snp_means_ = new float[num_snps_];
  snp_std_ = new tuple<float, float, float>[num_snps_];

  m_sig_x_ = new float[num_ind_];

  results_ = new float*[num_ind_];
  for (size_t i = 0; i < num_ind_; ++i) {
    m_sig_x_[i] = 0;
    results_[i] = new float[i + 1];
    for (size_t j = 0; j < i + 1; ++j) {
      results_[i][j] = 0;
    }
  }
  InputData();
}

GRMCalculator::~GRMCalculator() {
  std::cout << timer_ << std::endl;
  delete[] snp_means_;
  delete[] m_sig_x_;
  delete[] snp_std_;
  for (size_t i = 0; i < num_ind_; ++i) {
    if (results_[i] != nullptr) delete[] results_[i];
  }
  delete[] results_;
}

void GRMCalculator::InputData() {
  timer_.Start("InputData");

  size_t ind_num = 0;
  size_t snp_num = 0;
  unsigned long num_hetero = 0;
  unsigned long num_homo = 0;
  unsigned long num_missing = 0;

  while (ind_num < num_ind_ && snp_num < num_snps_) {
    unsigned char next_byte = GetNextSnpByte();
    for (int i = 0; i < 4; ++i) {
      char plink_val = next_byte & 3;

      if (plink_val == 1) {
        ++num_missing;
      } else if (plink_val < 3) {
        if (plink_val == 0) {
          ++num_homo;
        } else if (plink_val == 2) {
          ++num_hetero;
        }

        ind_snp_[ind_num].push_back({snp_num, 1 - (plink_val / 2)});
        snp_ind_[snp_num].push_back({ind_num, 1 - (plink_val / 2)});
      }

      ++ind_num;
      if (ind_num == num_ind_) {
        unsigned long sum = num_hetero + (num_homo * 2);
        snp_means_[snp_num] = sum / float(num_ind_);

        float exp_squared = pow(snp_means_[snp_num], 2.0);
        float four_homo = 4 * num_homo;

        float std_dev_numerator =
            exp_squared * (num_ind_ - num_missing) -
            snp_means_[snp_num] * (four_homo + 2 * num_hetero) + four_homo +
            num_hetero;

        float std = ((num_ind_ - num_missing) / std_dev_numerator);
        snp_std_[snp_num] = {std, 2 * std, 4 * std};

        ++snp_num;
        ind_num = 0;
        num_hetero = 0;
        num_homo = 0;
        num_missing = 0;
      }
      next_byte >>= 2;
    }
  }

  // Print statements left for debugging

  // std::cout << "stds: " << std::endl;
  // for (size_t snp_num = 0; snp_num < num_snps_; ++snp_num) {
  //   std::cout << std::get<0>(snp_std_[snp_num]) << " ";
  // }
  // std::cout << std::endl;

  // for (size_t i = 0; i < num_ind_; ++i) {
  //   for (size_t j = 0; j < ind_snp_[i].size(); ++j) {
  //     std::cout << "(" << std::get<0>(ind_snp_[i][j]) << ", "
  //               << static_cast<int>(std::get<1>(snp_ind_[i][j])) << ") ";
  //   }
  //   std::cout << std::endl;
  // }
  // for (size_t snp_num = 0; snp_num < num_snps_; ++snp_num) {
  //   for (size_t i = 0; i < snp_ind_[snp_num].size(); ++i) {
  //     std::cout << "(" << std::get<0>(snp_ind_[snp_num][i]) << ", "
  //               << static_cast<int>(std::get<1>(snp_ind_[snp_num][i])) << ")
  //               ";
  //   }
  //   std::cout << std::endl;
  // }
  timer_.End("InputData");
}

void GRMCalculator::CalculateStandardizationVects() {
  timer_.Start("Run::CalcStdVec");
  // Calculate m_sig_x

  for (size_t i = 0; i < num_ind_; ++i) {
    m_sig_x_[i] = 0;
  }

  for (size_t snp_num = 0; snp_num < num_snps_; ++snp_num) {
    float factor = snp_means_[snp_num] * std::get<0>(snp_std_[snp_num]);

    for (size_t i = 0; i < snp_ind_[snp_num].size(); ++i) {
      m_sig_x_[std::get<0>(snp_ind_[snp_num][i])] +=
          factor * (1 + std::get<1>(snp_ind_[snp_num][i]));
    }
  }

  // std::cout << "m_sig_x: " << std::endl;
  // for (size_t i = 0; i < num_ind_; ++i) {
  //   std::cout << m_sig_x_[i] << " ";
  // }
  // std::cout << std::endl;

  // Calculate m_sig_m
  m_sig_m_ = 0;
  for (size_t i = 0; i < num_snps_; ++i) {
    m_sig_m_ += pow(snp_means_[i], 2.0) * std::get<0>(snp_std_[i]);
  }
  // std::cout << "m_sig_m: " << m_sig_m_ << std::endl;

  timer_.End("Run::CalcStdVec");
}

void GRMCalculator::RunCalculator() {
  timer_.Start("Run");
  CalculateStandardizationVects();

  for (size_t i = 0; i < num_ind_; ++i) {
    for (size_t ind_snp_index = 0; ind_snp_index < ind_snp_[i].size();
         ++ind_snp_index) {
      size_t snp_num = std::get<0>(ind_snp_[i][ind_snp_index]);
      char ind_val = std::get<1>(ind_snp_[i][ind_snp_index]);

      for (size_t snp_ind_index = 0; snp_ind_index < snp_ind_[snp_num].size();
           ++snp_ind_index) {
        size_t j = std::get<0>(snp_ind_[snp_num][snp_ind_index]);
        if (j > i) break;

        char final_val =
            ind_val + std::get<1>(snp_ind_[snp_num][snp_ind_index]);

        switch (final_val) {
          case 0:
            results_[i][j] += std::get<0>(snp_std_[snp_num]);
            break;
          case 1:
            results_[i][j] += std::get<1>(snp_std_[snp_num]);
            break;
          case 2:
            results_[i][j] += std::get<2>(snp_std_[snp_num]);
            break;

          default:
            break;
        }
      }
    }
    // Scale and add back to results
    for (size_t j = 0; j < i + 1; ++j) {
      results_[i][j] -= (m_sig_x_[i] + m_sig_x_[j]);
      results_[i][j] += m_sig_m_;
      results_[i][j] /= num_snps_;
    }
  }

  timer_.End("Run");
}

std::ostream& operator<<(std::ostream& os, GRMCalculator& grm_calc) {
  grm_calc.timer_.Start("Output");

  for (size_t i = 0; i < grm_calc.num_ind_; ++i) {
    for (size_t j = 0; j < i + 1; ++j) {
      unsigned char const* result =
          reinterpret_cast<unsigned char const*>(&grm_calc.results_[i][j]);
      for (std::size_t k = 0; k != sizeof(float); ++k) os << result[k];
    }
  }

  // for (std::size_t i = 0; i != sizeof(float); ++i) os << p2[i];
  // for (std::size_t i = 0; i != sizeof(float); ++i) os << p3[i];
  // os << *p1;
  // os << *p2;
  // os << *p3;
  //  << std::endl;
  // os << grm_calc.results_[0][0] << std::endl;
  // os << grm_calc.results_[1][0] << std::endl;
  // os << grm_calc.results_[1][1] << std::endl;
  // os << "Ind\t";
  // for (size_t i = 0; i < grm_calc.num_ind_; ++i) {
  //   os << grm_calc.individual_ids_[i] << "\t";
  // }
  // os << "\n";
  // for (size_t i = 0; i < grm_calc.num_ind_; ++i) {
  //   os << grm_calc.individual_ids_[i] << "\t";
  //   for (size_t j = 0; j < i + 1; ++j) {
  //     os << grm_calc.results_[i][j] << "\t";
  //   }

  //   // ! Wildly cache inefficient...
  //   for (size_t j = i + 1; j < grm_calc.num_ind_; ++j) {
  //     os << grm_calc.results_[j][i] << "\t";
  //   }
  //   os << "\n";
  // }
  // os << std::endl;
  grm_calc.timer_.End("Output");
  return os;
}

void GRMCalculator::Test() {
  // std::cout << results_[0][0] << "\n";

  // std::cout << results_[1][0] << "\t";
  // std::cout << results_[1][1] << "\n";

  // std::cout << results_[2][0] << "\t";
  // std::cout << results_[2][1] << "\t";
  // std::cout << results_[2][2] << "\n";
}

}  // namespace grmcalc