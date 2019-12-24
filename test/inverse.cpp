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
#include <qpmad/inverse.h>


template<int t_size>
    class InverseFixture
{
    public:
        Eigen::MatrixXd         M;


    public:
        InverseFixture()
        {
            qpmad_utils::getRandomPositiveDefinititeMatrix(M, t_size);
            qpmad::CholeskyFactorization::compute(M);
        }


        void computeAndCheck()
        {
            Eigen::MatrixXd         M_inv;
            M_inv.resize(t_size, t_size);
            qpmad::TriangularInversion::compute(M_inv, M);

            Eigen::MatrixXd     tmp1 = M.transpose().template triangularView<Eigen::Upper>();
            Eigen::MatrixXd     tmp2 = M_inv.triangularView<Eigen::Upper>();

            BOOST_CHECK((tmp1*tmp2).isApprox(Eigen::MatrixXd::Identity(t_size, t_size), g_default_tolerance));
        }
};



BOOST_FIXTURE_TEST_CASE( test_result00, InverseFixture<50> )
{
    computeAndCheck();
}



template<int t_size>
    class InverseTimeFixture : public InverseFixture<t_size>
{
    public:
        using InverseFixture<t_size>::M;

    public:
        void measureTime(std::size_t num_repetitions = 100)
        {
            boost::timer::cpu_timer     qpmad_timer;

            Eigen::MatrixXd         M_inv;
            M_inv.resize(t_size, t_size);

            qpmad_timer.start();
            for (std::size_t i = 0; i < num_repetitions; ++i)
            {
                qpmad::TriangularInversion::compute(M_inv, M);
            }
            qpmad_timer.stop();


            BOOST_TEST_MESSAGE( "Matrix size " + boost::lexical_cast<std::string>(t_size)
                                + " ||| QPMAD time : " + boost::lexical_cast<std::string>(qpmad_timer.elapsed().wall));
        }
};



BOOST_FIXTURE_TEST_CASE( test_time00, InverseTimeFixture<50> )
{
    measureTime();
}
