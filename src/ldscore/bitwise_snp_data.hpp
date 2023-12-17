#ifndef SPARSE_LDSCORE_BITWISE_SNP_DATA_H_
#define SPARSE_LDSCORE_BITWISE_SNP_DATA_H_

#include <stddef.h>
#include <stdint.h>

#include "bitstring.hpp"
#include "snp_data.hpp"

namespace ldscore {

class BitwiseSnpData : public SnpData {
 private:
  // MARK: - Construction
  // void SetStandardDev();

  // void SetExpectedVal();

  // MARK: - Properties
  Bitstring heterozygous_data_;
  Bitstring homozygous_data_;

  // MARK: - Correlation
  float GetDotProduct(const SnpData* other) const;

 public:
  // MARK: - Construction

  BitwiseSnpData(size_t num_ind);

  void AddChar(unsigned char next);

  void ProcessConstruction();

  float Correlation(const SnpData* other) const;

  void Test() const;
};

}  // namespace ldscore

#endif