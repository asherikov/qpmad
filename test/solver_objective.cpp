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


BOOST_FIXTURE_TEST_CASE_TEMPLATE(objective05, t_Solver, TypeListObjective<Eigen::Dynamic>, SolverObjectiveFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;

    this->initRandomHessian(size);
    this->h.setRandom(size);

    Eigen::Ref<Eigen::MatrixXd> H = this->xH.H.topLeftCorner(50, 50);
    this->xH.status = this->solver.solve(this->xH.x, H, this->h, this->A, this->Alb, this->Aub);

    Eigen::VectorXd tmp;
    if (this->h.size() > 0)
    {
        tmp = this->H_copy * this->xH.x + this->h;
    }
    else
    {
        tmp = this->H_copy * this->xH.x;
    }

    BOOST_CHECK_EQUAL(this->xH.status, qpmad::Solver::OK);
    BOOST_CHECK(tmp.norm() < g_default_tolerance);
}

