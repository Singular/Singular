/*
Compute the Groebner fan of an ideal
$Author: monerjan $
$Date: 2009-04-21 15:23:54 $
$Header: /exports/cvsroot-2/cvsroot/kernel/gfan.cc,v 1.37 2009-04-21 15:23:54 monerjan Exp $
$Id: gfan.cc,v 1.37 2009-04-21 15:23:54 monerjan Exp $
*/

#include "mod2.h"

#ifdef HAVE_GFAN

#include "kstd1.h"
#include "kutil.h"
#include "intvec.h"
#include "polys.h"
#include "ideals.h"
#include "kmatrix.h"
#include "fast_maps.h"	//Mapping of ideals
#include "maps.h"
#include "ring.h"
#include "prCopy.h"
#include <iostream.h>	//deprecated

/*remove the following at your own risk*/
#ifndef GMPRATIONAL
#define GMPRATIONAL
#endif

//Hacks for different working places
#define ITWM

#ifdef UNI
#include "/users/urmel/alggeom/monerjan/cddlib/include/setoper.h" //Support for cddlib. Dirty hack
#include "/users/urmel/alggeom/monerjan/cddlib/include/cdd.h"
#endif

#ifdef HOME
#include "/home/momo/studium/diplomarbeit/cddlib/include/setoper.h"
#include "/home/momo/studium/diplomarbeit/cddlib/include/cdd.h"
#endif

#ifdef ITWM
#include "/u/slg/monerjan/cddlib/include/setoper.h"
#include "/u/slg/monerjan/cddlib/include/cdd.h"
#include "/u/slg/monerjan/cddlib/include/cddmp.h"
#endif

#ifndef gfan_DEBUG
#define gfan_DEBUG
#endif

//#include gcone.h

/**
*\brief Class facet
*	Implements the facet structure as a linked list
*
*/
class facet
{
	private:
		/** \brief Inner normal of the facet, describing it uniquely up to isomorphism */
		intvec *fNormal;
		
		/** \brief The Groebner basis on the other side of a shared facet
		 *
		 * In order not to have to compute the flipped GB twice we store the basis we already get
		 * when identifying search facets. Thus in the next step of the reverse search we can 
		 * just copy the old cone and update the facet and the gcBasis.
		 * facet::flibGB is set via facet::setFlipGB() and printed via facet::printFlipGB
		 */
		ideal flipGB;		//The Groebner Basis on the other side, computed via gcone::flip

			
	public:
		/** The default constructor. Do I need a constructor of type facet(intvec)? */
		facet()			
		{
			// Pointer to next facet.  */
			/* Defaults to NULL. This way there is no need to check explicitly */
			this->next=NULL; 
		}
		
		/** The default destructor */
		~facet(){;}
		
		/** Stores the facet normal \param intvec*/
		void setFacetNormal(intvec *iv){
			this->fNormal = ivCopy(iv);
			//return;
		}
		
		/** Hopefully returns the facet normal */
		intvec *getFacetNormal()
		{	
			//this->fNormal->show();	cout << endl;	
			return this->fNormal;
		}
		
		/** Method to print the facet normal*/
		void printNormal()
		{
			fNormal->show();
		}
		
		/** Store the flipped GB*/
		void setFlipGB(ideal I)
		{
			this->flipGB=I;
		}
		
		/** Print the flipped GB*/
		void printFlipGB()
		{
			idShow(this->flipGB);
		}
		
		bool isFlippable;	//flippable facet? Want to have cone->isflippable.facet[i]
		bool isIncoming;	//Is the facet incoming or outgoing?
		facet *next;		//Pointer to next facet
};

/**
*\brief Implements the cone structure
*
* A cone is represented by a linked list of facet normals
* @see facet
*/
/*class gcone
finally this should become s.th. like gconelib.{h,cc} to provide an API
*/
class gcone
{
	private:
		int numFacets;		//#of facets of the cone		

	public:
		/** \brief Default constructor. 
		*
		* Initialises this->next=NULL and this->facetPtr=NULL
		*/
		gcone()
		{
			this->next=NULL;
			this->facetPtr=NULL;
		}
		~gcone();		//destructor
		
		/** Pointer to the first facet */
		facet *facetPtr;	//Will hold the adress of the first facet; set by gcone::getConeNormals
		poly gcMarkedTerm; 	//marked terms of the cone's Groebner basis
		int numVars;		//#of variables in the ring
		
		/** Contains the Groebner basis of the cone. Is set by gcone::getGB(ideal I)*/
		ideal gcBasis;		//GB of the cone, set by gcone::getGB();
		gcone *next;		//Pointer to *previous* cone in search tree
		
		/** \brief Compute the normals of the cone
		*
		* This method computes a representation of the cone in terms of facet normals. It takes an ideal
		* as its input. Redundancies are automatically removed using cddlib's dd_MatrixCanonicalize.
		* Other methods for redundancy checkings might be implemented later. See Anders' diss p.44.
		* Note that in order to use cddlib a 0-th column has to be added to the matrix since cddlib expects 
		* each row to represent an inequality of type const+x1+...+xn <= 0. While computing the normals we come across
		* the set \f$ \partial\mathcal{G} \f$ which we might store for later use. C.f p71 of journal
		* As a result of this procedure the pointer facetPtr points to the first facet of the cone.
		*/
		void getConeNormals(ideal const &I)
		{
#ifdef gfan_DEBUG
			std::cout << "*** Computing Inequalities... ***" << std::endl;
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
			dd_rowindex ddnewpos;		  // all to make dd_Canonicalize happy
			dd_NumberType ddnumb=dd_Integer; //Number type
			dd_ErrorType dderr=dd_NoError;	//
			// End of var declaration
#ifdef gfan_DEBUG
			cout << "The Groebner basis has " << lengthGB << " elements" << endl;
			cout << "The current ring has " << numvar << " variables" << endl;
#endif
			cols = numvar;
		
			//Compute the # inequalities i.e. rows of the matrix
			rows=0; //Initialization
			for (int ii=0;ii<IDELEMS(I);ii++)
			{
				aktpoly=(poly)I->m[ii];
				rows=rows+pLength(aktpoly)-1;
			}
#ifdef gfan_DEBUG
			cout << "rows=" << rows << endl;
			cout << "Will create a " << rows << " x " << cols << " matrix to store inequalities" << endl;
#endif
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
						aktexp[kk]=v[kk+1];
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
			ddrows = ddineq->rowsize;	//Size of the matrix with redundancies removed
			ddcols = ddineq->colsize;
#ifdef gfan_DEBUG
			cout << "Having removed redundancies, the normals now read:" << endl;
			dd_WriteMatrix(stdout,ddineq);
			cout << "Rows = " << ddrows << endl;
			cout << "Cols = " << ddcols << endl;
#endif
			
			/*Write the normals into class facet*/
#ifdef gfan_DEBUG
			cout << "Creating list of normals" << endl;
#endif
			/*The pointer *fRoot should be the return value of this function*/
			facet *fRoot = new facet();	//instantiate new facet
			this->facetPtr = fRoot;		//set variable facetPtr of class gcone to first facet
			facet *fAct; 			//instantiate pointer to active facet
			fAct = fRoot;			//Seems to do the trick. fRoot and fAct have to point to the same adress!
			std::cout << "fRoot = " << fRoot << ", fAct = " << fAct << endl;
			for (int kk = 0; kk<ddrows; kk++)
			{
				intvec *load = new intvec(numvar);	//intvec to store a single facet normal that will then be stored via setFacetNormal
				for (int jj = 1; jj <ddcols; jj++)
				{
#ifdef GMPRATIONAL
					double foo;
					foo = mpq_get_d(ddineq->matrix[kk][jj]);
#ifdef gfan_DEBUG
					std::cout << "fAct is " << foo << " at " << fAct << std::endl;
#endif
					(*load)[jj-1] = (int)foo;		//store typecasted entry at pos jj-1 of load
#endif
#ifndef GMPRATIONAL
					double *foo;
					foo = (double*)ddineq->matrix[kk][jj];	//get entry from actual position#endif
#ifdef gfan_DEBUG
					std::cout << "fAct is " << *foo << " at " << fAct << std::endl;
#endif
					(*load)[jj-1] = (int)*foo;		//store typecasted entry at pos jj-1 of load
#endif //GMPRATIONAL					
			

					//(*load)[jj-1] = (int)foo;		//store typecasted entry at pos jj-1 of load
					//check for flipability here
					if (jj<ddcols)				//Is this facet NOT the last facet? Writing while instead of if is a really bad idea :)
					{
						//fAct->next = new facet();	//If so: instantiate new facet. Otherwise this->next=NULL due to the constructor						
					}
				}//for (int jj = 1; jj <ddcols; jj++)
				/*Quick'n'dirty hack for flippability*/	
				bool isFlippable;			
				for (int jj = 0; jj<this->numVars; jj++)
				{					
					intvec *ivCanonical = new intvec(this->numVars);
					(*ivCanonical)[jj]=1;					
					if (dotProduct(load,ivCanonical)>=0)
					{
						isFlippable=FALSE;						
					}
					else
					{
						isFlippable=TRUE;
					}					
					delete ivCanonical;
				}//for (int jj = 0; jj<this->numVars; jj++)
				if (isFlippable==FALSE)
				{
					cout << "Ignoring facet";
					load->show();
					//fAct->next=NULL;
				}
				else
				{	/*Now load should be full and we can call setFacetNormal*/
					fAct->setFacetNormal(load);
					fAct->next = new facet();
					//fAct->printNormal();
					fAct=fAct->next;	//this should definitely not be called in the above while-loop :D
				}//if (isFlippable==FALSE)
				delete load;
			}//for (int kk = 0; kk<ddrows; kk++)
			/*
			Now we should have a linked list containing the facet normals of those facets that are
			-irredundant
			-flipable
			Adressing is done via *facetPtr
			*/
			
			//Clean up but don't delete the return value! (Whatever it will turn out to be)
			dd_FreeMatrix(ddineq);
			set_free(ddredrows);
			free(ddnewpos);
			set_free(ddlinset);
			dd_free_global_constants();

		}//method getConeNormals(ideal I)	
		
		
		/** \brief Compute the Groebner Basis on the other side of a shared facet 
		*
		* Implements algorithm 4.3.2 from Anders' thesis.
		* As shown there it is not necessary to compute an interior point. The knowledge of the facet normal
		* suffices. A term \f$ x^\gamma \f$ of \f$ g \f$ is in \f$  in_\omega(g) \f$ iff \f$ \gamma - leadexp(g)\f$
		* is parallel to \f$ leadexp(g) \f$
		* Parallelity is checked using basic linear algebra. See gcone::isParallel.
		* Other possibilities includes  computing the rank of the matrix consisting of the vectors in question and
		* computing an interior point of the facet and taking all terms having the same weight with respect 
		* to this interior point.
		*\param ideal, facet
		* Input: a marked,reduced Groebner basis and a facet
		*/
		void flip(ideal gb, facet *f)		//Compute "the other side"
		{			
			intvec *fNormal = new intvec(this->numVars);	//facet normal, check for parallelity			
			fNormal = f->getFacetNormal();	//read this->fNormal;
#ifdef gfan_DEBUG
			std::cout << "===" << std::endl;
			std::cout << "running gcone::flip" << std::endl;
			std::cout << "fNormal=";
			fNormal->show();
			std::cout << std::endl;
#endif				
			/*1st step: Compute the initial ideal*/
			poly initialFormElement[IDELEMS(gb)];	//array of #polys in GB to store initial form
			ideal initialForm=idInit(IDELEMS(gb),this->gcBasis->rank);
			poly aktpoly;
			intvec *check = new intvec(this->numVars);	//array to store the difference of LE and v
			
			for (int ii=0;ii<IDELEMS(gb);ii++)
			{
				aktpoly = (poly)gb->m[ii];								
				int *v=(int *)omAlloc((this->numVars+1)*sizeof(int));
				int *leadExpV=(int *)omAlloc((this->numVars+1)*sizeof(int));
				pGetExpV(aktpoly,leadExpV);	//find the leading exponent in leadExpV[1],...,leadExpV[n], use pNext(p)
				initialFormElement[ii]=pHead(aktpoly);
				
				while(pNext(aktpoly)!=NULL)	/*loop trough terms and check for parallelity*/
				{
					aktpoly=pNext(aktpoly);	//next term
					pSetm(aktpoly);
					pGetExpV(aktpoly,v);		
					/* Convert (int)v into (intvec)check */			
					for (int jj=0;jj<this->numVars;jj++)
					{
						//cout << "v["<<jj+1<<"]="<<v[jj+1]<<endl;
						//cout << "leadExpV["<<jj+1<<"]="<<leadExpV[jj+1]<<endl;
						(*check)[jj]=v[jj+1]-leadExpV[jj+1];
					}
#ifdef gfan_DEBUG
					cout << "check=";			
					check->show();
					cout << endl;
#endif
					if (isParallel(check,fNormal)) //pass *check when 
					{
						cout << "Parallel vector found, adding to initialFormElement" << endl;			
						initialFormElement[ii] = pAdd(pCopy(initialFormElement[ii]),(poly)pHead(aktpoly));
					}						
				}//while
#ifdef gfan_DEBUG
				cout << "Initial Form=";				
				pWrite(initialFormElement[ii]);
				cout << "---" << endl;
#endif
				/*Now initialFormElement must be added to (ideal)initialForm */
				initialForm->m[ii]=initialFormElement[ii];
			}//for
			f->setFlipGB(initialForm);	//FIXME PROBABLY WRONG TO STORE HERE SINCE INA!=flibGB			
#ifdef gfan_DEBUG
			cout << "Initial ideal is: " << endl;
			idShow(initialForm);
			f->printFlipGB();
			cout << "===" << endl;
#endif
			delete check;
			
			/*2nd step: lift initial ideal to a GB of the neighbouring cone using minus alpha as weight*/
			/*Substep 2.1
			compute $G_{-\alpha}(in_v(I)) 
			see journal p. 66
			*/
			ring srcRing=currRing;
			ring dstRing=rCopy0(srcRing);
			dstRing->order[0]=ringorder_a;
			dstRing->wvhdl[0] =( int *)omAlloc((fNormal->length())*sizeof(int)); //found in Singular/ipshell.cc
			
			for (int ii=0;ii<this->numVars;ii++)
			{				
				dstRing->wvhdl[0][ii]=-(*fNormal)[ii];	//What exactly am I doing here?
				//cout << tmpring->wvhdl[0][ii] << endl;
			}
			rComplete(dstRing);
			rChangeCurrRing(dstRing);
			
			rWrite(currRing); cout << endl;
			ideal ina;			
			ina=idrCopyR(initialForm,srcRing);			
#ifdef gfan_DEBUG
			cout << "ina=";
			idShow(ina); cout << endl;
#endif
			ideal H;
			H=kStd(ina,NULL,isHomog,NULL);	//we know it is homogeneous
			idSkipZeroes(H);
#ifdef gfan_DEBUG
			cout << "H="; idShow(H); cout << endl;
#endif
			/*Substep 2.2
			do the lifting and mark according to H
			*/
			rChangeCurrRing(srcRing);
			ideal srcRing_H;
			ideal srcRing_HH;			
			srcRing_H=idrCopyR(H,dstRing);
			idShow(srcRing_H);			
			srcRing_HH=ffG(srcRing_H,this->gcBasis);		
			idShow(srcRing_HH);
			/*Substep 2.2.1
			Mark according to G_-\alpha
			Here we have a minimal basis srcRing_HH. In order to turn this basis into a reduced basis
			we have to compute an interior point of C(srcRing_HH). For this we need to know the cone
			represented by srcRing_HH MARKED ACCORDING TO G_{-\alpha}
			Thus we check whether the leading monomials of srcRing_HH and srcRing_H coincide. If not we 
			compute the difference accordingly
			*/
			dd_set_global_constants();
			bool markingsAreCorrect=FALSE;
			dd_MatrixPtr intPointMatrix;
			int iPMatrixRows=0;
			dd_rowrange aktrow=0;			
			for (int ii=0;ii<IDELEMS(srcRing_HH);ii++)
			{
				poly aktpoly=(poly)srcRing_HH->m[ii];
				iPMatrixRows = iPMatrixRows+pLength(aktpoly)-1;
			}
			/* additionally one row for the standard-simplex and another for a row that becomes 0 during
			construction of the differences
			*/
			intPointMatrix = dd_CreateMatrix(iPMatrixRows+2,this->numVars+1);
			intPointMatrix->numbtype=dd_Integer;	//NOTE: DO NOT REMOVE OR CHANGE TO dd_Rational
			
			for (int ii=0;ii<IDELEMS(srcRing_HH);ii++)
			{
				markingsAreCorrect=FALSE;	//crucial to initialise here
				poly aktpoly=srcRing_HH->m[ii];
				/*Comparison of leading monomials is done via exponent vectors*/
				for (int jj=0;jj<IDELEMS(H);jj++)
				{
					int *src_ExpV = (int *)omAlloc((this->numVars+1)*sizeof(int));
					int *dst_ExpV = (int *)omAlloc((this->numVars+1)*sizeof(int));
					pGetExpV(aktpoly,src_ExpV);
					rChangeCurrRing(dstRing);	//this ring change is crucial!
					pGetExpV(pCopy(H->m[ii]),dst_ExpV);
					rChangeCurrRing(srcRing);
					bool expVAreEqual=TRUE;
					for (int kk=1;kk<=this->numVars;kk++)
					{
						cout << src_ExpV[kk] << "," << dst_ExpV[kk] << endl;
						if (src_ExpV[kk]!=dst_ExpV[kk])
						{
							expVAreEqual=FALSE;
						}
					}					
					//if (*src_ExpV == *dst_ExpV)
					if (expVAreEqual==TRUE)
					{
						markingsAreCorrect=TRUE; //everything is fine
						cout << "correct markings" << endl;
					}//if (pHead(aktpoly)==pHead(H->m[jj])
					delete src_ExpV;
					delete dst_ExpV;
				}//for (int jj=0;jj<IDELEMS(H);jj++)
				
				int *v=(int *)omAlloc((this->numVars+1)*sizeof(int));
				int *leadExpV=(int *)omAlloc((this->numVars+1)*sizeof(int));
				if (markingsAreCorrect==TRUE)
				{
					pGetExpV(aktpoly,leadExpV);
				}
				else
				{
					rChangeCurrRing(dstRing);
					pGetExpV(pHead(H->m[ii]),leadExpV); //We use H->m[ii] as leading monomial
					rChangeCurrRing(srcRing);
				}
				/*compute differences of the expvects*/				
				while (pNext(aktpoly)!=NULL)
				{
					/*The following if-else-block makes sure the first term (i.e. the wrongly marked term) 
					is not omitted when computing the differences*/
					if(markingsAreCorrect==TRUE)
					{
						aktpoly=pNext(aktpoly);
						pGetExpV(aktpoly,v);
					}
					else
					{
						pGetExpV(pHead(aktpoly),v);
						markingsAreCorrect=TRUE;
					}

					for (int jj=0;jj<this->numVars;jj++)
					{				
						/*Store into ddMatrix*/								
						dd_set_si(intPointMatrix->matrix[aktrow][jj+1],leadExpV[jj+1]-v[jj+1]);
					}
					aktrow +=1;
				}
				delete v;
				delete leadExpV;
			}//for (int ii=0;ii<IDELEMS(srcRing_HH);ii++)
			/*Now we add the constraint for the standard simplex*/
			/*NOTE:Might actually work without the standard simplex*/
			dd_set_si(intPointMatrix->matrix[aktrow][0],-1);
			for (int jj=1;jj<=this->numVars;jj++)
			{
				dd_set_si(intPointMatrix->matrix[aktrow][jj],1);
			}
			dd_WriteMatrix(stdout,intPointMatrix);
			interiorPoint(intPointMatrix);	//TODO This should finally return an intvec
			dd_FreeMatrix(intPointMatrix);
			//dd_free_global_constants();
			
			/*Step 3
			turn the minimal basis into a reduced one
			*/
			
		}//void flip(ideal gb, facet *f)
				
		/** \brief Compute the remainder of a polynomial by a given ideal
		*
		* Compute \f$ f^{\mathcal{G}} \f$
		* Algorithm is taken from Cox, Little, O'Shea, IVA 2nd Ed. p 62
		* However, since we are only interested in the remainder, there is no need to
		* compute the factors \f$ a_i \f$
		*/
		//NOTE: Should be replaced by kNF or kNF2
		poly restOfDiv(poly const &f, ideal const &I)
		{
			cout << "Entering restOfDiv" << endl;
			poly p=f;
			pWrite(p);
			//poly r=kCreateZeroPoly(,currRing,currRing);	//The 0-polynomial, hopefully
			poly r=NULL;	//The zero polynomial
			int ii;
			bool divOccured;
			
			while (p!=NULL)
			{
				ii=1;
				divOccured=FALSE;
				
				while( (ii<=IDELEMS(I) && (divOccured==FALSE) ))
				{					
					if (pDivisibleBy(I->m[ii-1],p))	//does LM(I->m[ii]) divide LM(p) ?
					{
						//cout << "TICK 3" << endl;
						poly step1,step2,step3;
						cout << "dividing "; pWrite(pHead(p));cout << "by ";pWrite(pHead(I->m[ii-1])); cout << endl;
						step1 = pDivideM(pHead(p),pHead(I->m[ii-1]));
						cout << "LT(p)/LT(f_i)="; pWrite(step1); cout << endl;
						//cout << "TICK 3.1" << endl;
						step2 = ppMult_qq(step1, I->m[ii-1]);
						//cout << "TICK 3.2" << endl;
						step3 = pSub(pCopy(p), step2);
						//p=pSub(p,pMult( pDivide(pHead(p),pHead(I->m[ii])), I->m[ii]));
						//cout << "TICK 4" << endl;
						//pSetm(p);
						pSort(step3); //must be here, otherwise strange behaviour with many +o+o+o+o+ terms
						p=step3;
						pWrite(p);						
						divOccured=TRUE;
					}
					else
					{
						ii += 1;
					}//if (pLmDivisibleBy(I->m[ii],p,currRing))
				}//while( (ii<IDELEMS(I) && (divOccured==FALSE) ))
				if (divOccured==FALSE)
				{
					//cout << "TICK 5" << endl;
					r=pAdd(pCopy(r),pHead(p));
					pSetm(r);
					pSort(r);
					cout << "r="; pWrite(r); cout << endl;
					//cout << "TICK 6" << endl;
					if (pLength(p)!=1)
					{
						p=pSub(pCopy(p),pHead(p));	//Here it may occur that p=0 instead of p=NULL
					}
					else
					{
						p=NULL;	//Hack to correct this situation						
					}
					//cout << "TICK 7" << endl;
					cout << "p="; pWrite(p);
				}//if (divOccured==FALSE)
			}//while (p!=0)
			return r;
		}//poly restOfDiv(poly const &f, ideal const &I)
		
		/** \brief Compute \f$ f-f^{\mathcal{G}} \f$
		*/
		//NOTE: use kNF or kNF2 instead of restOfDivision
		ideal ffG(ideal const &H, ideal const &G)
		{
			cout << "Entering ffG" << endl;
			int size=IDELEMS(H);
			ideal res=idInit(size,1);
			poly temp1, temp2, temp3;	//polys to temporarily store values for pSub
			for (int ii=0;ii<size;ii++)
			{
				res->m[ii]=restOfDiv(H->m[ii],G);
				//res->m[ii]=kNF(H->m[ii],G);
				temp1=H->m[ii];
				temp2=res->m[ii];				
				temp3=pSub(temp1, temp2);
				res->m[ii]=temp3;
				//res->m[ii]=pSub(temp1,temp2); //buggy
				//pSort(res->m[ii]);
				//pSetm(res->m[ii]);
				cout << "res->m["<<ii<<"]=";pWrite(res->m[ii]);						
			}			
			return res;
		}
		
		/** \brief Compute a Groebner Basis
		*
		* Computes the Groebner basis and stores the result in gcone::gcBasis
		*\param ideal
		*\return void
		*/
		void getGB(ideal const &inputIdeal)
		{
			ideal gb;
			gb=kStd(inputIdeal,NULL,testHomog,NULL);
			idSkipZeroes(gb);
			this->gcBasis=gb;	//write the GB into gcBasis
		}//void getGB
		
		ideal GenGrbWlk(ideal, ideal);	//Implementation of the Generic Groebner Walk. Needed for a) Computing the sink and b) finding search facets


		/** \brief Compute the dot product of two intvecs
		*
		*/
		int dotProduct(intvec const &iva, intvec const &ivb)				
		{
			//intvec iva=a;
			//intvec ivb=b;
			int res=0;
			for (int i=0;i<this->numVars;i++)
			{
				res = res+(iva[i]*ivb[i]);
			}
			return res;
		}//int dotProduct

		/** \brief Check whether two intvecs are parallel
		*
		* \f$ \alpha\parallel\beta\Leftrightarrow\langle\alpha,\beta\rangle^2=\langle\alpha,\alpha\rangle\langle\beta,\beta\rangle \f$
		*/
		bool isParallel(intvec const &a, intvec const &b)
		{			
			int lhs,rhs;
			lhs=dotProduct(a,b)*dotProduct(a,b);
			rhs=dotProduct(a,a)*dotProduct(b,b);
			cout << "LHS="<<lhs<<", RHS="<<rhs<<endl;
			if (lhs==rhs)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}//bool isParallel
		
		void interiorPoint(dd_MatrixPtr &M) //no const &M here since we want to remove redundant rows
		{
			dd_LPPtr lp,lpInt;
			dd_ErrorType err=dd_NoError;
			dd_LPSolverType solver=dd_DualSimplex;
			dd_LPSolutionPtr lpSol=NULL;
			dd_rowset ddlinset,ddredrows;	//needed for dd_FindRelativeInterior
			dd_rowindex ddnewpos;
			dd_NumberType numb;	
			//M->representation=dd_Inequality;
			//M->objective-dd_LPMin;  //Not sure whether this is needed
			dd_set_si(M->rowvec[0],1);dd_set_si(M->rowvec[1],1);dd_set_si(M->rowvec[2],1);
			//cout << "TICK 1" << endl;
						
			//dd_MatrixCanonicalize(&M, &ddlinset, &ddredrows, &ddnewpos, &err);
			//if (err!=dd_NoError){cout << "Error during dd_MatrixCanonicalize" << endl;}
			//cout << "Tick 2" << endl;
			//dd_WriteMatrix(stdout,M);
			
			lp=dd_Matrix2LP(M, &err);
			if (err!=dd_NoError){cout << "Error during dd_Matrix2LP in gcone::interiorPoint" << endl;}			
			if (lp==NULL){cout << "LP is NULL" << endl;}
			dd_WriteLP(stdout,lp);
			//cout << "Tick 3" << endl;
						
			lpInt=dd_MakeLPforInteriorFinding(lp);
			if (err!=dd_NoError){cout << "Error during dd_MakeLPForInteriorFinding in gcone::interiorPoint" << endl;}
			dd_WriteLP(stdout,lpInt);
			//cout << "Tick 4" << endl;
			
			dd_FindRelativeInterior(M,&ddlinset,&ddredrows,&lpSol,&err);
			if (err!=dd_NoError)
			{
				cout << "Error during dd_FindRelativeInterior in gcone::interiorPoint" << endl;
				dd_WriteErrorMessages(stdout, err);
			}
			
			dd_LPSolve(lpInt,solver,&err);	//This will not result in a point from the relative interior
			if (err!=dd_NoError){cout << "Error during dd_LPSolve" << endl;}
			//cout << "Tick 5" << endl;
									
			//lpSol=dd_CopyLPSolution(lpInt);
			if (err!=dd_NoError){cout << "Error during dd_CopyLPSolution" << endl;}			
			//cout << "Tick 6" << endl;
			
			cout << "Interior point: ";
			for (int ii=1; ii<(lpSol->d)-1;ii++)
			{
				dd_WriteNumber(stdout,lpSol->sol[ii]);				
			}
			dd_FreeLPSolution(lpSol);
			dd_FreeLPData(lpInt);
			dd_FreeLPData(lp);
			set_free(ddlinset);
			set_free(ddredrows);
			/*At this point we have an interior point of type mpq_t 
			Need to convert to an intvec
			*/
			
		}//void interiorPoint(dd_MatrixPtr const &M)
};//class gcone

ideal gfan(ideal inputIdeal)
{
	int numvar = pVariables; 
	
	#ifdef gfan_DEBUG
	cout << "Now in subroutine gfan" << endl;
	#endif
	ring inputRing=currRing;	// The ring the user entered
	ring rootRing;			// The ring associated to the target ordering
	ideal res;
	//matrix ineq; 			//Matrix containing the boundary inequalities
	facet *fRoot;
	
	/*
	1. Select target order, say dp.
	2. Compute GB of inputIdeal wrt target order -> newRing, setCurrRing etc...
	3. getConeNormals
	*/
	
	/* Construct a new ring which will serve as our root
	Does not yet work as expected. Will work fine with order dp,Dp but otherwise hangs in getGB
	resolved 07.04.2009 MM
	*/
	rootRing=rCopy0(currRing);
	rootRing->order[0]=ringorder_lp;
	rComplete(rootRing);
	rChangeCurrRing(rootRing);
	
	/* Fetch the inputIdeal into our rootRing */
	map theMap=(map)idMaxIdeal(1);	//evil hack!
	theMap->preimage=NULL;	//neccessary?
	ideal rootIdeal;
	rootIdeal=fast_map(inputIdeal,inputRing,(ideal)theMap, currRing);
#ifdef gfan_DEBUG
	cout << "Root ideal is " << endl;
	idShow(rootIdeal);
	cout << "The root ring is " << endl;
	rWrite(rootRing);
	cout << endl;
#endif	
	
	gcone *gcRoot = new gcone();	//Instantiate the sink
	gcone *gcAct;
	gcAct = gcRoot;
	gcAct->numVars=pVariables;
	gcAct->getGB(rootIdeal);	//sets gcone::gcBasis
	idShow(gcAct->gcBasis);
	gcAct->getConeNormals(gcAct->gcBasis);	//hopefully compute the normals
	gcAct->flip(gcAct->gcBasis,gcAct->facetPtr);
	/*Now it is time to compute the search facets, respectively start the reverse search.
	But since we are in the root all facets should be search facets. IS THIS TRUE?
	MIND: AS OF NOW, THE LIST OF FACETS IS NOT PURGED OF NON-FLIPPAPLE FACETS
	*/
	
	/*As of now extra.cc expects gfan to return type ideal. Probably this will change in near future.
	The return type will then be of type LIST_CMD
	Assume gfan has finished, thus we have enumerated all the cones
	Create an array of size of #cones. Let each entry in the array contain a pointer to the respective
	Groebner Basis and merge this somehow with LIST_CMD
	=> Count the cones!
	*/
	rChangeCurrRing(inputRing);
	res=gcAct->gcBasis;
	//cout << fRoot << endl;
	return res;
	//return GBlist;
}
/*
Since gfan.cc is #included from extra.cc there must not be a int main(){} here
*/
#endif
