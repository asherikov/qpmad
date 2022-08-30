/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include "solver_fixtures.h"


//===========================================================================
// SolverObjectiveFixture
//===========================================================================

template <int t_primal_size>
using TypeListObjective = boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, t_primal_size, 0, 0> >;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(objective00, t_Solver, TypeListObjective<50>, SolverObjectiveFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;

    this->initRandomHessian(size);
    this->h.setRandom(size);

    this->checkObjective();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(objective01, t_Solver, TypeListObjective<50>, SolverObjectiveFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;

    this->initRandomHessian(size);
    this->h.setZero(size);

    this->checkObjective();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(objective02, t_Solver, TypeListObjective<50>, SolverObjectiveFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;

    this->initRandomHessian(size);

    this->checkObjective();
}

// qpmad::SolverTemplate<double, 1, 0, 0>  does not compile due to Eigen asserts
BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        objective03,
        t_Solver,
        boost::mpl::vector<qpmad::Solver>,
        SolverObjectiveFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 1;

    this->initRandomHessian(size);
    this->h.setRandom(size);

    this->checkObjective();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        objective04_semidefinite,
        t_Solver,
        TypeListObjective<50>,
        SolverObjectiveFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;

    this->initRandomHessian(size);
    this->xH.H(5, 5) = 0.0;  // break positive definiteness
    this->h.setRandom(size);

    BOOST_CHECK_THROW(this->checkObjective(), std::exception);
}
