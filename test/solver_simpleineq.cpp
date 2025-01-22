/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include "solver_fixtures.h"


//===========================================================================
// SolverSimpleInequalitiesFixture
//===========================================================================

template <int t_primal_size>
using TypeListSolverSimpleInequalities00 =
        boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, t_primal_size, 1, 0>>;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        simple_inequalities00,
        t_Solver,
        TypeListSolverSimpleInequalities00<2>,
        SolverSimpleInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;

    this->xH.initIdentityHessian(size);
    this->h.setOnes(size);

    this->lb.resize(size);
    this->ub.resize(size);
    this->lb << -10, -g_infinity;
    this->ub << -5, g_infinity;

    this->x_ref.resize(size);
    this->x_ref << -5, -1;

    this->checkSimpleInequalities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        simple_inequalities20,
        t_Solver,
        TypeListSolverSimpleInequalities00<20>,
        SolverSimpleInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 20;

    this->xH.initIdentityHessian(size);
    this->h.setOnes(size);


    this->lb.resize(size);
    this->ub.resize(size);
    this->lb << 1, 2, 3, 4, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5,
            -0.5;
    this->ub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

    this->x_ref.resize(size);
    this->x_ref << 1.0, 2.0, 3.0, 4.0, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5,
            -0.5, -0.5, -0.5;

    this->checkSimpleInequalities();
}


using TypeListSolverSimpleInequalities01 = boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 20, 1, 1>>;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        simple_inequalities21,
        t_Solver,
        TypeListSolverSimpleInequalities01,
        SolverSimpleInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_eq_ctr = 1;

    this->xH.initIdentityHessian(size);
    this->h.setOnes(size);


    this->A.resize(num_eq_ctr, size);
    this->A.setOnes();
    this->Alb.resize(num_eq_ctr);
    this->Aub.resize(num_eq_ctr);
    this->Alb << -1.5;
    this->Aub << 1.5;

    this->lb.resize(size);
    this->ub.resize(size);
    this->lb << 1, 2, 3, 4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5;
    this->ub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

    this->x_ref.resize(size);
    this->x_ref << 1.0, 2.0, 3.0, 4.0, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875,
            -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875;
    this->checkSimpleInequalities();


    BOOST_CHECK_EQUAL(1, this->solver.getNumberOfInequalityIterations());

    Eigen::VectorXd dual;
    Eigen::Matrix<qpmad::MatrixIndex, Eigen::Dynamic, 1> indices;
    Eigen::Matrix<bool, Eigen::Dynamic, 1> is_lower;

    this->solver.getInequalityDual(dual, indices, is_lower);
    qpmad::testing::printDualVariables(dual, indices, is_lower);

    BOOST_CHECK_EQUAL(1, dual.size());
}
