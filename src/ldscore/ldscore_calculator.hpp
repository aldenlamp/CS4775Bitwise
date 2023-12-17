#ifndef SPARSE_LDSCORE_LDSCORE_CALCULATOR_H_
#define SPARSE_LDSCORE_LDSCORE_CALCULATOR_H_

#include <iostream>
#include <vector>

#include "../arg_parse/configs/ld_score_config.hpp"
#include "../file_input/annotation_data.hpp"
#include "../file_input/genotype_data.hpp"
#include "../metadata/timer.hpp"
#include "ld_manager.hpp"
#include "snp_data.hpp"

namespace ldscore {

class LDScoreCalculator : public fileinput::AnnotationData,
                          public fileinput::GenotypeData {
 private:
  metadata::Timer timer;

  LDManager* ld_managers_;

  // MARK: - Init

  void VerifyInput();

  void SetUpManagers(size_t window_size);

  // MARK: - LD Calculations

  SnpData* GetNextSnp();

  // MARK: - Output

  // I Tried... and failed...
  // Iterator left as an archive for for possible future implementation

  /*

  struct ConstIterator {
   private:
    float current_;
    float** ptrs_;
    std::vector<char>::const_iterator annot_itr_;
    bool has_annot_;

   public:
    size_t snp_num_;
    float operator*() const { return current_; }

    // Prefix increment
    ConstIterator& operator++() {
      size_t annot = has_annot_ ? *(++annot_itr_) : 0;
      current_ = *(++ptrs_[annot]);
      ++snp_num_;
      return *this;
    }

    friend bool operator!=(const ConstIterator& a, const ConstIterator& b) {
      return a.snp_num_ != b.snp_num_;
    };

    ConstIterator(const LDManager* managers) {
      ptrs_ = new float*[1];
      ptrs_[0] = managers[0].ld_results_head_;
      snp_num_ = 0;
      has_annot_ = false;
    }

    ConstIterator(const LDManager* managers, const int num_managers,
                  std::vector<char>::const_iterator itr) {
      ptrs_ = new float*[num_managers];
      for (int i = 0; i < num_managers; ++i) {
        ptrs_[i] = managers[i].ld_results_head_;
      }
      snp_num_ = 0;
      annot_itr_ = itr;
      has_annot_ = true;
    }

    ConstIterator(const int end_snp) { snp_num_ = end_snp; }

    ~ConstIterator() {
      if (ptrs_ != nullptr) {
        delete[] ptrs_;
      }
    }
  };

  ConstIterator Begin() const;

  ConstIterator End() const;
  */

  friend std::ostream& operator<<(std::ostream& os,
                                  const LDScoreCalculator& snp);

 public:
  LDScoreCalculator(const argparse::LDScoreConfig* config);

  void ProcessLDScore();

  ~LDScoreCalculator();
};

}  // namespace ldscore

#endif