#include "bitwise_snp_data.hpp"

#include <bit>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <iostream>

namespace ldscore {

BitwiseSnpData::BitwiseSnpData(size_t num_ind)
    : SnpData(num_ind),
      heterozygous_data_(num_ind),
      homozygous_data_(num_ind) {}

void BitwiseSnpData::AddChar(unsigned char next) {
  for (int i = 0; i < 4; ++i) {
    unsigned char val = next & 3;
    if (val == 2) {
      // Heterozygous
      ++num_hetero_;
      heterozygous_data_.SetNext();
      // homozygous_data_.IncrementInd();
    } else if (val == 3) {
      // Homozygous
      ++num_homo_;

      homozygous_data_.SetNext();
    } else if (val == 1) {
      // Missing
      ++num_missing_;
    }
    heterozygous_data_.IncrementInd();
    homozygous_data_.IncrementInd();
    next >>= 2;
  }
}

void BitwiseSnpData::ProcessConstruction() {
  // Calculate mean

  unsigned long sum = num_hetero_ + (num_homo_ * 2);
  expected_value_ = sum / float(num_individuals_);

  // Calculate Std
  float exp_squared = pow(expected_value_, 2.0);
  float four_homo = 4 * num_homo_;

  float std_dev_numerator = exp_squared * (num_individuals_ - num_missing_) -
                            expected_value_ * (four_homo + 2 * num_hetero_) +
                            four_homo + num_hetero_;

  standard_deviation_ =
      sqrt(std_dev_numerator / (num_individuals_ - num_missing_));
}

float BitwiseSnpData::Correlation(const SnpData* other) const {
  float currSum = 0;

  // Bitwise x Bitwise
  const BitwiseSnpData* bitwise_other =
      dynamic_cast<const BitwiseSnpData*>(other);
  if (bitwise_other) {
    size_t num_homo_homo =
        homozygous_data_.CountOverlap(bitwise_other->homozygous_data_);
    size_t num_homo_hetero =
        homozygous_data_.CountOverlap(bitwise_other->heterozygous_data_);
    size_t num_hetero_homo =
        heterozygous_data_.CountOverlap(bitwise_other->homozygous_data_);
    size_t num_hetero_hetero =
        heterozygous_data_.CountOverlap(bitwise_other->heterozygous_data_);

    currSum = 4 * num_homo_homo + 2 * (num_hetero_homo + num_homo_hetero) +
              num_hetero_hetero;

    // TODO: Handle missing data??
    float correlation = (currSum / float(num_individuals_) -
                         (expected_value_ * bitwise_other->expected_value_)) /
                        standard_deviation_ /
                        bitwise_other->standard_deviation_;

    return correlation;
  }

  return 0;
}

void BitwiseSnpData::Test() const {
  // Testing Accuracy of processing construction
  std::cout << "ExpectedVal: " << expected_value_
            << "\tStd: " << standard_deviation_ << std::endl;

  // Testing Construction with bitstring

  // Testing Accuracy of construction

  // std::cout << "HETERO: \n";
  // heterozygous_data_.Print(std::cout);

  // std::cout << "HOMO: \n";
  // homozygous_data_.Print(std::cout);
  // std::cout << std::endl;
}

}  // namespace ldscore