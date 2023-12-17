#ifndef SPARSE_LDSCORE_SNP_DATA_H_
#define SPARSE_LDSCORE_SNP_DATA_H_

namespace ldscore {

class SnpData {
 public:
  SnpData(size_t num_ind) : num_individuals_(num_ind){};

  SnpData* next_;

  // ? Is this necessary?
  SnpData* prev_;

  size_t position_;

  // ? Once split into bitwise and sparse reformat this ?
  virtual float Correlation(const SnpData* other) const { return 0; };

 protected:
  unsigned long num_individuals_ = 0;
  unsigned long num_hetero_ = 0;
  unsigned long num_homo_ = 0;
  unsigned long num_missing_ = 0;
  float expected_value_;
  float standard_deviation_;
};

}  // namespace ldscore

#endif