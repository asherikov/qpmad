/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once

#include <qpmad/config.h>

// tracing functions allocate stuff
#ifndef QPMAD_ENABLE_TRACING
#    define EIGEN_RUNTIME_NO_MALLOC
#endif


#define BOOST_TEST_MODULE qpmad
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/timer/timer.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

#include "eigenut_misc.h"


const double g_default_tolerance = 1e-12;
const double g_infinity = 1e+20;


struct GlobalFixtureConfig
{
    GlobalFixtureConfig()
    {
        // boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_successful_tests);
        boost::unit_test::results_reporter::set_level(boost::unit_test::DETAILED_REPORT);
    }
    ~GlobalFixtureConfig()
    {
    }
};


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
// Depending on Boost version a compiler may issue a warning about extra ';',
// at the same time, compilation may fail on some systems if ';' is omitted.
BOOST_GLOBAL_FIXTURE(GlobalFixtureConfig);
#pragma GCC diagnostic pop

namespace qpmad_utils
{
    template <class t_Solver>
    class HessianSolution
    {
    public:
        Eigen::VectorXd x;
        Eigen::MatrixXd H;
        typename t_Solver::ReturnStatus status;

    public:
        template <typename t_Index>
        void initRandomHessian(const t_Index size)
        {
            qpmad_utils::getRandomPositiveDefiniteMatrix(H, size);
        }

        template <typename t_Index>
        void initIdentityHessian(const t_Index size)
        {
            H.setIdentity(size, size);
        }

        void resizeSolution()
        {
            x.resize(H.rows());
        }
    };
}  // namespace qpmad_utils
