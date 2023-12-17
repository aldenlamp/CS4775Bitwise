#ifndef SPARSE_LDSCORE_BITSTRING_H_
#define SPARSE_LDSCORE_BITSTRING_H_

#include <stddef.h>
#include <stdint.h>

#include <iostream>

namespace ldscore {

class Bitstring {
 private:
  size_t num_uints_;
  uint64_t* bitstr_;

  uint64_t* construction_ptr_;
  int construction_ind_ = 0;

 public:
  Bitstring(size_t size);
  ~Bitstring();

  // MARK: - Construction Functions (for snp data)

  inline void IncrementInd() {
    if (construction_ind_ != 0) {
      --construction_ind_;
    } else {
      construction_ind_ = 63;
      ++construction_ptr_;
    }
  }

  inline void SetNext() {
    *construction_ptr_ |= (uint64_t(1) << construction_ind_);
  }
  // MARK: - Usage Functions

  size_t Count() const;

  size_t CountOverlap(const Bitstring& other) const;

  // MARK: - Testing

  void Print(std::ostream& os) const;
};

}  // namespace ldscore

#endif