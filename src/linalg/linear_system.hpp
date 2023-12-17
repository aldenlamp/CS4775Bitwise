// LinearSystem.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include <iostream>
#include <vector>
#define ASSERT(condition, message)       \
  {                                      \
    if (!(condition)) {                  \
      std::cerr << message << std::endl; \
    }                                    \
  }

typedef std::vector<float> vec;
typedef std::vector<vec> mat;

namespace linalg {

// solver for the linearsystem Ax = b
vec LinearSystemSolver(mat A, vec b) {
  int nRow = A.size();
  int nCol = A[0].size();
  for (int i = 1; i < nRow; i++) {
    ASSERT(nCol == A[i].size(), "A is not a valid matrix.");
  }
  ASSERT(nRow == nCol, "A is not a square matrix.");
  ASSERT(nRow == b.size(), "The number of rows of A does not match that of b.");
  // Gauss elimination algorithm
  for (int i = 0; i < nRow; i++) {
    if (A[i][i] == 0) {
      for (int j = i + 1; j < nRow; j++) {
        if (A[j][i] != 0) {
          A[i].swap(A[j]);
          std::swap(b[i], b[j]);
          break;
        }
      }
    }
    ASSERT(A[i][i] != 0, "A is a singular matrix.");
    b[i] /= A[i][i];
    for (int j = nRow - 1; j >= i; j--) {
      A[i][j] /= A[i][i];
    }
    for (int j = i + 1; j < nRow; j++) {
      b[j] -= b[i] * A[j][i];
      for (int k = nRow - 1; k >= i; k--) {
        A[j][k] -= A[i][k] * A[j][i];
      }
    }
  }
  vec x(nRow);  // solution vector
  for (int i = nRow - 1; i >= 0; i--) {
    for (int j = nRow - 1; j > i; j--) {
      b[i] -= A[i][j] * x[j];
    }
    x[i] = b[i];
  }
  return x;
}

}  // namespace linalg
   // int main() {
   //   vec b = {6, 7, 8};
   //   mat A = {{1, 2, 3}, {2, 1, 4}, {3, 4, 1}};
   //   vec x = linearSystemSolver(A, b);
   //   for (int i = 0; i < 3; i++) {
   //     std::cout << x[i] << " ";
   //   }
   // }