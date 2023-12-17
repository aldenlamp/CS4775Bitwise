
#include "ld_manager.hpp"

#include <cmath>

namespace ldscore {

void LDManager::Init(size_t num_snps, size_t window_size) {
  ld_results_head_ = new float[num_snps - 1];
  results_ptr_ = ld_results_head_;
  window_size_ = window_size;
  head_ = nullptr;
  tail_ = nullptr;
}

void LDManager::AddSnpData(SnpData* snp) {
  // No pointers
  if (head_ == nullptr || tail_ == nullptr) {
    // assert (head_ == nullptr && tail_ == nullptr)
    head_ = snp;
    tail_ = snp;
    return;
  }

  // Update tail
  tail_->next_ = snp;
  tail_ = snp;

  // Find new head (based on window size)
  while (snp->position_ - head_->position_ > window_size_ &&
         window_size_ != -1) {
    SnpData* old_head = head_;
    head_ = head_->next_;
    delete old_head;

    ++results_ptr_;
  }

  // Add correlations to results
  SnpData* snp_itr = head_;
  float* result_itr = results_ptr_;
  float total_corr = 0;

  while (snp_itr != snp) {
    float corr = snp->Correlation(snp_itr);
    corr = pow(corr, 2);
    *result_itr += corr;
    total_corr += corr;

    snp_itr = snp_itr->next_;
    ++result_itr;
  }

  *result_itr = total_corr;
}

}  // namespace ldscore
