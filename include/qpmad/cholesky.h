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
    /**
     * @brief Cholesky factorization
     *
     * We need our own implementation of Cholesky factorization since inplace
     * factorization is not supported by old versions of Eigen. Also, we may
     * want to add regularization at this stage.
     */
    class CholeskyFactorization
    {
        public:
            template <class t_MatrixType>
                inline static void compute(t_MatrixType &M)
            {
                const MatrixIndex   size    = M.rows();

                M(0, 0) = std::sqrt(M(0,0));

                for (MatrixIndex i = 1; i < size; ++i)
                {
                    M.col(i-1).segment(i, size-i) /= M(i-1,i-1);

                    M.col(i).segment(i, size-i).noalias() -= M.block(i, 0, size-i, i) * M.row(i).segment(0, i).transpose();

                    M(i, i) = std::sqrt(M(i,i));
                }
            }


            template <  class t_OutputVectorType,
                        class t_InputMatrixType0,
                        class t_InputMatrixType1>
                inline static void solve(   t_OutputVectorType &x,
                                            t_InputMatrixType0 &L,
                                            t_InputMatrixType1 &v)
            {
                x = L.template triangularView<Eigen::Lower>().solve(v);
                L.transpose().template triangularView<Eigen::Upper>().solveInPlace(x);
            }
    };
}
