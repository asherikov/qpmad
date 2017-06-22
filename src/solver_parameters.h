/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once

namespace qpmad
{
    class SolverParameters
    {
        public:
            enum HessianType
            {
                UNDEFINED                   = 0,
                HESSIAN_LOWER_TRIANGULAR    = 1,
                HESSIAN_CHOLESKY_FACTOR     = 2
                //HESSIAN_DIAGONAL         = 1,
            };


        public:
            HessianType     hessian_type_;

            double          tolerance_;

            int             max_iter_;


        public:
            SolverParameters()
            {
                hessian_type_ = HESSIAN_LOWER_TRIANGULAR;

                tolerance_ = 1e-12;

                max_iter_ = -1;
            }
    };
}
