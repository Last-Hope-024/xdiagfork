// SPDX-FileCopyrightText: 2025 Alexander Wietek <awietek@pks.mpg.de>
//
// SPDX-License-Identifier: Apache-2.0

#include "symmetry_operations.hpp"

#include <algorithm>
#include <fstream>

#include <xdiag/common.hpp>

#include <xdiag/combinatorics/bit_patterns.hpp>
#include <xdiag/combinatorics/combinations_index.hpp>
#include <xdiag/symmetries/group_action/group_action.hpp>
#include <xdiag/symmetries/group_action/group_action_lookup.hpp>
#include <xdiag/utils/logger.hpp>

#ifdef _OPENMP
#include <xdiag/parallel/omp/omp_utils.hpp>
#endif

namespace xdiag::symmetries {

bool is_valid_permutation(int64_t nsites, const int64_t *permutation) {
  for (int64_t i = 0; i < nsites; ++i) {
    if (std::find(permutation, permutation + nsites, i) ==
        permutation + nsites)
      return false;
  }
  return true;
}

//////////////////////////////////////////////////////////
template <typename bit_t>
bit_t apply_permutation(bit_t state, int64_t nsites,
                        const int64_t *permutation) {
  bit_t tstate = 0;
  for (int64_t site = 0; site < nsites; ++site) {
    tstate |= ((state >> site) & 1) << permutation[site];
  }
  return tstate;
}

template uint16_t apply_permutation<uint16_t>(uint16_t, int64_t,
                                              const int64_t *);
template uint32_t apply_permutation<uint32_t>(uint32_t, int64_t,
                                              const int64_t *);
template uint64_t apply_permutation<uint64_t>(uint64_t, int64_t,
                                              const int64_t *);

//////////////////////////////////////////////////////////
template <typename bit_t>
bit_t apply_permutation(bit_t state, gsl::span<int64_t const> permutation) {
  bit_t tstate = 0;
  for (std::size_t site = 0; site < permutation.size(); ++site) {
    tstate |= ((state >> site) & 1) << permutation[site];
  }
  return tstate;
}

template uint16_t apply_permutation<uint16_t>(uint16_t,
                                              gsl::span<int64_t const>);
template uint32_t apply_permutation<uint32_t>(uint32_t,
                                              gsl::span<int64_t const>);
template uint64_t apply_permutation<uint64_t>(uint64_t,
                                              gsl::span<int64_t const>);

} // namespace xdiag::symmetries
