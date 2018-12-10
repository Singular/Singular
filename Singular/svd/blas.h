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

#ifndef _blas_h
#define _blas_h

#include "ap.h"
#include "amp.h"
namespace blas
{
    template<unsigned int Precision>
    amp::ampf<Precision> vectornorm2(const ap::template_1d_array< amp::ampf<Precision> >& x,
        int i1,
        int i2);
    template<unsigned int Precision>
    int vectoridxabsmax(const ap::template_1d_array< amp::ampf<Precision> >& x,
        int i1,
        int i2);
    template<unsigned int Precision>
    int columnidxabsmax(const ap::template_2d_array< amp::ampf<Precision> >& x,
        int i1,
        int i2,
        int j);
    template<unsigned int Precision>
    int rowidxabsmax(const ap::template_2d_array< amp::ampf<Precision> >& x,
        int j1,
        int j2,
        int i);
    template<unsigned int Precision>
    amp::ampf<Precision> upperhessenberg1norm(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int i1,
        int i2,
        int j1,
        int j2,
        ap::template_1d_array< amp::ampf<Precision> >& work);
    template<unsigned int Precision>
    void copymatrix(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int is1,
        int is2,
        int js1,
        int js2,
        ap::template_2d_array< amp::ampf<Precision> >& b,
        int id1,
        int id2,
        int jd1,
        int jd2);
    template<unsigned int Precision>
    void inplacetranspose(ap::template_2d_array< amp::ampf<Precision> >& a,
        int i1,
        int i2,
        int j1,
        int j2,
        ap::template_1d_array< amp::ampf<Precision> >& work);
    template<unsigned int Precision>
    void copyandtranspose(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int is1,
        int is2,
        int js1,
        int js2,
        ap::template_2d_array< amp::ampf<Precision> >& b,
        int id1,
        int id2,
        int jd1,
        int jd2);
    template<unsigned int Precision>
    void matrixvectormultiply(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int i1,
        int i2,
        int j1,
        int j2,
        bool trans,
        const ap::template_1d_array< amp::ampf<Precision> >& x,
        int ix1,
        int ix2,
        amp::ampf<Precision> alpha,
        ap::template_1d_array< amp::ampf<Precision> >& y,
        int iy1,
        int iy2,
        amp::ampf<Precision> beta);
    template<unsigned int Precision>
    amp::ampf<Precision> pythag2(amp::ampf<Precision> x,
        amp::ampf<Precision> y);
    template<unsigned int Precision>
    void matrixmatrixmultiply(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int ai1,
        int ai2,
        int aj1,
        int aj2,
        bool transa,
        const ap::template_2d_array< amp::ampf<Precision> >& b,
        int bi1,
        int bi2,
        int bj1,
        int bj2,
        bool transb,
        amp::ampf<Precision> alpha,
        ap::template_2d_array< amp::ampf<Precision> >& c,
        int ci1,
        int ci2,
        int cj1,
        int cj2,
        amp::ampf<Precision> beta,
        ap::template_1d_array< amp::ampf<Precision> >& work);


    template<unsigned int Precision>
    amp::ampf<Precision> vectornorm2(const ap::template_1d_array< amp::ampf<Precision> >& x,
        int i1,
        int i2)
    {
        amp::ampf<Precision> result;
        int n;
        int ix;
        amp::ampf<Precision> absxi;
        amp::ampf<Precision> scl;
        amp::ampf<Precision> ssq;


        n = i2-i1+1;
        if( n<1 )
        {
            result = 0;
            return result;
        }
        if( n==1 )
        {
            result = amp::abs<Precision>(x(i1));
            return result;
        }
        scl = 0;
        ssq = 1;
        for(ix=i1; ix<=i2; ix++)
        {
            if( x(ix)!=0 )
            {
                absxi = amp::abs<Precision>(x(ix));
                if( scl<absxi )
                {
                    ssq = 1+ssq*amp::sqr<Precision>(scl/absxi);
                    scl = absxi;
                }
                else
                {
                    ssq = ssq+amp::sqr<Precision>(absxi/scl);
                }
            }
        }
        result = scl*amp::sqrt<Precision>(ssq);
        return result;
    }


    template<unsigned int Precision>
    int vectoridxabsmax(const ap::template_1d_array< amp::ampf<Precision> >& x,
        int i1,
        int i2)
    {
        int result;
        int i;
        amp::ampf<Precision> a;


        result = i1;
        a = amp::abs<Precision>(x(result));
        for(i=i1+1; i<=i2; i++)
        {
            if( amp::abs<Precision>(x(i))>amp::abs<Precision>(x(result)) )
            {
                result = i;
            }
        }
        return result;
    }


    template<unsigned int Precision>
    int columnidxabsmax(const ap::template_2d_array< amp::ampf<Precision> >& x,
        int i1,
        int i2,
        int j)
    {
        int result;
        int i;
        amp::ampf<Precision> a;


        result = i1;
        a = amp::abs<Precision>(x(result,j));
        for(i=i1+1; i<=i2; i++)
        {
            if( amp::abs<Precision>(x(i,j))>amp::abs<Precision>(x(result,j)) )
            {
                result = i;
            }
        }
        return result;
    }


    template<unsigned int Precision>
    int rowidxabsmax(const ap::template_2d_array< amp::ampf<Precision> >& x,
        int j1,
        int j2,
        int i)
    {
        int result;
        int j;
        amp::ampf<Precision> a;


        result = j1;
        a = amp::abs<Precision>(x(i,result));
        for(j=j1+1; j<=j2; j++)
        {
            if( amp::abs<Precision>(x(i,j))>amp::abs<Precision>(x(i,result)) )
            {
                result = j;
            }
        }
        return result;
    }


    template<unsigned int Precision>
    amp::ampf<Precision> upperhessenberg1norm(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int i1,
        int i2,
        int j1,
        int j2,
        ap::template_1d_array< amp::ampf<Precision> >& work)
    {
        amp::ampf<Precision> result;
        int i;
        int j;


        ap::ap_error::make_assertion(i2-i1==j2-j1);
        for(j=j1; j<=j2; j++)
        {
            work(j) = 0;
        }
        for(i=i1; i<=i2; i++)
        {
            for(j=ap::maxint(j1, j1+i-i1-1); j<=j2; j++)
            {
                work(j) = work(j)+amp::abs<Precision>(a(i,j));
            }
        }
        result = 0;
        for(j=j1; j<=j2; j++)
        {
            result = amp::maximum<Precision>(result, work(j));
        }
        return result;
    }


    template<unsigned int Precision>
    void copymatrix(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int is1,
        int is2,
        int js1,
        int js2,
        ap::template_2d_array< amp::ampf<Precision> >& b,
        int id1,
        int id2,
        int jd1,
        int jd2)
    {
        int isrc;
        int idst;


        if( is1>is2 || js1>js2 )
        {
            return;
        }
        ap::ap_error::make_assertion(is2-is1==id2-id1);
        ap::ap_error::make_assertion(js2-js1==jd2-jd1);
        for(isrc=is1; isrc<=is2; isrc++)
        {
            idst = isrc-is1+id1;
            ap::vmove(b.getrow(idst, jd1, jd2), a.getrow(isrc, js1, js2));
        }
    }


    template<unsigned int Precision>
    void inplacetranspose(ap::template_2d_array< amp::ampf<Precision> >& a,
        int i1,
        int i2,
        int j1,
        int j2,
        ap::template_1d_array< amp::ampf<Precision> >& work)
    {
        int i;
        int j;
        int ips;
        int jps;
        int l;


        if( i1>i2 || j1>j2 )
        {
            return;
        }
        ap::ap_error::make_assertion(i1-i2==j1-j2);
        for(i=i1; i<=i2-1; i++)
        {
            j = j1+i-i1;
            ips = i+1;
            jps = j1+ips-i1;
            l = i2-i;
            ap::vmove(work.getvector(1, l), a.getcolumn(j, ips, i2));
            ap::vmove(a.getcolumn(j, ips, i2), a.getrow(i, jps, j2));
            ap::vmove(a.getrow(i, jps, j2), work.getvector(1, l));
        }
    }


    template<unsigned int Precision>
    void copyandtranspose(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int is1,
        int is2,
        int js1,
        int js2,
        ap::template_2d_array< amp::ampf<Precision> >& b,
        int id1,
        int id2,
        int jd1,
        int jd2)
    {
        int isrc;
        int jdst;


        if( is1>is2 || js1>js2 )
        {
            return;
        }
        ap::ap_error::make_assertion(is2-is1==jd2-jd1);
        ap::ap_error::make_assertion(js2-js1==id2-id1);
        for(isrc=is1; isrc<=is2; isrc++)
        {
            jdst = isrc-is1+jd1;
            ap::vmove(b.getcolumn(jdst, id1, id2), a.getrow(isrc, js1, js2));
        }
    }


    template<unsigned int Precision>
    void matrixvectormultiply(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int i1,
        int i2,
        int j1,
        int j2,
        bool trans,
        const ap::template_1d_array< amp::ampf<Precision> >& x,
        int ix1,
        int ix2,
        amp::ampf<Precision> alpha,
        ap::template_1d_array< amp::ampf<Precision> >& y,
        int iy1,
        int iy2,
        amp::ampf<Precision> beta)
    {
        int i;
        amp::ampf<Precision> v;


        if( !trans )
        {

            //
            // y := alpha*A*x + beta*y;
            //
            if( i1>i2 || j1>j2 )
            {
                return;
            }
            ap::ap_error::make_assertion(j2-j1==ix2-ix1);
            ap::ap_error::make_assertion(i2-i1==iy2-iy1);

            //
            // beta*y
            //
            if( beta==0 )
            {
                for(i=iy1; i<=iy2; i++)
                {
                    y(i) = 0;
                }
            }
            else
            {
                ap::vmul(y.getvector(iy1, iy2), beta);
            }

            //
            // alpha*A*x
            //
            for(i=i1; i<=i2; i++)
            {
                v = ap::vdotproduct(a.getrow(i, j1, j2), x.getvector(ix1, ix2));
                y(iy1+i-i1) = y(iy1+i-i1)+alpha*v;
            }
        }
        else
        {

            //
            // y := alpha*A'*x + beta*y;
            //
            if( i1>i2 || j1>j2 )
            {
                return;
            }
            ap::ap_error::make_assertion(i2-i1==ix2-ix1);
            ap::ap_error::make_assertion(j2-j1==iy2-iy1);

            //
            // beta*y
            //
            if( beta==0 )
            {
                for(i=iy1; i<=iy2; i++)
                {
                    y(i) = 0;
                }
            }
            else
            {
                ap::vmul(y.getvector(iy1, iy2), beta);
            }

            //
            // alpha*A'*x
            //
            for(i=i1; i<=i2; i++)
            {
                v = alpha*x(ix1+i-i1);
                ap::vadd(y.getvector(iy1, iy2), a.getrow(i, j1, j2), v);
            }
        }
    }


    template<unsigned int Precision>
    amp::ampf<Precision> pythag2(amp::ampf<Precision> x,
        amp::ampf<Precision> y)
    {
        amp::ampf<Precision> result;
        amp::ampf<Precision> w;
        amp::ampf<Precision> xabs;
        amp::ampf<Precision> yabs;
        amp::ampf<Precision> z;


        xabs = amp::abs<Precision>(x);
        yabs = amp::abs<Precision>(y);
        w = amp::maximum<Precision>(xabs, yabs);
        z = amp::minimum<Precision>(xabs, yabs);
        if( z==0 )
        {
            result = w;
        }
        else
        {
            result = w*amp::sqrt<Precision>(1+amp::sqr<Precision>(z/w));
        }
        return result;
    }


    template<unsigned int Precision>
    void matrixmatrixmultiply(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int ai1,
        int ai2,
        int aj1,
        int aj2,
        bool transa,
        const ap::template_2d_array< amp::ampf<Precision> >& b,
        int bi1,
        int bi2,
        int bj1,
        int bj2,
        bool transb,
        amp::ampf<Precision> alpha,
        ap::template_2d_array< amp::ampf<Precision> >& c,
        int ci1,
        int ci2,
        int cj1,
        int cj2,
        amp::ampf<Precision> beta,
        ap::template_1d_array< amp::ampf<Precision> >& work)
    {
        int arows;
        int acols;
        int brows;
        int bcols;
        int crows;
        int ccols;
        int i;
        int j;
        int k;
        int l;
        int r;
        amp::ampf<Precision> v;



        //
        // Setup
        //
        if( !transa )
        {
            arows = ai2-ai1+1;
            acols = aj2-aj1+1;
        }
        else
        {
            arows = aj2-aj1+1;
            acols = ai2-ai1+1;
        }
        if( !transb )
        {
            brows = bi2-bi1+1;
            bcols = bj2-bj1+1;
        }
        else
        {
            brows = bj2-bj1+1;
            bcols = bi2-bi1+1;
        }
        ap::ap_error::make_assertion(acols==brows);
        if( arows<=0 || acols<=0 || brows<=0 || bcols<=0 )
        {
            return;
        }
        crows = arows;
        ccols = bcols;

        //
        // Test WORK
        //
        i = ap::maxint(arows, acols);
        i = ap::maxint(brows, i);
        i = ap::maxint(i, bcols);
        work(1) = 0;
        work(i) = 0;

        //
        // Prepare C
        //
        if( beta==0 )
        {
            for(i=ci1; i<=ci2; i++)
            {
                for(j=cj1; j<=cj2; j++)
                {
                    c(i,j) = 0;
                }
            }
        }
        else
        {
            for(i=ci1; i<=ci2; i++)
            {
                ap::vmul(c.getrow(i, cj1, cj2), beta);
            }
        }

        //
        // A*B
        //
        if( !transa && !transb )
        {
            for(l=ai1; l<=ai2; l++)
            {
                for(r=bi1; r<=bi2; r++)
                {
                    v = alpha*a(l,aj1+r-bi1);
                    k = ci1+l-ai1;
                    ap::vadd(c.getrow(k, cj1, cj2), b.getrow(r, bj1, bj2), v);
                }
            }
            return;
        }

        //
        // A*B'
        //
        if( !transa && transb )
        {
            if( arows*acols<brows*bcols )
            {
                for(r=bi1; r<=bi2; r++)
                {
                    for(l=ai1; l<=ai2; l++)
                    {
                        v = ap::vdotproduct(a.getrow(l, aj1, aj2), b.getrow(r, bj1, bj2));
                        c(ci1+l-ai1,cj1+r-bi1) = c(ci1+l-ai1,cj1+r-bi1)+alpha*v;
                    }
                }
                return;
            }
            else
            {
                for(l=ai1; l<=ai2; l++)
                {
                    for(r=bi1; r<=bi2; r++)
                    {
                        v = ap::vdotproduct(a.getrow(l, aj1, aj2), b.getrow(r, bj1, bj2));
                        c(ci1+l-ai1,cj1+r-bi1) = c(ci1+l-ai1,cj1+r-bi1)+alpha*v;
                    }
                }
                return;
            }
        }

        //
        // A'*B
        //
        if( transa && !transb )
        {
            for(l=aj1; l<=aj2; l++)
            {
                for(r=bi1; r<=bi2; r++)
                {
                    v = alpha*a(ai1+r-bi1,l);
                    k = ci1+l-aj1;
                    ap::vadd(c.getrow(k, cj1, cj2), b.getrow(r, bj1, bj2), v);
                }
            }
            return;
        }

        //
        // A'*B'
        //
        if( transa && transb )
        {
            if( arows*acols<brows*bcols )
            {
                for(r=bi1; r<=bi2; r++)
                {
                    for(i=1; i<=crows; i++)
                    {
                        work(i) = amp::ampf<Precision>("0.0");
                    }
                    for(l=ai1; l<=ai2; l++)
                    {
                        v = alpha*b(r,bj1+l-ai1);
                        k = cj1+r-bi1;
                        ap::vadd(work.getvector(1, crows), a.getrow(l, aj1, aj2), v);
                    }
                    ap::vadd(c.getcolumn(k, ci1, ci2), work.getvector(1, crows));
                }
                return;
            }
            else
            {
                for(l=aj1; l<=aj2; l++)
                {
                    k = ai2-ai1+1;
                    ap::vmove(work.getvector(1, k), a.getcolumn(l, ai1, ai2));
                    for(r=bi1; r<=bi2; r++)
                    {
                        v = ap::vdotproduct(work.getvector(1, k), b.getrow(r, bj1, bj2));
                        c(ci1+l-aj1,cj1+r-bi1) = c(ci1+l-aj1,cj1+r-bi1)+alpha*v;
                    }
                }
                return;
            }
        }
    }
} // namespace

#endif
