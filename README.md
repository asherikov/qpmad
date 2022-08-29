qpmad
=====
<table>
  <tr>
    <td align="center">
        CI status
    </td>
    <td align="center">
        Debian package
    </td>
  </tr>
  <tr>
    <td align="center">
        <a href="https://github.com/asherikov/qpmad/actions?query=workflow%3A.github%2Fworkflows%2Fmaster.yml+branch%3Amaster">
        <img src="https://github.com/asherikov/qpmad/workflows/.github/workflows/master.yml/badge.svg?branch=master" alt="Build Status">
        </a>
    </td>
    <td align="center">
        <a href="https://cloudsmith.io/~asherikov-aV7/repos/qpmad/packages/detail/deb/qpmad/latest/a=all;d=any-distro%252Fany-version;t=binary/">
        <img src="https://api-prd.cloudsmith.io/v1/badges/version/asherikov-aV7/qpmad/deb/qpmad/latest/a=all;d=any-distro%252Fany-version;t=binary/?render=true&show_latest=true" alt="Latest version of 'qpmad' @ Cloudsmith">
        </a>
    </td>
  </tr>
</table>

Eigen-based, header-only C++ implementation of Goldfarb-Idnani dual active set
algorithm for quadratic programming. The package is ROS compatible.

The solver is optimized for performance, for this reason some of the
computations are omitted as described below. See
https://github.com/asherikov/qpmad_benchmark for comparison with `qpOASES` and
`eiQuadProg`.


Contents
--------
* [Links](#links)
* [Features](#features)
* [Dependencies](#dependencies)
* [Notes](#notes)
* [Documentation](#docs)


<a name="links"></a>
Links
=====
* Doxygen: https://asherikov.github.io/qpmad/
* GitHub: https://github.com/asherikov/qpmad


<a name="features"></a>
Features:
=========
- Double sided inequality constraints: `lb <= A*x <= ub`. Such constraints
  can be handled in a more efficient way than `lb <= A*x` commonly used in
  other implementations of the algorithm. `A` can be sparse.

- Simple bounds: `lb <= x <= ub`.

- Lazy data initialization, e.g., perform inversion of the Cholesky factor
  only if some of the constraints are activated.

- Works with positive-definite problems only (add regularization if necessary).

- Performs in-place factorization of Hessian and can reuse it on subsequent
  iterations. Can optionally store inverted Cholesky factor in the Hessian
  matrix for additional performance gain.

- Does not compute value of the objective function.

- Does not compute/update Lagrange multipliers for equality constraints.

- Three types of memory allocation:
    - on demand (default);
    - on compile time using template parameters;
    - dynamic preallocation using `reserve()` method.


<a name="dependencies"></a>
Dependencies:
=============
- C++11 compatible compiler
- cmake >= 3.0
- Eigen >= 3.3.0
- Boost (for C++ tests)


<a name="notes"></a>
Notes:
======

1. Before computing the full step length I check that the dot product of the
   chosen constraint with the step direction is not zero instead of checking
   the norm of the step direction. The former approach makes more sense since
   the said dot product appears later as a divisor and we can avoid computation
   of a useless norm.

2. I am aware that activation of simple bounds zeroes out parts of matrix 'J'.
   Unfortunately, I don't see a way to exploit this on modern hardware --
   updating the whole 'J' at once is computationally cheaper than doing this
   line by line selectively or permuting 'J' to collect sparse rows in one
   place.

3. Since the solver may arbitrarily choose violated constraints for activation,
   it always prefers the cheapest ones, i.e., the simple bounds. In particular,
   this allows to avoid computation of violations of general constraints if
   there are violated bounds.

4. Vector 'd' and primal step direction are updated during partial steps
   instead of being computed from scratch. This, however, does not result in a
   significant performance improvement.


<a name="docs"></a>
Documentation and examples
==========================

* Precompiled Doxygen documentation: https://asherikov.github.io/qpmad/
* Introductory demo: https://asherikov.github.io/qpmad/DEMO.html [`./test/dependency/demo.cpp`]

