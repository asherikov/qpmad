/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include "solver_fixtures.h"


//===========================================================================
// SolverGeneralInequalitiesFixture
//===========================================================================


template <int t_num_ctr>
using TypeListGeneralInequalities00 = boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 2, 0, t_num_ctr>>;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities00,
        t_Solver,
        TypeListGeneralInequalities00<1>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 1;

    this->xH.initIdentityHessian(size);
    this->h.setOnes(size);

    this->A = Eigen::MatrixXd::Identity(size, size).topRows(1);
    this->A_sparse = this->A.sparseView();
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb(0) = -10;
    this->Aub(0) = -5;

    this->x_ref.resize(size);
    this->x_ref << -5, -1;

    this->checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities01,
        t_Solver,
        TypeListGeneralInequalities00<3>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    this->xH.initIdentityHessian(size);

    this->A.resize(num_ctr, size);
    this->A << 1, 0, 1, 1, 2, 0.5;
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb << 0, 1, 1;
    this->Aub << 0, 1e10, 1e10;

    this->x_ref.resize(size);
    this->x_ref << 0, 2;

    this->checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities02,
        t_Solver,
        TypeListGeneralInequalities00<3>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    this->xH.initIdentityHessian(size);

    this->A.resize(num_ctr, size);
    this->A << 1, 0, 1, 1, 2, 0.5;
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb << 0, -1e10, -1e10;
    this->Aub << 0, -1, -1;

    this->x_ref.resize(size);
    this->x_ref << 0, -2;

    this->checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities03,
        t_Solver,
        TypeListGeneralInequalities00<3>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    this->xH.initIdentityHessian(size);

    this->A.resize(num_ctr, size);
    this->A << 1, 0, -1, -1, -2, -0.5;
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb << 0, -1e10, -1e10;
    this->Aub << 0, -1, -1;

    this->x_ref.resize(size);
    this->x_ref << 0, 2;

    this->checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities04,
        t_Solver,
        TypeListGeneralInequalities00<3>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    this->xH.initIdentityHessian(size);

    this->A.resize(num_ctr, size);
    this->A << 1, 0, -1, -1, -2, -0.5;
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb << 0, 1, 1;
    this->Aub << 0, 1e10, 1e10;

    this->x_ref.resize(size);
    this->x_ref << 0, -2;

    this->checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities05_sparse,
        t_Solver,
        TypeListGeneralInequalities00<3>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    this->xH.initIdentityHessian(size);
    this->h.resize(size);
    this->h << -1.0, 0.0;

    // x + y <= 1, x >= 0, y == 0.5.
    this->A_sparse.resize(num_ctr, size);
    this->A_sparse.insert(0, 0) = 1.0;
    this->A_sparse.insert(0, 1) = 1.0;
    this->A_sparse.insert(1, 0) = 1.0;
    this->A_sparse.insert(2, 1) = 1.0;
    this->A = Eigen::MatrixXd(this->A_sparse);
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb << -g_infinity, 0.0, 0.0;
    this->Aub << 1.0, g_infinity, g_infinity;

    this->x_ref.resize(size);
    this->x_ref << 1.0, 0.0;

    this->checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities06_sparse,
        t_Solver,
        TypeListGeneralInequalities00<3>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    this->xH.initIdentityHessian(size);
    this->h.resize(size);
    this->h << -1.0, 0.0;

    // x + y <= 1, x >= 0, y == 0.5.
    this->A_sparse.resize(num_ctr, size);
    this->A_sparse.insert(0, 0) = 1.0;
    this->A_sparse.insert(0, 1) = 1.0;
    this->A_sparse.insert(1, 0) = 1.0;
    this->A_sparse.insert(2, 1) = 1.0;
    this->A = Eigen::MatrixXd(this->A_sparse);
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb << -g_infinity, 0.0, 0.5;
    this->Aub << 1.0, g_infinity, 0.5;

    this->x_ref.resize(size);
    this->x_ref << 0.5, 0.5;

    this->checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities07_sparse_infeas,
        t_Solver,
        TypeListGeneralInequalities00<2>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 2;

    this->xH.initIdentityHessian(size);
    this->h.resize(size);
    this->h << 0.0, 0.0;

    // x = 10, x > 100
    this->A_sparse.resize(num_ctr, size);
    this->A_sparse.insert(0, 0) = 1.0;
    this->A_sparse.insert(1, 0) = 1.0;
    this->A = Eigen::MatrixXd(this->A_sparse);
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb << 10, 100.0;
    this->Aub << 10, g_infinity;

    BOOST_CHECK_THROW(
            this->xH.status = this->solver.solve(this->xH.x, this->xH.H, this->h, this->A, this->Alb), std::exception);
}


template <int t_num_ctr>
using TypeListGeneralInequalities01 =
        boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 20, 0, t_num_ctr>>;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities20,
        t_Solver,
        TypeListGeneralInequalities01<20>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_ctr = size;

    this->xH.initIdentityHessian(size);
    this->h.setOnes(size);


    this->A = Eigen::MatrixXd::Identity(num_ctr, size);
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb << 1, 2, 3, 4, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5,
            -0.5;
    this->Aub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

    this->x_ref.resize(size);
    this->x_ref << 1.0, 2.0, 3.0, 4.0, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5,
            -0.5, -0.5, -0.5;

    this->checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities21,
        t_Solver,
        TypeListGeneralInequalities01<21>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_ctr = size + 1;

    this->xH.initIdentityHessian(size);
    this->h.setOnes(size);


    this->A.resize(num_ctr, size);
    this->A.topRows(size) = Eigen::MatrixXd::Identity(size, size);
    this->A.row(num_ctr - 1).setOnes();
    this->Alb.resize(num_ctr);
    this->Aub.resize(num_ctr);
    this->Alb << 1, 2, 3, 4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -1.5;
    this->Aub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1.5;

    this->x_ref.resize(size);
    this->x_ref << 1.0, 2.0, 3.0, 4.0, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875,
            -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875, -0.71875;
    this->checkGeneralInequalities();
}
