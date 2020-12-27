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
    template<int t_primal_size>
    class ActiveSet
    {
    public:
        Eigen::Array<qpmad_utils::EigenIndex, t_primal_size, 1> active_constraints_indices_;
        qpmad_utils::EigenIndex size_;
        qpmad_utils::EigenIndex num_equalities_;
        qpmad_utils::EigenIndex num_inequalities_;


    public:
        void initialize(const qpmad_utils::EigenIndex max_size)
        {
            active_constraints_indices_.resize(max_size);
            size_ = 0;
            num_equalities_ = 0;
            num_inequalities_ = 0;
        }


        qpmad_utils::EigenIndex getIndex(const qpmad_utils::EigenIndex index) const
        {
            return (active_constraints_indices_[index]);
        }


        bool hasEmptySpace() const
        {
            return (size_ < active_constraints_indices_.size());
        }


        void addEquality(const qpmad_utils::EigenIndex index)
        {
            active_constraints_indices_[size_] = index;
            ++size_;
            ++num_equalities_;
        }


        void addInequality(const qpmad_utils::EigenIndex index)
        {
            active_constraints_indices_[size_] = index;
            ++size_;
            ++num_inequalities_;
        }


        void removeInequality(const qpmad_utils::EigenIndex index)
        {
            dropElementWithoutResize(active_constraints_indices_, index, size_);
            --size_;
            --num_inequalities_;
        }
    };
}  // namespace qpmad
