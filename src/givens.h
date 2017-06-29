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
    /**
     * @brief
     *
     * Represents Givens reflection
     *
     *  [ cos,  sin ]
     *  [           ]
     *  [ sin, -cos ]
     *
     * for a given vector (a, b) is defined with
     *
     *  [ cos,  sin ]       [ a ]       [ d ]
     *  [           ]   *   [   ]   =   [   ]
     *  [ sin, -cos ]       [ b ]       [ 0 ]
     *
     *  sin^2 + cos^2 = 1
     *
     * Special cases:
     *  COPY        b == 0: cos = 1, sin = 0
     *  SWAP    (b != 0) && (a == 0): cos = 0, sin = 1
     */
    class GivensReflection
    {
        public:
            enum Type
            {
                NONTRIVIAL = 0,
                COPY = 1,
                SWAP = 2
            };


        public:
            Type computeAndApply(double & a, double & b, const double eps)
            {
                double abs_b = std::fabs(b);

                if (abs_b > eps)
                {
                    double abs_a = std::fabs(a);

                    if (abs_a > eps)
                    {
                        double t;
                        if (abs_a > abs_b)
                        {
                            t = (abs_b / abs_a);
                            t = abs_a * std::sqrt(1.0 + t * t);
                        }
                        else
                        {
                            if (abs_a < abs_b)
                            {
                                t = (abs_a / abs_b);
                                t = abs_b * std::sqrt(1.0 + t * t);
                            }
                            else
                            {
                                t = abs_a * std::sqrt(2.0);
                            }
                        }
                        t = copysign(t, a);

                        cos = a / t;
                        sin = b / t;
                        xny = sin / (1.0 + cos);

                        a = t;
                        b = 0.0;

                        type = NONTRIVIAL;
                    }
                    else
                    {
                        //cos = 0.0;
                        //sin = 1.0;
                        //xny = 1.0;
                        swap(a,b);
                        type = SWAP;
                    }
                }
                else
                {
                    //cos = 1.0;
                    //sin = 0.0;
                    //xny = 0;
                    type = COPY;
                }

                return (type);
            }


            void apply(double & a, double & b) const
            {
                switch (type)
                {
                    case COPY:
                        return;
                    case SWAP:
                        swap(a,b);
                        return;
                    case NONTRIVIAL:
                        applyNonTrivial(a,b);
                        return;
                }
            }

            template<class t_MatrixType>
                void applyColumnWise(   t_MatrixType &M,
                                        const int start,
                                        const int end,
                                        const int column_1,
                                        const int column_2) const
            {
                switch (type)
                {
                    case COPY:
                        return;
                    case SWAP:
                        M.col(column_1).segment(start, end-start).swap(
                                M.col(column_2).segment(start, end-start)   );
                        return;
                    case NONTRIVIAL:
                        for (int k = start; k < end; k++)
                        {
                            applyNonTrivial(M(k, column_1), M(k, column_2));
                        }
                        return;
                }
            }


            template<class t_MatrixType>
                void applyRowWise(  t_MatrixType &M,
                                    const int start,
                                    const int end,
                                    const int row_1,
                                    const int row_2) const
            {
                switch (type)
                {
                    case COPY:
                        return;
                    case SWAP:
                        M.row(row_1).segment(start, end-start).swap(
                                M.row(row_2).segment(start, end-start)   );
                        return;
                    case NONTRIVIAL:
                        for (int k = start; k < end; k++)
                        {
                            applyNonTrivial(M(row_1, k), M(row_2, k));
                        }
                        return;
                }
            }

        private:
            Type    type;
            double  cos;
            double  sin;
            double  xny;


        private:
            inline void swap(double & a, double & b) const
            {
                std::swap(a,b);
            }

            inline void applyNonTrivial(double & a, double & b) const
            {
                double t1 = a;
                double t2 = b;
                a = t1 * cos + t2 * sin;
                b = xny * (t1 + a) - t2;
            }
    };
}
