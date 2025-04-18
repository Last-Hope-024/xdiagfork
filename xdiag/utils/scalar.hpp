// SPDX-FileCopyrightText: 2025 Alexander Wietek <awietek@pks.mpg.de>
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <variant>
#include <xdiag/common.hpp>

namespace xdiag {

class Scalar {
public:
  using value_t = std::variant<double, complex>;

  XDIAG_API Scalar() = default;
  XDIAG_API Scalar(double value);
  XDIAG_API Scalar(complex value);

  XDIAG_API bool operator==(Scalar const &rhs) const;
  XDIAG_API bool operator!=(Scalar const &rhs) const;

  // field operations
  XDIAG_API Scalar &operator+=(Scalar const &rhs);
  XDIAG_API Scalar &operator-=(Scalar const &rhs);
  XDIAG_API Scalar &operator*=(Scalar const &rhs);
  XDIAG_API Scalar &operator/=(Scalar const &rhs);

  XDIAG_API Scalar operator-() const;
  XDIAG_API Scalar operator+(Scalar const &b) const;
  XDIAG_API Scalar operator-(Scalar const &b) const;
  XDIAG_API Scalar operator*(Scalar const &b) const;
  XDIAG_API Scalar operator/(Scalar const &b) const;

  template <typename T> bool is() const;
  template <typename T> T as() const;

  bool isreal() const;
  double real() const;
  double imag() const;
  double abs() const;
  Scalar conj() const;
  bool isapprox(Scalar const &y, double rtol, double atol) const;

private:
  value_t value_;
};
Scalar zero(Scalar s);

XDIAG_API bool isreal(Scalar const &s);
XDIAG_API double real(Scalar const &s);
XDIAG_API double imag(Scalar const &s);
XDIAG_API double abs(Scalar const &s);
XDIAG_API Scalar conj(Scalar const &s);
XDIAG_API bool isapprox(Scalar const &a, Scalar const &b, double rtol = 1e-12,
                        double atol = 1e-12);

XDIAG_API std::ostream &operator<<(std::ostream &out, Scalar const &v);
XDIAG_API std::string to_string(Scalar const &v);

} // namespace xdiag
