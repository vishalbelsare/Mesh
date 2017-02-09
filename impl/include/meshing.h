// -*- mode: c++ -*-
// Copyright 2016 University of Massachusetts, Amherst

#pragma once
#ifndef MESH__MESHING_H
#define MESH__MESHING_H

#include "common.h"

namespace mesh {

bool meshable(const uint64_t *bitmap1, const uint64_t *bitmap2, size_t len) {
  bitmap1 = (const uint64_t *)__builtin_assume_aligned(bitmap1, 16);
	bitmap2 = (const uint64_t *)__builtin_assume_aligned(bitmap2, 16);

  for (size_t i = 0; i < len; i++) {
    if ((bitmap1[i] ^ bitmap2[i]) != 0)
      return false;
  }
  return true;
}
}

#endif  // MESH__MESHING_H