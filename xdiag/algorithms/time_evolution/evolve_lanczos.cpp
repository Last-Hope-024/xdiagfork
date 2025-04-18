// SPDX-FileCopyrightText: 2025 Alexander Wietek <awietek@pks.mpg.de>
//
// SPDX-License-Identifier: Apache-2.0

#include "evolve_lanczos.hpp"

#include <xdiag/algebra/algebra.hpp>
#include <xdiag/algebra/apply.hpp>
#include <xdiag/algorithms/lanczos/lanczos_convergence.hpp>
#include <xdiag/algorithms/time_evolution/exp_sym_v.hpp>
#include <xdiag/operators/logic/hc.hpp>
#include <xdiag/operators/logic/isapprox.hpp>
#include <xdiag/operators/logic/real.hpp>

namespace xdiag {

EvolveLanczosResult evolve_lanczos(OpSum const &H, State psi, double tau,
                                   double precision, double shift,
                                   bool normalize, int64_t max_iterations,
                                   double deflation_tol) try {
  auto r = evolve_lanczos_inplace(H, psi, tau, precision, shift, normalize,
                                  max_iterations, deflation_tol);
  return {r.alphas, r.betas, r.eigenvalues, r.niterations, r.criterion, psi};
} catch (Error const &e) {
  XDIAG_RETHROW(e);
}

EvolveLanczosResult evolve_lanczos(OpSum const &H, State psi, complex tau,
                                   double precision, double shift,
                                   bool normalize, int64_t max_iterations,
                                   double deflation_tol) try {
  auto r = evolve_lanczos_inplace(H, psi, tau, precision, shift, normalize,
                                  max_iterations, deflation_tol);
  return {r.alphas, r.betas, r.eigenvalues, r.niterations, r.criterion, psi};
} catch (Error const &e) {
  XDIAG_RETHROW(e);
}

EvolveLanczosInplaceResult evolve_lanczos_inplace(OpSum const &H, State &psi,
                                                  double tau, double precision,
                                                  double shift, bool normalize,
                                                  int64_t max_iterations,
                                                  double deflation_tol) try {
  if (!isapprox(H, hc(H))) {
    XDIAG_THROW("Input OpSum is not hermitian. Evolution using the Lanczos "
                "algorithm requires the operator to be hermitian.");
  }

  if (!isvalid(psi)) {
    XDIAG_THROW("Initial state must be a valid state (i.e. not default "
                "constructed by e.g. an annihilation operator)");
  }

  if (norm(psi) == 0.) {
    XDIAG_THROW("Initial state has zero norm");
  }

  auto const &block = psi.block();

  // Real time evolution is possible
  if (psi.isreal() && isreal(H)) {
    int iter = 1;
    auto mult = [&iter, &H, &block](arma::vec const &v, arma::vec &w) {
      auto ta = rightnow();
      apply(H, block, v, block, w);
      Log(2, "Lanczos iteration {}", iter);
      timing(ta, rightnow(), "MVM", 1);
      ++iter;
    };
    auto dot_f = [&block](arma::vec const &v, arma::vec const &w) {
      return dot(block, v, w);
    };

    arma::vec v = psi.vector(0, false);
    auto r = exp_sym_v(mult, dot_f, v, tau, precision, shift, normalize,
                       max_iterations, deflation_tol);
    return {r.alphas, r.betas, r.eigenvalues, r.niterations, r.criterion};
    // Refer to complex time evolution
  } else {
    return evolve_lanczos_inplace(H, psi, complex(tau), precision, shift,
                                  normalize, max_iterations, deflation_tol);
  }
} catch (Error const &e) {
  XDIAG_RETHROW(e);
}

EvolveLanczosInplaceResult evolve_lanczos_inplace(OpSum const &H, State &psi,
                                                  complex tau, double precision,
                                                  double shift, bool normalize,
                                                  int64_t max_iterations,
                                                  double deflation_tol) try {

  if (!isapprox(H, hc(H))) {
    XDIAG_THROW("Input OpSum is not hermitian. Evolution using the Lanczos "
                "algorithm requires the operator to be hermitian.");
  }
  if (!isvalid(psi)) {
    XDIAG_THROW("Initial state must be a valid state (i.e. not default "
                "constructed by e.g. an annihilation operator)");
  }

  if (norm(psi) == 0.) {
    XDIAG_THROW("Initial state has zero norm");
  }

  if (psi.isreal()) {
    psi.make_complex();
  }
  auto const &block = psi.block();

  int iter = 1;
  auto mult = [&iter, &H, &block](arma::cx_vec const &v, arma::cx_vec &w) {
    auto ta = rightnow();
    apply(H, block, v, block, w);
    Log(2, "Lanczos iteration {}", iter);
    timing(ta, rightnow(), "MVM", 1);
    ++iter;
  };
  auto dot_f = [&block](arma::cx_vec const &v, arma::cx_vec const &w) {
    return dot(block, v, w);
  };
  arma::cx_vec v = psi.vectorC(0, false);
  auto r = exp_sym_v(mult, dot_f, v, tau, precision, shift, normalize,
                     max_iterations, deflation_tol);
  return {r.alphas, r.betas, r.eigenvalues, r.niterations, r.criterion};
} catch (Error const &e) {
  XDIAG_RETHROW(e);
}

} // namespace xdiag
