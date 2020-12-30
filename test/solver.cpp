/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include "utf_common.h"

#include <boost/mpl/vector.hpp>

#include <qpmad/solver.h>
#include <qpmad/testing.h>


//===========================================================================
// SolverObjectiveFixture
//===========================================================================

#define QPMAD_USING_PARENT_FIXTURE(Parent)                                                                             \
    using Parent::x;                                                                                                   \
    using Parent::H;                                                                                                   \
    using Parent::H_copy;                                                                                              \
    using Parent::h;                                                                                                   \
    using Parent::A;                                                                                                   \
    using Parent::Alb;                                                                                                 \
    using Parent::Aub;                                                                                                 \
    using Parent::lb;                                                                                                  \
    using Parent::ub;                                                                                                  \
    using Parent::solver;                                                                                              \
    using Parent::status;                                                                                              \
    using Parent::checkObjective;                                                                                      \
    using Parent::initRandomHessian;

template <class t_Solver>
class SolverObjectiveFixture
{
public:
    Eigen::VectorXd x;
    Eigen::MatrixXd H;
    Eigen::MatrixXd H_copy;
    Eigen::VectorXd h;
    Eigen::MatrixXd A;
    Eigen::VectorXd Alb;
    Eigen::VectorXd Aub;
    Eigen::VectorXd lb;
    Eigen::VectorXd ub;

    t_Solver solver;
    typename t_Solver::ReturnStatus status;


public:
    void checkObjective()
    {
        status = solver.solve(x, H, h, A, Alb, Aub);

        Eigen::VectorXd tmp;
        if (h.size() > 0)
        {
            tmp = H_copy * x + h;
        }
        else
        {
            tmp = H_copy * x;
        }

        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
        BOOST_CHECK(tmp.norm() < g_default_tolerance);
    }

    void initRandomHessian(const qpmad::MatrixIndex size)
    {
        qpmad_utils::getRandomPositiveDefiniteMatrix(H, size);
        H_copy = H;
    }
};


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
    this->H(5, 5) = 0.0;  // break positive definiteness
    this->h.setRandom(size);

    BOOST_CHECK_THROW(this->checkObjective(), std::exception);
}


//===========================================================================
// SolverGeneralEqualitiesFixture
//===========================================================================

template <class t_Solver>
class SolverGeneralEqualitiesFixture : public SolverObjectiveFixture<t_Solver>
{
public:
    QPMAD_USING_PARENT_FIXTURE(SolverObjectiveFixture<t_Solver>)

public:
    void checkGeneralEqualities()
    {
        status = solver.solve(x, H, h, A, Alb, Alb);

        Eigen::MatrixXd Hi = H_copy.inverse();
        Eigen::VectorXd tmp;
        if (h.size() > 0)
        {
            tmp = Hi * A.transpose() * (A * Hi * A.transpose()).inverse() * (Alb + A * Hi * h) - Hi * h;
        }
        else
        {
            tmp = Hi * A.transpose() * (A * Hi * A.transpose()).inverse() * Alb;
        }

        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
        BOOST_CHECK(x.isApprox(tmp, g_default_tolerance));
    }
};


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
    BOOST_CHECK_THROW(
            this->status = this->solver.solve(this->x, this->H, this->h, this->A, this->Alb, this->Alb),
            std::exception);
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
    this->Alb.resize(num_ctr);
    this->Alb.setConstant(1.0);
    this->Alb(num_ctr - 1) = 100.0;

    this->status = this->solver.solve(this->x, this->H, this->h, this->A, this->Alb, this->Alb);

    BOOST_CHECK_EQUAL(this->status, qpmad::Solver::OK);
    BOOST_CHECK(this->x.isApprox(Eigen::VectorXd::Ones(size), g_default_tolerance));
}


//===========================================================================
// SolverGeneralInequalitiesFixture
//===========================================================================

template <class t_Solver>
class SolverGeneralInequalitiesFixture : public SolverObjectiveFixture<t_Solver>
{
public:
    QPMAD_USING_PARENT_FIXTURE(SolverObjectiveFixture<t_Solver>)

public:
    Eigen::VectorXd x_ref;

public:
    void checkGeneralInequalities()
    {
        status = solver.solve(x, H, h, A, Alb, Aub);

        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
        BOOST_CHECK(x.isApprox(x_ref, g_default_tolerance));
    }
};


template <int t_num_ctr>
using TypeListGeneralInequalities00 =
        boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 2, 0, t_num_ctr> >;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities00,
        t_Solver,
        TypeListGeneralInequalities00<1>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 1;

    this->H.setIdentity(size, size);
    this->h.setOnes(size);

    this->A = Eigen::MatrixXd::Identity(size, size).topRows(1);
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

    this->H.setIdentity(size, size);

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

    this->H.setIdentity(size, size);

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

    this->H.setIdentity(size, size);

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

    this->H.setIdentity(size, size);

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


template <int t_num_ctr>
using TypeListGeneralInequalities01 =
        boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 20, 0, t_num_ctr> >;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        general_inequalities20,
        t_Solver,
        TypeListGeneralInequalities01<20>,
        SolverGeneralInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_ctr = size;

    this->H.setIdentity(size, size);
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

    this->H.setIdentity(size, size);
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


//===========================================================================
// SolverSimpleInequalitiesFixture
//===========================================================================

template <class t_Solver>
class SolverSimpleInequalitiesFixture : public SolverObjectiveFixture<t_Solver>
{
public:
    QPMAD_USING_PARENT_FIXTURE(SolverObjectiveFixture<t_Solver>)

public:
    Eigen::VectorXd x_ref;

public:
    void checkSimpleInequalities()
    {
        status = solver.solve(x, H, h, lb, ub, A, Alb, Aub);

        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
        BOOST_CHECK(x.isApprox(x_ref, g_default_tolerance));
    }
};


template <int t_primal_size>
using TypeListSolverSimpleInequalities00 =
        boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, t_primal_size, 1, 0> >;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        simple_inequalities00,
        t_Solver,
        TypeListSolverSimpleInequalities00<2>,
        SolverSimpleInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 2;

    this->H.setIdentity(size, size);
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

    this->H.setIdentity(size, size);
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


using TypeListSolverSimpleInequalities01 = boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 20, 1, 1> >;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        simple_inequalities21,
        t_Solver,
        TypeListSolverSimpleInequalities01,
        SolverSimpleInequalitiesFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_eq_ctr = 1;

    this->H.setIdentity(size, size);
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
