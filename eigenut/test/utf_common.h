/**
    @file
    @author  Alexander Sherikov

    @copyright 2019 Alexander Sherikov. Licensed under the Apache License, Version 2.0.
    (see @ref LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#define BOOST_TEST_MODULE eigenut
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/timer/timer.hpp>


struct GlobalFixtureConfig
{
    GlobalFixtureConfig()
    {
        //boost::unit_test::unit_test_log.set_threshold_level( boost::unit_test::log_successful_tests );
        boost::unit_test::results_reporter::set_level( boost::unit_test::DETAILED_REPORT );
    }
    ~GlobalFixtureConfig() {}
};


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
// Depending on Boost version a compiler may issue a warning about extra ';',
// at the same time, compilation may fail on some systems if ';' is omitted.
BOOST_GLOBAL_FIXTURE( GlobalFixtureConfig ) ;
#pragma GCC diagnostic pop

