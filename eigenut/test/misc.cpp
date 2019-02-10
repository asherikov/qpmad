/**
    @file
    @author  Alexander Sherikov

    @copyright 2019 Alexander Sherikov. Licensed under the Apache License, Version 2.0.
    (see @ref LICENSE or http://www.apache.org/licenses/LICENSE-2.0)
    @copyright 2014-2017 INRIA. Licensed under the Apache License, Version 2.0.
    (see @ref LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#include <eigenut/all.h>
#include "utf_common.h"


namespace
{
    BOOST_AUTO_TEST_CASE(ConcatenateMatrixVertically)
    {
        // empty matrix
        Eigen::MatrixXd a;
        // full matrix
        Eigen::MatrixXd b;
        b.resize(2, 2);
        b << 0., 1., 1., 0.;
        // empty matrix
        Eigen::MatrixXd c;
        // full matrix
        Eigen::MatrixXd d;
        d.resize(2, 2);
        d << 0., 2., 2., 0.;

        // matrix to match against
        Eigen::MatrixXd match;
        match.resize(4, 2);
        match << 0., 1., 1., 0., 0., 2., 2., 0.;

        std::vector<Eigen::MatrixXd> matrices;
        matrices.push_back(a);
        matrices.push_back(b);
        matrices.push_back(c);
        matrices.push_back(d);

        Eigen::MatrixXd result;

        eigenut::concatenateMatricesVertically(result, matrices);
        BOOST_REQUIRE(result.isApprox(match,  1e-8));
    }


    BOOST_AUTO_TEST_CASE(ConcatenateMatrixHorizontallyTwo)
    {
        // empty matrix
        Eigen::MatrixXd a;
        // full matrix
        Eigen::MatrixXd b;
        b.resize(2, 2);
        b << 0., 1., 1., 0.;
        // full matrix
        Eigen::MatrixXd c;
        c.resize(2, 2);
        c << 0., 2., 2., 0.;

        // matrix to match against
        Eigen::MatrixXd match;
        match.resize(2, 4);
        match << 0., 1., 0., 2., 1., 0., 2., 0.;

        Eigen::MatrixXd result;

        eigenut::concatenateMatricesHorizontally(result, a, b);
        BOOST_REQUIRE(result.isApprox(b,  1e-8));

        eigenut::concatenateMatricesHorizontally(result, b, c);
        BOOST_REQUIRE(result.isApprox(match,  1e-8));
    }


    BOOST_AUTO_TEST_CASE(ConcatenateMatrixHorizontallyThree)
    {
        // empty matrix
        Eigen::MatrixXd a;
        // full matrix
        Eigen::MatrixXd b;
        b.resize(2, 2);
        b << 0., 1., 1., 0.;
        // empty matrix
        Eigen::MatrixXd c;
        // full matrix
        Eigen::MatrixXd d;
        d.resize(2, 2);
        d << 0., 2., 2., 0.;
        // full matrix
        Eigen::MatrixXd e;
        e.resize(2, 2);
        e << 0., 3., 3., 0.;

        // matrix to match against
        Eigen::MatrixXd match;
        match.resize(2, 6);
        match << 0., 1., 0., 2., 0., 3., 1., 0., 2., 0., 3., 0.;

        Eigen::MatrixXd result;

        eigenut::concatenateMatricesHorizontally(result, a, b, c);
        BOOST_REQUIRE(result.isApprox(b,  1e-8));

        eigenut::concatenateMatricesHorizontally(result, b, d, e);
        BOOST_REQUIRE(result.isApprox(match,  1e-8));
    }
}
