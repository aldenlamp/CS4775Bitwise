#include "phenotype_data.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../linalg/vect_ops.hpp"

using argparse::PhenotypeFileConfig;
using linalg::standardize;
using std::ifstream;
using std::pow;
using std::string;
using std::stringstream;
using std::vector;

namespace fileinput {

PhenotypeData::PhenotypeData(const PhenotypeFileConfig* config) {
  ifstream input_stream(config->phenotype_file_);
  if (!input_stream.is_open()) {
    std::cout << "Could not open file: " << config->phenotype_file_
              << std::endl;
    exit(EXIT_FAILURE);
  }

  pheno_num_ind_ = 0;
  pheno_sum_ = 0;

  string pheno_line;
  getline(input_stream, pheno_line);

  while (getline(input_stream, pheno_line)) {
    stringstream buffer(pheno_line);
    string pheno_value;
    getline(buffer, pheno_value, ' ');
    getline(buffer, pheno_value, ' ');
    getline(buffer, pheno_value, ' ');
    float val = std::stof(pheno_value);
    phenotypes_.push_back(val);
    pheno_sum_ += val;
    ++pheno_num_ind_;
  }

  input_stream.close();

  if (config->standardize_) {
    standardize(phenotypes_, 0, 1);
  }

  yty_ = 0;
  for (vector<float>::const_iterator it = phenotypes_.begin();
       it != phenotypes_.end(); ++it) {
    yty_ += pow(*it, 2.0);
    phenotypes2_.push_back(2 * (*it));
  }
}

}  // namespace fileinput