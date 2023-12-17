#ifndef SPARSE_FILE_INPUT_GENOTYPE_DATA_H_
#define SPARSE_FILE_INPUT_GENOTYPE_DATA_H_

#include <fstream>
#include <string>

#include "../arg_parse/configs/genotype_file_config.hpp"
#include "../linalg/tensor.hpp"

namespace fileinput {

// TODO: - Consistency of size_t and unsigned long

class GenotypeData {
 private:
  const int kBufferSize = 512;
  char* buffer_;
  int pos_ = 0;
  std::ifstream bed_file_;

  size_t update_count_;
  size_t max_full_updates_;

  void InputBimFam(const std::string& file_path);

  void UpdateGenotypeInputBuffer();

 protected:
  size_t num_snps_;
  size_t num_ind_;
  size_t bytes_per_snp_;

  std::vector<size_t> snp_locations_;
  std::vector<std::string> snp_ids_;
  std::vector<std::string> individual_ids_;

  inline unsigned char GetNextSnpByte() {
    if (pos_ == kBufferSize) {
      UpdateGenotypeInputBuffer();
      pos_ = 0;
    }
    return buffer_[pos_++];
  }

  void CloseGenotypeData();

 public:
  GenotypeData(const argparse::GenotypeFileConfig* config);
};

}  // namespace fileinput

#endif