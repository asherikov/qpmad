/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once


namespace qpmad
{
    class ChosenConstraint
    {
        public:
            double                      violation_;
            double                      dual_;
            MatrixIndex                 index_;
            bool                        is_lower_;
            bool                        is_simple_;

        public:
            ChosenConstraint()
            {
                dual_ = 0.0;
                violation_ = 0.0;
                index_ = 0;
                is_lower_ = true;
                is_simple_ = false;
            }
    };
}
