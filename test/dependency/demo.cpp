/**
    @file
    @author  Alexander Sherikov

    @copyright 2019 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/


#include <qpmad/solver.h>


int main()
{
    Eigen::VectorXd         x;
    Eigen::MatrixXd         H;
    Eigen::VectorXd         h;
    Eigen::MatrixXd         A;
    Eigen::VectorXd         Alb;
    Eigen::VectorXd         Aub;
    Eigen::VectorXd         lb;
    Eigen::VectorXd         ub;


    qpmad::MatrixIndex size = 20;
    qpmad::MatrixIndex num_ctr = 1;

    H.setIdentity(size, size);
    h.setOnes(size);


    A.resize(num_ctr, size);
    A.setOnes();
    Alb.resize(num_ctr);
    Aub.resize(num_ctr);
    Alb << -1.5;
    Aub << 1.5;

    lb.resize(size);
    ub.resize(size);
    lb << 1, 2, 3, 4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5;
    ub << 1, 2, 3, 4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;


    qpmad::Solver               solver;

    qpmad::Solver::ReturnStatus status = solver.solve(x, H, h, lb, ub, A, Alb, Aub);

    return (0);
}
