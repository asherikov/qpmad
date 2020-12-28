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
    void initRandomHessian(const qpmad::MatrixIndex size)
    {
        qpmad_utils::getRandomPositiveDefiniteMatrix(H, size);
        H_copy = H;
    }

    void solve()
    {
        status = solver.solve(x, H, h, lb, ub, A, Alb, Aub, param);
        BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
    }
};


using TypeListResolve = boost::mpl::vector<qpmad::Solver, qpmad::SolverTemplate<double, 20, 1, 1> >;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(
        resolve_with_cholesky,
        t_Solver,
        TypeListResolve,
        ResolveFixture<t_Solver>)
{
    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_eq_ctr = 1;

    this->initRandomHessian(size);
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

    this->solve();
    // Hessian changed;
    BOOST_CHECK(not this->H_copy.isApprox(this->H, g_default_tolerance));

    // next iteration
    this->H_copy = this->H;
    Eigen::VectorXd x_copy = this->x;
    this->param.hessian_type_ = this->solver.getHessianType();

    this->solve();
    // Hessian not changed
    BOOST_CHECK(this->H_copy.isApprox(this->H, g_default_tolerance));
    // solution is the same
    BOOST_CHECK(x_copy.isApprox(this->x, g_default_tolerance));
}
