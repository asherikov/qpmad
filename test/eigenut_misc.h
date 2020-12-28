/**
    @file
    @author  Alexander Sherikov
    @author  Jan Michalczyk
    @copyright 2019 Alexander Sherikov. Licensed under the Apache License, Version 2.0.
    @copyright 2014-2017 INRIA. Licensed under the Apache License, Version 2.0.
    (see @ref LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/


#ifndef H_QPMAD_UTILS_MISC
#define H_QPMAD_UTILS_MISC

#include <Eigen/Core>


/// @ingroup eigenut
namespace qpmad_utils
{
    inline void getRandomPositiveDefiniteMatrix(Eigen::MatrixXd &M, const std::size_t size)
    {
        M.setRandom(size, size);
        M = M.transpose()*M + Eigen::MatrixXd::Identity(size, size);
    }
}
#endif
