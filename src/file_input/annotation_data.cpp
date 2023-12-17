#include "annotation_data.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using argparse::AnnotationFileConfig;
using std::ifstream;
using std::string;
using std::stringstream;
using std::vector;

namespace fileinput {

AnnotationData::AnnotationData(const AnnotationFileConfig* config) {
  // Check if an annotation file was set
  did_set_ = config->DidSet();
  if (!did_set_) {
    num_annotations_ = 1;
    annot_num_snps_ = -1;
    return;
  }

  string annotation_file = config->annotation_file_;

  // Verify that file can be opened
  // TODO: - Move to config functionality
  ifstream input_stream(annotation_file);
  if (!input_stream.is_open()) {
    std::cout << "Could not open file: " << annotation_file << std::endl;
    exit(EXIT_FAILURE);
  }

  // TODO: - Is there a better way to handle number of annotations?
  num_annotations_ = 0;
  annot_num_snps_ = 0;

  string annotation_line;

  bool is_first_line = true;
  while (getline(input_stream, annotation_line)) {
    stringstream ann_buffer(annotation_line);
    string ann_category;
    int category = 0;
    while (getline(ann_buffer, ann_category, ' ')) {
      if (is_first_line) ++num_annotations_;

      if (std::stoi(ann_category) == 1) {
        annotations_.push_back(category);
        ++annot_num_snps_;
        if (!is_first_line) {
          ++annotation_counts_[category];
          break;
        }
      }
      ++category;
    }

    if (is_first_line) {
      annotation_counts_ = new int[num_annotations_];
      for (int i = 0; i < num_annotations_; ++i) {
        annotation_counts_[i] = 0;
      }
      annotation_counts_[annotations_[0]] = 1;
      is_first_line = false;
    }
  }
}

AnnotationData::~AnnotationData() {
  if (num_annotations_ > 1) {
    delete[] annotation_counts_;
  }
}

}  // namespace fileinput