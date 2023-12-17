#ifndef SPARSE_FILE_INPUT_ANNOTATION_DATA_H_
#define SPARSE_FILE_INPUT_ANNOTATION_DATA_H_

#include <string>
#include <vector>

#include "../arg_parse/configs/annotation_file_config.hpp"

namespace fileinput {

class AnnotationData {
 private:
  std::vector<char> annotations_;
  bool did_set_;

 protected:
  int* annotation_counts_;
  char num_annotations_;
  int annot_num_snps_;

  inline unsigned char GetAnnotation(int index) const {
    return did_set_ ? annotations_[index] : 0;
  }

  inline bool HasAnnotations() const { return did_set_; }

 public:
  AnnotationData(const argparse::AnnotationFileConfig* config);
  ~AnnotationData();

  inline std::vector<char>::const_iterator GetAnnotItr() const {
    return annotations_.begin();
  }
};

}  // namespace fileinput

#endif