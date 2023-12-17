#ifndef SPARSE_LDSCORE_SPARSE_SNP_DATA_H_
#define SPARSE_LDSCORE_SPARSE_SNP_DATA_H_

#include <vector>

namespace ldscore {

class SparseSnpData {
 private:
  int* homozygous_data_;
  int* heterozygous_data_;
  // std::vector<int> missing_data_;

 public:
  SparseSnpData();
};

}  // namespace ldscore

#endif