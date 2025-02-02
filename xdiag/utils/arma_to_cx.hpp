#pragma once

#include <xdiag/extern/armadillo/armadillo>

namespace xdiag {

arma::cx_vec to_cx_vec(arma::vec const &A);
arma::cx_vec to_cx_vec(arma::cx_vec const &A);
arma::cx_mat to_cx_mat(arma::mat const &A);
arma::cx_mat to_cx_mat(arma::cx_mat const &A);

} // namespace xdiag
