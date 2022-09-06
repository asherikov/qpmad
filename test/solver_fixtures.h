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
    using Parent::xH;                                                                                                  \
    using Parent::xH_sparse;                                                                                           \
    using Parent::xH_prealloc;                                                                                         \
    using Parent::H_copy;                                                                                              \
    using Parent::h;                                                                                                   \
    using Parent::A;                                                                                                   \
    using Parent::A_sparse;                                                                                            \
    using Parent::Alb;                                                                                                 \
    using Parent::Aub;                                                                                                 \
    using Parent::lb;                                                                                                  \
    using Parent::ub;                                                                                                  \
    using Parent::solver;                                                                                              \
    using Parent::solver_prealloc;                                                                                     \
    using Parent::checkObjective;                                                                                      \
    using Parent::initRandomHessian;                                                                                   \
    using Parent::copyH;


template <class t_Solver>
class SolverObjectiveFixture
{
public:
    qpmad_utils::HessianSolution<t_Solver> xH;
    qpmad_utils::HessianSolution<t_Solver> xH_sparse;
    qpmad_utils::HessianSolution<t_Solver> xH_prealloc;

    Eigen::MatrixXd H_copy;
    Eigen::VectorXd h;
    Eigen::MatrixXd A;
    Eigen::SparseMatrix<double> A_sparse;
    Eigen::VectorXd Alb;
    Eigen::VectorXd Aub;
    Eigen::VectorXd lb;
    Eigen::VectorXd ub;

    t_Solver solver;
    t_Solver solver_prealloc;


public:
    void checkObjective()
    {
        xH.status = solver.solve(xH.x, xH.H, h, A, Alb, Aub);

        Eigen::VectorXd tmp;
        if (h.size() > 0)
        {
            tmp = H_copy * xH.x + h;
        }
        else
        {
            tmp = H_copy * xH.x;
        }

        BOOST_CHECK_EQUAL(xH.status, qpmad::Solver::OK);
        BOOST_CHECK(tmp.norm() < g_default_tolerance);
    }

    void initRandomHessian(const qpmad::MatrixIndex size)
    {
        xH.initRandomHessian(size);
        copyH();
    }

    void copyH()
    {
        H_copy = xH.H;
        xH_sparse = xH;
        xH_prealloc = xH;
        xH_prealloc.resizeSolution();
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
        xH.status = solver.solve(xH.x, xH.H, h, A, Alb);

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

        BOOST_CHECK_EQUAL(xH.status, qpmad::Solver::OK);
        BOOST_CHECK(xH.x.isApprox(tmp, g_default_tolerance));

        if (0 != this->A_sparse.rows() and 0 != this->A_sparse.cols())
        {
            xH_sparse.status = solver.solve(xH_sparse.x, xH_sparse.H, h, A_sparse, Alb);
            BOOST_CHECK_EQUAL(xH.status, xH_sparse.status);
            BOOST_CHECK(xH.x.isApprox(xH_sparse.x, g_default_tolerance));
        }

#ifdef EIGEN_RUNTIME_NO_MALLOC
        solver_prealloc.reserve(xH_prealloc.H.rows(), 0, A.rows());

        Eigen::internal::set_is_malloc_allowed(false);
        xH_prealloc.status = solver_prealloc.solve(xH_prealloc.x, xH_prealloc.H, h, A, Alb);
        Eigen::internal::set_is_malloc_allowed(true);

        BOOST_CHECK_EQUAL(xH.status, xH_prealloc.status);
        BOOST_CHECK(xH.x.isApprox(xH_prealloc.x, g_default_tolerance));
#endif
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
        copyH();

        xH.status = solver.solve(xH.x, xH.H, h, A, Alb, Aub);

        BOOST_CHECK_EQUAL(xH.status, qpmad::Solver::OK);
        BOOST_CHECK(xH.x.isApprox(x_ref, g_default_tolerance));

        if (0 != this->A_sparse.rows() and 0 != this->A_sparse.cols())
        {
            xH_sparse.status = solver.solve(xH_sparse.x, xH_sparse.H, h, A_sparse, Alb, Aub);
            BOOST_CHECK_EQUAL(xH.status, xH_sparse.status);
            BOOST_CHECK(xH.x.isApprox(xH_sparse.x, g_default_tolerance));
        }

#ifdef EIGEN_RUNTIME_NO_MALLOC
        solver_prealloc.reserve(xH_prealloc.H.rows(), 0, A.rows());

        Eigen::internal::set_is_malloc_allowed(false);
        xH_prealloc.status = solver.solve(xH_prealloc.x, xH_prealloc.H, h, A, Alb, Aub);
        Eigen::internal::set_is_malloc_allowed(true);

        BOOST_CHECK_EQUAL(xH.status, xH_prealloc.status);
        BOOST_CHECK(xH.x.isApprox(xH_prealloc.x, g_default_tolerance));
#endif
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
        copyH();

        if (A.rows() > 0)
        {
            xH.status = solver.solve(xH.x, xH.H, h, lb, ub, A, Alb, Aub);
        }
        else
        {
            xH.status = solver.solve(xH.x, xH.H, h, lb, ub);
        }

        BOOST_CHECK_EQUAL(xH.status, qpmad::Solver::OK);
        BOOST_CHECK(xH.x.isApprox(x_ref, g_default_tolerance));

        if (0 != this->A_sparse.rows() and 0 != this->A_sparse.cols())
        {
            xH_sparse.status = solver.solve(xH_sparse.x, xH_sparse.H, h, lb, ub, A_sparse, Alb, Aub);
            BOOST_CHECK_EQUAL(xH.status, xH_sparse.status);
            BOOST_CHECK(xH.x.isApprox(xH_sparse.x, g_default_tolerance));
        }

#ifdef EIGEN_RUNTIME_NO_MALLOC
        solver_prealloc.reserve(xH_prealloc.H.rows(), lb.rows(), A.rows());

        Eigen::internal::set_is_malloc_allowed(false);
        if (A.rows() > 0)
        {
            xH_prealloc.status = solver.solve(xH_prealloc.x, xH_prealloc.H, h, lb, ub, A, Alb, Aub);
        }
        else
        {
            xH_prealloc.status = solver.solve(xH_prealloc.x, xH_prealloc.H, h, lb, ub);
        }
        Eigen::internal::set_is_malloc_allowed(true);

        BOOST_CHECK_EQUAL(xH.status, xH_prealloc.status);
        BOOST_CHECK(xH.x.isApprox(xH_prealloc.x, g_default_tolerance));
#endif
    }
};
