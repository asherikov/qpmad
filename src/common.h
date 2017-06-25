/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once

#include <stdexcept>
#include <cmath>
#include <Eigen/Dense>

#include "config.h"


#define QPMAD_ASSERT(condition, message)    if (!(condition)) \
                                            {throw std::runtime_error(std::string("In ") + __func__ + "() // " + (message));}
#define QPMAD_THROW(message)                throw std::runtime_error(std::string("In ") + __func__ + "() // " + (message));


#ifdef QPMAD_ENABLE_TRACING
#define QPMAD_TRACE(info)                   std::cout << info << std::endl;
#else
#define QPMAD_TRACE(info)
#endif


namespace qpmad
{
    typedef     int     MatrixIndex;

    typedef     Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>     QPMatrix;
    typedef     Eigen::Matrix<double, Eigen::Dynamic, 1>                  QPVector;


    template <class t_VectorType>
        inline  void dropElementWithoutResize(  t_VectorType &vector,
                                                const MatrixIndex index,
                                                const MatrixIndex size)
    {
        vector.segment(index, size - index - 1) = vector.segment(index+1, size - index - 1);
    }
}
