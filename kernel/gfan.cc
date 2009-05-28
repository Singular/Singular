/*
Compute the Groebner fan of an ideal
$Author: monerjan $
$Date: 2009-05-28 06:27:09 $
$Header: /exports/cvsroot-2/cvsroot/kernel/gfan.cc,v 1.56 2009-05-28 06:27:09 monerjan Exp $
$Id: gfan.cc,v 1.56 2009-05-28 06:27:09 monerjan Exp $
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
#include <iostream>
#include <bitset>
#include <fstream>	//read-write cones to files
#include <gmp.h>
//#include <gmpxx.h>

/*DO NOT REMOVE THIS*/
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
using namespace std;

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
		
		/** \brief An interior point of the facet*/
		intvec *interiorPoint;
		
		/** \brief Universal Cone Number
		* The number of the cone the facet belongs to
		*/
		int UCN;
		
		/** \brief The Groebner basis on the other side of a shared facet
		 *
		 * In order not to have to compute the flipped GB twice we store the basis we already get
		 * when identifying search facets. Thus in the next step of the reverse search we can 
		 * just copy the old cone and update the facet and the gcBasis.
		 * facet::flibGB is set via facet::setFlipGB() and printed via facet::printFlipGB
		 */
		ideal flipGB;		//The Groebner Basis on the other side, computed via gcone::flip		
			
	public:		
		//bool isFlippable;	//flippable facet? Want to have cone->isflippable.facet[i]
		bool isIncoming;	//Is the facet incoming or outgoing?
		facet *next;		//Pointer to next facet
		//intvec **codim2Normals =(intvec**)omAlloc0(sizeof(intvec*));	//Integer matrix containing the (codim-2)-facets
		struct c2N{
			intvec normal;
			intvec *next;
		};
				
		/** The default constructor. Do I need a constructor of type facet(intvec)? */
		facet()			
		{
			// Pointer to next facet.  */
			/* Defaults to NULL. This way there is no need to check explicitly */
			this->next=NULL; 
			this->UCN=NULL;
			//this->codim2Normals=NULL;			
		}
		
		/** The default destructor */
		~facet(){;}
		
		/** Stores the facet normal \param intvec*/
		void setFacetNormal(intvec *iv)
		{
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
		
		/** Return the flipped GB*/
		ideal getFlipGB()
		{
			return this->flipGB;
		}
		
		/** Print the flipped GB*/
		void printFlipGB()
		{
			idShow(this->flipGB);
		}
		
		void setUCN(int n)
		{
			this->UCN=n;
		}
		
		int getUCN()
		{
			return this->UCN;
		}
		
		void setInteriorPoint(intvec *iv)
		{
			this->interiorPoint = ivCopy(iv);
		}
		
		intvec *getInteriorPoint()
		{
			return this->interiorPoint;
		}
		
		/*bool isFlippable(intvec &load)
		{
			bool res=TRUE;			
			int jj;
			for (int jj = 0; jj<load.length(); jj++)
			{
				intvec *ivCanonical = new intvec(load.length());
				(*ivCanonical)[jj]=1;				
				if (ivMult(&load,ivCanonical)<0)
				{
					res=FALSE;
					break;
				}
			}
			return res;
			
			/*while (dotProduct(load,ivCanonical)>=0)
			{
				if (jj!=this->numVars)
				{
					intvec *ivCanonical = new intvec(this->numVars);
					(*ivCanonical)[jj]=1;			
				 	res=TRUE;
					jj += 1;
				} 
			}
			if (jj==this->numVars)
			{			
				delete ivCanonical;
				return FALSE;
			}
			else
			{
				delete ivCanonical;
				return TRUE;
		}*/						
		//}//bool isFlippable(facet &f)
		
		
		friend class gcone;	//Bad style
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
		ring rootRing;		//good to know this -> generic walk
		ideal inputIdeal;	//the original
		ring baseRing;		//the basering of the cone		
		/* TODO in order to save memory use pointers to rootRing and inputIdeal instead */
		intvec *ivIntPt;	//an interior point of the cone
		int UCN;		//unique number of the cone
		
	public:	
		/** \brief Pointer to the first facet */
		facet *facetPtr;	//Will hold the adress of the first facet; set by gcone::getConeNormals
		
		/** # of variables in the ring */
		int numVars;		//#of variables in the ring
		
		/** # of facets of the cone
		* This value is set by gcone::getConeNormals
		*/
		int numFacets;		//#of facets of the cone
		
		/** Contains the Groebner basis of the cone. Is set by gcone::getGB(ideal I)*/
		ideal gcBasis;		//GB of the cone, set by gcone::getGB();
		gcone *next;		//Pointer to *previous* cone in search tree	
		
		/** \brief Default constructor. 
		*
		* Initialises this->next=NULL and this->facetPtr=NULL
		*/
		gcone()
		{
			this->next=NULL;
			this->facetPtr=NULL;
			this->baseRing=currRing;
			this->UCN=1;
		}
		
		/** \brief Constructor with ring and ideal
		*
		* This constructor takes the root ring and the root ideal as parameters and stores 
		* them in the private members gcone::rootRing and gcone::inputIdeal
		* Since knowledge of the root ring is only needed when using reverse search,
		* this constructor is not needed when using the "second" method
		*/
		gcone(ring r, ideal I)
		{
			this->next=NULL;
			this->facetPtr=NULL;
			this->rootRing=r;
			this->inputIdeal=I;
			this->baseRing=currRing;
			this->UCN=1;
		}
		
		/** \brief Copy constructor 
		*
		* Copies one cone, sets this->gcBasis to the flipped GB and reverses the 
		* direction of the according facet normal
		*/			
		//NOTE Prolly need to specify the facet to flip over
		gcone(const gcone& gc)		
		{
			this->next=NULL;
			this->numVars=gc.numVars;
			this->UCN=(gc.UCN)+1;	//add 1 to the UCN of previous cone
			facet *fAct= new facet();			
			this->facetPtr=fAct;
			
			intvec *ivtmp = new intvec(this->numVars);						
			ivtmp = gc.facetPtr->getFacetNormal();			
			
			ideal gb;
			gb=gc.facetPtr->getFlipGB();			
			this->gcBasis=gb;	//this cone's GB is the flipped GB			
			
			/*Reverse direction of the facet normal to make it an inner normal*/			
			for (int ii=0; ii<this->numVars;ii++)
			{
				(*ivtmp)[ii]=-(*ivtmp)[ii];				
			}
			
			fAct->setFacetNormal(ivtmp);
			delete ivtmp;
			delete fAct;						
		}
		
		/** \brief Default destructor */
		~gcone(){;}		//destructor
			
		
		/** \brief Set the interior point of a cone */
		void setIntPoint(intvec *iv)
		{
			this->ivIntPt=ivCopy(iv);
		}
		
		/** \brief Return the interior point */
		intvec *getIntPoint()
		{
			return this->ivIntPt;
		}
		
		void showIntPoint()
		{
			ivIntPt->show();
		}
		
		void showFacets()
		{
			facet *f = new facet();
			f = this->facetPtr;
			intvec *iv = new intvec(this->numVars);
			
			while (f->next!=NULL)
			{
				iv = f->getFacetNormal();
				iv->show();
				f=f->next;
			}
 			//delete iv;
 			//delete f;
		}
		
		/** \brief Set gcone::numFacets */
		void setNumFacets()
		{
		}
		
		/** \brief Get gcone::numFacets */
		int getNumFacets()
		{
			return this->numFacets;
		}
		
		int getUCN()
		{
			return this->UCN;
		}
		
		/** \brief Compute the normals of the cone
		*
		* This method computes a representation of the cone in terms of facet normals. It takes an ideal
		* as its input. Redundancies are automatically removed using cddlib's dd_MatrixCanonicalize.
		* Other methods for redundancy checkings might be implemented later. See Anders' diss p.44.
		* Note that in order to use cddlib a 0-th column has to be added to the matrix since cddlib expects 
		* each row to represent an inequality of type const+x1+...+xn <= 0. While computing the normals we come across
		* the set \f$ \partial\mathcal{G} \f$ which we might store for later use. C.f p71 of journal
		* As a result of this procedure the pointer facetPtr points to the first facet of the cone.
		*
		* Optionally, if the parameter bool compIntPoint is set to TRUE the method will also compute
		* an interior point of the cone.
		*/
		void getConeNormals(ideal const &I, bool compIntPoint=FALSE)
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
#ifdef gfan_DEBUG
				cout << "Poly No. " << i << " has " << pCompCount << " components" << endl;
#endif
		
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
			//std::cout << "fRoot = " << fRoot << ", fAct = " << fAct << endl;
			for (int kk = 0; kk<ddrows; kk++)
			{
				intvec *load = new intvec(this->numVars);	//intvec to store a single facet normal that will then be stored via setFacetNormal
				for (int jj = 1; jj <ddcols; jj++)
				{
					double foo;
					foo = mpq_get_d(ddineq->matrix[kk][jj]);
#ifdef gfan_DEBUG
					std::cout << "fAct is " << foo << " at " << fAct << std::endl;
#endif
					(*load)[jj-1] = (int)foo;		//store typecasted entry at pos jj-1 of load		
				}//for (int jj = 1; jj <ddcols; jj++)
				
				/*Quick'n'dirty hack for flippability*/	
				bool isFlippable=FALSE;				
				for (int jj = 0; jj<load->length(); jj++)
				{
					intvec *ivCanonical = new intvec(load->length());
					(*ivCanonical)[jj]=1;
					cout << "dotProd=" << dotProduct(*load,*ivCanonical) << endl;
					if (dotProduct(*load,*ivCanonical)<0)	
					//if (ivMult(load,ivCanonical)<0)
					{
						isFlippable=TRUE;
						break;	//URGHS
					}
				}	
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
					fAct=fAct->next;	//this should definitely not be called in the above while-loop :D
				}//if (isFlippable==FALSE)
				//delete load;
			}//for (int kk = 0; kk<ddrows; kk++)
			
			/*
			Now we should have a linked list containing the facet normals of those facets that are
			-irredundant
			-flipable
			Adressing is done via *facetPtr
			*/
			
			if (compIntPoint==TRUE)
			{
				intvec *iv = new intvec(this->numVars);
				interiorPoint(ddineq, *iv);	//NOTE ddineq contains non-flippable facets
				this->setIntPoint(iv);	//stores the interior point in gcone::ivIntPt
				//delete iv;
			}
			
			//Compute the number of facets
			this->numFacets=ddineq->rowsize;
			
			//Clean up but don't delete the return value! (Whatever it will turn out to be)			
			//dd_FreeMatrix(ddineq);
			//set_free(ddredrows);
			//free(ddnewpos);
			//set_free(ddlinset);
			//NOTE Commented out. Solved the bug that after facet2Matrix there were facets lost
			//THIS SUCKS
			//dd_free_global_constants();

		}//method getConeNormals(ideal I)	
		
		
		/** \brief Compute the Groebner Basis on the other side of a shared facet 
		*
		* Implements algorithm 4.3.2 from Anders' thesis.
		* As shown there it is not necessary to compute an interior point. The knowledge of the facet normal
		* suffices. A term \f$ x^\gamma \f$ of \f$ g \f$ is in \f$  in_\omega(g) \f$ iff \f$ \gamma - leadexp(g)\f$
		* is parallel to \f$ leadexp(g) \f$
		* Parallelity is checked using basic linear algebra. See gcone::isParallel.
		* Other possibilities include computing the rank of the matrix consisting of the vectors in question and
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
// 			std::cout << "fNormal=";
// 			fNormal->show();
// 			std::cout << std::endl;
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
// 					cout << "check=";			
// 					check->show();
// 					cout << endl;
#endif
					//TODO why not *check, *fNormal????
					if (isParallel(*check,*fNormal)) //pass *check when 
					{
// 						cout << "Parallel vector found, adding to initialFormElement" << endl;			
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
#ifdef gfan_DEBUG
			cout << "Initial ideal is: " << endl;
			idShow(initialForm);
			//f->printFlipGB();
			cout << "===" << endl;
#endif
			//delete check;
			
			/*2nd step: lift initial ideal to a GB of the neighbouring cone using minus alpha as weight*/
			/*Substep 2.1
			compute $G_{-\alpha}(in_v(I)) 
			see journal p. 66
			*/
			ring srcRing=currRing;

			//intvec *negfNormal = new intvec(this->numVars);
			//negfNormal=ivNeg(fNormal);
			ring tmpRing=rCopyAndAddWeight(srcRing,ivNeg(fNormal));
			rChangeCurrRing(tmpRing);
			
			//rWrite(currRing); cout << endl;
			
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
// 			cout << "H="; idShow(H); cout << endl;
#endif
			/*Substep 2.2
			do the lifting and mark according to H
			*/
			rChangeCurrRing(srcRing);
			ideal srcRing_H;
			ideal srcRing_HH;			
			srcRing_H=idrCopyR(H,tmpRing);
#ifdef gfan_DEBUG
// 			cout << "srcRing_H = ";
// 			idShow(srcRing_H); cout << endl;
#endif
			srcRing_HH=ffG(srcRing_H,this->gcBasis);		
#ifdef gfan_DEBUG
// 			cout << "srcRing_HH = ";
// 			idShow(srcRing_HH); cout << endl;
#endif
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
					rChangeCurrRing(tmpRing);	//this ring change is crucial!
					pGetExpV(pCopy(H->m[ii]),dst_ExpV);
					rChangeCurrRing(srcRing);
					bool expVAreEqual=TRUE;
					for (int kk=1;kk<=this->numVars;kk++)
					{
#ifdef gfan_DEBUG
						//cout << src_ExpV[kk] << "," << dst_ExpV[kk] << endl;
#endif
						if (src_ExpV[kk]!=dst_ExpV[kk])
						{
							expVAreEqual=FALSE;
						}
					}					
					//if (*src_ExpV == *dst_ExpV)
					if (expVAreEqual==TRUE)
					{
						markingsAreCorrect=TRUE; //everything is fine
#ifdef gfan_DEBUG
//						cout << "correct markings" << endl;
#endif
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
					rChangeCurrRing(tmpRing);
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
			intvec *iv_weight = new intvec(this->numVars);
			interiorPoint(intPointMatrix, *iv_weight);	//iv_weight now contains the interior point
			dd_FreeMatrix(intPointMatrix);
			dd_free_global_constants();
			
			/*Step 3
			turn the minimal basis into a reduced one
			*/
			int i,j;
			ring dstRing=rCopy0(srcRing);
			i=rBlocks(srcRing);
			
			dstRing->order=(int *)omAlloc((i+1)*sizeof(int));
			for(j=i;j>0;j--)
			{
				dstRing->order[j]=srcRing->order[j-1];
				dstRing->block0[j]=srcRing->block0[j-1];
				dstRing->block1[j]=srcRing->block1[j-1];
				if (srcRing->wvhdl[j-1] != NULL)
				{
					dstRing->wvhdl[j] = (int*) omMemDup(srcRing->wvhdl[j-1]);
				}
			}
			dstRing->order[0]=ringorder_a;
			dstRing->order[1]=ringorder_dp;
			dstRing->order[2]=ringorder_C;			
			dstRing->wvhdl[0] =( int *)omAlloc((iv_weight->length())*sizeof(int));
			
			for (int ii=0;ii<this->numVars;ii++)
			{				
				dstRing->wvhdl[0][ii]=(*iv_weight)[ii];				
			}
			rComplete(dstRing);
			
			// NOTE May assume that at this point srcRing already has 3 blocks of orderins, starting with a
			// Thus: 
			//ring dstRing=rCopyAndChangeWeight(srcRing,iv_weight);
			//cout << "PLING" << endl;
			/*ring dstRing=rCopy0(srcRing);
			for (int ii=0;ii<this->numVars;ii++)
			{				
				dstRing->wvhdl[0][ii]=(*iv_weight)[ii];				
			}*/
			rChangeCurrRing(dstRing);
#ifdef gfan_DEBUG
			rWrite(dstRing); cout << endl;
#endif
			ideal dstRing_I;			
			dstRing_I=idrCopyR(srcRing_HH,srcRing);			
			//validOpts<1>=TRUE;
#ifdef gfan_DEBUG
			//idShow(dstRing_I);
#endif			
			BITSET save=test;
			test|=Sy_bit(OPT_REDSB);
			test|=Sy_bit(6);	//OPT_DEBUG					
			dstRing_I=kStd(idrCopyR(this->inputIdeal,this->rootRing),NULL,testHomog,NULL);					
			kInterRed(dstRing_I);
			idSkipZeroes(dstRing_I);
			test=save;
			/*End of step 3 - reduction*/
			
			f->setFlipGB(dstRing_I);//store the flipped GB
#ifdef gfan_DEBUG
			cout << "Flipped GB is: " << endl;
			f->printFlipGB();
#endif			
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
			//pWrite(p);
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
						poly step1,step2,step3;
						//cout << "dividing "; pWrite(pHead(p));cout << "by ";pWrite(pHead(I->m[ii-1])); cout << endl;
						step1 = pDivideM(pHead(p),pHead(I->m[ii-1]));
						//cout << "LT(p)/LT(f_i)="; pWrite(step1); cout << endl;				
						step2 = ppMult_qq(step1, I->m[ii-1]);						
						step3 = pSub(pCopy(p), step2);
						//p=pSub(p,pMult( pDivide(pHead(p),pHead(I->m[ii])), I->m[ii]));			
						//pSetm(p);
						pSort(step3); //must be here, otherwise strange behaviour with many +o+o+o+o+ terms
						p=step3;
						//pWrite(p);						
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
					//cout << "r="; pWrite(r); cout << endl;
					
					if (pLength(p)!=1)
					{
						p=pSub(pCopy(p),pHead(p));	//Here it may occur that p=0 instead of p=NULL
					}
					else
					{
						p=NULL;	//Hack to correct this situation						
					}					
					//cout << "p="; pWrite(p);
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
				//cout << "res->m["<<ii<<"]=";pWrite(res->m[ii]);						
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
		
		/** \brief The Generic Groebner Walk due to FJLT
		* Needed for computing the search facet
		*/
		ideal GenGrbWlk(ideal, ideal)
		{
		}//GGW
		
		/** \brief Compute the negative of a given intvec
		*/		
		intvec *ivNeg(intvec const &iv)
		{
			intvec *res = new intvec(this->numVars);
			for(int ii=0;ii<this->numVars;ii++)
			{
				res[ii] = -(int)iv[ii];
			}
			return res;
		}


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
			//cout << "LHS="<<lhs<<", RHS="<<rhs<<endl;
			if (lhs==rhs)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}//bool isParallel
		
		/** \brief Compute an interior point of a given cone
		* Result will be written into intvec iv
		*/
		void interiorPoint(dd_MatrixPtr const &M, intvec &iv) //no const &M here since we want to remove redundant rows
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
			
			//NOTE: Make this n-dimensional!
			//dd_set_si(M->rowvec[0],1);dd_set_si(M->rowvec[1],1);dd_set_si(M->rowvec[2],1);
									
			//dd_MatrixCanonicalize(&M, &ddlinset, &ddredrows, &ddnewpos, &err);
			//if (err!=dd_NoError){cout << "Error during dd_MatrixCanonicalize" << endl;}
			//cout << "Tick 2" << endl;
			//dd_WriteMatrix(stdout,M);
			
			lp=dd_Matrix2LP(M, &err);
			if (err!=dd_NoError){cout << "Error during dd_Matrix2LP in gcone::interiorPoint" << endl;}			
			if (lp==NULL){cout << "LP is NULL" << endl;}
#ifdef gfan_DEBUG
//			dd_WriteLP(stdout,lp);
#endif	
					
			lpInt=dd_MakeLPforInteriorFinding(lp);
			if (err!=dd_NoError){cout << "Error during dd_MakeLPForInteriorFinding in gcone::interiorPoint" << endl;}
#ifdef gfan_DEBUG
// 			dd_WriteLP(stdout,lpInt);
#endif			

			dd_FindRelativeInterior(M,&ddlinset,&ddredrows,&lpSol,&err);
			if (err!=dd_NoError)
			{
				cout << "Error during dd_FindRelativeInterior in gcone::interiorPoint" << endl;
				dd_WriteErrorMessages(stdout, err);
			}
			
			//dd_LPSolve(lpInt,solver,&err);	//This will not result in a point from the relative interior
			if (err!=dd_NoError){cout << "Error during dd_LPSolve" << endl;}
			//cout << "Tick 5" << endl;
									
			//lpSol=dd_CopyLPSolution(lpInt);
			if (err!=dd_NoError){cout << "Error during dd_CopyLPSolution" << endl;}			
			//cout << "Tick 6" << endl;
#ifdef gfan_DEBUG
			cout << "Interior point: ";
#endif
			for (int ii=1; ii<(lpSol->d)-1;ii++)
			{
#ifdef gfan_DEBUG
				dd_WriteNumber(stdout,lpSol->sol[ii]);
#endif
				/* NOTE This works only as long as gmp returns fractions with the same denominator*/
				(iv)[ii-1]=(int)mpz_get_d(mpq_numref(lpSol->sol[ii]));	//looks evil, but does the trick
			}
			dd_FreeLPSolution(lpSol);
			dd_FreeLPData(lpInt);
			dd_FreeLPData(lp);
			set_free(ddlinset);
			set_free(ddredrows);			
			
		}//void interiorPoint(dd_MatrixPtr const &M)
		
		/** \brief Copy a ring and add a weighted ordering in first place
		* Kudos to walkSupport.cc
		*/
		ring rCopyAndAddWeight(ring const &r, intvec const *ivw)				
		{
			ring res=(ring)omAllocBin(ip_sring_bin);
			memcpy4(res,r,sizeof(ip_sring));
			res->VarOffset = NULL;
			res->ref=0;
			
			if (r->algring!=NULL)
				r->algring->ref++;
			if (r->parameter!=NULL)
			{
				res->minpoly=nCopy(r->minpoly);
				int l=rPar(r);
				res->parameter=(char **)omAlloc(l*sizeof(char_ptr));
				int i;
				for(i=0;i<rPar(r);i++)
				{
					res->parameter[i]=omStrDup(r->parameter[i]);
				}
			}
			
			int i=rBlocks(r);
			int jj;
			
			res->order =(int *)omAlloc((i+1)*sizeof(int));
			res->block0=(int *)omAlloc((i+1)*sizeof(int));
			res->block1=(int *)omAlloc((i+1)*sizeof(int));
			res->wvhdl =(int **)omAlloc((i+1)*sizeof(int**));
			for(jj=0;jj<i;jj++)
			{				
				if (r->wvhdl[jj] != NULL)
				{
					res->wvhdl[jj] = (int*) omMemDup(r->wvhdl[jj-1]);
				}
				else
				{
					res->wvhdl[jj+1]=NULL;
				}
			}
			
			for (jj=0;jj<i;jj++)
			{
				res->order[jj+1]=r->order[jj];
				res->block0[jj+1]=r->block0[jj];
				res->block1[jj+1]=r->block1[jj];
			}						
			
			res->order[0]=ringorder_a;
			res->order[1]=ringorder_dp;	//basically useless, since that should never be used			
			int length=ivw->length();
			int *A=(int *)omAlloc(length*sizeof(int));
			for (jj=0;jj<length;jj++)
			{				
				A[jj]=(*ivw)[jj];				
			}			
			res->wvhdl[0]=(int *)A;
			res->block0[0]=1;
			res->block1[0]=length;
			
			res->names = (char **)omAlloc0(rVar(r) * sizeof(char_ptr));
			for (i=rVar(res)-1;i>=0; i--)
			{
				res->names[i] = omStrDup(r->names[i]);
			}			
			rComplete(res);
			return res;
		}//rCopyAndAdd
		
		ring rCopyAndChangeWeight(ring const &r, intvec *ivw)
		{
			ring res=rCopy0(currRing);
			rComplete(res);
			rSetWeightVec(res,(int64*)ivw);
			//rChangeCurrRing(rnew);
			return res;
		}
		
		/** \brief Checks whether a given facet is a search facet
		* Determines whether a given facet of a cone is the search facet of a neighbouring cone
		* This is done in the following way:
		* We loop through all facets of the cone and find the "smallest" facet, i.e. the unique facet
		* that is first crossed during the generic walk.
		* We then check whether the fNormal of this facet is parallel to the fNormal of our testfacet.
		* If this is the case, then our facet is indeed a search facet and TRUE is retuned. 
		*/
		bool isSearchFacet(gcone &gcTmp, facet *testfacet)
		{				
			ring actRing=currRing;
			facet *facetPtr=(facet*)gcTmp.facetPtr;			
			facet *fMin=new facet(*facetPtr);	//Pointer to the "minimal" facet
			//facet *fMin = new facet(tmpcone.facetPtr);
			//fMin=tmpcone.facetPtr;		//Initialise to first facet of tmpcone
			facet *fAct;	//Ptr to alpha_i
			facet *fCmp;	//Ptr to alpha_j
			fAct = fMin;
			fCmp = fMin->next;				
			
			rChangeCurrRing(this->rootRing);	//because we compare the monomials in the rootring			
			poly p=pInit();
			poly q=pInit();
			intvec *alpha_i = new intvec(this->numVars);			
			intvec *alpha_j = new intvec(this->numVars);
			intvec *sigma = new intvec(this->numVars);
			sigma=gcTmp.getIntPoint();
			
			int *u=(int *)omAlloc((this->numVars+1)*sizeof(int));
			int *v=(int *)omAlloc((this->numVars+1)*sizeof(int));
			u[0]=0; v[0]=0;
			int weight1,weight2;
			while(fAct->next->next!=NULL)	//NOTE this is ugly. Can it be done without fCmp?
			{
				/* Get alpha_i and alpha_{i+1} */
				alpha_i=fAct->getFacetNormal();
				alpha_j=fCmp->getFacetNormal();
#ifdef gfan_DEBUG
				alpha_i->show(); 
				alpha_j->show();
#endif
				/*Compute the dot product of sigma and alpha_{i,j}*/
				weight1=dotProduct(sigma,alpha_i);
				weight2=dotProduct(sigma,alpha_j);
#ifdef gfan_DEBUG
				cout << "weight1=" << weight1 << " " << "weight2=" << weight2 << endl;
#endif
				/*Adjust alpha_i and alpha_i+1 accordingly*/
				for(int ii=1;ii<=this->numVars;ii++)
				{					
					u[ii]=weight1*(*alpha_i)[ii-1];
					v[ii]=weight2*(*alpha_j)[ii-1];
				}				
				
				/*Now p_weight and q_weight need to be compared as exponent vectors*/
				pSetCoeff0(p,nInit(1));
				pSetCoeff0(q,nInit(1));
				pSetExpV(p,u); 
				pSetm(p);			
				pSetExpV(q,v); 
				pSetm(q);
#ifdef gfan_DEBUG				
				pWrite(p);pWrite(q);
#endif	
				/*We want to check whether x^p < x^q 
				=> want to check for return value 1 */
				if (pLmCmp(p,q)==1)	//i.e. x^q is smaller
				{
					fMin=fCmp;
					fAct=fMin;
					fCmp=fCmp->next;
				}
				else
				{
					//fAct=fAct->next;
					if(fCmp->next!=NULL)
					{
						fCmp=fCmp->next;
					}
					else
					{
						fAct=fAct->next;
					}
				}
				//fAct=fAct->next;
			}//while(fAct.facetPtr->next!=NULL)
			delete alpha_i,alpha_j,sigma;
			
			/*If testfacet was minimal then fMin should still point there */
			
			//if(fMin->getFacetNormal()==ivNeg(testfacet.getFacetNormal()))			
#ifdef gfan_DEBUG
			cout << "Checking for parallelity" << endl <<" fMin is";
			fMin->printNormal();
			cout << "testfacet is ";
			testfacet->printNormal();
			cout << endl;
#endif
			if (fMin==gcTmp.facetPtr)			
			//if(areEqual(fMin->getFacetNormal(),ivNeg(testfacet.getFacetNormal())))
			//if (isParallel(fMin->getFacetNormal(),testfacet->getFacetNormal()))
			{				
				cout << "Parallel" << endl;
				rChangeCurrRing(actRing);
				//delete alpha_min, test;
				return TRUE;
			}
			else 
			{
				cout << "Not parallel" << endl;
				rChangeCurrRing(actRing);
				//delete alpha_min, test;
				return FALSE;
			}
		}//bool isSearchFacet
		
		/** \brief Check for equality of two intvecs
		*/
		bool areEqual(intvec const &a, intvec const &b)
		{
			bool res=TRUE;
			for(int ii=0;ii<this->numVars;ii++)
			{
				if(a[ii]!=b[ii])
				{
					res=FALSE;
					break;
				}
			}
			return res;
		}
		
		/** \brief The reverse search algorithm
		*/
		void reverseSearch(gcone *gcAct) //no const possible here since we call gcAct->flip
		{
			facet *fAct=new facet();
			fAct = gcAct->facetPtr;			
			
			while(fAct->next!=NULL)  //NOTE NOT SURE WHETHER THIS IS RIGHT! Do I reach EVERY facet or only all but the last?
			{
				cout << "==========================================================================================="<< endl;
				gcAct->flip(gcAct->gcBasis,gcAct->facetPtr);
				gcone *gcTmp = new gcone(*gcAct);
				//idShow(gcTmp->gcBasis);
				gcTmp->getConeNormals(gcTmp->gcBasis, TRUE);
#ifdef gfan_DEBUG
				facet *f = new facet();
				f=gcTmp->facetPtr;
				while(f->next!=NULL)
				{
					f->printNormal();
					f=f->next;					
				}
#endif
				gcTmp->showIntPoint();
				/*recursive part goes gere*/
				if (isSearchFacet(*gcTmp,(facet*)gcAct->facetPtr))
				{
					gcAct->next=gcTmp;
					cout << "PING"<< endl;
					reverseSearch(gcTmp);
				}
				else
				{
					delete gcTmp;
					/*NOTE remove fAct from linked list. It's no longer needed*/
				}
				/*recursion ends*/
				fAct = fAct->next;		
			}//while(fAct->next!=NULL)
		}//reverseSearch
		
		/** \brief The new method of Markwig and Jensen
		* Compute gcBasis and facets for the arbitrary starting cone. Store \f$(codim-1)\f$-facets as normals.
		* In order to represent a facet uniquely compute also the \f$(codim-2)\f$-facets and norm by the gcd of the components.
		* Keep a list of facets as a linked list containing an intvec and an integer matrix.
		* Since a \f$(codim-1)\f$-facet belongs to exactly two full dimensional cones, we remove a facet from the list as
		* soon as we compute this facet again. Comparison of facets is done by...
		*/
		void noRevS(gcone &gc, bool usingIntPoint=FALSE)
		{
			facet *fListPtr = new facet();	//The list containing ALL facets we come across			
			facet *fAct = new facet();
			fAct = gc.facetPtr;
			
#ifdef gfan_DEBUG
			cout << "NoRevs" << endl;
			cout << "Facets are:" << endl;
 			gc.showFacets();
#endif
			
			dd_set_global_constants();
			dd_rowrange ddrows;
			dd_colrange ddcols;
			ddrows=2;	//Each facet is described by its normal
			ddcols=gc.numVars+1;	// plus one for the standard simplex
			if(usingIntPoint){
				while(fAct->next!=NULL)
				{
					/*Compute an interior point for each facet*/				
					dd_MatrixPtr ddineq;	
					ddineq=dd_CreateMatrix(ddrows,ddcols);
					intvec *comp = new intvec(this->numVars);
					comp=fAct->getFacetNormal();				
					for(int ii=0; ii<this->numVars; ii++)					
					{										
						dd_set_si(ddineq->matrix[0][ii+1],(*comp)[ii]);
						dd_set_si(ddineq->matrix[1][ii+1],1);	//Assure we search in the pos. orthant		
					}
					set_addelem(ddineq->linset,1);	//We want equality in the first row
					//dd_WriteMatrix(stdout,ddineq);
					interiorPoint(ddineq,*comp);				
					/**/
#ifdef gfan_DEBUG
					cout << "IP is";
					comp->show(); cout << endl;
#endif
					//Store the interior point and the UCN
 					fListPtr->setInteriorPoint( comp );				
 					fListPtr->setUCN( gc.getUCN() );	
								
					dd_FreeMatrix(ddineq);
					fAct=fAct->next;	//iterate
				}	
			}
			else/*Facets of facets*/
			{
				dd_MatrixPtr ddineq,P,ddakt;
				dd_rowset impl_linset, redset;
				dd_ErrorType err;
				dd_rowindex newpos;			
				
				fAct = fListPtr;

#ifdef gfan_DEBUG
				cout << "before f2M" << endl;
				gc.showFacets();
				ddineq = facets2Matrix(gc);
				cout << "after f2M" << endl;
				gc.showFacets();
#endif
				
				/*Now set appropriate linearity*/
				dd_PolyhedraPtr ddpolyh;
				for (int ii=0; ii<this->numFacets; ii++)
				{	
					cout << "------------" << endl;
 					ddakt = dd_CopyMatrix(ddineq);
					set_addelem(ddakt->linset,ii+1);
										
					dd_MatrixCanonicalize(&ddakt, &impl_linset, &redset, &newpos, &err);			
					
 					dd_WriteMatrix(stdout,ddakt);
					ddpolyh=dd_DDMatrix2Poly(ddakt, &err);
					P=dd_CopyGenerators(ddpolyh);
					dd_WriteMatrix(stdout,P);
					
					/* We loop through each row of P
					* normalize it by making all entries integer ones
					* and add the resulting vector to the int matrix facet::codim2Facets
					*/
					for (int jj=1;jj<=P->rowsize;jj++)
					{
						intvec *n = new intvec(this->numVars);
						normalize(P,jj,*n);
						//fAct->addCodim2Facet(n);						
						n->show();
						delete n;									
					}
					
					//intvec *load = new intvec(this->numVars);
					
					dd_FreeMatrix(ddakt);
					dd_FreePolyhedra(ddpolyh);
				}
				//gc.showFacets();
				
			}			
	
			
			//NOTE Hm, comment in and get a crash for free...
			//dd_free_global_constants();				
			gc.writeConeToFile(gc);
			
			/*2nd step
			Choose a facet from fListPtr, flip it and forget the previous cone
			*/
			fAct = fListPtr;
			//gcone *gcTmp = new gcone(gc);	//copy
			//gcTmp->flip(gcTmp->gcBasis,fAct);
			//NOTE: gcTmp may be deleted, gcRoot from main proc should probably not!
			
		}//void noRevS(gcone &gc)
		
		void normalize(dd_MatrixPtr const &M, int line, intvec &n)
		{			
			mpz_t denom[this->numVars];
			for(int ii=0;ii<this->numVars;ii++)
			{
				mpz_init_set_str(denom[ii],"0",10);
			}
		
			mpz_t kgV,tmp;
			mpz_init(kgV);
			mpz_init(tmp);
 			//intvec *ivres = new intvec(this->numVars);
// 			intvec ivres(this->numVars);
			
			for (int ii=0;ii<(M->colsize)-1;ii++)
			{
				mpz_t z;
				mpz_init(z);
				mpq_get_den(z,M->matrix[line-1][ii+1]);
				//mpz_out_str(stdout,10,z);
				mpz_set( denom[ii], z);
				mpz_clear(z);				
			}
			/*Compute lcm of the denominators*/
			mpz_set(tmp,denom[0]);
			for (int ii=0;ii<(M->colsize)-1;ii++)
			{
				mpz_lcm(kgV,tmp,denom[ii]);				
			}
			/*Multiply the nominators by kgV*/
			mpq_t qkgV,res;
			mpq_init(qkgV);
// 			mpq_canonicalize(qkgV);
			mpq_set_str(qkgV,"1",10);
// 			mpq_canonicalize(qkgV);
			mpq_init(res);
			mpq_set_str(res,"1",10);
// 			mpq_canonicalize(res);
			//mpq_set_z(qkgV,kgV);
			mpq_set_num(qkgV,kgV);
			//mpq_set_den(qkgV,1);
// 			mpq_canonicalize(qkgV);
			for (int ii=0;ii<(M->colsize)-1;ii++)
			{
				mpq_mul(res,qkgV,M->matrix[line-1][ii+1]);
// 	      			(*ivres)[ii]=(int)mpz_get_d(mpq_numref(res));
				n[ii]=(int)mpz_get_d(mpq_numref(res));
			}
			//mpz_clear(denom[this->numVars]);
			mpz_clear(kgV);
			mpq_clear(qkgV); mpq_clear(res);
			
			//return ivres;
		}
		
		/** \brief Construct a dd_MatrixPtr from a cone's list of facets
		* 
		*/
		dd_MatrixPtr facets2Matrix(gcone const &gc)
		{
			facet *fAct = new facet();
			fAct = gc.facetPtr;
	
			dd_MatrixPtr M;
			dd_rowrange ddrows;
			dd_colrange ddcols;
			ddcols=(this->numVars)+1;
			ddrows=this->numFacets;
			dd_NumberType numb = dd_Integer;
			M=dd_CreateMatrix(ddrows,ddcols);			
			
			//dd_set_global_constants();						
						
			intvec *comp = new intvec(this->numVars);			
			
			for (int jj=0; jj<M->rowsize; jj++)
			{
				comp = fAct->getFacetNormal();
				for (int ii=0; ii<this->numVars; ii++)
				{
					dd_set_si(M->matrix[jj][ii+1],(*comp)[ii]);
				}
				fAct = fAct->next;				
			}//jj
			
			//delete fAct;
			//delete comp;			
			return M;
		}
		
		/** \brief Write information about a cone into a file on disk
		*
		* This methods writes the information needed for the "second" method into a file.
		* The file's is divided in sections containing information on
		* 1) the ring
		* 2) the cone's Groebner Basis
		* 3) the cone's facets
		* Each line contains exactly one date
		* Each section starts with its name in CAPITALS
		*/
		void writeConeToFile(gcone const &gc, bool usingIntPoints=FALSE)
		{
			ofstream gcOutputFile("/tmp/cone1.gc");
			facet *fAct = new facet();
			fAct = gc.facetPtr;
			
			if (!gcOutputFile)
			{
				cout << "Error opening file for writing in writeConeToFile" << endl;
			}
			else
			{	/*gcOutputFile << "UCN" << endl;
				gcOutputFile << this->UCN << endl;*/
				gcOutputFile << "RING" << endl;				
				//Write this->gcBasis into file
				gcOutputFile << "GCBASIS" << endl;				
				for (int ii=0;ii<IDELEMS(gc.gcBasis);ii++)
				{					
 					gcOutputFile << p_String((poly)gc.gcBasis->m[ii],gc.baseRing) << endl;
				}				
				
				gcOutputFile << "FACETS" << endl;								
				while(fAct->next!=NULL)
				{	
 					intvec *iv = new intvec(gc.numVars);
					iv=fAct->getFacetNormal();
					for (int ii=0;ii<iv->length();ii++)
					{
						if (ii<iv->length()-1)
						{
							gcOutputFile << (*iv)[ii] << ",";
						}
						else
						{
							gcOutputFile << (*iv)[ii] << endl;
						}
					}
					fAct=fAct->next;
					//delete iv; iv=NULL;
				}				
				
				gcOutputFile.close();
				//delete fAct; fAct=NULL;
			}
			
		}//writeConeToFile(gcone const &gc)
		
		/** \brief Reads a cone from a file identified by its number
		*/
		void readConeFromFile(int gcNum)
		{
		}
		
	friend class facet;
};//class gcone

ideal gfan(ideal inputIdeal)
{
	int numvar = pVariables; 
	
	enum searchMethod {
		reverseSearch,
		noRevS
	};
	
	searchMethod method;
	method = noRevS;
// 	method = reverseSearch;
	
#ifdef gfan_DEBUG
	cout << "Now in subroutine gfan" << endl;
#endif
	ring inputRing=currRing;	// The ring the user entered
	ring rootRing;			// The ring associated to the target ordering
	ideal res;	
	facet *fRoot;
	
	if (method==reverseSearch)
	{
	
	/* Construct a new ring which will serve as our root*/
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
	
	//gcone *gcRoot = new gcone();	//Instantiate the sink
	gcone *gcRoot = new gcone(rootRing,rootIdeal);
	gcone *gcAct;
	gcAct = gcRoot;
	gcAct->numVars=pVariables;
	gcAct->getGB(rootIdeal);	//sets gcone::gcBasis
	idShow(gcAct->gcBasis);
	gcAct->getConeNormals(gcAct->gcBasis);	//hopefully compute the normals	
	//gcAct->flip(gcAct->gcBasis,gcAct->facetPtr);	
	/*Now it is time to compute the search facets, respectively start the reverse search.
	But since we are in the root all facets should be search facets. IS THIS TRUE?
	NOTE: Check for flippability is not very sophisticated
	*/	
	//gcAct->reverseSearch(gcAct);	
	rChangeCurrRing(rootRing);
	res=gcRoot->gcBasis;	
	}//if method==reverSearch
	
	if(method==noRevS)
	{
		gcone *gcRoot = new gcone(currRing,inputIdeal);
		gcone *gcAct;
		gcAct = gcRoot;
		gcAct->numVars=pVariables;
		gcAct->getGB(inputIdeal);
		gcAct->getConeNormals(gcAct->gcBasis);
		cout << "ding" << endl;		
		gcAct->noRevS(*gcAct);		
		res=gcAct->gcBasis;	
	}
	
	/*As of now extra.cc expects gfan to return type ideal. Probably this will change in near future.
	The return type will then be of type LIST_CMD
	Assume gfan has finished, thus we have enumerated all the cones
	Create an array of size of #cones. Let each entry in the array contain a pointer to the respective
	Groebner Basis and merge this somehow with LIST_CMD
	=> Count the cones!
	*/
	//rChangeCurrRing(rootRing);
	//res=gcAct->gcBasis;
	//res=gcRoot->gcBasis;	
	return res;
	//return GBlist;
}
/*
Since gfan.cc is #included from extra.cc there must not be a int main(){} here
*/
#endif
