#include "genotype_data.hpp"

#include <cmath>
#include <fstream>
#include <sstream>

#include "../linalg/tensor.hpp"

using argparse::GenotypeFileConfig;
using linalg::Tensor2;
using std::ifstream;
using std::string;
using std::stringstream;

namespace fileinput {
GenotypeData::GenotypeData(const GenotypeFileConfig* config) {
  buffer_ = new char[kBufferSize];
  InputBimFam(config->genotype_file_);

  bytes_per_snp_ = ceil(num_ind_ / 4.0);
  max_full_updates_ = (bytes_per_snp_ * num_snps_) / kBufferSize;

  bed_file_ = ifstream(config->genotype_file_ + ".bed");
  char emb[] = {0x6c, 0x1b, 0x01};
  char mb[3];

  bed_file_.read(mb, 3);
  if (mb[0] != emb[0] || mb[1] != emb[1] || mb[2] != emb[2]) {
    std::cout << "Corrupted PLINK file" << std::endl;
    exit(EXIT_FAILURE);
  }

  pos_ = 0;
  update_count_ = 0;

  UpdateGenotypeInputBuffer();
}

void GenotypeData::InputBimFam(const string& file_path) {
  ifstream fam_file(file_path + ".fam");
  ifstream bim_file(file_path + ".bim");

  if (!fam_file.is_open()) {
    std::cout << "Could not open " << file_path << ".fam" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (!bim_file.is_open()) {
    std::cout << "Could not open " << file_path << ".bim" << std::endl;
    exit(EXIT_FAILURE);
  }

  string dummy_line;

  string position_str;

  num_snps_ = 0;
  while (getline(bim_file, dummy_line)) {
    stringstream line_buff(dummy_line);

    getline(line_buff, position_str, '\t');  // chromosome code
    getline(line_buff, position_str, '\t');  // Variant ID

    snp_ids_.push_back(position_str);

    getline(line_buff, position_str, '\t');  // Position
    getline(line_buff, position_str, '\t');  // Coordinate

    snp_locations_.push_back(std::stoull(position_str));

    ++num_snps_;
  }

  num_ind_ = 0;
  while (getline(fam_file, dummy_line)) {
    stringstream line_buff(dummy_line);

    getline(line_buff, position_str, '\t');  // Family ID
    getline(line_buff, position_str, '\t');  // Within familty ID
    individual_ids_.push_back(position_str);
    ++num_ind_;
  }

  fam_file.close();
  bim_file.close();
}

// TODO: - Test udpate values at last udpate

void GenotypeData::UpdateGenotypeInputBuffer() {
  if (update_count_ != max_full_updates_) {
    bed_file_.read(buffer_, kBufferSize);
  } else if (update_count_ == max_full_updates_) {
    size_t remainder_num_bytes_ =
        (bytes_per_snp_ * num_snps_) % (max_full_updates_ * kBufferSize);
    bed_file_.read(buffer_, remainder_num_bytes_);
  } else {
    std::cout << "Failed to update genotype file buffer" << std::endl;
    exit(EXIT_FAILURE);
  }
  ++update_count_;
}

void GenotypeData::CloseGenotypeData() {
  bed_file_.close();
  delete[] buffer_;
}

}  // namespace fileinput