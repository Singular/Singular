
#ifndef _testsvdunit_h
#define _testsvdunit_h

#include <stdio.h>
#include "ap.h"
#include "amp.h"
#include "reflections.h"
#include "bidiagonal.h"
#include "qr.h"
#include "lq.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
namespace testsvdunit
{
    template<unsigned int Precision>
    bool testsvd(bool silent);
    template<unsigned int Precision>
    void fillsparsea(ap::template_2d_array< amp::ampf<Precision> >& a,
        int m,
        int n,
        amp::ampf<Precision> sparcity);
    template<unsigned int Precision>
    void getsvderror(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int m,
        int n,
        const ap::template_2d_array< amp::ampf<Precision> >& u,
        const ap::template_1d_array< amp::ampf<Precision> >& w,
        const ap::template_2d_array< amp::ampf<Precision> >& vt,
        amp::ampf<Precision>& materr,
        amp::ampf<Precision>& orterr,
        bool& wsorted);
    template<unsigned int Precision>
    void testsvdproblem(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int m,
        int n,
        amp::ampf<Precision>& materr,
        amp::ampf<Precision>& orterr,
        amp::ampf<Precision>& othererr,
        bool& wsorted,
        bool& wfailed);
    template<unsigned int Precision>
    bool testsvdunit_test_silent();
    template<unsigned int Precision>
    bool testsvdunit_test();


    static int failcount;
    static int succcount;


    /*!
************************************************************************
    Testing SVD decomposition subroutine
    ************************************************************************
*/
    template<unsigned int Precision>
    bool testsvd(bool silent)
    {
        bool result;
        ap::template_2d_array< amp::ampf<Precision> > a;
        int m;
        int n;
        int maxmn;
        int i;
        int j;
        int gpass;
        int pass;
        bool waserrors;
        bool wsorted;
        bool wfailed;
        amp::ampf<Precision> materr;
        amp::ampf<Precision> orterr;
        amp::ampf<Precision> othererr;
        amp::ampf<Precision> threshold;
        amp::ampf<Precision> failthreshold;
        amp::ampf<Precision> failr;


        failcount = 0;
        succcount = 0;
        materr = 0;
        orterr = 0;
        othererr = 0;
        wsorted = true;
        wfailed = false;
        waserrors = false;
        maxmn = 30;
        threshold = 5*100*amp::ampf<Precision>::getAlgoPascalEpsilon();
        failthreshold = amp::ampf<Precision>("5.0E-3");
        a.setbounds(0, maxmn-1, 0, maxmn-1);

        //
        // TODO: div by zero fail, convergence fail
        //
        for(gpass=1; gpass<=1; gpass++)
        {

            //
            // zero matrix, several cases
            //
            for(i=0; i<=maxmn-1; i++)
            {
                for(j=0; j<=maxmn-1; j++)
                {
                    a(i,j) = 0;
                }
            }
            for(i=1; i<=ap::minint(5, maxmn); i++)
            {
                for(j=1; j<=ap::minint(5, maxmn); j++)
                {
                    testsvdproblem<Precision>(a, i, j, materr, orterr, othererr, wsorted, wfailed);
                }
            }

            //
            // Long dense matrix
            //
            for(i=0; i<=maxmn-1; i++)
            {
                for(j=0; j<=ap::minint(5, maxmn)-1; j++)
                {
                    a(i,j) = 2*amp::ampf<Precision>::getRandom()-1;
                }
            }
            for(i=1; i<=maxmn; i++)
            {
                for(j=1; j<=ap::minint(5, maxmn); j++)
                {
                    testsvdproblem<Precision>(a, i, j, materr, orterr, othererr, wsorted, wfailed);
                }
            }
            for(i=0; i<=ap::minint(5, maxmn)-1; i++)
            {
                for(j=0; j<=maxmn-1; j++)
                {
                    a(i,j) = 2*amp::ampf<Precision>::getRandom()-1;
                }
            }
            for(i=1; i<=ap::minint(5, maxmn); i++)
            {
                for(j=1; j<=maxmn; j++)
                {
                    testsvdproblem<Precision>(a, i, j, materr, orterr, othererr, wsorted, wfailed);
                }
            }

            //
            // Dense matrices
            //
            for(m=1; m<=ap::minint(10, maxmn); m++)
            {
                for(n=1; n<=ap::minint(10, maxmn); n++)
                {
                    for(i=0; i<=m-1; i++)
                    {
                        for(j=0; j<=n-1; j++)
                        {
                            a(i,j) = 2*amp::ampf<Precision>::getRandom()-1;
                        }
                    }
                    testsvdproblem<Precision>(a, m, n, materr, orterr, othererr, wsorted, wfailed);
                }
            }

            //
            // Sparse matrices, very sparse matrices, incredible sparse matrices
            //
            for(m=1; m<=10; m++)
            {
                for(n=1; n<=10; n++)
                {
                    for(pass=1; pass<=2; pass++)
                    {
                        fillsparsea<Precision>(a, m, n, amp::ampf<Precision>("0.8"));
                        testsvdproblem<Precision>(a, m, n, materr, orterr, othererr, wsorted, wfailed);
                        fillsparsea<Precision>(a, m, n, amp::ampf<Precision>("0.9"));
                        testsvdproblem<Precision>(a, m, n, materr, orterr, othererr, wsorted, wfailed);
                        fillsparsea<Precision>(a, m, n, amp::ampf<Precision>("0.95"));
                        testsvdproblem<Precision>(a, m, n, materr, orterr, othererr, wsorted, wfailed);
                    }
                }
            }
        }

        //
        // report
        //
        failr = amp::ampf<Precision>(failcount)/(amp::ampf<Precision>(succcount+failcount));
        waserrors = materr>threshold || orterr>threshold || othererr>threshold || !wsorted || failr>failthreshold;
        if( !silent )
        {
            printf("TESTING SVD DECOMPOSITION\n");
            printf("SVD decomposition error:                 %5.3le\n",
                double(amp::ampf<Precision>(materr).toDouble()));
            printf("SVD orthogonality error:                 %5.3le\n",
                double(amp::ampf<Precision>(orterr).toDouble()));
            printf("SVD with different parameters error:     %5.3le\n",
                double(amp::ampf<Precision>(othererr).toDouble()));
            printf("Singular values order:                   ");
            if( wsorted )
            {
                printf("OK\n");
            }
            else
            {
                printf("FAILED\n");
            }
            printf("Always converged:                        ");
            if( !wfailed )
            {
                printf("YES\n");
            }
            else
            {
                printf("NO\n");
                printf("Fail ratio:                              %5.3lf\n",
                    double(amp::ampf<Precision>(failr).toDouble()));
            }
            printf("Threshold:                               %5.3le\n",
                double(amp::ampf<Precision>(threshold).toDouble()));
            if( waserrors )
            {
                printf("TEST FAILED\n");
            }
            else
            {
                printf("TEST PASSED\n");
            }
            printf("\n\n");
        }
        result = !waserrors;
        return result;
    }


    template<unsigned int Precision>
    void fillsparsea(ap::template_2d_array< amp::ampf<Precision> >& a,
        int m,
        int n,
        amp::ampf<Precision> sparcity)
    {
        int i;
        int j;


        for(i=0; i<=m-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                if( amp::ampf<Precision>::getRandom()>=sparcity )
                {
                    a(i,j) = 2*amp::ampf<Precision>::getRandom()-1;
                }
                else
                {
                    a(i,j) = 0;
                }
            }
        }
    }


    template<unsigned int Precision>
    void getsvderror(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int m,
        int n,
        const ap::template_2d_array< amp::ampf<Precision> >& u,
        const ap::template_1d_array< amp::ampf<Precision> >& w,
        const ap::template_2d_array< amp::ampf<Precision> >& vt,
        amp::ampf<Precision>& materr,
        amp::ampf<Precision>& orterr,
        bool& wsorted)
    {
        int i;
        int j;
        int k;
        int minmn;
        amp::ampf<Precision> locerr;
        amp::ampf<Precision> sm;


        minmn = ap::minint(m, n);

        //
        // decomposition error
        //
        locerr = 0;
        for(i=0; i<=m-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                sm = 0;
                for(k=0; k<=minmn-1; k++)
                {
                    sm = sm+w(k)*u(i,k)*vt(k,j);
                }
                locerr = amp::maximum<Precision>(locerr, amp::abs<Precision>(a(i,j)-sm));
            }
        }
        materr = amp::maximum<Precision>(materr, locerr);

        //
        // orthogonality error
        //
        locerr = 0;
        for(i=0; i<=minmn-1; i++)
        {
            for(j=i; j<=minmn-1; j++)
            {
                sm = ap::vdotproduct(u.getcolumn(i, 0, m-1), u.getcolumn(j, 0, m-1));
                if( i!=j )
                {
                    locerr = amp::maximum<Precision>(locerr, amp::abs<Precision>(sm));
                }
                else
                {
                    locerr = amp::maximum<Precision>(locerr, amp::abs<Precision>(sm-1));
                }
                sm = ap::vdotproduct(vt.getrow(i, 0, n-1), vt.getrow(j, 0, n-1));
                if( i!=j )
                {
                    locerr = amp::maximum<Precision>(locerr, amp::abs<Precision>(sm));
                }
                else
                {
                    locerr = amp::maximum<Precision>(locerr, amp::abs<Precision>(sm-1));
                }
            }
        }
        orterr = amp::maximum<Precision>(orterr, locerr);

        //
        // values order error
        //
        for(i=1; i<=minmn-1; i++)
        {
            if( w(i)>w(i-1) )
            {
                wsorted = false;
            }
        }
    }


    template<unsigned int Precision>
    void testsvdproblem(const ap::template_2d_array< amp::ampf<Precision> >& a,
        int m,
        int n,
        amp::ampf<Precision>& materr,
        amp::ampf<Precision>& orterr,
        amp::ampf<Precision>& othererr,
        bool& wsorted,
        bool& wfailed)
    {
        ap::template_2d_array< amp::ampf<Precision> > u;
        ap::template_2d_array< amp::ampf<Precision> > vt;
        ap::template_2d_array< amp::ampf<Precision> > u2;
        ap::template_2d_array< amp::ampf<Precision> > vt2;
        ap::template_1d_array< amp::ampf<Precision> > w;
        ap::template_1d_array< amp::ampf<Precision> > w2;
        int i;
        int j;
        int k;
        int ujob;
        int vtjob;
        int memjob;
        int ucheck;
        int vtcheck;
        amp::ampf<Precision> v;
        amp::ampf<Precision> mx;



        //
        // Main SVD test
        //
        if( !svd::rmatrixsvd<Precision>(a, m, n, 2, 2, 2, w, u, vt) )
        {
            failcount = failcount+1;
            wfailed = true;
            return;
        }
        getsvderror<Precision>(a, m, n, u, w, vt, materr, orterr, wsorted);

        //
        // Additional SVD tests
        //
        for(ujob=0; ujob<=2; ujob++)
        {
            for(vtjob=0; vtjob<=2; vtjob++)
            {
                for(memjob=0; memjob<=2; memjob++)
                {
                    if( !svd::rmatrixsvd<Precision>(a, m, n, ujob, vtjob, memjob, w2, u2, vt2) )
                    {
                        failcount = failcount+1;
                        wfailed = true;
                        return;
                    }
                    ucheck = 0;
                    if( ujob==1 )
                    {
                        ucheck = ap::minint(m, n);
                    }
                    if( ujob==2 )
                    {
                        ucheck = m;
                    }
                    vtcheck = 0;
                    if( vtjob==1 )
                    {
                        vtcheck = ap::minint(m, n);
                    }
                    if( vtjob==2 )
                    {
                        vtcheck = n;
                    }
                    for(i=0; i<=m-1; i++)
                    {
                        for(j=0; j<=ucheck-1; j++)
                        {
                            othererr = amp::maximum<Precision>(othererr, amp::abs<Precision>(u(i,j)-u2(i,j)));
                        }
                    }
                    for(i=0; i<=vtcheck-1; i++)
                    {
                        for(j=0; j<=n-1; j++)
                        {
                            othererr = amp::maximum<Precision>(othererr, amp::abs<Precision>(vt(i,j)-vt2(i,j)));
                        }
                    }
                    for(i=0; i<=ap::minint(m, n)-1; i++)
                    {
                        othererr = amp::maximum<Precision>(othererr, amp::abs<Precision>(w(i)-w2(i)));
                    }
                }
            }
        }

        //
        // update counter
        //
        succcount = succcount+1;
    }


    /*!
************************************************************************
    Silent unit test
    ************************************************************************
*/
    template<unsigned int Precision>
    bool testsvdunit_test_silent()
    {
        bool result;


        result = testsvd<Precision>(true);
        return result;
    }


    /*!
************************************************************************
    Unit test
    ************************************************************************
*/
    template<unsigned int Precision>
    bool testsvdunit_test()
    {
        bool result;


        result = testsvd<Precision>(false);
        return result;
    }
} // namespace

#endif
