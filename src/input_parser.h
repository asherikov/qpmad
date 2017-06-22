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
    class InputParser
    {
        protected:
            MatrixIndex     primal_size_;
            MatrixIndex     h_size_;
            MatrixIndex     num_simple_bounds_;
            MatrixIndex     num_general_constraints_;


        protected:
            InputParser()
            {
                primal_size_ = 0;
                h_size_ = 0;
                num_simple_bounds_ = 0;
                num_general_constraints_ = 0;
            }


            template<   class t_DerivedH,
                        class t_Derivedh>
                void    parseObjective( const Eigen::PlainObjectBase<t_DerivedH> & H,
                                        const Eigen::PlainObjectBase<t_Derivedh> & h)
            {
                primal_size_ = H.rows();
                h_size_ = h.rows();

                QPMAD_ASSERT(   primal_size_ > 0,
                                "Hessian must not be empty.");
                QPMAD_ASSERT(   primal_size_ == H.cols(),
                                "Hessian must be square.");
                QPMAD_ASSERT(   ((primal_size_ == h_size_) && (1 == h.cols()))
                                || (0 == h_size_),
                                "Wrong size of h.");
            }


            template<class t_Derived>
                void    parseSimpleBounds(  const Eigen::PlainObjectBase<t_Derived> & lb,
                                            const Eigen::PlainObjectBase<t_Derived> & ub)
            {
                num_simple_bounds_ == lb.rows();

                QPMAD_ASSERT(   (0 == num_simple_bounds_) || (primal_size_ == num_simple_bounds_),
                                "Vector of lower simple bounds has wrong size.");
                QPMAD_ASSERT(   ub.rows() == num_simple_bounds_,
                                "Vector of upper simple bounds has wrong size.");

                QPMAD_ASSERT(   ((num_simple_bounds_ > 0) && (1 == lb.cols())) || (0 == lb.cols()),
                                "Vector of lower simple bounds has wrong size.");
                QPMAD_ASSERT(   ((num_simple_bounds_ > 0) && (1 == ub.cols())) || (0 == ub.cols()),
                                "Vector of upper simple bounds has wrong size.");
            }


            template<   class t_DerivedA,
                        class t_Derivedlb,
                        class t_Derivedub>
                void    parseGeneralConstraints(const Eigen::PlainObjectBase<t_DerivedA> & A,
                                                const Eigen::PlainObjectBase<t_Derivedlb> & lb,
                                                const Eigen::PlainObjectBase<t_Derivedub> & ub)
            {
                num_general_constraints_ = A.rows();

                QPMAD_ASSERT(   (A.cols() == primal_size_)
                                || ((0 == num_general_constraints_) && (0 == A.cols())),
                                "Matrix of general constraints has wrong size.");

                QPMAD_ASSERT(   lb.rows() == num_general_constraints_,
                                "Vector of lower bounds of general constraints has wrong size.");
                QPMAD_ASSERT(   ub.rows() == num_general_constraints_,
                                "Vector of upper bounds of general constraints has wrong size.");

                QPMAD_ASSERT(   ((num_general_constraints_ > 0) && (1 == lb.cols())) || (0 == lb.rows()),
                                "Vector of lower bounds of general constraints has wrong size.");
                QPMAD_ASSERT(   ((num_general_constraints_ > 0) && (1 == ub.cols())) || (0 == ub.rows()),
                                "Vector of upper bounds of general constraints has wrong size.");
            }
    };
}
