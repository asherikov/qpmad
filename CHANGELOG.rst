^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ariles_ros
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Use Eigen Cholesky decomposition instead of a custom one, detect
  semi-definite Hessians. This change introduces dependency on Eigen 3.3, use
  older version of `qpmad` if not available.
* Refactor API to avoid instantiation of dynamic Eigen matrices and vectors.
* Allow specification of scalar type and problem dimensions at compilation time.
* The source code is now dependent on C++11 features.


1.0.2 (2019-12-31)
------------------

* Added missing dependency in package.xml.


1.0.1 (2019-12-24)
------------------
* Doxygen documentation.
* Initial ROS release.
