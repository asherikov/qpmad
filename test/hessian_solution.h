/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once

#include <qpmad/solver.h>

namespace qpmad_utils
{
    template <class t_Solver>
    class HessianSolution
    {
    public:
        Eigen::VectorXd x;
        Eigen::MatrixXd H;
        typename t_Solver::ReturnStatus status = t_Solver::UNDEFINED;

    public:
        template <typename t_Index>
        void initRandomHessian(const t_Index size)
        {
            qpmad_utils::getRandomPositiveDefiniteMatrix(H, size);
        }

        template <typename t_Index>
        void initIdentityHessian(const t_Index size)
        {
            H.setIdentity(size, size);
        }

        void resizeSolution()
        {
            x.resize(H.rows());
        }

        qpmad::MatrixIndex size() const
        {
            return (H.rows());
        }

        void compare(const HessianSolution<t_Solver> &other) const
        {
            BOOST_CHECK(x.isApprox(other.x, g_default_tolerance));
            BOOST_CHECK_EQUAL(status, other.status);
        }

        void check(const Eigen::VectorXd &solution) const
        {
            BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
            BOOST_CHECK(x.isApprox(solution, g_default_tolerance));
        }
    };
}  // namespace qpmad_utils
