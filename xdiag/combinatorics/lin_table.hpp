// SPDX-FileCopyrightText: 2025 Alexander Wietek <awietek@pks.mpg.de>
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include <xdiag/bits/bitops.hpp>
#include <xdiag/combinatorics/combinations.hpp>
#include <xdiag/combinatorics/combinations_index.hpp>
#include <xdiag/common.hpp>

namespace xdiag::combinatorics {

template <class bit_t> class LinTable {
public:
  LinTable() = default;
  LinTable(int64_t n, int64_t k);

  inline int64_t n() const { return n_; }
  inline int64_t k() const { return k_; }

  inline int64_t index(bit_t bits) const {
    return left_indices_[bits >> n_right_] +
           right_indices_[bits::gbits(bits, 0, n_right_)];
  }

  inline combinatorics::Combinations<bit_t> states() const {
    return combinatorics::Combinations<bit_t>(n_, k_);
  }

  inline combinatorics::CombinationsIndex<bit_t> states_indices() const {
    return combinatorics::CombinationsIndex<bit_t>(n_, k_);
  }
  inline int64_t size() const { return size_; }

  bool operator==(LinTable<bit_t> const &rhs) const;
  bool operator!=(LinTable<bit_t> const &rhs) const;

private:
  int64_t n_, k_;
  int64_t n_left_;
  int64_t n_right_;

  int64_t left_table_size_;
  int64_t right_table_size_;

  std::vector<int64_t> left_indices_;
  std::vector<int64_t> right_indices_;

  int64_t size_;
};

} // namespace xdiag::combinatorics
