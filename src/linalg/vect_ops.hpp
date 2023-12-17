#ifndef SPARSE_LINALG_VECTOR_OPS_H_
#define SPARSE_LINALG_VECTOR_OPS_H_

#include <cmath>
#include <string>
#include <vector>

namespace linalg {

void standardize(std::vector<float>& vec, float target_mean = 0,
                 float target_var = 1) {
  float mean = 0;
  for (std::vector<float>::const_iterator it = vec.begin(); it != vec.end();
       ++it) {
    mean += *it;
  }
  mean = mean / vec.size();

  float var = 0;
  for (std::vector<float>::const_iterator it = vec.begin(); it != vec.end();
       ++it) {
    var += pow((*it - mean), 2.0);
  }
  var = var / vec.size();

  float var_adjust = sqrt(target_var / var);
  float mean_adjust = target_mean - mean * var_adjust;

  for (size_t i = 0; i < vec.size(); ++i) {
    vec[i] = vec[i] * var_adjust + mean_adjust;
  }
}

}  // namespace linalg

#endif