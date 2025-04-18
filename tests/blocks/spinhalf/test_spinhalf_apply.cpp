// SPDX-FileCopyrightText: 2025 Alexander Wietek <awietek@pks.mpg.de>
//
// SPDX-License-Identifier: Apache-2.0

#include "../../catch.hpp"

#include <iostream>

#include "testcases_spinhalf.hpp"
#include <xdiag/algebra/algebra.hpp>
#include <xdiag/algebra/apply.hpp>
#include <xdiag/algebra/matrix.hpp>
#include <xdiag/algorithms/sparse_diag.hpp>
#include <xdiag/algebra/isapprox.hpp>
#include <xdiag/utils/timing.hpp>
#include <xdiag/io/file_toml.hpp>

using namespace xdiag;

void test_apply(int N, OpSum ops) {
  for (int nup = 1; nup <= N; ++nup) {
    auto block = Spinhalf(N, nup);
    auto H = matrix(ops, block, block);
    REQUIRE(H.is_hermitian(1e-8));

    arma::vec v(block.size(), arma::fill::randn);
    arma::vec w1 = H * v;
    arma::vec w2(block.size(), arma::fill::zeros);
    // tic();
    apply(ops, block, v, block, w2);
    // toc("1 M-V-M");

    arma::vec w3 = H * H * v;
    arma::vec w4(block.size(), arma::fill::zeros);
    apply(ops, block, w2, block, w4);
    REQUIRE(isapprox(w3, w4));

    arma::vec evals_mat;
    arma::eig_sym(evals_mat, H);

    double e0_mat = evals_mat(0);
    // double e0_app = eigval0(ops, block);
    auto [e0_app, ev] = eig0(ops, block);
    // Log("H: {}, nup: {}, mat: {:.5f} app: {:.5f}", N, nup, e0_mat, e0_app);
    REQUIRE(isapprox(e0_mat, e0_app));
  }
}

void test_apply_mat(int N, OpSum ops) {
  for (int nup = 1; nup <= N; ++nup) {
    auto block = Spinhalf(N, nup);
    auto H = matrix(ops, block, block);
    REQUIRE(H.is_hermitian(1e-8));

    arma::mat v(block.size(), 5, arma::fill::randn);
    arma::mat w1 = H * v;
    arma::mat w2(block.size(), 5, arma::fill::zeros);
    // tic();
    apply(ops, block, v, block, w2);
    // toc("5 column M-M-M");

    arma::mat w3 = H * H * v;
    arma::mat w4(block.size(), 5, arma::fill::zeros);
    apply(ops, block, w2, block, w4);
    REQUIRE(isapprox(w3, w4));

    arma::vec evals_mat;
    arma::eig_sym(evals_mat, H);

    double e0_mat = evals_mat(0);
    // double e0_app = eigval0(ops, block);
    auto [e0_app, ev] = eig0(ops, block);
    // Log("H: {}, nup: {}, mat: {:.5f} app: {:.5f}", N, nup, e0_mat, e0_app);
    REQUIRE(isapprox(e0_mat, e0_app));
  }
}

TEST_CASE("spinhalf_apply", "[spinhalf]") {
  using namespace xdiag::testcases::spinhalf;

  Log.out("spinhalf_apply: Heisenberg chain apply test, J=1.0, N=2,..,6, "
          "matrix-matrix multiplication");
  for (int N = 2; N <= 6; ++N) {
    auto ops = HBchain(N, 1.0);
    test_apply(N, ops);
    test_apply_mat(N, ops);
  }

  Log.out("spinhalf_apply: Heisenberg alltoall apply test, N=2,..,6, "
          "matrix-matrix multiplication");
  for (int N = 2; N <= 6; ++N) {
    auto ops = HB_alltoall(N);
    test_apply(N, ops);
    test_apply_mat(N, ops);
  }

  Log.out("spinhalf_apply: Heisenberg all-to-all Sz <-> NoSz comparison, "
          "matrix-matrix multiplication");
  for (int nsites = 2; nsites <= 6; ++nsites) {
    auto ops = HB_alltoall(nsites);
    auto block_no_sz = Spinhalf(nsites);
    auto e0_no_sz = eigval0(ops, block_no_sz);
    auto e0s_sz = std::vector<double>();

    for (int nup = 0; nup <= nsites; ++nup) {
      auto block_sz = Spinhalf(nsites, nup);
      auto e0_sz = eigval0(ops, block_sz);
      e0s_sz.push_back(e0_sz);
    }
    auto e0_sz = *std::min_element(e0s_sz.begin(), e0s_sz.end());
    REQUIRE(isapprox(e0_sz, e0_no_sz));
  }

  {
    Log("spinhalf_matrix: Triangular J1J2Jchi N=12");
    std::string lfile =
        XDIAG_DIRECTORY "/misc/data/triangular.j1j2jch/"
                        "triangular.12.j1j2jch.sublattices.fsl.toml";

    auto fl = FileToml(lfile);
    auto ops = fl["Interactions"].as<OpSum>();
    ops["J1"] = 1.00;
    ops["J2"] = 0.15;
    ops["Jchi"] = 0.09;

    int nsites = 12;
    int nup = 6;
    auto spinhalf = Spinhalf(nsites, nup);
    auto e0 = eigval0(ops, spinhalf);
    double energy = -6.9456000700824329641;

    // Log("{:.18f} {:.18f}", e0, energy);

    REQUIRE(isapprox(e0, energy));
  }
}
