/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include "utf_common.h"

#include "../src/common.h"
#include "../src/givens.h"


class GivensFixture
{
    public:
        qpmad::GivensReflection givens;
        double a;
        double b;
        qpmad::GivensReflection::Type type;
        double tolerance;

    public:
        GivensFixture()
        {
            tolerance = 1e-12;
        }

        ~GivensFixture(){}
};


BOOST_FIXTURE_TEST_CASE( test_case00, GivensFixture )
{
    a = 0.0;
    b = 0.0;
    type = givens.computeAndApply(a, b, tolerance);

    BOOST_CHECK_EQUAL(a, 0.0);
    BOOST_CHECK_EQUAL(b, 0.0);
    BOOST_CHECK_EQUAL(type, qpmad::GivensReflection::COPY);
}


BOOST_FIXTURE_TEST_CASE( test_case01, GivensFixture )
{
    a = 1.0;
    b = 0.0;
    type = givens.computeAndApply(a, b, tolerance);

    BOOST_CHECK_EQUAL(a, 1.0);
    BOOST_CHECK_EQUAL(b, 0.0);
    BOOST_CHECK_EQUAL(type, qpmad::GivensReflection::COPY);
}


BOOST_FIXTURE_TEST_CASE( test_case02, GivensFixture )
{
    a = 0.0;
    b = 1.0;
    type = givens.computeAndApply(a, b, tolerance);

    BOOST_CHECK_EQUAL(a, 1.0);
    BOOST_CHECK_EQUAL(b, 0.0);
    BOOST_CHECK_EQUAL(type, qpmad::GivensReflection::SWAP);
}


BOOST_FIXTURE_TEST_CASE( test_case03, GivensFixture )
{
    a = 1.0;
    b = 1.0;
    type = givens.computeAndApply(a, b, tolerance);

    BOOST_CHECK_EQUAL(type, qpmad::GivensReflection::NONTRIVIAL);
}
