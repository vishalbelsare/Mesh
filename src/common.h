// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// Copyright 2017 University of Massachusetts, Amherst

#pragma once
#ifndef MESH__COMMON_H
#define MESH__COMMON_H

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <random>
#include <unordered_map>
#include <vector>

#include "static/staticlog.h"
#include "utility/ilog2.h"

using std::function;
using std::lock_guard;
using std::mt19937_64;
using std::mutex;

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#define ATTRIBUTE_HIDDEN __attribute__((visibility("hidden")))
#define ATTRIBUTE_ALIGNED(s) __attribute__((aligned(s)))
#define CACHELINE 64

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName &);              \
  void operator=(const TypeName &)

// dynamic (runtime) assert
#ifndef NDEBUG
#define d_assert_msg(expr, fmt, ...) \
  ((likely(expr))                    \
       ? static_cast<void>(0)        \
       : mesh::internal::__mesh_assert_fail(#expr, __FILE__, __PRETTY_FUNCTION__, __LINE__, fmt, __VA_ARGS__))

#define d_assert(expr)                   \
  ((likely(expr)) ? static_cast<void>(0) \
                  : mesh::internal::__mesh_assert_fail(#expr, __FILE__, __PRETTY_FUNCTION__, __LINE__, ""))
#else
#define d_assert_msg(expr, fmt, ...)
#define d_assert(expr)
#endif

namespace mesh {

void debug(const char *fmt, ...);

static constexpr size_t MinObjectSize = 16;
static constexpr size_t kMaxSize = 16384;
static constexpr size_t kClassSizesMax = 96;
static constexpr size_t kAlignment = 8;
static constexpr int kMinAlign = 16;
static constexpr int kPageSize = 4096;


// inline constexpr size_t class2Size(const int i) {
//   return static_cast<size_t>(1ULL << (i + staticlog(MinObjectSize)));
// }

// inline int size2Class(const size_t sz) {
//   return static_cast<int>(HL::ilog2((sz < 8) ? 8 : sz) - staticlog(MinObjectSize));
// }

namespace internal {

void StopTheWorld() noexcept;
void StartTheWorld() noexcept;

inline static mutex *getSeedMutex() {
  static char muBuf[sizeof(mutex)];
  static mutex *mu = new (muBuf) mutex();
  return mu;
}

// we must re-initialize our seed on program startup and after fork.
// Must be called with getSeedMutex() held
inline mt19937_64 *initSeed() {
  static char mtBuf[sizeof(mt19937_64)];

  static_assert(sizeof(mt19937_64::result_type) == sizeof(uint64_t), "expected 64-bit result_type for PRNG");

  // seed this Mersenne Twister PRNG with entropy from the host OS
  std::random_device rd;
  return new (mtBuf) std::mt19937_64(rd());
}

// cryptographically-strong thread-safe PRNG seed
inline uint64_t seed() {
  static mt19937_64 *mt = NULL;

  lock_guard<mutex> lock(*getSeedMutex());

  if (unlikely(mt == nullptr))
    mt = initSeed();

  return (*mt)();
}

// assertions that don't attempt to recursively malloc
void __attribute__((noreturn))
__mesh_assert_fail(const char *assertion, const char *file, const char *func, int line, const char *fmt, ...);
}  // namespace internal

#define PREDICT_TRUE likely

// from tcmalloc/gperftools
class SizeMap {
private:
  //-------------------------------------------------------------------
  // Mapping from size to size_class and vice versa
  //-------------------------------------------------------------------

  // Sizes <= 1024 have an alignment >= 8.  So for such sizes we have an
  // array indexed by ceil(size/8).  Sizes > 1024 have an alignment >= 128.
  // So for these larger sizes we have an array indexed by ceil(size/128).
  //
  // We flatten both logical arrays into one physical array and use
  // arithmetic to compute an appropriate index.  The constants used by
  // ClassIndex() were selected to make the flattening work.
  //
  // Examples:
  //   Size       Expression                      Index
  //   -------------------------------------------------------
  //   0          (0 + 7) / 8                     0
  //   1          (1 + 7) / 8                     1
  //   ...
  //   1024       (1024 + 7) / 8                  128
  //   1025       (1025 + 127 + (120<<7)) / 128   129
  //   ...
  //   32768      (32768 + 127 + (120<<7)) / 128  376
  static const int kMaxSmallSize = 1024;
  static const size_t kClassArraySize = ((kMaxSize + 127 + (120 << 7)) >> 7) + 1;
  static const unsigned char class_array_[kClassArraySize];

  static inline size_t SmallSizeClass(size_t s) {
    return (static_cast<uint32_t>(s) + 7) >> 3;
  }

  static inline size_t LargeSizeClass(size_t s) {
    return (static_cast<uint32_t>(s) + 127 + (120 << 7)) >> 7;
  }

  // If size is no more than kMaxSize, compute index of the
  // class_array[] entry for it, putting the class index in noutput
  // parameter idx and returning true. Otherwise return false.
  static inline bool ATTRIBUTE_ALWAYS_INLINE ClassIndexMaybe(size_t s, uint32_t *idx) {
    if (PREDICT_TRUE(s <= kMaxSmallSize)) {
      *idx = (static_cast<uint32_t>(s) + 7) >> 3;
      return true;
    } else if (s <= kMaxSize) {
      *idx = (static_cast<uint32_t>(s) + 127 + (120 << 7)) >> 7;
      return true;
    }
    return false;
  }

  // Compute index of the class_array[] entry for a given size
  static inline size_t ClassIndex(size_t s) {
    // Use unsigned arithmetic to avoid unnecessary sign extensions.
    d_assert(0 <= s);
    d_assert(s <= kMaxSize);
    if (PREDICT_TRUE(s <= kMaxSmallSize)) {
      return SmallSizeClass(s);
    } else {
      return LargeSizeClass(s);
    }
  }


  // Mapping from size class to max size storable in that class
  static const int32_t class_to_size_[kClassSizesMax];

public:
  static constexpr size_t num_size_classes = 25;

  // Constructor should do nothing since we rely on explicit Init()
  // call, which may or may not be called before the constructor runs.
  SizeMap() {
  }

  static inline int SizeClass(size_t size) {
    return class_array_[ClassIndex(size)];
  }

  // Check if size is small enough to be representable by a size
  // class, and if it is, put matching size class into *cl. Returns
  // true iff matching size class was found.
  static inline bool ATTRIBUTE_ALWAYS_INLINE GetSizeClass(size_t size, uint32_t *cl) {
    uint32_t idx;
    if (!ClassIndexMaybe(size, &idx)) {
      return false;
    }
    *cl = class_array_[idx];
    return true;
  }

  // Get the byte-size for a specified class
  //static inline int32_t ATTRIBUTE_ALWAYS_INLINE ByteSizeForClass(uint32_t cl) {
  static inline size_t ATTRIBUTE_ALWAYS_INLINE ByteSizeForClass(int32_t cl) {
    return class_to_size_[static_cast<uint32_t>(cl)];
  }

  // Mapping from size class to max size storable in that class
  static inline int32_t class_to_size(uint32_t cl) {
    return class_to_size_[cl];
  }
};

inline size_t ATTRIBUTE_ALWAYS_INLINE class2Size(const int i) {
  return SizeMap::ByteSizeForClass(i);
}

inline int ATTRIBUTE_ALWAYS_INLINE size2Class(const size_t sz) {
  return SizeMap::SizeClass(sz);
}

}  // namespace mesh

#endif  // MESH__COMMON_H
