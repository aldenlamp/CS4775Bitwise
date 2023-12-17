#ifndef SPARSE_ARG_PARSER_CONFIGS_PHENOTYPE_FILE_CONFIG_H_
#define SPARSE_ARG_PARSER_CONFIGS_PHENOTYPE_FILE_CONFIG_H_

#include <string>

#include "../arg.hpp"
#include "../config.hpp"

namespace argparse {

class PhenotypeFileConfig : public virtual Config {
 protected:
  void AddAllArgs() {
    AddArg(new Arg<std::string>("-p", &phenotype_file_, "PhenotypeFile",
                                "The path to the phenotype file", true));

    AddArg(new Arg<bool>("-std", &standardize_, "Standardize",
                         "Should standardize the phenotype"));
  }

 public:
  PhenotypeFileConfig() {
    config_name_ = "Phenotype File Config";
    AddAllArgs();
  }

  std::string phenotype_file_;
  bool standardize_;
};

}  // namespace argparse

#endif
