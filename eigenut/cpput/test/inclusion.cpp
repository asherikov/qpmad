/**
    @file
    @author  Alexander Sherikov

    @brief
*/

#include <cpput/config.h>
#include <cpput/all.h>
#include "boost_utf_common.h"


BOOST_AUTO_TEST_CASE(Inclusion)
{
    cpput::print_backtrace();
    BOOST_REQUIRE(true);
}
