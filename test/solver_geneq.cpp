/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include "solver_fixtures.h"


//===========================================================================
// SolverGeneralEqualitiesFixture
//===========================================================================

template <int t_num_ctr>
using TypeListGeneralEqualities = boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 50, 0, t_num_ctr> >;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_equalities00,
        t_Solver,
        TypeListGeneralEqualities<20>,
        SolverGeneralEqualitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 20;

    this->initRandomHessian(size);
    this->h.setRandom(size);

    this->A = Eigen::MatrixXd::Identity(size, size).topRows(num_ctr);
    this->A_sparse = this->A.sparseView();
    this->Alb.setRandom(num_ctr);

    this->checkGeneralEqualities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_equalities01,
        t_Solver,
        TypeListGeneralEqualities<20>,
        SolverGeneralEqualitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 20;

    this->initRandomHessian(size);

    this->A = Eigen::MatrixXd::Identity(size, size).topRows(num_ctr);
    this->A_sparse = this->A.sparseView();
    this->Alb.setRandom(num_ctr);

    this->checkGeneralEqualities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_equalities02,
        t_Solver,
        TypeListGeneralEqualities<50>,
        SolverGeneralEqualitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 50;

    this->initRandomHessian(size);
    this->h.setRandom(size);

    this->A = Eigen::MatrixXd::Identity(size, size).topRows(num_ctr);
    this->A_sparse = this->A.sparseView();
    this->Alb.setRandom(num_ctr);

    this->checkGeneralEqualities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_equalities03,
        t_Solver,
        TypeListGeneralEqualities<51>,
        SolverGeneralEqualitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 51;

    this->initRandomHessian(size);
    this->h.setRandom(size);

    this->A.setRandom(num_ctr, size);
    this->A.topRows(size) += Eigen::MatrixXd::Identity(size, size);
    this->Alb.setRandom(num_ctr);
    this->Alb(num_ctr - 1) += 100.0;

    // Infeasible equalities
    BOOST_CHECK_THROW(this->status = this->solver.solve(this->x, this->H, this->h, this->A, this->Alb), std::exception);
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_equalities04,
        t_Solver,
        TypeListGeneralEqualities<51>,
        SolverGeneralEqualitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 51;

    this->initRandomHessian(size);
    this->h.setRandom(size);

    this->A.setZero(num_ctr, size);
    this->A.topRows(size) += Eigen::MatrixXd::Identity(size, size);
    this->A(num_ctr - 1, 0) = 100.0;
    this->A_sparse = this->A.sparseView();
    this->Alb.resize(num_ctr);
    this->Alb.setConstant(1.0);
    this->Alb(num_ctr - 1) = 100.0;

    this->status = this->solver.solve(this->x, this->H, this->h, this->A, this->Alb);

    BOOST_CHECK_EQUAL(this->status, qpmad::Solver::OK);
    BOOST_CHECK(this->x.isApprox(Eigen::VectorXd::Ones(size), g_default_tolerance));

    if (0 != this->A_sparse.rows() and 0 != this->A_sparse.cols())
    {
        this->status_sparse = this->solver.solve(this->x_sparse, this->H_copy, this->h, this->A_sparse, this->Alb);
        BOOST_CHECK_EQUAL(this->status, this->status_sparse);
        BOOST_CHECK(this->x.isApprox(this->x_sparse, g_default_tolerance));
    }
}
