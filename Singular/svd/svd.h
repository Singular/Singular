/*!
************************************************************************
Copyright (c) 2005-2007, Sergey Bochkanov (ALGLIB project).

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

#ifndef _svd_h
#define _svd_h

/*!
MAKEHEADER
*/
#include "ap.h"

/*!
MAKEHEADER
*/
#include "amp.h"

/*!
MAKEHEADER
*/
#include "reflections.h"

/*!
MAKEHEADER
*/
#include "bidiagonal.h"

/*!
MAKEHEADER
*/
#include "qr.h"

/*!
MAKEHEADER
*/
#include "lq.h"

/*!
MAKEHEADER
*/
#include "blas.h"

/*!
MAKEHEADER
*/
#include "rotations.h"

/*!
MAKEHEADER
*/
#include "bdsvd.h"

namespace svd
{
    template<unsigned int Precision>
    bool rmatrixsvd(ap::template_2d_array< amp::ampf<Precision> > a,
        int m,
        int n,
        int uneeded,
        int vtneeded,
        int additionalmemory,
        ap::template_1d_array< amp::ampf<Precision> >& w,
        ap::template_2d_array< amp::ampf<Precision> >& u,
        ap::template_2d_array< amp::ampf<Precision> >& vt);
    template<unsigned int Precision>
    bool svddecomposition(ap::template_2d_array< amp::ampf<Precision> > a,
        int m,
        int n,
        int uneeded,
        int vtneeded,
        int additionalmemory,
        ap::template_1d_array< amp::ampf<Precision> >& w,
        ap::template_2d_array< amp::ampf<Precision> >& u,
        ap::template_2d_array< amp::ampf<Precision> >& vt);


    /*!
************************************************************************
    Singular value decomposition of a rectangular matrix.

    The algorithm calculates the singular value decomposition of a matrix of
    size MxN: A = U * S * V^T

    The algorithm finds the singular values and, optionally, matrices U and V^T.
    The algorithm can find both first min(M,N) columns of matrix U and rows of
    matrix V^T (singular vectors), and matrices U and V^T wholly (of sizes MxM
    and NxN respectively).

    Take into account that the subroutine does not return matrix V but V^T.

    Input parameters:
        A           -   matrix to be decomposed.
                        Array whose indexes range within [0..M-1, 0..N-1].
        M           -   number of rows in matrix A.
        N           -   number of columns in matrix A.
        UNeeded     -   0, 1 or 2. See the description of the parameter U.
        VTNeeded    -   0, 1 or 2. See the description of the parameter VT.
        AdditionalMemory -
                        If the parameter:
                         * equals 0, the algorithm doesn’t use additional
                           memory (lower requirements, lower performance).
                         * equals 1, the algorithm uses additional
                           memory of size min(M,N)*min(M,N) of real numbers.
                           It often speeds up the algorithm.
                         * equals 2, the algorithm uses additional
                           memory of size M*min(M,N) of real numbers.
                           It allows to get a maximum performance.
                        The recommended value of the parameter is 2.

    Output parameters:
        W           -   contains singular values in descending order.
        U           -   if UNeeded=0, U isn't changed, the left singular vectors
                        are not calculated.
                        if Uneeded=1, U contains left singular vectors (first
                        min(M,N) columns of matrix U). Array whose indexes range
                        within [0..M-1, 0..Min(M,N)-1].
                        if UNeeded=2, U contains matrix U wholly. Array whose
                        indexes range within [0..M-1, 0..M-1].
        VT          -   if VTNeeded=0, VT isn’t changed, the right singular vectors
                        are not calculated.
                        if VTNeeded=1, VT contains right singular vectors (first
                        min(M,N) rows of matrix V^T). Array whose indexes range
                        within [0..min(M,N)-1, 0..N-1].
                        if VTNeeded=2, VT contains matrix V^T wholly. Array whose
                        indexes range within [0..N-1, 0..N-1].

      -- ALGLIB --
         Copyright 2005 by Bochkanov Sergey
    ************************************************************************
*/
    template<unsigned int Precision>
    bool rmatrixsvd(ap::template_2d_array< amp::ampf<Precision> > a,
        int m,
        int n,
        int uneeded,
        int vtneeded,
        int additionalmemory,
        ap::template_1d_array< amp::ampf<Precision> >& w,
        ap::template_2d_array< amp::ampf<Precision> >& u,
        ap::template_2d_array< amp::ampf<Precision> >& vt)
    {
        bool result;
        ap::template_1d_array< amp::ampf<Precision> > tauq;
        ap::template_1d_array< amp::ampf<Precision> > taup;
        ap::template_1d_array< amp::ampf<Precision> > tau;
        ap::template_1d_array< amp::ampf<Precision> > e;
        ap::template_1d_array< amp::ampf<Precision> > work;
        ap::template_2d_array< amp::ampf<Precision> > t2;
        bool isupper;
        int minmn;
        int ncu;
        int nrvt;
        int nru;
        int ncvt;
        int i;
        int j;
        int im1;
        amp::ampf<Precision> sm;


        result = true;
        if( m==0 || n==0 )
        {
            return result;
        }
        ap::ap_error::make_assertion(uneeded>=0 && uneeded<=2);
        ap::ap_error::make_assertion(vtneeded>=0 && vtneeded<=2);
        ap::ap_error::make_assertion(additionalmemory>=0 && additionalmemory<=2);

        //
        // initialize
        //
        minmn = ap::minint(m, n);
        w.setbounds(1, minmn);
        ncu = 0;
        nru = 0;
        if( uneeded==1 )
        {
            nru = m;
            ncu = minmn;
            u.setbounds(0, nru-1, 0, ncu-1);
        }
        if( uneeded==2 )
        {
            nru = m;
            ncu = m;
            u.setbounds(0, nru-1, 0, ncu-1);
        }
        nrvt = 0;
        ncvt = 0;
        if( vtneeded==1 )
        {
            nrvt = minmn;
            ncvt = n;
            vt.setbounds(0, nrvt-1, 0, ncvt-1);
        }
        if( vtneeded==2 )
        {
            nrvt = n;
            ncvt = n;
            vt.setbounds(0, nrvt-1, 0, ncvt-1);
        }

        //
        // M much larger than N
        // Use bidiagonal reduction with QR-decomposition
        //
        if( m>amp::ampf<Precision>("1.6")*n )
        {
            if( uneeded==0 )
            {

                //
                // No left singular vectors to be computed
                //
                qr::rmatrixqr<Precision>(a, m, n, tau);
                for(i=0; i<=n-1; i++)
                {
                    for(j=0; j<=i-1; j++)
                    {
                        a(i,j) = 0;
                    }
                }
                bidiagonal::rmatrixbd<Precision>(a, n, n, tauq, taup);
                bidiagonal::rmatrixbdunpackpt<Precision>(a, n, n, taup, nrvt, vt);
                bidiagonal::rmatrixbdunpackdiagonals<Precision>(a, n, n, isupper, w, e);
                result = bdsvd::rmatrixbdsvd<Precision>(w, e, n, isupper, false, u, 0, a, 0, vt, ncvt);
                return result;
            }
            else
            {

                //
                // Left singular vectors (may be full matrix U) to be computed
                //
                qr::rmatrixqr<Precision>(a, m, n, tau);
                qr::rmatrixqrunpackq<Precision>(a, m, n, tau, ncu, u);
                for(i=0; i<=n-1; i++)
                {
                    for(j=0; j<=i-1; j++)
                    {
                        a(i,j) = 0;
                    }
                }
                bidiagonal::rmatrixbd<Precision>(a, n, n, tauq, taup);
                bidiagonal::rmatrixbdunpackpt<Precision>(a, n, n, taup, nrvt, vt);
                bidiagonal::rmatrixbdunpackdiagonals<Precision>(a, n, n, isupper, w, e);
                if( additionalmemory<1 )
                {

                    //
                    // No additional memory can be used
                    //
                    bidiagonal::rmatrixbdmultiplybyq<Precision>(a, n, n, tauq, u, m, n, true, false);
                    result = bdsvd::rmatrixbdsvd<Precision>(w, e, n, isupper, false, u, m, a, 0, vt, ncvt);
                }
                else
                {

                    //
                    // Large U. Transforming intermediate matrix T2
                    //
                    work.setbounds(1, ap::maxint(m, n));
                    bidiagonal::rmatrixbdunpackq<Precision>(a, n, n, tauq, n, t2);
                    blas::copymatrix<Precision>(u, 0, m-1, 0, n-1, a, 0, m-1, 0, n-1);
                    blas::inplacetranspose<Precision>(t2, 0, n-1, 0, n-1, work);
                    result = bdsvd::rmatrixbdsvd<Precision>(w, e, n, isupper, false, u, 0, t2, n, vt, ncvt);
                    blas::matrixmatrixmultiply<Precision>(a, 0, m-1, 0, n-1, false, t2, 0, n-1, 0, n-1, true, amp::ampf<Precision>("1.0"), u, 0, m-1, 0, n-1, amp::ampf<Precision>("0.0"), work);
                }
                return result;
            }
        }

        //
        // N much larger than M
        // Use bidiagonal reduction with LQ-decomposition
        //
        if( n>amp::ampf<Precision>("1.6")*m )
        {
            if( vtneeded==0 )
            {

                //
                // No right singular vectors to be computed
                //
                lq::rmatrixlq<Precision>(a, m, n, tau);
                for(i=0; i<=m-1; i++)
                {
                    for(j=i+1; j<=m-1; j++)
                    {
                        a(i,j) = 0;
                    }
                }
                bidiagonal::rmatrixbd<Precision>(a, m, m, tauq, taup);
                bidiagonal::rmatrixbdunpackq<Precision>(a, m, m, tauq, ncu, u);
                bidiagonal::rmatrixbdunpackdiagonals<Precision>(a, m, m, isupper, w, e);
                work.setbounds(1, m);
                blas::inplacetranspose<Precision>(u, 0, nru-1, 0, ncu-1, work);
                result = bdsvd::rmatrixbdsvd<Precision>(w, e, m, isupper, false, a, 0, u, nru, vt, 0);
                blas::inplacetranspose<Precision>(u, 0, nru-1, 0, ncu-1, work);
                return result;
            }
            else
            {

                //
                // Right singular vectors (may be full matrix VT) to be computed
                //
                lq::rmatrixlq<Precision>(a, m, n, tau);
                lq::rmatrixlqunpackq<Precision>(a, m, n, tau, nrvt, vt);
                for(i=0; i<=m-1; i++)
                {
                    for(j=i+1; j<=m-1; j++)
                    {
                        a(i,j) = 0;
                    }
                }
                bidiagonal::rmatrixbd<Precision>(a, m, m, tauq, taup);
                bidiagonal::rmatrixbdunpackq<Precision>(a, m, m, tauq, ncu, u);
                bidiagonal::rmatrixbdunpackdiagonals<Precision>(a, m, m, isupper, w, e);
                work.setbounds(1, ap::maxint(m, n));
                blas::inplacetranspose<Precision>(u, 0, nru-1, 0, ncu-1, work);
                if( additionalmemory<1 )
                {

                    //
                    // No additional memory available
                    //
                    bidiagonal::rmatrixbdmultiplybyp<Precision>(a, m, m, taup, vt, m, n, false, true);
                    result = bdsvd::rmatrixbdsvd<Precision>(w, e, m, isupper, false, a, 0, u, nru, vt, n);
                }
                else
                {

                    //
                    // Large VT. Transforming intermediate matrix T2
                    //
                    bidiagonal::rmatrixbdunpackpt<Precision>(a, m, m, taup, m, t2);
                    result = bdsvd::rmatrixbdsvd<Precision>(w, e, m, isupper, false, a, 0, u, nru, t2, m);
                    blas::copymatrix<Precision>(vt, 0, m-1, 0, n-1, a, 0, m-1, 0, n-1);
                    blas::matrixmatrixmultiply<Precision>(t2, 0, m-1, 0, m-1, false, a, 0, m-1, 0, n-1, false, amp::ampf<Precision>("1.0"), vt, 0, m-1, 0, n-1, amp::ampf<Precision>("0.0"), work);
                }
                blas::inplacetranspose<Precision>(u, 0, nru-1, 0, ncu-1, work);
                return result;
            }
        }

        //
        // M<=N
        // We can use inplace transposition of U to get rid of columnwise operations
        //
        if( m<=n )
        {
            bidiagonal::rmatrixbd<Precision>(a, m, n, tauq, taup);
            bidiagonal::rmatrixbdunpackq<Precision>(a, m, n, tauq, ncu, u);
            bidiagonal::rmatrixbdunpackpt<Precision>(a, m, n, taup, nrvt, vt);
            bidiagonal::rmatrixbdunpackdiagonals<Precision>(a, m, n, isupper, w, e);
            work.setbounds(1, m);
            blas::inplacetranspose<Precision>(u, 0, nru-1, 0, ncu-1, work);
            result = bdsvd::rmatrixbdsvd<Precision>(w, e, minmn, isupper, false, a, 0, u, nru, vt, ncvt);
            blas::inplacetranspose<Precision>(u, 0, nru-1, 0, ncu-1, work);
            return result;
        }

        //
        // Simple bidiagonal reduction
        //
        bidiagonal::rmatrixbd<Precision>(a, m, n, tauq, taup);
        bidiagonal::rmatrixbdunpackq<Precision>(a, m, n, tauq, ncu, u);
        bidiagonal::rmatrixbdunpackpt<Precision>(a, m, n, taup, nrvt, vt);
        bidiagonal::rmatrixbdunpackdiagonals<Precision>(a, m, n, isupper, w, e);
        if( additionalmemory<2 || uneeded==0 )
        {

            //
            // We cant use additional memory or there is no need in such operations
            //
            result = bdsvd::rmatrixbdsvd<Precision>(w, e, minmn, isupper, false, u, nru, a, 0, vt, ncvt);
        }
        else
        {

            //
            // We can use additional memory
            //
            t2.setbounds(0, minmn-1, 0, m-1);
            blas::copyandtranspose<Precision>(u, 0, m-1, 0, minmn-1, t2, 0, minmn-1, 0, m-1);
            result = bdsvd::rmatrixbdsvd<Precision>(w, e, minmn, isupper, false, u, 0, t2, m, vt, ncvt);
            blas::copyandtranspose<Precision>(t2, 0, minmn-1, 0, m-1, u, 0, m-1, 0, minmn-1);
        }
        return result;
    }


    /*!
************************************************************************
    Obsolete 1-based subroutine.
    See RMatrixSVD for 0-based replacement.
    ************************************************************************
*/
    template<unsigned int Precision>
    bool svddecomposition(ap::template_2d_array< amp::ampf<Precision> > a,
        int m,
        int n,
        int uneeded,
        int vtneeded,
        int additionalmemory,
        ap::template_1d_array< amp::ampf<Precision> >& w,
        ap::template_2d_array< amp::ampf<Precision> >& u,
        ap::template_2d_array< amp::ampf<Precision> >& vt)
    {
        bool result;
        ap::template_1d_array< amp::ampf<Precision> > tauq;
        ap::template_1d_array< amp::ampf<Precision> > taup;
        ap::template_1d_array< amp::ampf<Precision> > tau;
        ap::template_1d_array< amp::ampf<Precision> > e;
        ap::template_1d_array< amp::ampf<Precision> > work;
        ap::template_2d_array< amp::ampf<Precision> > t2;
        bool isupper;
        int minmn;
        int ncu;
        int nrvt;
        int nru;
        int ncvt;
        int i;
        int j;
        int im1;
        amp::ampf<Precision> sm;


        result = true;
        if( m==0 || n==0 )
        {
            return result;
        }
        ap::ap_error::make_assertion(uneeded>=0 && uneeded<=2);
        ap::ap_error::make_assertion(vtneeded>=0 && vtneeded<=2);
        ap::ap_error::make_assertion(additionalmemory>=0 && additionalmemory<=2);

        //
        // initialize
        //
        minmn = ap::minint(m, n);
        w.setbounds(1, minmn);
        ncu = 0;
        nru = 0;
        if( uneeded==1 )
        {
            nru = m;
            ncu = minmn;
            u.setbounds(1, nru, 1, ncu);
        }
        if( uneeded==2 )
        {
            nru = m;
            ncu = m;
            u.setbounds(1, nru, 1, ncu);
        }
        nrvt = 0;
        ncvt = 0;
        if( vtneeded==1 )
        {
            nrvt = minmn;
            ncvt = n;
            vt.setbounds(1, nrvt, 1, ncvt);
        }
        if( vtneeded==2 )
        {
            nrvt = n;
            ncvt = n;
            vt.setbounds(1, nrvt, 1, ncvt);
        }

        //
        // M much larger than N
        // Use bidiagonal reduction with QR-decomposition
        //
        if( m>amp::ampf<Precision>("1.6")*n )
        {
            if( uneeded==0 )
            {

                //
                // No left singular vectors to be computed
                //
                qr::qrdecomposition<Precision>(a, m, n, tau);
                for(i=2; i<=n; i++)
                {
                    for(j=1; j<=i-1; j++)
                    {
                        a(i,j) = 0;
                    }
                }
                bidiagonal::tobidiagonal<Precision>(a, n, n, tauq, taup);
                bidiagonal::unpackptfrombidiagonal<Precision>(a, n, n, taup, nrvt, vt);
                bidiagonal::unpackdiagonalsfrombidiagonal<Precision>(a, n, n, isupper, w, e);
                result = bdsvd::bidiagonalsvddecomposition<Precision>(w, e, n, isupper, false, u, 0, a, 0, vt, ncvt);
                return result;
            }
            else
            {

                //
                // Left singular vectors (may be full matrix U) to be computed
                //
                qr::qrdecomposition<Precision>(a, m, n, tau);
                qr::unpackqfromqr<Precision>(a, m, n, tau, ncu, u);
                for(i=2; i<=n; i++)
                {
                    for(j=1; j<=i-1; j++)
                    {
                        a(i,j) = 0;
                    }
                }
                bidiagonal::tobidiagonal<Precision>(a, n, n, tauq, taup);
                bidiagonal::unpackptfrombidiagonal<Precision>(a, n, n, taup, nrvt, vt);
                bidiagonal::unpackdiagonalsfrombidiagonal<Precision>(a, n, n, isupper, w, e);
                if( additionalmemory<1 )
                {

                    //
                    // No additional memory can be used
                    //
                    bidiagonal::multiplybyqfrombidiagonal<Precision>(a, n, n, tauq, u, m, n, true, false);
                    result = bdsvd::bidiagonalsvddecomposition<Precision>(w, e, n, isupper, false, u, m, a, 0, vt, ncvt);
                }
                else
                {

                    //
                    // Large U. Transforming intermediate matrix T2
                    //
                    work.setbounds(1, ap::maxint(m, n));
                    bidiagonal::unpackqfrombidiagonal<Precision>(a, n, n, tauq, n, t2);
                    blas::copymatrix<Precision>(u, 1, m, 1, n, a, 1, m, 1, n);
                    blas::inplacetranspose<Precision>(t2, 1, n, 1, n, work);
                    result = bdsvd::bidiagonalsvddecomposition<Precision>(w, e, n, isupper, false, u, 0, t2, n, vt, ncvt);
                    blas::matrixmatrixmultiply<Precision>(a, 1, m, 1, n, false, t2, 1, n, 1, n, true, amp::ampf<Precision>("1.0"), u, 1, m, 1, n, amp::ampf<Precision>("0.0"), work);
                }
                return result;
            }
        }

        //
        // N much larger than M
        // Use bidiagonal reduction with LQ-decomposition
        //
        if( n>amp::ampf<Precision>("1.6")*m )
        {
            if( vtneeded==0 )
            {

                //
                // No right singular vectors to be computed
                //
                lq::lqdecomposition<Precision>(a, m, n, tau);
                for(i=1; i<=m-1; i++)
                {
                    for(j=i+1; j<=m; j++)
                    {
                        a(i,j) = 0;
                    }
                }
                bidiagonal::tobidiagonal<Precision>(a, m, m, tauq, taup);
                bidiagonal::unpackqfrombidiagonal<Precision>(a, m, m, tauq, ncu, u);
                bidiagonal::unpackdiagonalsfrombidiagonal<Precision>(a, m, m, isupper, w, e);
                work.setbounds(1, m);
                blas::inplacetranspose<Precision>(u, 1, nru, 1, ncu, work);
                result = bdsvd::bidiagonalsvddecomposition<Precision>(w, e, m, isupper, false, a, 0, u, nru, vt, 0);
                blas::inplacetranspose<Precision>(u, 1, nru, 1, ncu, work);
                return result;
            }
            else
            {

                //
                // Right singular vectors (may be full matrix VT) to be computed
                //
                lq::lqdecomposition<Precision>(a, m, n, tau);
                lq::unpackqfromlq<Precision>(a, m, n, tau, nrvt, vt);
                for(i=1; i<=m-1; i++)
                {
                    for(j=i+1; j<=m; j++)
                    {
                        a(i,j) = 0;
                    }
                }
                bidiagonal::tobidiagonal<Precision>(a, m, m, tauq, taup);
                bidiagonal::unpackqfrombidiagonal<Precision>(a, m, m, tauq, ncu, u);
                bidiagonal::unpackdiagonalsfrombidiagonal<Precision>(a, m, m, isupper, w, e);
                work.setbounds(1, ap::maxint(m, n));
                blas::inplacetranspose<Precision>(u, 1, nru, 1, ncu, work);
                if( additionalmemory<1 )
                {

                    //
                    // No additional memory available
                    //
                    bidiagonal::multiplybypfrombidiagonal<Precision>(a, m, m, taup, vt, m, n, false, true);
                    result = bdsvd::bidiagonalsvddecomposition<Precision>(w, e, m, isupper, false, a, 0, u, nru, vt, n);
                }
                else
                {

                    //
                    // Large VT. Transforming intermediate matrix T2
                    //
                    bidiagonal::unpackptfrombidiagonal<Precision>(a, m, m, taup, m, t2);
                    result = bdsvd::bidiagonalsvddecomposition<Precision>(w, e, m, isupper, false, a, 0, u, nru, t2, m);
                    blas::copymatrix<Precision>(vt, 1, m, 1, n, a, 1, m, 1, n);
                    blas::matrixmatrixmultiply<Precision>(t2, 1, m, 1, m, false, a, 1, m, 1, n, false, amp::ampf<Precision>("1.0"), vt, 1, m, 1, n, amp::ampf<Precision>("0.0"), work);
                }
                blas::inplacetranspose<Precision>(u, 1, nru, 1, ncu, work);
                return result;
            }
        }

        //
        // M<=N
        // We can use inplace transposition of U to get rid of columnwise operations
        //
        if( m<=n )
        {
            bidiagonal::tobidiagonal<Precision>(a, m, n, tauq, taup);
            bidiagonal::unpackqfrombidiagonal<Precision>(a, m, n, tauq, ncu, u);
            bidiagonal::unpackptfrombidiagonal<Precision>(a, m, n, taup, nrvt, vt);
            bidiagonal::unpackdiagonalsfrombidiagonal<Precision>(a, m, n, isupper, w, e);
            work.setbounds(1, m);
            blas::inplacetranspose<Precision>(u, 1, nru, 1, ncu, work);
            result = bdsvd::bidiagonalsvddecomposition<Precision>(w, e, minmn, isupper, false, a, 0, u, nru, vt, ncvt);
            blas::inplacetranspose<Precision>(u, 1, nru, 1, ncu, work);
            return result;
        }

        //
        // Simple bidiagonal reduction
        //
        bidiagonal::tobidiagonal<Precision>(a, m, n, tauq, taup);
        bidiagonal::unpackqfrombidiagonal<Precision>(a, m, n, tauq, ncu, u);
        bidiagonal::unpackptfrombidiagonal<Precision>(a, m, n, taup, nrvt, vt);
        bidiagonal::unpackdiagonalsfrombidiagonal<Precision>(a, m, n, isupper, w, e);
        if( additionalmemory<2 || uneeded==0 )
        {

            //
            // We cant use additional memory or there is no need in such operations
            //
            result = bdsvd::bidiagonalsvddecomposition<Precision>(w, e, minmn, isupper, false, u, nru, a, 0, vt, ncvt);
        }
        else
        {

            //
            // We can use additional memory
            //
            t2.setbounds(1, minmn, 1, m);
            blas::copyandtranspose<Precision>(u, 1, m, 1, minmn, t2, 1, minmn, 1, m);
            result = bdsvd::bidiagonalsvddecomposition<Precision>(w, e, minmn, isupper, false, u, 0, t2, m, vt, ncvt);
            blas::copyandtranspose<Precision>(t2, 1, minmn, 1, m, u, 1, m, 1, minmn);
        }
        return result;
    }
} // namespace

#endif
