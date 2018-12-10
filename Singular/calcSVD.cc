#include <stdio.h>



#include "kernel/mod2.h"

#ifdef HAVE_SVD

#include "Singular/svd_si.h"
#include "kernel/structs.h"
#include "kernel/polys.h"
#include "polys/matpol.h"
#include "Singular/lists.h"

template class std::vector< amp::mpfr_record* >;

poly p_svdInit(char *s)
{
  poly p=pInit();
  n_Read(s, &pGetCoeff(p), currRing->cf);
  return p;
}

/*!
!
************************************************************************
Testing SVD decomposition subroutine
************************************************************************

*/
lists testsvd(matrix M)
{

    const unsigned int Precision=300;

    int max_i=M->nrows;
    int max_j=M->ncols;
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


    materr = 0;
    orterr = 0;
    othererr = 0;
    wsorted = true;
    wfailed = false;
    waserrors = false;
    maxmn = 50;
    threshold = 5*100*amp::ampf<Precision>::getAlgoPascalEpsilon();
    failthreshold = amp::ampf<Precision>("5.0E-3");
    a.setbounds(1, max_i, 1, max_j);


        //
        // fill matrix a entries from M
        //
        for(i=1; i<=max_i; i++)
        {
            for(j=1; j<=max_j; j++)
            {
            	char *str=pString(MATELEM(M,i,j));
              	Print(" to svd:%d,%d=%s\n",i,j,str);
                a(i,j) = amp::ampf<Precision>(str);
            }
        }
        //testsvdproblem<Precision>(a, max_i, max_j, materr, orterr, othererr, wsorted, wfailed);
    ap::template_2d_array< amp::ampf<Precision> > u;
    ap::template_2d_array< amp::ampf<Precision> > vt;
    ap::template_1d_array< amp::ampf<Precision> > w;
    svd::svddecomposition<Precision>(a, max_i, max_j, 2, 2, 2, w, u, vt);
    matrix Mu,Mw,Mvt;
    Mu=mpNew(max_i,max_i);
    Mw=mpNew(max_i,max_j);
    Mvt=mpNew(max_j,max_j);
    for(i=1;i<=max_i;i++)
    {
    	for(j=1;j<=max_i;j++)
    	{
    		MATELEM(Mu,i,j)=p_svdInit(u(i,j).toString());
//        		Print(" after svd:%d,%d=%s\n",i,j,u(i,j).toString());
    	}
    }
    for(i=1;i<=si_min(max_i,max_j);i++)
    {
    		MATELEM(Mw,i,i)=p_svdInit(w(i).toString());
//Print(" after svd:%d,%d=%s\n",i,w(i).toString());
    }
    for(i=1;i<=max_j;i++)
    {
    	for(j=1;j<=max_j;j++)
    	{
    		MATELEM(Mvt,i,j)=p_svdInit(vt(i,j).toString());
//Print(" after svd:%d,%d=%s\n",i,j,vt(i,j).toString());
    	}
    }
    lists L=(lists)omAlloc(sizeof(slists));
    L->Init(3);
    L->m[0].rtyp=MATRIX_CMD;
    L->m[1].rtyp=MATRIX_CMD;
    L->m[2].rtyp=MATRIX_CMD;
    L->m[0].data=(char*)Mu;
    L->m[1].data=(char*)Mw;
    L->m[2].data=(char*)Mvt;
    return L;
}

#endif
