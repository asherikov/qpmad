/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include "utf_common.h"

#include <boost/mpl/vector.hpp>

#include "hessian_solution.h"
#include <qpmad/testing.h>


//===========================================================================
// ResolveFixture
//===========================================================================


template <class t_Solver>
class ResolveFixture
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
    qpmad::SolverParameters param;


public:
    ResolveFixture()
    {
        qpmad::MatrixIndex size = 20;
        qpmad::MatrixIndex num_general_ctr = 1;

        xH.initRandomHessian(size);
        H_copy = xH.H;
        xH_sparse = xH;
        h.setOnes(size);

        A.resize(num_general_ctr, size);
        A.setOnes();
        A_sparse = A.sparseView();
        Alb.resize(num_general_ctr);
        Aub.resize(num_general_ctr);
        Alb << -1.5;
        Aub << 1.5;

        lb.resize(size);
        ub.resize(size);
        lb << 1, 2, 3, 4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5;
        ub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

#ifdef EIGEN_RUNTIME_NO_MALLOC
        xH_prealloc = xH;
        xH_prealloc.resizeSolution();
        solver_prealloc.reserve(xH_prealloc.H.rows(), lb.rows(), A.rows());
#endif
    }

    void solve()
    {
        xH.status = solver.solve(xH.x, xH.H, h, lb, ub, A, Alb, Aub, param);
        BOOST_CHECK_EQUAL(xH.status, qpmad::Solver::OK);

        if (0 != this->A_sparse.rows() && 0 != this->A_sparse.cols())
        {
            xH_sparse.status = solver.solve(xH_sparse.x, xH_sparse.H, h, lb, ub, A_sparse, Alb, Aub, param);
            xH_sparse.compare(xH);
        }

#ifdef EIGEN_RUNTIME_NO_MALLOC
        Eigen::internal::set_is_malloc_allowed(false);
        xH_prealloc.status = solver.solve(xH_prealloc.x, xH_prealloc.H, h, lb, ub, A, Alb, Aub, param);
        Eigen::internal::set_is_malloc_allowed(true);

        xH_prealloc.compare(xH);
#endif
    }
};


using TypeListResolve = boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 20, 1, 1> >;


BOOST_FIXTURE_TEST_CASE_TEMPLATE(resolve_with_cholesky, t_Solver, TypeListResolve, ResolveFixture<t_Solver>)
{
    this->solve();
    // Hessian changed;
    BOOST_CHECK(!this->H_copy.isApprox(this->xH.H, g_default_tolerance));

    // next iteration
    this->H_copy = this->xH.H;
    Eigen::VectorXd x_copy = this->xH.x;
    this->param.hessian_type_ = this->solver.getHessianType();
    BOOST_CHECK_EQUAL(this->param.hessian_type_, qpmad::SolverParameters::HESSIAN_CHOLESKY_FACTOR);

    this->solve();
    // Hessian not changed
    BOOST_CHECK(this->H_copy.isApprox(this->xH.H, g_default_tolerance));
    // solution is the same
    BOOST_CHECK(x_copy.isApprox(this->xH.x, g_default_tolerance));
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(resolve_with_inverted_cholesky, t_Solver, TypeListResolve, ResolveFixture<t_Solver>)
{
    this->param.return_inverted_cholesky_factor_ = true;
    this->solve();
    // Hessian changed;
    BOOST_CHECK(!this->H_copy.isApprox(this->xH.H, g_default_tolerance));

    // next iteration
    this->H_copy = this->xH.H;
    Eigen::VectorXd x_copy = this->xH.x;
    this->param.hessian_type_ = this->solver.getHessianType();
    BOOST_CHECK_EQUAL(this->param.hessian_type_, qpmad::SolverParameters::HESSIAN_INVERTED_CHOLESKY_FACTOR);

    this->solve();
    // Hessian not changed
    BOOST_CHECK(this->H_copy.isApprox(this->xH.H, g_default_tolerance));
    // solution is the same
    BOOST_CHECK(x_copy.isApprox(this->xH.x, g_default_tolerance));
}


//===========================================================================
// ResolveUnconstrainedFixture
//===========================================================================


template <class t_Solver>
class ResolveUnconstrainedFixture
{
public:
    qpmad_utils::HessianSolution<t_Solver> xH;
    qpmad_utils::HessianSolution<t_Solver> xH_prealloc;
    Eigen::MatrixXd H_copy;
    Eigen::VectorXd h;
    Eigen::VectorXd lb;
    Eigen::VectorXd ub;

    t_Solver solver;
    t_Solver solver_prealloc;
    qpmad::SolverParameters param;


public:
    ResolveUnconstrainedFixture()
    {
        qpmad::MatrixIndex size = 20;

        qpmad_utils::getRandomPositiveDefiniteMatrix(xH.H, size);
        H_copy = xH.H;
        h.setOnes(size);

        lb.setConstant(size, -1e20);
        ub.setConstant(size, 1e20);

        lb(0) = 1;
        ub(0) = 1;

#ifdef EIGEN_RUNTIME_NO_MALLOC
        xH_prealloc = xH;
        xH_prealloc.resizeSolution();
        solver_prealloc.reserve(xH_prealloc.H.rows(), lb.rows(), 0);
#endif
    }

    void solve()
    {
        xH.status = solver.solve(xH.x, xH.H, h, lb, ub, param);
        BOOST_CHECK_EQUAL(xH.status, qpmad::Solver::OK);

#ifdef EIGEN_RUNTIME_NO_MALLOC
        Eigen::internal::set_is_malloc_allowed(false);
        xH_prealloc.status = solver.solve(xH_prealloc.x, xH_prealloc.H, h, lb, ub, param);
        Eigen::internal::set_is_malloc_allowed(true);

        xH_prealloc.compare(xH);
#endif
    }
};


using TypeListResolveUnconstrained = boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 20, 1, 0> >;


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        resolve_unconstrained_with_cholesky,
        t_Solver,
        TypeListResolveUnconstrained,
        ResolveUnconstrainedFixture<t_Solver>)
{
    this->solve();
    // Hessian changed;
    BOOST_CHECK(!this->H_copy.isApprox(this->xH.H, g_default_tolerance));

    // next iteration
    this->H_copy = this->xH.H;
    Eigen::VectorXd x_copy = this->xH.x;
    this->param.hessian_type_ = this->solver.getHessianType();
    BOOST_CHECK_EQUAL(this->param.hessian_type_, qpmad::SolverParameters::HESSIAN_CHOLESKY_FACTOR);

    this->solve();
    // Hessian not changed
    BOOST_CHECK(this->H_copy.isApprox(this->xH.H, g_default_tolerance));
    // solution is the same
    BOOST_CHECK(x_copy.isApprox(this->xH.x, g_default_tolerance));
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        resolve_unconstrained_with_inverted_cholesky,
        t_Solver,
        TypeListResolveUnconstrained,
        ResolveUnconstrainedFixture<t_Solver>)
{
    this->param.return_inverted_cholesky_factor_ = true;
    this->solve();
    // Hessian changed;
    BOOST_CHECK(!this->H_copy.isApprox(this->xH.H, g_default_tolerance));

    // next iteration
    this->H_copy = this->xH.H;
    Eigen::VectorXd x_copy = this->xH.x;
    this->param.hessian_type_ = this->solver.getHessianType();
    BOOST_CHECK_EQUAL(this->param.hessian_type_, qpmad::SolverParameters::HESSIAN_INVERTED_CHOLESKY_FACTOR);

    this->solve();
    // Hessian not changed
    BOOST_CHECK(this->H_copy.isApprox(this->xH.H, g_default_tolerance));
    // solution is the same
    BOOST_CHECK(x_copy.isApprox(this->xH.x, g_default_tolerance));
}
