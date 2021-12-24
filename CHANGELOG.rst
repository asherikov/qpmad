^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ariles_ros
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.1.1 (2021-12-24)
------------------

* Bugfixes
* Binary debian package generation


1.1.0 (2021-01-18)
------------------

* Use Eigen Cholesky decomposition instead of a custom one, detect
  semi-definite Hessians. This change introduces dependency on Eigen 3.3, use
  older version of `qpmad` if not available.
* Refactor API to avoid instantiation of dynamic Eigen matrices and vectors.
* Allow specification of scalar type and problem dimensions at compilation time.
* The source code is now dependent on C++11 features.
* Added methods that expose number of iterations and dual variables.
* Added optional hot-starting with inverted Cholesky factor.
* DANGER: Solver does not perform Cholesky factorization in some trivial cases
  anymore, i.e. the Hessian is not necessarily modified. Use `getHessianType()`
  to get the correct Hessian type when using solver with constant Hessian.
* DANGER: Solver does not return error codes anymore, exceptions are used
  instead in order to make error handling consistent (some errors used to be
  exceptions in older versions as well). The return codes are: OK,
  MAXIMAL_NUMBER_OF_ITERATIONS.


1.0.2 (2019-12-31)
------------------

* Added missing dependency in package.xml.


1.0.1 (2019-12-24)
------------------
* Doxygen documentation.
* Initial ROS release.
