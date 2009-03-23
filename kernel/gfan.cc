/*
Compute the Groebner fan of an ideal
$Author: monerjan $
$Date: 2009-03-23 16:59:56 $
$Header: /exports/cvsroot-2/cvsroot/kernel/gfan.cc,v 1.18 2009-03-23 16:59:56 monerjan Exp $
$Id: gfan.cc,v 1.18 2009-03-23 16:59:56 monerjan Exp $
*/

#include "mod2.h"

#ifdef HAVE_GFAN

#include "kstd1.h"
#include "intvec.h"
#include "polys.h"
#include "ideals.h"
#include "kmatrix.h"
#include "iostream.h"

//Hacks for different working places
#define HOME

#ifdef UNI
#include "/users/urmel/alggeom/monerjan/cddlib/include/setoper.h" //Support for cddlib. Dirty hack
#include "/users/urmel/alggeom/monerjan/cddlib/include/cdd.h"
#endif

#ifdef HOME
#include "/home/momo/studium/diplomarbeit/cddlib/include/setoper.h"
#include "/home/momo/studium/diplomarbeit/cddlib/include/cdd.h"
#endif

#ifdef ITWM
#endif

#ifndef gfan_DEBUG
#define gfan_DEBUG
#endif

class facet
{
	private:
		intvec fNormal;		//inner normal, describing the facet uniquely
	public:
		facet(intvec FN)		//constructor
		{
			fNormal = FN;
		}
		
		~facet();		//destructor
		void printNormal(){
			std::cout << "The normal is = " << "fNormal" << endl;
			return;
		}
		
		bool isFlippable;	//flippable facet? Want to have cone->isflippable.facet[i]
		bool isIncoming;	//Is the facet incoming or outgoing?
};

/*class gcone
finally this should become s.th. like gconelib.{h,cc} to provide an API
*/
class gcone
{
	private:
		int numFacets;		//#of facets of the cone

	public:
		gcone(int);		//constructor with dimension
		~gcone();		//destructor
		poly gcMarkedTerm; 	//marked terms of the cone's Gr�bner basis
		ideal gcBasis;		//GB of the cone
		gcone *next;		//Pointer to *previous* cone in search tree
	
		void flip();		//Compute "the other side"
		void remRedFacets();	//Remove redundant facets of the cone NOT NEEDED since this is already done by cddlib while compunting the normals
		
		ideal GenGrbWlk(ideal, ideal);	//Implementation of the Generic Groebner Walk. Needed for a) Computing the sink and b) finding search facets
		

};//class gcone

ideal getGB(ideal inputIdeal)
{
	#ifdef gfan_DEBUG
	cout << "Computing a groebner basis..." << endl;
	#endif

  	ideal gb;
	gb=kStd(inputIdeal,NULL,testHomog,NULL); //Possible to call without testHomog/isHomog?
	idSkipZeroes(gb); //Get rid of zero entries

	return gb;
}

/****** getConeNormals computes the inequalities ***/
/*INPUT_TYPE: ideal                             */
/*RETURN_TYPE: matrix                           */
/************************************************/
void getConeNormals(ideal I)
{
	#ifdef gfan_DEBUG
	cout << "*** Computing Inequalities... ***" << endl;
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
	dd_rowset ddlinset;		// the opposite
	dd_rowindex ddnewpos;		// all to make dd_Canonicalize happy
	dd_NumberType ddnumb=dd_Real;	//Number type
	dd_ErrorType dderr=dd_NoError;	//
	// End of var declaration

	cout << "The Groebner basis has " << lengthGB << " elements" << endl;
	cout << "The current ring has " << numvar << " variables" << endl;
	cols = numvar;

	//Compute the # inequalities i.e. rows of the matrix
	rows=0; //Initialization
	for (int ii=0;ii<IDELEMS(I);ii++)
	{
		aktpoly=(poly)I->m[ii];
		rows=rows+pLength(aktpoly)-1;
	}
	cout << "rows=" << rows << endl;
	cout << "Will create a " << rows << " x " << cols << " matrix to store inequalities" << endl;

	dd_rowrange aktmatrixrow=0;	// needed to store the diffs of the expvects in the rows of ddineq
	dd_set_global_constants();
	ddrows=rows;
	ddcols=cols;
	dd_MatrixPtr ddineq; 		//Matrix to store the inequalities
	ddineq=dd_CreateMatrix(ddrows,ddcols+1); //The first col has to be 0 since cddlib checks for additive consts there

	// We loop through each g\in GB and compute the resulting inequalities
	for (int i=0; i<IDELEMS(I); i++)
	{
		aktpoly=(poly)I->m[i];		//get aktpoly as i-th component of I
		pCompCount=pLength(aktpoly);	//How many terms does aktpoly consist of?
		cout << "Poly No. " << i << " has " << pCompCount << " components" << endl;

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
				dd_set_si(ddineq->matrix[(dd_rowrange)aktmatrixrow][kk+1],leadexp[kk]-aktexp[kk]); //because of the 1st col being const 0
			}
			aktmatrixrow=aktmatrixrow+1;
		}//while

	} //for

	//Maybe add another row to contain the constraints of the standard simplex?

	#ifdef gfan_DEBUG
	cout << "The inequality matrix is" << endl;
	dd_WriteMatrix(stdout, ddineq);
	#endif

	// The inequalities are now stored in ddineq
	// Next we check for superflous rows
	ddredrows = dd_RedundantRows(ddineq, &dderr);
	if (dderr!=dd_NoError)			// did an error occur?
	{
		 dd_WriteErrorMessages(stderr,dderr);	//if so tell us
	} else
	{
		cout << "Redundant rows: ";
		set_fwrite(stdout, ddredrows);		//otherwise print the redundant rows
	}//if dd_Error

	//Remove reduntant rows here!
	dd_MatrixCanonicalize(&ddineq, &ddlinset, &ddredrows, &ddnewpos, &dderr);
	#ifdef gfan_DEBUG
	cout << "Having removed redundancies, the normals now read:" << endl;
	dd_WriteMatrix(stdout,ddineq);
	#endif

	/*dd_PolyhedraPtr ddpolyh;
	dd_MatrixPtr G;
	ddpolyh=dd_DDMatrix2Poly(ddineq, &dderr);
	G=dd_CopyGenerators(ddpolyh);
	printf("\nSpanning vectors = rows:\n");
	dd_WriteMatrix(stdout, G);
	*/
	
	
	/*Write the normals into class facet
		How do I get the #rows in ddineq? \exists s.th. like dd_NumRows?
	Strange enough: facet *f=new facet(intvec(2,3)) does work for the constructor facet(intvec FN){fNormal = FN;}
	*/
	for (int kk = 1; kk<=2; kk++){
		facet *f = new facet(intvec(2,3));
		//facet *fPtr = &f;
		cout << &f << endl;
 		f->printNormal();
	}
	
	//ddineq->representation=dd_Inequality;		//We want our LP to be Ax>=0

	//Clean up but don't delete the return value! (Whatever it will turn out to be)
	dd_FreeMatrix(ddineq);
	//set_free(ddrows);
	//set_free(ddcols);
	set_free(ddredrows);
	//set_free(ddnumb);
	//set_free(dderr);
	free(ddnewpos);
	//set_free(ddlinset);
	dd_free_global_constants();

	//res=(ideal)aktpoly;
	//return res;
}

ideal gfan(ideal inputIdeal)
{
	#ifdef gfan_DEBUG
	cout << "Now in subroutine gfan" << endl;
	#endif
	ideal res;
	matrix ineq; //Matrix containing the boundary inequalities
	/*
	1. Select target order
	2. Compute GB of inputIdeal wrt target order
	3. getConeNormals
	*/
	res=getGB(inputIdeal);
	getConeNormals(res);
	return res;
}
#endif
