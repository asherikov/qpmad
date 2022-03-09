/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once

#include "utf_common.h"

#include <boost/mpl/vector.hpp>

#include <qpmad/solver.h>
#include <qpmad/testing.h>


//===========================================================================
// SolverObjectiveFixture
//===========================================================================

#define QPMAD_USING_PARENT_FIXTURE(Parent)                                                                             \
    using Parent::x;                                                                                                   \
    using Parent::x_sparse;                                                                                            \
    using Parent::H;                                                                                                   \
    using Parent::H_copy;                                                                                              \
    using Parent::h;                                                                                                   \
    using Parent::A;                                                                                                   \
    using Parent::A_sparse;                                                                                            \
    using Parent::Alb;                                                                                                 \
    using Parent::Aub;                                                                                                 \
    using Parent::lb;                                                                                                  \
    using Parent::ub;                                                                                                  \
    using Parent::solver;                                                                                              \
    using Parent::status;                                                                                              \
    using Parent::status_sparse;                                                                                       \
    using Parent::checkObjective;                                                                                      \
    using Parent::initRandomHessian;

template <class t_Solver>
class SolverObjectiveFixture
{
public:
    Eigen::VectorXd x;
    Eigen::VectorXd x_sparse;
    Eigen::MatrixXd H;
    Eigen::MatrixXd H_copy;
    Eigen::VectorXd h;
    Eigen::MatrixXd A;
    Eigen::SparseMatrix<double> A_sparse;
    Eigen::VectorXd Alb;
    Eigen::VectorXd Aub;
    Eigen::VectorXd lb;
    Eigen::VectorXd ub;

    t_Solver solver;
    typename t_Solver::ReturnStatus status;
    typename t_Solver::ReturnStatus status_sparse;


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
        status = solver.solve(x, H, h, A, Alb);

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

        if (0 != this->A_sparse.rows() and 0 != this->A_sparse.cols())
        {
            status_sparse = solver.solve(x_sparse, H_copy, h, A_sparse, Alb);
            BOOST_CHECK_EQUAL(status, status_sparse);
            BOOST_CHECK(x.isApprox(x_sparse, g_default_tolerance));
        }
    }
};


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
        H_copy = H;

        status = solver.solve(x, H, h, A, Alb, Aub);

        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
        BOOST_CHECK(x.isApprox(x_ref, g_default_tolerance));

        if (0 != this->A_sparse.rows() and 0 != this->A_sparse.cols())
        {
            status_sparse = solver.solve(x_sparse, H_copy, h, A_sparse, Alb, Aub);
            BOOST_CHECK_EQUAL(status, status_sparse);
            BOOST_CHECK(x.isApprox(x_sparse, g_default_tolerance));
        }
    }
};


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
        H_copy = H;

        status = solver.solve(x, H, h, lb, ub, A, Alb, Aub);

        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
        BOOST_CHECK(x.isApprox(x_ref, g_default_tolerance));

        if (0 != this->A_sparse.rows() and 0 != this->A_sparse.cols())
        {
            status_sparse = solver.solve(x_sparse, H_copy, h, A_sparse, Alb, Aub);
            BOOST_CHECK_EQUAL(status, status_sparse);
            BOOST_CHECK(x.isApprox(x_sparse, g_default_tolerance));
        }
    }
};
