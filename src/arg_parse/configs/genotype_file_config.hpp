#ifndef SPARSE_ARG_PARSER_GENOTYPE_CONFIG_H_
#define SPARSE_ARG_PARSER_GENOTYPE_CONFIG_H_

#include <string>

#include "../arg.hpp"
#include "../config.hpp"

namespace argparse {

class GenotypeFileConfig : public virtual Config {
 protected:
  /**
   * @brief Adds all arguments to the TestConfig
   *
   */
  void AddAllArgs() {
    AddArg(new Arg<std::string>("-g", &genotype_file_, "GenotypeFile",
                                "The path to the genotype file", true));

    // TODO: - Is this still supported?
    AddArg(new Arg<bool>("-vcf", &is_vcf_, "UseVCF", "Flag if using vcf"));

    // TODO: - Is this still supported?
    // For now im going to assume it is not...
    // AddArg(new Arg<int>("-sl", &snp_limit_, "SnpLim",
    //                     "The max amount of snps to input", false, true,
    //                     "-1"));

    // AddArg(new Arg<int>("-il", &ind_limit_, "IndLim",
    //                     "The max amount of individals to input", false, true,
    //                     "-1"));
  }

 public:
  /**
   * @brief Construct a new Config object
   *
   * @param argc the number of args to use
   * @param argv a pointer to a list of args to use
   * @param is_leaf represents if this arg is a leaf arg
   * @param is_super represents if this class is used as a super config
   */
  GenotypeFileConfig() {
    config_name_ = "Genotype File Config";
    AddAllArgs();
  }

  std::string genotype_file_;
  bool is_vcf_;

  // int snp_limit_;
  // int ind_limit_;
};

}  // namespace argparse

#endif
