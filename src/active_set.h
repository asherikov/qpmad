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
    class ActiveSet
    {
        public:
            std::vector<MatrixIndex>                active_constraints_indices_;
            MatrixIndex                             size_;
            MatrixIndex                             num_equalities_;
            MatrixIndex                             num_inequalities_;


        public:
            void initialize(const MatrixIndex max_size)
            {
                active_constraints_indices_.resize(max_size);
                size_ = 0;
                num_equalities_ = 0;
                num_inequalities_ = 0;
            }


            MatrixIndex getIndex(const MatrixIndex index) const
            {
                return(active_constraints_indices_[index]);
            }


            bool hasEmptySpace() const
            {
                return(static_cast<std::size_t>(size_) < active_constraints_indices_.size());
            }


            void addEquality(const MatrixIndex index)
            {
                active_constraints_indices_[size_] = index;
                ++size_;
                ++num_equalities_;
            }


            void addInequality(const MatrixIndex index)
            {
                active_constraints_indices_[size_] = index;
                ++size_;
                ++num_inequalities_;
            }


            void removeInequality(const MatrixIndex index)
            {
                if (size_ - index > 1)
                {
                    // deactivated constraint is not the last one added
                    std::copy(  active_constraints_indices_.begin() + index + 1,
                                active_constraints_indices_.begin() + size_,
                                active_constraints_indices_.begin() + index);
                }
                --size_;
                --num_inequalities_;
            }
    };
}
