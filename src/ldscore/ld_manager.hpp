#ifndef SPARSE_LDSCORE_LD_MANAGER_H_
#define SPARSE_LDSCORE_LD_MANAGER_H_

#include <stddef.h>

#include "snp_data.hpp"

namespace ldscore {

class LDManager {
 private:
  int window_size_;

  float* results_ptr_;
  SnpData* head_;
  SnpData* tail_;

 public:
  float* ld_results_head_;
  LDManager(){};

  void Init(size_t num_snps, size_t window_size);

  void AddSnpData(SnpData* snp);

  inline const float* GetLD() { return ld_results_head_; }
};

}  // namespace ldscore

#endif