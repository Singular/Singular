/*!
************************************************************************
Copyright (c) 1992-2007 The University of Tennessee.  All rights reserved.

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from FORTRAN to
      pseudocode.

See subroutines comments for additional copyrights.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.

- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************
*/

#ifndef _reflections_h
#define _reflections_h

#include "ap.h"
#include "amp.h"
namespace reflections
{
    template<unsigned int Precision>
    void generatereflection(ap::template_1d_array< amp::ampf<Precision> >& x,
        int n,
        amp::ampf<Precision>& tau);
    template<unsigned int Precision>
    void applyreflectionfromtheleft(ap::template_2d_array< amp::ampf<Precision> >& c,
        amp::ampf<Precision> tau,
        const ap::template_1d_array< amp::ampf<Precision> >& v,
        int m1,
        int m2,
        int n1,
        int n2,
        ap::template_1d_array< amp::ampf<Precision> >& work);
    template<unsigned int Precision>
    void applyreflectionfromtheright(ap::template_2d_array< amp::ampf<Precision> >& c,
        amp::ampf<Precision> tau,
        const ap::template_1d_array< amp::ampf<Precision> >& v,
        int m1,
        int m2,
        int n1,
        int n2,
        ap::template_1d_array< amp::ampf<Precision> >& work);


    /*!
************************************************************************
    Generation of an elementary reflection transformation

    The subroutine generates elementary reflection H of order N, so that, for
    a given X, the following equality holds true:

        ( X(1) )   ( Beta )
    H * (  ..  ) = (  0   )
        ( X(n) )   (  0   )

    where
                  ( V(1) )
    H = 1 - Tau * (  ..  ) * ( V(1), ..., V(n) )
                  ( V(n) )

    where the first component of vector V equals 1.

    Input parameters:
        X   -   vector. Array whose index ranges within [1..N].
        N   -   reflection order.

    Output parameters:
        X   -   components from 2 to N are replaced with vector V.
                The first component is replaced with parameter Beta.
        Tau -   scalar value Tau. If X is a null vector, Tau equals 0,
                otherwise 1 <= Tau <= 2.

    This subroutine is the modification of the DLARFG subroutines from
    the LAPACK library. It has a similar functionality except for the
    fact that it doesn�t handle errors when the intermediate results
    cause an overflow.


    MODIFICATIONS:
        24.12.2005 sign(Alpha) was replaced with an analogous to the Fortran SIGN code.

      -- LAPACK auxiliary routine (version 3.0) --
         Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
         Courant Institute, Argonne National Lab, and Rice University
         September 30, 1994
    ************************************************************************
*/
    template<unsigned int Precision>
    void generatereflection(ap::template_1d_array< amp::ampf<Precision> >& x,
        int n,
        amp::ampf<Precision>& tau)
    {
        int j;
        amp::ampf<Precision> alpha;
        amp::ampf<Precision> xnorm;
        amp::ampf<Precision> v;
        amp::ampf<Precision> beta;
        amp::ampf<Precision> mx;



        //
        // Executable Statements ..
        //
        if( n<=1 )
        {
            tau = 0;
            return;
        }

        //
        // XNORM = DNRM2( N-1, X, INCX )
        //
        alpha = x(1);
        mx = 0;
        for(j=2; j<=n; j++)
        {
            mx = amp::maximum<Precision>(amp::abs<Precision>(x(j)), mx);
        }
        xnorm = 0;
        if( mx!=0 )
        {
            for(j=2; j<=n; j++)
            {
                xnorm = xnorm+amp::sqr<Precision>(x(j)/mx);
            }
            xnorm = amp::sqrt<Precision>(xnorm)*mx;
        }
        if( xnorm==0 )
        {

            //
            // H  =  I
            //
            tau = 0;
            return;
        }

        //
        // general case
        //
        mx = amp::maximum<Precision>(amp::abs<Precision>(alpha), amp::abs<Precision>(xnorm));
        beta = -mx*amp::sqrt<Precision>(amp::sqr<Precision>(alpha/mx)+amp::sqr<Precision>(xnorm/mx));
        if( alpha<0 )
        {
            beta = -beta;
        }
        tau = (beta-alpha)/beta;
        v = 1/(alpha-beta);
        ap::vmul(x.getvector(2, n), v);
        x(1) = beta;
    }


    /*!
************************************************************************
    Application of an elementary reflection to a rectangular matrix of size MxN

    The algorithm pre-multiplies the matrix by an elementary reflection transformation
    which is given by column V and scalar Tau (see the description of the
    GenerateReflection procedure). Not the whole matrix but only a part of it
    is transformed (rows from M1 to M2, columns from N1 to N2). Only the elements
    of this submatrix are changed.

    Input parameters:
        C       -   matrix to be transformed.
        Tau     -   scalar defining the transformation.
        V       -   column defining the transformation.
                    Array whose index ranges within [1..M2-M1+1].
        M1, M2  -   range of rows to be transformed.
        N1, N2  -   range of columns to be transformed.
        WORK    -   working array whose indexes goes from N1 to N2.

    Output parameters:
        C       -   the result of multiplying the input matrix C by the
                    transformation matrix which is given by Tau and V.
                    If N1>N2 or M1>M2, C is not modified.

      -- LAPACK auxiliary routine (version 3.0) --
         Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
         Courant Institute, Argonne National Lab, and Rice University
         September 30, 1994
    ************************************************************************
*/
    template<unsigned int Precision>
    void applyreflectionfromtheleft(ap::template_2d_array< amp::ampf<Precision> >& c,
        amp::ampf<Precision> tau,
        const ap::template_1d_array< amp::ampf<Precision> >& v,
        int m1,
        int m2,
        int n1,
        int n2,
        ap::template_1d_array< amp::ampf<Precision> >& work)
    {
        amp::ampf<Precision> t;
        int i;
        int vm;


        if( tau==0 || n1>n2 || m1>m2 )
        {
            return;
        }

        //
        // w := C' * v
        //
        vm = m2-m1+1;
        for(i=n1; i<=n2; i++)
        {
            work(i) = 0;
        }
        for(i=m1; i<=m2; i++)
        {
            t = v(i+1-m1);
            ap::vadd(work.getvector(n1, n2), c.getrow(i, n1, n2), t);
        }

        //
        // C := C - tau * v * w'
        //
        for(i=m1; i<=m2; i++)
        {
            t = v(i-m1+1)*tau;
            ap::vsub(c.getrow(i, n1, n2), work.getvector(n1, n2), t);
        }
    }


    /*!
************************************************************************
    Application of an elementary reflection to a rectangular matrix of size MxN

    The algorithm post-multiplies the matrix by an elementary reflection transformation
    which is given by column V and scalar Tau (see the description of the
    GenerateReflection procedure). Not the whole matrix but only a part of it
    is transformed (rows from M1 to M2, columns from N1 to N2). Only the
    elements of this submatrix are changed.

    Input parameters:
        C       -   matrix to be transformed.
        Tau     -   scalar defining the transformation.
        V       -   column defining the transformation.
                    Array whose index ranges within [1..N2-N1+1].
        M1, M2  -   range of rows to be transformed.
        N1, N2  -   range of columns to be transformed.
        WORK    -   working array whose indexes goes from M1 to M2.

    Output parameters:
        C       -   the result of multiplying the input matrix C by the
                    transformation matrix which is given by Tau and V.
                    If N1>N2 or M1>M2, C is not modified.

      -- LAPACK auxiliary routine (version 3.0) --
         Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
         Courant Institute, Argonne National Lab, and Rice University
         September 30, 1994
    ************************************************************************
*/
    template<unsigned int Precision>
    void applyreflectionfromtheright(ap::template_2d_array< amp::ampf<Precision> >& c,
        amp::ampf<Precision> tau,
        const ap::template_1d_array< amp::ampf<Precision> >& v,
        int m1,
        int m2,
        int n1,
        int n2,
        ap::template_1d_array< amp::ampf<Precision> >& work)
    {
        amp::ampf<Precision> t;
        int i;
        int vm;


        if( tau==0 || n1>n2 || m1>m2 )
        {
            return;
        }

        //
        // w := C * v
        //
        vm = n2-n1+1;
        for(i=m1; i<=m2; i++)
        {
            t = ap::vdotproduct(c.getrow(i, n1, n2), v.getvector(1, vm));
            work(i) = t;
        }

        //
        // C := C - w * v'
        //
        for(i=m1; i<=m2; i++)
        {
            t = work(i)*tau;
            ap::vsub(c.getrow(i, n1, n2), v.getvector(1, vm), t);
        }
    }
} // namespace

#endif
