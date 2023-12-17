#ifndef SPARSE_LINALG_TENSOR_H_
#define SPARSE_LINALG_TENSOR_H_

#include <iostream>

namespace linalg {

// TODO: - Iterators.......

struct Tensor4 {
  float* f;
  size_t l1_, l2_, l3_;
  size_t j_, b_, k_, n_;

  Tensor4() { f = nullptr; }

  ~Tensor4() { Delete(); }

  void inline Delete() {
    if (f != nullptr) {
      delete[] f;
      f = nullptr;
    }
  }

  inline void Init(long j, long k, long b, long n, bool should_zero = false) {
    Delete();
    f = new float[j * b * k * n];

    l1_ = n;
    l2_ = l1_ * b;
    l3_ = l2_ * k;

    j_ = j;
    b_ = b;
    k_ = k;
    n_ = n;

    if (should_zero) Zero();
  }

  inline void Zero() {
    for (size_t i = 0; i < j_ * k_ * b_ * n_; ++i) f[i] = 0;
  }

  inline size_t Index(size_t j, size_t k, size_t b, size_t n) {
    return j * l3_ + k * l2_ + b * l1_ + n;
  }

  inline void Set(size_t j, size_t k, size_t b, size_t n, float val) {
    f[Index(j, k, b, n)] = val;
  }

  inline float Get(size_t j, size_t k, size_t b, size_t n) {
    return f[Index(j, k, b, n)];
  }

  inline void Add(size_t j, size_t k, size_t b, size_t n, float val) {
    f[Index(j, k, b, n)] += val;
  }

  inline void Mult(size_t j, size_t k, size_t b, size_t n, float val) {
    f[Index(j, k, b, n)] *= val;
  }
};

struct Tensor3 {
  float* f;
  size_t l1_, l2_;
  size_t b_, k_, n_;

  Tensor3() { f = nullptr; }

  ~Tensor3() { Delete(); }

  inline void Delete() {
    if (f != nullptr) {
      delete[] f;
      f = nullptr;
    }
  }

  inline void Init(size_t b, size_t k, size_t n, bool should_zero = false) {
    Delete();
    f = new float[b * k * n];
    l1_ = n;
    l2_ = l1_ * k;

    b_ = b;
    k_ = k;
    n_ = n;

    if (should_zero) Zero();
  }

  inline void Zero() {
    for (size_t i = 0; i < b_ * k_ * n_; ++i) f[i] = 0;
  }

  inline size_t Index(size_t b, size_t k, size_t n) {
    return b * l2_ + k * l1_ + n;
  }

  inline void Set(size_t b, size_t k, size_t n, float val) {
    f[Index(b, k, n)] = val;
  }

  inline float Get(size_t b, size_t k, size_t n) { return f[Index(b, k, n)]; }

  inline void Add(size_t b, size_t k, size_t n, float val) {
    f[Index(b, k, n)] += val;
  }

  inline void Mult(size_t b, size_t k, size_t n, float val) {
    f[Index(b, k, n)] *= val;
  }
};

template <typename T>
struct Tensor2 {
  T* f;
  size_t l1_;
  size_t k_, n_;

  Tensor2() { f = nullptr; }

  Tensor2(size_t k, size_t n, bool should_zero = false) {
    Init(k, n, should_zero);
  }

  ~Tensor2() { Delete(); }

  inline void Delete() {
    if (f != nullptr) {
      delete[] f;
      f = nullptr;
    }
  }

  inline void Init(size_t k, size_t n, bool should_zero = false) {
    Delete();

    k_ = k;
    n_ = n;
    f = new T[k * n];
    l1_ = n;
    if (should_zero) Zero();
  }

  inline void Zero() {
    for (size_t i = 0; i < k_ * n_; ++i) f[i] = 0;
  }

  inline size_t Index(size_t k, size_t n) { return k * l1_ + n; }

  inline void Set(size_t k, size_t n, T val) { f[Index(k, n)] = val; }

  inline T Get(size_t k, size_t n) { return f[Index(k, n)]; }

  inline void Add(size_t k, size_t n, T val) { f[Index(k, n)] += val; }

  inline void Mult(size_t k, size_t n, T val) { f[Index(k, n)] *= val; }
};

struct Tensor1 {
  float* f;
  size_t n_;

  Tensor1() { f = nullptr; }

  // Tensor1(long n, bool should_zero = false) { Init(n, should_zero); }

  ~Tensor1() { Delete(); }

  inline void Delete() {
    if (f != nullptr) {
      delete[] f;
      f = nullptr;
    }
  }

  inline void Init(size_t n, bool should_zero = false) {
    Delete();
    n_ = n;
    f = new float[n];
    if (should_zero) Zero();
  }

  inline void Zero() {
    for (size_t i = 0; i < n_; ++i) f[i] = 0;
  }

  inline void Combine(float* other_f) {
    for (size_t i = 0; i < n_; ++i) f[i] += other_f[i];
  }

  inline float Sum() {
    float s = 0;
    for (size_t i = 0; i < n_; ++i) s += f[i];
    return s;
  }

  inline void Set(size_t n, float val) { f[n] = val; }

  inline float Get(size_t n) { return f[n]; }

  inline void Add(size_t n, float val) { f[n] += val; }

  inline void Mult(size_t n, float val) { f[n] *= val; }
};

}  // namespace linalg

#endif