#include "ldscore_calculator.hpp"

#include <iostream>

#include "bitwise_snp_data.hpp"

using argparse::LDScoreConfig;
using std::ifstream;
using std::string;

namespace ldscore {

// MARK: - Init

LDScoreCalculator::LDScoreCalculator(const LDScoreConfig* config)
    : AnnotationData(config), GenotypeData(config) {
  timer.Start("LDCalc:Init");

  VerifyInput();
  SetUpManagers(config->window_);

  timer.End("LDCalc:Init");
}

LDScoreCalculator::~LDScoreCalculator() { delete[] ld_managers_; }

void LDScoreCalculator::VerifyInput() {
  // ! Currently just used for testing

  // TODO: - Implement (probably checking for files??)
  // TODO: - Move checking to file input
}

void LDScoreCalculator::SetUpManagers(size_t window_size) {
  ld_managers_ = new LDManager[num_annotations_];
  for (size_t i = 0; i < num_annotations_; ++i) {
    size_t ann_count = num_annotations_ > 1 ? annotation_counts_[i] : num_snps_;
    ld_managers_[i].Init(ann_count, window_size);
  }
}

// MARK: - LD Calculations

void LDScoreCalculator::ProcessLDScore() {
  for (size_t snp_num = 0; snp_num < num_snps_; ++snp_num) {
    SnpData* data = GetNextSnp();

    data->position_ = snp_locations_[snp_num];

    size_t annotation = GetAnnotation(snp_num);
    ld_managers_[annotation].AddSnpData(data);
  }
}

std::ostream& operator<<(std::ostream& os, const LDScoreCalculator& snp) {
  os << "ID\tLD\n";

  float** ptrs = new float*[snp.num_annotations_];
  for (int i = 0; i < snp.num_annotations_; ++i) {
    ptrs[i] = snp.ld_managers_[i].ld_results_head_;
  }

  for (size_t i = 0; i < snp.num_snps_; ++i) {
    unsigned char annot = snp.GetAnnotation(i);
    os << snp.snp_ids_[i] << "\t" << *ptrs[annot] << "\n";
    ++ptrs[annot];
  }

  delete[] ptrs;
  return os;
}

SnpData* LDScoreCalculator::GetNextSnp() {
  // TODO: - Convert to accept sparse as well
  BitwiseSnpData* next_snp = new BitwiseSnpData(num_ind_);
  for (int i = 0; i < bytes_per_snp_; ++i) {
    next_snp->AddChar(GetNextSnpByte());
  }
  next_snp->ProcessConstruction();

  return next_snp;
}

}  // namespace ldscore