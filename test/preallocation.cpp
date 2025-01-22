/**
    @file
    @author  Alexander Sherikov

    @copyright 2020 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#define EIGEN_RUNTIME_NO_MALLOC

#include "utf_common.h"
#include <boost/mpl/vector.hpp>

#include <qpmad/solver.h>


template <int t_primal_size, int t_num_ctr>
class PreallocationFixture
{
public:
    typename qpmad::Solver::template Vector<t_primal_size> x;
    typename qpmad::Solver::template Vector<t_primal_size> x_ref;

    typename qpmad::Solver::template Matrix<t_primal_size, t_primal_size> H;
    typename qpmad::Solver::template Vector<t_primal_size> h;
    typename qpmad::Solver::template Matrix<t_num_ctr, t_primal_size> A;
    typename qpmad::Solver::template Vector<t_num_ctr> Alb;
    typename qpmad::Solver::template Vector<t_num_ctr> Aub;
    typename qpmad::Solver::template Vector<t_primal_size> lb;
    typename qpmad::Solver::template Vector<t_primal_size> ub;

    qpmad::Solver solver;
    typename qpmad::Solver::ReturnStatus status;


public:
    void checkSimpleInequalities()
    {
        solver.reserve(t_primal_size, lb.rows(), t_num_ctr);

        Eigen::internal::set_is_malloc_allowed(false);
        status = solver.solve(x, H, h, lb, ub, A, Alb, Aub);
        Eigen::internal::set_is_malloc_allowed(true);

        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
        BOOST_CHECK(x.isApprox(x_ref, g_default_tolerance));
    }
};

using TypeListSolverSimpleInequalities01 = boost::mpl::vector<PreallocationFixture<20, 1>>;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(simple_inequalities21, t_Fixture, TypeListSolverSimpleInequalities01, t_Fixture)
{
    this->H.setIdentity();
    this->h.setOnes();

    this->A.setOnes();
    this->Alb << -1.5;
    this->Aub << 1.5;

    this->lb << 1, 2, 3, 4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5;
    this->ub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

    this->x_ref << 1.0, 2.0, 3.0, 4.0, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875,
            -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875;
    this->checkSimpleInequalities();
}
