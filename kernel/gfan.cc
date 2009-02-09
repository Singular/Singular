/*
Compute the Grï¿½bner fan of an ideal
Author: $Author: monerjan $
Date: $Date: 2009-02-09 20:45:27 $
Header: $Header: /exports/cvsroot-2/cvsroot/kernel/gfan.cc,v 1.5 2009-02-09 20:45:27 monerjan Exp $
Id: $id$
*/

#include "mod2.h"
#include "kstd1.h"
#include "intvec.h"
#include "polys.h"
#include "ideals.h"

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
	printf("Computing Inequalities...\n");
	#endif

	// Exponentenvektor
	Exponent_t leadexp,aktexp,diffexp;
	poly aktpoly;

	int lengthGB=IDELEMS(I);
	printf("The Gröbner basis has %i elements\n",lengthGB);

	// We loop through each g\in GB
	for (int i=0; i<IDELEMS(I); i++)
	{
		aktpoly=(poly)I->m[i];
		leadexp = pGetExp(aktpoly,1); //get the exp.vect of leading monomial
		for (int j=2;j<=pLength(aktpoly);j++)
		{
			aktexp=pGetExp(aktpoly,j);
			//diffexp=pSubExp(aktpoly, leadexp,aktexp); //Dang! => runtime error
			//printf("Exponentenvektor=%i\n",expmark);
			//printf("Diff=%i\n",expmark-pGetExp(aktpoly,j));
		}
		int pCompCount;
		pCompCount=pLength(aktpoly);
		printf("Poly No. %i has %i components\n",i,pCompCount);
	} //for
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
