#ifndef SPARSE_ARG_PARSER_CONFIGS_ANNOTATION_FILE_CONFIG_H_
#define SPARSE_ARG_PARSER_CONFIGS_ANNOTATION_FILE_CONFIG_H_

#include <string>

#include "../arg.hpp"
#include "../config.hpp"

namespace argparse {

class AnnotationFileConfig : public virtual Config {
 private:
  Arg<std::string>* annot_arg_;

 protected:
  /**
   * @brief Adds all arguments to the TestConfig
   *
   */
  void AddAllArgs() {
    annot_arg_ = new Arg<std::string>("-a", &annotation_file_, "AnnotationFile",
                                      "The path to the annotation file", false);
    AddArg(annot_arg_);
  }

 public:
  AnnotationFileConfig() {
    config_name_ = "Annotation File Config";
    AddAllArgs();
  }

  bool DidSet() const { return annot_arg_->DidSet(); }

  // bool hasAnnotationFile() { return annot_arg_-; }

  std::string annotation_file_;
};

}  // namespace argparse

#endif
