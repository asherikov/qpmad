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
    class BlockMatrixTests
    {
        protected:
            void createMatrix00_00()
            {
                eigenut::GenericBlockMatrix<2,2> M;
            }

            void createMatrix00_01()
            {
                eigenut::GenericBlockMatrix<0,0> M;
            }

            void createMatrix00_02()
            {
                eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                            eigenut::MatrixBlockSizeType::DYNAMIC> M;
            }

            void createMatrix00_03()
            {
                eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                            eigenut::MatrixBlockSizeType::DYNAMIC> M(2,2);
            }

            void createMatrix00_04()
            {
                eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                            1> M(2,2);
            }

            void createMatrix00_05()
            {
                eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                            1> M(2,eigenut::MatrixBlockSizeType::UNDEFINED);
            }


            void manipulateDynamicMatrix00_00()
            {
                eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                            eigenut::MatrixBlockSizeType::DYNAMIC> M(2,2);

                M.setZero(3,1);
            }

            void manipulateDynamicMatrix00_01()
            {
                eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                            eigenut::MatrixBlockSizeType::DYNAMIC> M(2,2);

                M.setZero(3);

                M(2) = Eigen::MatrixXd::Random(2,2);
            }


            void manipulateDynamicMatrix01_00()
            {
                std::vector< eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                                         eigenut::MatrixBlockSizeType::DYNAMIC> > vector_of_matrices;

                vector_of_matrices.resize(3);
            }


            void manipulateDynamicMatrix02_00()
            {
                eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                            eigenut::MatrixBlockSizeType::DYNAMIC>  M;
                M.setBlockSize(3, 2);
            }

            void manipulateDynamicMatrix02_01()
            {
                eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                            eigenut::MatrixBlockSizeType::DYNAMIC>  M;
                M.setBlockSize(3, 0);
            }

            void manipulateDynamicMatrix02_02()
            {
                eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                            2>  M;
                M.setBlockSize(3, eigenut::MatrixBlockSizeType::UNDEFINED);
            }

            void manipulateDynamicMatrix01_01()
            {
                std::vector< eigenut::GenericBlockMatrix< eigenut::MatrixBlockSizeType::DYNAMIC,
                                                         eigenut::MatrixBlockSizeType::DYNAMIC> > vector_of_matrices;

                vector_of_matrices.resize(3);
                vector_of_matrices[1].setBlockSize(3, 1);;
            }
    };


    BOOST_FIXTURE_TEST_CASE(Creation, BlockMatrixTests)
    {
        BOOST_REQUIRE_NO_THROW(createMatrix00_00());
        BOOST_REQUIRE_THROW(createMatrix00_01(), std::runtime_error);
        BOOST_REQUIRE_NO_THROW(createMatrix00_02());
        BOOST_REQUIRE_NO_THROW(createMatrix00_03());
        BOOST_REQUIRE_THROW(createMatrix00_04(), std::runtime_error);
        BOOST_REQUIRE_NO_THROW(createMatrix00_05());
    }


    BOOST_FIXTURE_TEST_CASE(Manipulation, BlockMatrixTests)
    {
        BOOST_REQUIRE_NO_THROW(manipulateDynamicMatrix00_00());
        BOOST_REQUIRE_NO_THROW(manipulateDynamicMatrix00_01());
        BOOST_REQUIRE_NO_THROW(manipulateDynamicMatrix02_00());
        BOOST_REQUIRE_THROW(manipulateDynamicMatrix02_01(), std::runtime_error);
        BOOST_REQUIRE_NO_THROW(manipulateDynamicMatrix02_02());
        BOOST_REQUIRE_NO_THROW(manipulateDynamicMatrix01_00());
        BOOST_REQUIRE_NO_THROW(manipulateDynamicMatrix01_01());
    }
}
