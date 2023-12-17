#ifndef SPARSE_FILE_INPUT_PHENOTYPE_DATA_H_
#define SPARSE_FILE_INPUT_PHENOTYPE_DATA_H_

#include <vector>

#include "../arg_parse/configs/phenotype_file_config.hpp"

namespace fileinput {

class PhenotypeData {
 private:
  std::vector<float> phenotypes_;
  std::vector<float> phenotypes2_;

 protected:
  int pheno_num_ind_;
  float yty_;
  float pheno_sum_;

  inline float GetPhenotype(int index) const { return phenotypes_[index]; }
  inline float GetDoubledPhenotype(int index) const {
    return phenotypes2_[index];
  }

 public:
  PhenotypeData(const argparse::PhenotypeFileConfig* config);
};

}  // namespace fileinput

#endif