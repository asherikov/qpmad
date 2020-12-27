^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ariles_ros
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Use Eigen Cholesky decomposition instead of a custom one, detect
  semi-definite Hessians. This change introduces dependency on Eigen 3.3, use
  older version of `qpmad` if not available.


1.0.2 (2019-12-31)
------------------

* Added missing dependency in package.xml.


1.0.1 (2019-12-24)
------------------
* Doxygen documentation.
* Initial ROS release.
