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
// ResolveFixture
//===========================================================================

template <class t_Solver>
class ResolveFixture
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
    qpmad::SolverParameters param;


public:
    ResolveFixture()
    {
        qpmad::MatrixIndex size = 20;
        qpmad::MatrixIndex num_general_ctr = 1;

        qpmad_utils::getRandomPositiveDefiniteMatrix(H, size);
        H_copy = H;
        h.setOnes(size);

        A.resize(num_general_ctr, size);
        A.setOnes();
        Alb.resize(num_general_ctr);
        Aub.resize(num_general_ctr);
        Alb << -1.5;
        Aub << 1.5;

        lb.resize(size);
        ub.resize(size);
        lb << 1, 2, 3, 4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5;
        ub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;
    }

    void solve()
    {
        status = solver.solve(x, H, h, lb, ub, A, Alb, Aub, param);
        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
    }
};


using TypeListResolve = boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 20, 1, 1> >;


BOOST_FIXTURE_TEST_CASE_TEMPLATE(resolve_with_cholesky, t_Solver, TypeListResolve, ResolveFixture<t_Solver>)
{
    this->solve();
    // Hessian changed;
    BOOST_CHECK(not this->H_copy.isApprox(this->H, g_default_tolerance));

    // next iteration
    this->H_copy = this->H;
    Eigen::VectorXd x_copy = this->x;
    this->param.hessian_type_ = this->solver.getHessianType();
    BOOST_CHECK_EQUAL(this->param.hessian_type_, qpmad::SolverParameters::HESSIAN_CHOLESKY_FACTOR);

    this->solve();
    // Hessian not changed
    BOOST_CHECK(this->H_copy.isApprox(this->H, g_default_tolerance));
    // solution is the same
    BOOST_CHECK(x_copy.isApprox(this->x, g_default_tolerance));
}


BOOST_FIXTURE_TEST_CASE_TEMPLATE(resolve_with_inverted_cholesky, t_Solver, TypeListResolve, ResolveFixture<t_Solver>)
{
    this->param.return_inverted_cholesky_factor_ = true;
    this->solve();
    // Hessian changed;
    BOOST_CHECK(not this->H_copy.isApprox(this->H, g_default_tolerance));

    // next iteration
    this->H_copy = this->H;
    Eigen::VectorXd x_copy = this->x;
    this->param.hessian_type_ = this->solver.getHessianType();
    BOOST_CHECK_EQUAL(this->param.hessian_type_, qpmad::SolverParameters::HESSIAN_INVERTED_CHOLESKY_FACTOR);

    this->solve();
    // Hessian not changed
    BOOST_CHECK(this->H_copy.isApprox(this->H, g_default_tolerance));
    // solution is the same
    BOOST_CHECK(x_copy.isApprox(this->x, g_default_tolerance));
}


//===========================================================================
// ResolveUnconstrainedFixture
//===========================================================================


template <class t_Solver>
class ResolveUnconstrainedFixture
{
public:
    Eigen::VectorXd x;
    Eigen::MatrixXd H;
    Eigen::MatrixXd H_copy;
    Eigen::VectorXd h;
    Eigen::VectorXd lb;
    Eigen::VectorXd ub;

    t_Solver solver;
    typename t_Solver::ReturnStatus status;
    qpmad::SolverParameters param;


public:
    ResolveUnconstrainedFixture()
    {
        qpmad::MatrixIndex size = 20;

        qpmad_utils::getRandomPositiveDefiniteMatrix(H, size);
        H_copy = H;
        h.setOnes(size);

        lb.setConstant(size, -1e20);
        ub.setConstant(size, 1e20);

        lb(0) = 1;
        ub(0) = 1;
    }

    void solve()
    {
        status = solver.solve(x, H, h, lb, ub, param);
        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
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
    BOOST_CHECK(not this->H_copy.isApprox(this->H, g_default_tolerance));

    // next iteration
    this->H_copy = this->H;
    Eigen::VectorXd x_copy = this->x;
    this->param.hessian_type_ = this->solver.getHessianType();
    BOOST_CHECK_EQUAL(this->param.hessian_type_, qpmad::SolverParameters::HESSIAN_CHOLESKY_FACTOR);

    this->solve();
    // Hessian not changed
    BOOST_CHECK(this->H_copy.isApprox(this->H, g_default_tolerance));
    // solution is the same
    BOOST_CHECK(x_copy.isApprox(this->x, g_default_tolerance));
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
    BOOST_CHECK(not this->H_copy.isApprox(this->H, g_default_tolerance));

    // next iteration
    this->H_copy = this->H;
    Eigen::VectorXd x_copy = this->x;
    this->param.hessian_type_ = this->solver.getHessianType();
    BOOST_CHECK_EQUAL(this->param.hessian_type_, qpmad::SolverParameters::HESSIAN_INVERTED_CHOLESKY_FACTOR);

    this->solve();
    // Hessian not changed
    BOOST_CHECK(this->H_copy.isApprox(this->H, g_default_tolerance));
    // solution is the same
    BOOST_CHECK(x_copy.isApprox(this->x, g_default_tolerance));
}

