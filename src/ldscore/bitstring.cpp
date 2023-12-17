#include "bitstring.hpp"

#include <bit>
#include <cmath>

namespace ldscore {

Bitstring::Bitstring(size_t size) {
  num_uints_ = ceil(size / float(64));
  bitstr_ = new uint64_t[num_uints_];

  for (size_t it = 0; it < num_uints_; ++it) {
    bitstr_[it] = 0;
  }

  construction_ptr_ = bitstr_;
  construction_ind_ = 63;
}

Bitstring::~Bitstring() { delete[] bitstr_; }

size_t Bitstring::Count() const {
  size_t count = 0;
  for (size_t i = 0; i < num_uints_; ++i) {
    count += std::popcount(bitstr_[i]);
  }
  return count;
}

size_t Bitstring::CountOverlap(const Bitstring& other) const {
  // TODO: - Check that other and this are same size

  size_t count = 0;
  for (size_t i = 0; i < num_uints_; ++i) {
    count += std::popcount(bitstr_[i] & other.bitstr_[i]);
  }
  return count;
}

void Bitstring::Print(std::ostream& os) const {
  for (size_t i = 0; i < num_uints_; ++i) {
    os << bitstr_[i] << " \t";
  }
  os << std::endl;
}

}  // namespace ldscore