/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/


#include <iostream>
#include <fstream>
#include <math.h>
#include <limits>

#include <qpmad/solver.h>
#include "mex.h"

using namespace std;

enum qpStatus
{
    QP_OK = 0,
    QP_INFEASIBLE = 1,
    QP_FAILURE = 2
};



void mexFunction( int num_output, mxArray *output[], int num_input, const mxArray *input[] )
{
    const mxArray *H = input[0];
    const mxArray *g = input[1];
    const mxArray *lb = input[2];
    const mxArray *ub = input[3];
    const mxArray *A = input[4];
    const mxArray *Alb = input[5];
    const mxArray *Aub = input[6];


    mxArray *x = NULL;


    int num_var = mxGetM(H);
    int num_ctr = mxGetM(A);
    int num_simple_bounds = mxGetM(lb);
    x = mxCreateDoubleMatrix(num_var, 1, mxREAL);


    Eigen::MatrixXd eH     = Eigen::Map<Eigen::MatrixXd>  ((double*) mxGetPr(H),   num_var, num_var);
    Eigen::VectorXd eg     = Eigen::Map<Eigen::VectorXd>  ((double*) mxGetPr(g),   num_var);
    Eigen::VectorXd elb    = Eigen::Map<Eigen::VectorXd>  ((double*) mxGetPr(lb),  num_simple_bounds);
    Eigen::VectorXd eub    = Eigen::Map<Eigen::VectorXd>  ((double*) mxGetPr(ub),  num_simple_bounds);
    Eigen::MatrixXd eA     = Eigen::Map<Eigen::MatrixXd>  ((double*) mxGetPr(A),   num_ctr, num_var);
    Eigen::VectorXd eAlb   = Eigen::Map<Eigen::VectorXd>  ((double*) mxGetPr(Alb), num_ctr);
    Eigen::VectorXd eAub   = Eigen::Map<Eigen::VectorXd>  ((double*) mxGetPr(Aub), num_ctr);
    Eigen::VectorXd ex     = Eigen::Map<Eigen::VectorXd>  ((double*) mxGetPr(x),   num_var);

// solve the problem
    qpmad::Solver   solver;
    qpStatus        qp_status;

    try
    {
        qpmad::Solver::ReturnStatus return_value = solver.solve(ex, eH, eg, elb, eub, eA, eAlb, eAub);
        if (return_value == qpmad::Solver::OK)
        {
            qp_status = QP_OK;
            for(std::size_t i = 0; i < num_var; ++i)
            {
                ((double*) mxGetPr(x))[i] = ex[i];
            }
        }
        else
        {
            qp_status = QP_INFEASIBLE;
        }
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
        qp_status = QP_FAILURE;
    }


// process results
    // solution
    output[0] = mxDuplicateArray(x);


    // info
    int num_info_fields = 1;
    const char *info_field_names[] = {
        "status"
    };

    output[1] = mxCreateStructMatrix(1, 1, num_info_fields, info_field_names);

    mxArray *info_status = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    ((INT32_T *) mxGetData (info_status))[0] = static_cast <int> (qp_status);
    mxSetField (output[1], 0, "status", info_status);

    return;
}
