/**
    @file
    @author  Alexander Sherikov

    @copyright 2020 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#define EIGEN_NO_MALLOC

#include "utf_common.h"
#include <boost/mpl/vector.hpp>

#include <qpmad/solver.h>


template <int t_primal_size, int t_num_ctr>
class AllocationFixture
{
public:
    using Solver = qpmad::SolverTemplate<double, t_primal_size, 1, t_num_ctr>;

public:
    typename Solver::template Vector<t_primal_size> x;
    typename Solver::template Vector<t_primal_size> x_ref;

    typename Solver::template Matrix<t_primal_size, t_primal_size> H;
    typename Solver::template Vector<t_primal_size> h;
    typename Solver::template Matrix<t_num_ctr, t_primal_size> A;
    typename Solver::template Vector<t_num_ctr> Alb;
    typename Solver::template Vector<t_num_ctr> Aub;
    typename Solver::template Vector<t_primal_size> lb;
    typename Solver::template Vector<t_primal_size> ub;

    Solver solver;
    typename Solver::ReturnStatus status;


public:
    void checkSimpleInequalities()
    {
        status = solver.solve(x, H, h, lb, ub, A, Alb, Aub);

        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
        BOOST_CHECK(x.isApprox(x_ref, g_default_tolerance));
    }
};

using TypeListSolverSimpleInequalities01 = boost::mpl::vector<AllocationFixture<20, 1> >;

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
