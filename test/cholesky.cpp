/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include "utf_common.h"


#include <qpmad/common.h>
#include <qpmad/cholesky.h>


template<int t_size>
    class CholeskyResultFixture
{
    public:
        Eigen::MatrixXd         M;
        Eigen::VectorXd         v;


    public:
        CholeskyResultFixture()
        {
            qpmad_utils::getRandomPositiveDefinititeMatrix(M, t_size);
            v.setRandom(t_size);
        }


        void compareFactorWithEigen()
        {
            Eigen::LLT< Eigen::MatrixXd > llt;
            llt.compute(M);
            qpmad::CholeskyFactorization::compute(M);

            Eigen::MatrixXd L_qpmad = M.triangularView<Eigen::Lower>();
            Eigen::MatrixXd L_eigen = llt.matrixL();

            BOOST_CHECK(L_qpmad.isApprox(L_eigen, g_default_tolerance));
        }


        void compareSolutionWithEigen()
        {
            Eigen::VectorXd x_qpmad;
            Eigen::VectorXd x_eigen;

            Eigen::LLT< Eigen::MatrixXd > llt;
            llt.compute(M);
            x_eigen = llt.solve(v);

            qpmad::CholeskyFactorization::compute(M);
            qpmad::CholeskyFactorization::solve(x_qpmad, M, v);

            BOOST_CHECK(x_qpmad.isApprox(x_eigen, g_default_tolerance));
        }
};



BOOST_FIXTURE_TEST_CASE( test_factors00, CholeskyResultFixture<50> )
{
    compareFactorWithEigen();
}

BOOST_FIXTURE_TEST_CASE( test_factors01, CholeskyResultFixture<1> )
{
    compareFactorWithEigen();
}

BOOST_FIXTURE_TEST_CASE( test_solution00, CholeskyResultFixture<50> )
{
    compareSolutionWithEigen();
}



template<int t_size>
    class CholeskyTimeFixture
{
    public:
        std::vector< CholeskyResultFixture<t_size> > matrices;

    public:
        void compareTimeWithEigen(std::size_t num_repetitions = 100)
        {
            boost::timer::cpu_timer     qpmad_timer;
            boost::timer::cpu_timer     eigen_timer;

            matrices.resize(num_repetitions);


            Eigen::LLT< Eigen::MatrixXd > llt(t_size);
            eigen_timer.start();
            for (std::size_t i = 0; i < num_repetitions; ++i)
            {
                llt.compute(matrices[i].M);
            }
            eigen_timer.stop();


            qpmad_timer.start();
            for (std::size_t i = 0; i < num_repetitions; ++i)
            {
                qpmad::CholeskyFactorization::compute(matrices[i].M);
            }
            qpmad_timer.stop();


            BOOST_TEST_MESSAGE( "Matrix size " + boost::lexical_cast<std::string>(t_size)
                                + " ||| QPMAD time : " + boost::lexical_cast<std::string>(qpmad_timer.elapsed().wall)
                                + " ||| Eigen time : " + boost::lexical_cast<std::string>(eigen_timer.elapsed().wall));
            BOOST_WARN(qpmad_timer.elapsed().wall < eigen_timer.elapsed().wall);
        }
};



BOOST_FIXTURE_TEST_CASE( test_time00, CholeskyTimeFixture<5> )
{
    compareTimeWithEigen();
}

BOOST_FIXTURE_TEST_CASE( test_time01, CholeskyTimeFixture<50> )
{
    compareTimeWithEigen();
}

BOOST_FIXTURE_TEST_CASE( test_time02, CholeskyTimeFixture<100> )
{
    compareTimeWithEigen();
}

/*
BOOST_FIXTURE_TEST_CASE( test_time03, CholeskyTimeFixture<500> )
{
    compareTimeWithEigen(5);
}
*/
