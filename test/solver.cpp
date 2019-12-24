/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include "utf_common.h"


#include <qpmad/solver.h>

//===========================================================================
// SolverObjectiveFixture
//===========================================================================

class SolverObjectiveFixture
{
    public:
        Eigen::VectorXd         x;
        Eigen::MatrixXd         H;
        Eigen::MatrixXd         H_copy;
        Eigen::VectorXd         h;
        Eigen::MatrixXd         A;
        Eigen::VectorXd         Alb;
        Eigen::VectorXd         Aub;
        Eigen::VectorXd         lb;
        Eigen::VectorXd         ub;

        qpmad::Solver               solver;

        qpmad::Solver::ReturnStatus status;


    public:
        void checkObjective()
        {
            status = solver.solve(x, H, h, A, Alb, Aub);

            Eigen::VectorXd tmp;
            if (h.size() > 0)
            {
                tmp = H_copy*x+h;
            }
            else
            {
                tmp = H_copy*x;
            }

            BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
            BOOST_CHECK(tmp.norm() < g_default_tolerance);
        }
};



BOOST_FIXTURE_TEST_CASE( objective00, SolverObjectiveFixture )
{
    qpmad::MatrixIndex size = 50;

    qpmad_utils::getRandomPositiveDefinititeMatrix(H, size);
    H_copy = H;
    h.setRandom(size);

    checkObjective();
}


BOOST_FIXTURE_TEST_CASE( objective01, SolverObjectiveFixture )
{
    qpmad::MatrixIndex size = 50;

    qpmad_utils::getRandomPositiveDefinititeMatrix(H, size);
    H_copy = H;
    h.setZero(size);

    checkObjective();
}


BOOST_FIXTURE_TEST_CASE( objective02, SolverObjectiveFixture )
{
    qpmad::MatrixIndex size = 50;

    qpmad_utils::getRandomPositiveDefinititeMatrix(H, size);
    H_copy = H;

    checkObjective();
}


BOOST_FIXTURE_TEST_CASE( objective03, SolverObjectiveFixture )
{
    qpmad::MatrixIndex size = 1;

    qpmad_utils::getRandomPositiveDefinititeMatrix(H, size);
    H_copy = H;
    h.setRandom(size);

    checkObjective();
}


//===========================================================================
// SolverGeneralEqualitiesFixture
//===========================================================================

class SolverGeneralEqualitiesFixture : public SolverObjectiveFixture
{
    public:
        void checkGeneralEqualities()
        {
            status = solver.solve(x, H, h, A, Alb, Alb);

            Eigen::MatrixXd Hi = H_copy.inverse();
            Eigen::VectorXd tmp;
            if (h.size() > 0)
            {
                tmp = Hi * A.transpose() * (A*Hi*A.transpose()).inverse() * (Alb + A*Hi*h) - Hi*h;
            }
            else
            {
                tmp = Hi * A.transpose() * (A*Hi*A.transpose()).inverse() * Alb;
            }

            BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
            BOOST_CHECK(x.isApprox(tmp, g_default_tolerance));
        }
};


BOOST_FIXTURE_TEST_CASE( general_equalities00, SolverGeneralEqualitiesFixture )
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 20;

    qpmad_utils::getRandomPositiveDefinititeMatrix(H, size);
    H_copy = H;
    h.setRandom(size);

    A = Eigen::MatrixXd::Identity(size,size).topRows(num_ctr);
    Alb.setRandom(num_ctr);

    checkGeneralEqualities();
}


BOOST_FIXTURE_TEST_CASE( general_equalities01, SolverGeneralEqualitiesFixture )
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 20;

    qpmad_utils::getRandomPositiveDefinititeMatrix(H, size);
    H_copy = H;

    A = Eigen::MatrixXd::Identity(size,size).topRows(num_ctr);
    Alb.setRandom(num_ctr);

    checkGeneralEqualities();
}


BOOST_FIXTURE_TEST_CASE( general_equalities02, SolverGeneralEqualitiesFixture )
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 50;

    qpmad_utils::getRandomPositiveDefinititeMatrix(H, size);
    H_copy = H;
    h.setRandom(size);

    A = Eigen::MatrixXd::Identity(size,size).topRows(num_ctr);
    Alb.setRandom(num_ctr);

    checkGeneralEqualities();
}


BOOST_FIXTURE_TEST_CASE( general_equalities03, SolverGeneralEqualitiesFixture )
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 51;

    qpmad_utils::getRandomPositiveDefinititeMatrix(H, size);
    H_copy = H;
    h.setRandom(size);

    A.setRandom(num_ctr, size);
    A.topRows(size) += Eigen::MatrixXd::Identity(size,size);
    Alb.setRandom(num_ctr);
    Alb(num_ctr-1) += 100.0;

    status = solver.solve(x, H, h, A, Alb, Alb);

    BOOST_CHECK_EQUAL(status, qpmad::Solver::INFEASIBLE_EQUALITY);
}


BOOST_FIXTURE_TEST_CASE( general_equalities04, SolverGeneralEqualitiesFixture )
{
    qpmad::MatrixIndex size = 50;
    qpmad::MatrixIndex num_ctr = 51;

    qpmad_utils::getRandomPositiveDefinititeMatrix(H, size);
    H_copy = H;
    h.setRandom(size);

    A.setZero(num_ctr, size);
    A.topRows(size) += Eigen::MatrixXd::Identity(size,size);
    A(num_ctr-1, 0) = 100.0;
    Alb.resize(num_ctr);
    Alb.setConstant(1.0);
    Alb(num_ctr-1) = 100.0;

    status = solver.solve(x, H, h, A, Alb, Alb);

    BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
    BOOST_CHECK(x.isApprox(Eigen::VectorXd::Ones(size), g_default_tolerance));
}


//===========================================================================
// SolverGeneralInequalitiesFixture
//===========================================================================

class SolverGeneralInequalitiesFixture : public SolverObjectiveFixture
{
    public:
        Eigen::VectorXd     x_ref;

    public:
        void checkGeneralInequalities()
        {
            status = solver.solve(x, H, h, A, Alb, Aub);

            BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
            BOOST_CHECK(x.isApprox(x_ref, g_default_tolerance));
        }
};


BOOST_FIXTURE_TEST_CASE( general_inequalities00, SolverGeneralInequalitiesFixture )
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 1;

    H.setIdentity(size, size);
    h.setOnes(size);

    A = Eigen::MatrixXd::Identity(size,size).topRows(1);
    Alb.resize(num_ctr);
    Aub.resize(num_ctr);
    Alb(0) = -10;
    Aub(0) = -5;

    x_ref.resize(size);
    x_ref << -5, -1;

    checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE( general_inequalities01, SolverGeneralInequalitiesFixture )
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    H.setIdentity(size, size);

    A.resize(num_ctr, size);
    A << 1, 0,
         1, 1,
         2, 0.5;
    Alb.resize(num_ctr);
    Aub.resize(num_ctr);
    Alb <<  0,
            1,
            1;
    Aub <<  0,
            1e10,
            1e10;

    x_ref.resize(size);
    x_ref << 0, 2;

    checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE( general_inequalities02, SolverGeneralInequalitiesFixture )
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    H.setIdentity(size, size);

    A.resize(num_ctr, size);
    A << 1, 0,
         1, 1,
         2, 0.5;
    Alb.resize(num_ctr);
    Aub.resize(num_ctr);
    Alb <<  0,
            -1e10,
            -1e10;
    Aub <<  0,
            -1,
            -1;

    x_ref.resize(size);
    x_ref << 0, -2;

    checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE( general_inequalities03, SolverGeneralInequalitiesFixture )
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    H.setIdentity(size, size);

    A.resize(num_ctr, size);
    A << 1, 0,
         -1, -1,
         -2, -0.5;
    Alb.resize(num_ctr);
    Aub.resize(num_ctr);
    Alb <<  0,
            -1e10,
            -1e10;
    Aub <<  0,
            -1,
            -1;

    x_ref.resize(size);
    x_ref << 0, 2;

    checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE( general_inequalities04, SolverGeneralInequalitiesFixture )
{
    qpmad::MatrixIndex size = 2;
    qpmad::MatrixIndex num_ctr = 3;

    H.setIdentity(size, size);

    A.resize(num_ctr, size);
    A << 1, 0,
         -1, -1,
         -2, -0.5;
    Alb.resize(num_ctr);
    Aub.resize(num_ctr);
    Alb <<  0,
            1,
            1;
    Aub <<  0,
            1e10,
            1e10;

    x_ref.resize(size);
    x_ref << 0, -2;

    checkGeneralInequalities();
}



BOOST_FIXTURE_TEST_CASE( general_inequalities20, SolverGeneralInequalitiesFixture )
{
    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_ctr = size;

    H.setIdentity(size, size);
    h.setOnes(size);


    A = Eigen::MatrixXd::Identity(num_ctr,size);
    Alb.resize(num_ctr);
    Aub.resize(num_ctr);
    Alb << 1, 2, 3, 4, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5;
    Aub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

    x_ref.resize(size);
    x_ref << 1.0, 2.0, 3.0, 4.0, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5;

    checkGeneralInequalities();
}


BOOST_FIXTURE_TEST_CASE( general_inequalities21, SolverGeneralInequalitiesFixture )
{
    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_ctr = size + 1;

    H.setIdentity(size, size);
    h.setOnes(size);


    A.resize(num_ctr, size);
    A.topRows(size) = Eigen::MatrixXd::Identity(size,size);
    A.row(num_ctr-1).setOnes();
    Alb.resize(num_ctr);
    Aub.resize(num_ctr);
    Alb << 1, 2, 3, 4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -1.5;
    Aub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1.5;

    x_ref.resize(size);
    x_ref << 1.0, 2.0, 3.0, 4.0, -0.71875, -0.71875,
            -0.71875, -0.71875, -0.71875, -0.71875,
            -0.71875, -0.71875, -0.71875, -0.71875,
            -0.71875, -0.71875, -0.71875, -0.71875,
            -0.71875, -0.71875;
    checkGeneralInequalities();
}


//===========================================================================
// SolverSimpleInequalitiesFixture
//===========================================================================

class SolverSimpleInequalitiesFixture : public SolverObjectiveFixture
{
    public:
        Eigen::VectorXd     x_ref;

    public:
        void checkSimpleInequalities()
        {
            status = solver.solve(x, H, h, lb, ub, A, Alb, Aub);

            BOOST_CHECK_EQUAL(status, qpmad::Solver::OK);
            BOOST_CHECK(x.isApprox(x_ref, g_default_tolerance));
        }
};


BOOST_FIXTURE_TEST_CASE( simple_inequalities00, SolverSimpleInequalitiesFixture )
{
    qpmad::MatrixIndex size = 2;

    H.setIdentity(size, size);
    h.setOnes(size);

    lb.resize(size);
    ub.resize(size);
    lb << -10, -g_infinity;
    ub << -5, g_infinity;

    x_ref.resize(size);
    x_ref << -5, -1;

    checkSimpleInequalities();
}


BOOST_FIXTURE_TEST_CASE( simple_inequalities20, SolverSimpleInequalitiesFixture )
{
    qpmad::MatrixIndex size = 20;

    H.setIdentity(size, size);
    h.setOnes(size);


    lb.resize(size);
    ub.resize(size);
    lb << 1, 2, 3, 4, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5;
    ub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

    x_ref.resize(size);
    x_ref << 1.0, 2.0, 3.0, 4.0, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5;

    checkSimpleInequalities();
}


BOOST_FIXTURE_TEST_CASE( simple_inequalities21, SolverSimpleInequalitiesFixture )
{
    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_eq_ctr = 1;

    H.setIdentity(size, size);
    h.setOnes(size);


    A.resize(num_eq_ctr, size);
    A.setOnes();
    Alb.resize(num_eq_ctr);
    Aub.resize(num_eq_ctr);
    Alb << -1.5;
    Aub << 1.5;

    lb.resize(size);
    ub.resize(size);
    lb << 1, 2, 3, 4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5;
    ub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

    x_ref.resize(size);
    x_ref << 1.0, 2.0, 3.0, 4.0, -0.71875, -0.71875,
            -0.71875, -0.71875, -0.71875, -0.71875,
            -0.71875, -0.71875, -0.71875, -0.71875,
            -0.71875, -0.71875, -0.71875, -0.71875,
            -0.71875, -0.71875;
    checkSimpleInequalities();
}
