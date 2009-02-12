/*
Compute the Groebner fan of an ideal
Author: $Author: Singular $
Date: $Date: 2009-02-12 08:35:05 $
Header: $Header: /exports/cvsroot-2/cvsroot/kernel/gfan.cc,v 1.11 2009-02-12 08:35:05 Singular Exp $
Id: $Id: gfan.cc,v 1.11 2009-02-12 08:35:05 Singular Exp $
*/

#include "mod2.h"

#ifdef HAVE_GFAN

#include "kstd1.h"
#include "intvec.h"
#include "polys.h"
#include "ideals.h"
#include "kmatrix.h"
#include "/users/urmel/alggeom/monerjan/cddlib/include/setoper.h" //Support for cddlib. Dirty hack
#include "/users/urmel/alggeom/monerjan/cddlib/include/cdd.h"

#ifndef gfan_DEBUG
#define gfan_DEBUG
#endif

ideal getGB(ideal inputIdeal)
{
	#ifdef gfan_DEBUG
	printf("Now in getGB\n");
	#endif

  	ideal gb;
	gb=kStd(inputIdeal,NULL,testHomog,NULL); //Possible to call without testHomog/isHomog?
	idSkipZeroes(gb); //Get rid of zero entries

	return gb;
}

/****** getWallIneq computes the inequalities ***/
/*INPUT_TYPE: ideal                             */
/*RETURN_TYPE: matrix                           */
/************************************************/
void getWallIneq(ideal I)
{
	#ifdef gfan_DEBUG
	printf("*** Computing Inequalities... ***\n");
	#endif

	//All variables go here - except ineq matrix and *v, see below
	int lengthGB=IDELEMS(I);	// # of polys in the groebner basis
	int pCompCount;			// # of terms in a poly
	poly aktpoly;
	int numvar = pVariables; 	// # of variables in a polynomial (or ring?)
	int leadexp[numvar];		// dirty hack of exp.vects
	int aktexp[numvar];
	int cols,rows; 			// will contain the dimensions of the ineq matrix - deprecated by
	dd_rowrange ddrows;
	dd_colrange ddcols;
	dd_rowset ddredrows;		// # of redundant rows in ddineq
	dd_NumberType ddnumb=dd_Real;	//Number type
	dd_ErrorType dderr=dd_NoError;	//
	// End of var declaration

	printf("The Groebner basis has %i elements\n",lengthGB);
	printf("The current ring has %i variables\n",numvar);
	cols = numvar;

	//Compute the # inequalities i.e. rows of the matrix
	rows=0; //Initialization
	for (int ii=0;ii<IDELEMS(I);ii++)
	{
		aktpoly=(poly)I->m[ii];
		rows=rows+pLength(aktpoly)-1;
	}
	printf("rows=%i\n",rows);
	printf("Will create a %i x %i - matrix to store inequalities\n",rows,cols);

	dd_rowrange aktmatrixrow=0;	// needed to store the diffs of the expvects in the rows of ddineq
	dd_set_global_constants();
	ddrows=rows;
	ddcols=cols;
	dd_MatrixPtr ddineq; 		//Matrix to store the inequalities
	ddineq=dd_CreateMatrix(ddrows,ddcols);

	// We loop through each g\in GB and compute the resulting inequalities
	for (int i=0; i<IDELEMS(I); i++)
	{
		aktpoly=(poly)I->m[i];		//get aktpoly as i-th component of I
		pCompCount=pLength(aktpoly);	//How many terms does aktpoly consist of?
		printf("Poly No. %i has %i components\n",i,pCompCount);

		int *v=(int *)omAlloc((numvar+1)*sizeof(int));
		pGetExpV(aktpoly,v);	//find the exp.vect in v[1],...,v[n], use pNext(p)
		
		//Store leadexp for aktpoly
		for (int kk=0;kk<numvar;kk++)
		{
			leadexp[kk]=v[kk+1];
			//printf("Leadexpcomp=%i\n",leadexp[kk]);
			//Since we need to know the difference of leadexp with the other expvects we do nothing here
			//but compute the diff below
		}

		
		while (pNext(aktpoly)!=NULL) //move to next term until NULL
		{
			aktpoly=pNext(aktpoly);
			pSetm(aktpoly);		//doesn't seem to help anything
			pGetExpV(aktpoly,v);
			for (int kk=0;kk<numvar;kk++)
			{
//The ordering somehow gets lost here but this is acceptable, since we are only interested in the inequalities
				aktexp[kk]=v[kk+1];
				//printf("aktexpcomp=%i\n",aktexp[kk]);
				//ineq[aktmatrixrow][kk]=leadexp[kk]-aktexp[kk];	//dito
				dd_set_si(ddineq->matrix[(dd_rowrange)aktmatrixrow][kk],leadexp[kk]-aktexp[kk]);
			}
			aktmatrixrow=aktmatrixrow+1;
		}//while

	} //for

	// The inequalities are now stored in ddineq
	// Next we check for superflous rows
	ddredrows = dd_RedundantRows(ddineq, &dderr);
	if (dderr!=dd_NoError)			// did an error occur?
	{
		 dd_WriteErrorMessages(stderr,dderr);	//if so tell us
	} else
	{
		printf("Redundant rows: ");
		set_fwrite(stdout, ddredrows);		//otherwise print the redundant rows
	}//if dd_Error

	#ifdef gfan_DEBUGs
	printf("Inequalitiy matrix\n");
	for (int i=0;i<rows;i++)
	{
		for (int j=0;j<cols;j++)
		{
			printf("%i ",ineq[i][j]);
		}
		printf("\n");
	}
	#endif
	//res=(ideal)aktpoly;
	//return res;
}

ideal gfan(ideal inputIdeal)
{
	#ifdef gfan_DEBUG
	printf("Now in subroutine gfan\n");
	#endif
	ideal res;
	matrix ineq; //Matrix containing the boundary inequalities

	res=getGB(inputIdeal);
	getWallIneq(res);
	return res;
}
#endif
