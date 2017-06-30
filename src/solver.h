/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/


#pragma once

#include <vector>

#include "common.h"
#include "cholesky.h"
#include "givens.h"
#include "input_parser.h"
#include "inverse.h"
#include "solver_parameters.h"
#include "constraint_status.h"
#include "active_set.h"
#include "factorization_data.h"

#ifdef QPMAD_ENABLE_TRACING
#include "testing.h"
#endif

namespace qpmad
{
    class Solver : public InputParser
    {
        public:
            enum ReturnStatus
            {
                OK = 0,
                INCONSISTENT = 1,
                INFEASIBLE_EQUALITY = 2,
                INFEASIBLE_INEQUALITY = 3,
                MAXIMAL_NUMBER_OF_ITERATIONS = 4
            };


        public:
            ReturnStatus    solve(  QPVector     & primal,
                                    QPMatrix     & H,
                                    const QPVector & h,
                                    const QPMatrix & A,
                                    const QPVector & Alb,
                                    const QPVector & Aub)
            {
                return (solve(  primal, H, h,
                                Eigen::VectorXd(), Eigen::VectorXd(),
                                A, Alb, Aub, SolverParameters()));
            }


            ReturnStatus    solve(  QPVector     & primal,
                                    QPMatrix     & H,
                                    const QPVector & h,
                                    const QPVector & lb,
                                    const QPVector & ub,
                                    const QPMatrix & A,
                                    const QPVector & Alb,
                                    const QPVector & Aub)
            {
                return (solve(  primal, H, h,
                                lb, ub,
                                A, Alb, Aub, SolverParameters()));
            }


            ReturnStatus    solve(  QPVector     & primal,
                                    QPMatrix     & H,
                                    const QPVector & h,
                                    const QPVector & lb,
                                    const QPVector & ub)
            {
                return (solve(  primal, H, h,
                                lb, ub,
                                Eigen::MatrixXd(), Eigen::VectorXd(), Eigen::VectorXd(), SolverParameters()));
            }



            ReturnStatus    solve(  QPVector     & primal,
                                    QPMatrix     & H,
                                    const QPVector & h,
                                    const QPVector & lb,
                                    const QPVector & ub,
                                    const QPMatrix & A,
                                    const QPVector & Alb,
                                    const QPVector & Aub,
                                    const SolverParameters & param)
            {
                QPMAD_TRACE(std::setprecision(std::numeric_limits<double>::digits10));

                machinery_initialized_ = false;

                parseObjective(H, h);
                parseSimpleBounds(lb, ub);
                parseGeneralConstraints(A, Alb, Aub);


                switch(param.hessian_type_)
                {
                    case SolverParameters::HESSIAN_LOWER_TRIANGULAR:
                        CholeskyFactorization::compute(H);
                        // no break here!
                    case SolverParameters::HESSIAN_CHOLESKY_FACTOR:
                        break;

                    default:
                        QPMAD_THROW("Malformed solver parameters!");
                        break;
                }


                // Unconstrained optimum
                if (h_size_ > 0)
                {
                    CholeskyFactorization::solve(primal, H, -h);
                }
                else
                {
                    primal.setZero(primal_size_);
                }


                num_constraints_ = num_simple_bounds_ + num_general_constraints_;

                if (0 == num_constraints_)
                {
                    // exit early -- avoid unnecessary memory allocations
                    return (OK);
                }



                // check consistency of general constraints and activate
                // equality constraints
                constraints_status_.resize(num_constraints_);
                MatrixIndex     num_equalities = 0;
                for (MatrixIndex i = 0; i < num_constraints_; ++i)
                {
                    double lb_i;
                    double ub_i;

                    if (i < num_simple_bounds_)
                    {
                        lb_i = lb(i);
                        ub_i = ub(i);
                    }
                    else
                    {
                        lb_i = Alb(i-num_simple_bounds_);
                        ub_i = Aub(i-num_simple_bounds_);
                    }


                    if (lb_i - param.tolerance_ > ub_i)
                    {
                        constraints_status_[i] = ConstraintStatus::INCONSISTENT;
                        return(INCONSISTENT);
                    }

                    if (std::abs(lb_i - ub_i) > param.tolerance_)
                    {
                        constraints_status_[i] = ConstraintStatus::INACTIVE;
                    }
                    else
                    {
                        constraints_status_[i] = ConstraintStatus::EQUALITY;
                        ++num_equalities;


                        double violation;
                        if (i < num_simple_bounds_)
                        {
                            violation = lb_i - primal(i);
                        }
                        else
                        {
                            violation = lb_i - A.row(i-num_simple_bounds_) * primal;
                        }

                        initializeMachineryLazy(H);

                        // if 'primal_size_' constraints are already activated
                        // all other constraints are linearly dependent
                        if (active_set_.hasEmptySpace())
                        {
                            double ctr_i_dot_primal_step_direction;

                            if (i < num_simple_bounds_)
                            {
                                factorization_data_.computeEqualityPrimalStep(
                                        primal_step_direction_, i, active_set_.size_);

                                ctr_i_dot_primal_step_direction = primal_step_direction_(i);
                            }
                            else
                            {
                                factorization_data_.computeEqualityPrimalStep(
                                        primal_step_direction_, A.row(i-num_simple_bounds_), active_set_.size_);

                                ctr_i_dot_primal_step_direction = A.row(i-num_simple_bounds_) * primal_step_direction_;
                            }

                            // if step direction is a zero vector, constraint is
                            // linearly dependent with previously added constraints
                            if (ctr_i_dot_primal_step_direction < -param.tolerance_)
                            {
                                double primal_step_length_ = violation / ctr_i_dot_primal_step_direction;

                                primal.noalias() += primal_step_length_ * primal_step_direction_;

                                if (false == factorization_data_.update(active_set_.size_, param.tolerance_))
                                {
                                    QPMAD_THROW("Failed to add an equality constraint -- is this possible?");
                                }
                                active_set_.addEquality(i);

                                continue;
                            }
                            // otherwise -- linear dependence
                        }

                        // this point is reached if constraint is linearly dependent

                        // check if this constraint is actually satisfied
                        if (std::abs(violation) > param.tolerance_)
                        {
                            // nope it is not
                            constraints_status_[i] = ConstraintStatus::INCONSISTENT;
                            return (INFEASIBLE_EQUALITY);
                        }
                        // otherwise keep going
                    }
                }


                if (num_equalities == num_constraints_)
                {
                    // exit early -- avoid unnecessary memory allocations
                    return (OK);
                }


                dual_.resize(primal_size_);
                dual_step_direction_.resize(primal_size_);


                ChosenConstraint chosen_ctr;
                chosen_ctr = chooseConstraint(primal, lb, ub, A, Alb, Aub, param.tolerance_);
                ReturnStatus return_status = MAXIMAL_NUMBER_OF_ITERATIONS;
                for(int iter = 0;
                    (iter < param.max_iter_) || (param.max_iter_ < 0);
                    ++iter)
                {
                    QPMAD_TRACE(">>>>>>>>>"  << iter << "<<<<<<<<<");
#ifdef QPMAD_ENABLE_TRACING
                    testing::computeObjective(H, h, primal);
#endif
                    QPMAD_TRACE("||| Chosen ctr index = " << chosen_ctr.index_);
                    QPMAD_TRACE("||| Chosen ctr dual = " << chosen_ctr.dual_);
                    QPMAD_TRACE("||| Chosen ctr violation = " << chosen_ctr.violation_);


                    if (std::abs(chosen_ctr.violation_) < param.tolerance_)
                    {
                        // all constraints are satisfied
                        return_status = OK;
                        break;
                    }


                    initializeMachineryLazy(H);

                    double chosen_ctr_dot_primal_step_direction = 0.0;
                    if (active_set_.hasEmptySpace())
                    {
                        // compute step direction in primal & dual space
                        if (chosen_ctr.index_ < num_simple_bounds_)
                        {
                            factorization_data_.computeInequalitySteps(
                                    primal_step_direction_,
                                    dual_step_direction_,
                                    chosen_ctr.index_,
                                    chosen_ctr.upper_or_lower_,
                                    active_set_);
                            chosen_ctr_dot_primal_step_direction = primal_step_direction_(chosen_ctr.index_);
                        }
                        else
                        {
                            factorization_data_.computeInequalitySteps(
                                    primal_step_direction_,
                                    dual_step_direction_,
                                    A.row(chosen_ctr.index_ - num_simple_bounds_),
                                    chosen_ctr.upper_or_lower_,
                                    active_set_);
                            chosen_ctr_dot_primal_step_direction =
                                A.row(chosen_ctr.index_ - num_simple_bounds_) * primal_step_direction_;
                        }
                    }
                    else
                    {
                        // compute step direction in dual space only
                        // primal vector cannot change until we deactive something
                        if (chosen_ctr.index_ < num_simple_bounds_)
                        {
                            factorization_data_.computeInequalityDualStep(
                                    dual_step_direction_,
                                    chosen_ctr.index_,
                                    chosen_ctr.upper_or_lower_,
                                    active_set_);
                        }
                        else
                        {
                            factorization_data_.computeInequalityDualStep(
                                    dual_step_direction_,
                                    A.row(chosen_ctr.index_ - num_simple_bounds_),
                                    chosen_ctr.upper_or_lower_,
                                    active_set_);
                        }
                    }


                    // check dual feasibility
                    MatrixIndex dual_blocking_index = primal_size_;
                    double dual_step_length = std::numeric_limits<double>::infinity();
                    for (MatrixIndex i = active_set_.num_equalities_; i < active_set_.size_; ++i)
                    {
                        if (dual_step_direction_(i) < -param.tolerance_)
                        {
                            double dual_step_length_i = -dual_(i) / dual_step_direction_(i);
                            if (dual_step_length_i < dual_step_length)
                            {
                                dual_step_length = dual_step_length_i;
                                dual_blocking_index = i;
                            }
                        }
                    }


#ifdef QPMAD_ENABLE_TRACING
                    testing::checkLagrangeMultipliers(
                            H, h, primal, A,
                            active_set_,
                            num_simple_bounds_,
                            constraints_status_,
                            dual_,
                            dual_step_direction_);
#endif


                    if ( active_set_.hasEmptySpace()
                        // if step direction is a zero vector, constraint is
                        // linearly dependent with previously added constraints
                        && (std::abs(chosen_ctr_dot_primal_step_direction) > param.tolerance_) )
                    {
                        double step_length = - chosen_ctr.violation_ / chosen_ctr_dot_primal_step_direction;

                        QPMAD_TRACE("======================");
                        QPMAD_TRACE("||| Primal step length = " << step_length);
                        QPMAD_TRACE("||| Dual step length = " << dual_step_length);
                        QPMAD_TRACE("======================");


                        bool partial_step = false;
                        QPMAD_ASSERT(   (step_length >= 0.0)
                                        && (dual_step_length >= 0.0),
                                        "Non-negative step lengths expected.");
                        if (dual_step_length <= step_length)
                        {
                            step_length = dual_step_length;
                            partial_step = true;
                        }


                        primal.noalias() += step_length * primal_step_direction_;

                        dual_.segment(active_set_.num_equalities_, active_set_.num_inequalities_).noalias()
                            += step_length
                                * dual_step_direction_.segment(active_set_.num_equalities_, active_set_.num_inequalities_);
                        chosen_ctr.dual_ += step_length;
                        chosen_ctr.violation_ += step_length * chosen_ctr_dot_primal_step_direction;

                        if (false == factorization_data_.update(active_set_.size_, param.tolerance_))
                        {
                            QPMAD_THROW("Failed to add an inequality constraint -- is this possible?");
                        }

                        QPMAD_TRACE("||| Chosen ctr dual = " << chosen_ctr.dual_);
                        QPMAD_TRACE("||| Chosen ctr violation = " << chosen_ctr.violation_);


                        if ((partial_step)
                            // if violation is almost zero -- assume that a full step is made
                            && (std::abs(chosen_ctr.violation_) > param.tolerance_) )
                        {
                            QPMAD_TRACE("||| PARTIAL STEP");
                            // deactivate blocking constraint
                            constraints_status_[ active_set_.getIndex(dual_blocking_index) ] = ConstraintStatus::INACTIVE;

                            dropElementWithoutResize(dual_, dual_blocking_index, active_set_.size_);

                            factorization_data_.downdate(dual_blocking_index, active_set_.size_, param.tolerance_);

                            active_set_.removeInequality(dual_blocking_index);
                        }
                        else
                        {
                            QPMAD_TRACE("||| FULL STEP");
                            // activate constraint
                            constraints_status_[chosen_ctr.index_] = chosen_ctr.upper_or_lower_;
                            dual_(active_set_.size_) = chosen_ctr.dual_;
                            active_set_.addInequality(chosen_ctr.index_);

                            chosen_ctr = chooseConstraint(primal, lb, ub, A, Alb, Aub, param.tolerance_);
                        }
                    }
                    else
                    {
                        if (dual_blocking_index == primal_size_)
                        {
                            return_status = INFEASIBLE_INEQUALITY;
                            break;
                        }
                        else
                        {
                            QPMAD_TRACE("======================");
                            QPMAD_TRACE("||| Dual step length = " << dual_step_length);
                            QPMAD_TRACE("======================");

                            // otherwise -- deactivate
                            dual_.segment(active_set_.num_equalities_, active_set_.num_inequalities_).noalias()
                                += dual_step_length
                                    * dual_step_direction_.segment(active_set_.num_equalities_, active_set_.num_inequalities_);
                            chosen_ctr.dual_ += dual_step_length;

                            constraints_status_[ active_set_.getIndex(dual_blocking_index) ] = ConstraintStatus::INACTIVE;

                            dropElementWithoutResize(dual_, dual_blocking_index, active_set_.size_);

                            factorization_data_.downdate(dual_blocking_index, active_set_.size_, param.tolerance_);

                            active_set_.removeInequality(dual_blocking_index);
                        }
                    }
                }

#ifdef QPMAD_ENABLE_TRACING
                if (machinery_initialized_)
                {
                    testing::printActiveSet(active_set_, constraints_status_, dual_);

                    testing::checkLagrangeMultipliers(
                            H, h, primal, A,
                            active_set_,
                            num_simple_bounds_,
                            constraints_status_,
                            dual_);
                }
                else
                {
                    QPMAD_TRACE("||| NO ACTIVE CONSTRAINTS");
                }
#endif
                return(return_status);
            }


        private:
            class ChosenConstraint
            {
                public:
                    double                      violation_;
                    double                      dual_;
                    MatrixIndex                 index_;
                    ConstraintStatus::Status    upper_or_lower_;

                public:
                    ChosenConstraint()
                    {
                        dual_ = 0.0;
                        violation_ = 0.0;
                        index_ = 0;
                        upper_or_lower_ = ConstraintStatus::UNDEFINED;
                    }
            };


        private:
            MatrixIndex     num_constraints_;
            bool            machinery_initialized_;

            ActiveSet           active_set_;
            FactorizationData   factorization_data_;

            QPVector    dual_;

            QPVector    primal_step_direction_;
            QPVector    dual_step_direction_;

            QPVector    general_ctr_dot_primal_;

            std::vector<ConstraintStatus::Status>   constraints_status_;


        private:
            template <class t_MatrixType>
                void initializeMachineryLazy(const t_MatrixType &H)
            {
                if (false == machinery_initialized_)
                {
                    active_set_.initialize(primal_size_);
                    factorization_data_.initialize(H, primal_size_);
                    primal_step_direction_.resize(primal_size_);
                    general_ctr_dot_primal_.resize(num_general_constraints_);

                    machinery_initialized_ = true;
                }
            }


            ChosenConstraint chooseConstraint(
                    const QPVector & primal,
                    const QPVector & lb,
                    const QPVector & ub,
                    const QPMatrix & A,
                    const QPVector & Alb,
                    const QPVector & Aub,
                    const double tolerance)
            {
                ChosenConstraint chosen_ctr;


                for(MatrixIndex i = 0; i < num_simple_bounds_; ++i)
                {
                    if(ConstraintStatus::INACTIVE == constraints_status_[i])
                    {
                        checkConstraintViolation(
                                chosen_ctr, i, lb(i), ub(i), primal(i));
                    }
                }

                if ((std::abs(chosen_ctr.violation_) < tolerance) && (num_general_constraints_ > 0))
                {
                    general_ctr_dot_primal_.noalias() = A * primal;
                    for(MatrixIndex i = num_simple_bounds_; i < num_constraints_; ++i)
                    {
                        if (ConstraintStatus::INACTIVE == constraints_status_[i])
                        {
                            checkConstraintViolation(   chosen_ctr,
                                                        i,
                                                        Alb(i - num_simple_bounds_),
                                                        Aub(i - num_simple_bounds_),
                                                        general_ctr_dot_primal_(i-num_simple_bounds_));
                        }
                    }
                }
                return (chosen_ctr);
            }


            void checkConstraintViolation(
                    ChosenConstraint & chosen_ctr,
                    const MatrixIndex i,
                    const double lb_i,
                    const double ub_i,
                    const double ctr_i_dot_primal)
            {
                double ctr_violation_i = ctr_i_dot_primal - lb_i;
                if (ctr_violation_i < -std::abs(chosen_ctr.violation_))
                {
                    chosen_ctr.upper_or_lower_ = ConstraintStatus::ACTIVE_LOWER_BOUND;
                    chosen_ctr.violation_ = ctr_violation_i;
                    chosen_ctr.index_ = i;
                }
                else
                {
                    ctr_violation_i = ctr_i_dot_primal - ub_i;
                    if (ctr_violation_i > std::abs(chosen_ctr.violation_))
                    {
                        chosen_ctr.upper_or_lower_ = ConstraintStatus::ACTIVE_UPPER_BOUND;
                        chosen_ctr.violation_ = ctr_violation_i;
                        chosen_ctr.index_ = i;
                    }
                }
            }
    };
}
