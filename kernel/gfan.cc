/*
Compute the Groebner fan of an ideal
$Author: monerjan $
$Date: 2009-09-23 09:36:06 $
$Header: /exports/cvsroot-2/cvsroot/kernel/gfan.cc,v 1.87 2009-09-23 09:36:06 monerjan Exp $
$Id: gfan.cc,v 1.87 2009-09-23 09:36:06 monerjan Exp $
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
#include <string>
#include <sstream>
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

#ifdef p800
#include "../../cddlib/include/setoper.h"
#include "../../cddlib/include/cdd.h"
#include "../../cddlib/include/cddmp.h"
#endif

#ifndef gfan_DEBUG
#define gfan_DEBUG
#ifndef gfan_DEBUGLEVEL
#define gfan_DEBUGLEVEL 1
#endif
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
		* The number of the cone the facet belongs to, Set in getConeNormals()
		*/
		int UCN;
		
		/** \brief The codim of the facet
		*/
		int codim;
		
		/** \brief The Groebner basis on the other side of a shared facet
		 *
		 * In order not to have to compute the flipped GB twice we store the basis we already get
		 * when identifying search facets. Thus in the next step of the reverse search we can 
		 * just copy the old cone and update the facet and the gcBasis.
		 * facet::flibGB is set via facet::setFlipGB() and printed via facet::printFlipGB
		 */
		ideal flipGB;		//The Groebner Basis on the other side, computed via gcone::flip		
			
	public:	
		/** \brief Boolean value to indicate whether a facet is flippable or not
		* This is also used to mark facets that nominally are flippable but which do
		* not intersect with the positive orthant. This check is done in gcone::getCodim2Normals
		*/	
		bool isFlippable;	//**flippable facet? */
		bool isIncoming;	//Is the facet incoming or outgoing in the reverse search?
		facet *next;		//Pointer to next facet
		facet *prev;		//Pointer to predecessor. Needed for the SearchList in noRevS
		facet *codim2Ptr;	//Pointer to (codim-2)-facet. Bit of recursion here ;-)
		int numCodim2Facets;	//#of (codim-2)-facets of this facet. Set in getCodim2Normals()
		ring flipRing;		//the ring on the other side of the facet
						
		/** The default constructor. Do I need a constructor of type facet(intvec)? */
		facet()			
		{
			// Pointer to next facet.  */
			/* Defaults to NULL. This way there is no need to check explicitly */
			this->fNormal=NULL;
			this->interiorPoint=NULL;		
			this->UCN=0;
			this->codim2Ptr=NULL;
			this->codim=1;		//default for (codim-1)-facets
			this->numCodim2Facets=0;
			this->flipGB=NULL;
			this->isIncoming=FALSE;
			this->next=NULL;
			this->prev=NULL;		
			this->flipRing=NULL;
			this->isFlippable=FALSE;
		}
		
		/** \brief Constructor for facets of codim >= 2
		*/
		facet(int const &n)
		{
			this->fNormal=NULL;
			this->interiorPoint=NULL;			
			this->UCN=0;
			this->codim2Ptr=NULL;
			if(n>1)
			{
				this->codim=n;
			}//NOTE Handle exception here!			
			this->numCodim2Facets=0;
			this->flipGB=NULL;
			this->isIncoming=FALSE;
			this->next=NULL;
			this->prev=NULL;
			this->flipRing=NULL;
			this->isFlippable=FALSE;
		}
		
		/** \brief The copy constructor
		*/
		facet(const facet& f)
		{
		}
		
		/** The default destructor */
		~facet();
		
		
		/** \brief Comparison of facets*/
		bool areEqual(facet &f, facet &g)
		{
			bool res = TRUE;
			intvec *fNormal = new intvec(pVariables);
			intvec *gNormal = new intvec(pVariables);
			fNormal = f.getFacetNormal();
			gNormal = g.getFacetNormal();
			if((fNormal == gNormal))//||(gcone::isParallel(fNormal,gNormal)))
			{
				if(f.numCodim2Facets==g.numCodim2Facets)
				{
					facet *f2Act;
					facet *g2Act;
					f2Act = f.codim2Ptr;
					g2Act = g.codim2Ptr;
					intvec *f2N = new intvec(pVariables);
					intvec *g2N = new intvec(pVariables);
					while(f2Act->next!=NULL && g2Act->next!=NULL)
					{
						for(int ii=0;ii<f2N->length();ii++)
						{
							if(f2Act->getFacetNormal() != g2Act->getFacetNormal())
							{
								res=FALSE;								
							}
							if (res==FALSE)
								break;
						}
						if(res==FALSE)
							break;
						
						f2Act = f2Act->next;
						g2Act = g2Act->next;
					}//while
				}//if		
			}//if
			else
			{
				res = FALSE;
			}
			return res;
		}
		
		/** Stores the facet normal \param intvec*/
		void setFacetNormal(intvec *iv)
		{
			this->fNormal = ivCopy(iv);			
		}
		
		/** Hopefully returns the facet normal */
		intvec *getFacetNormal()
		{				
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
			this->flipGB=idCopy(I);
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
		
		/** Set the UCN */
		void setUCN(int n)
		{
			this->UCN=n;
		}
		
		/** \brief Get the UCN 
		* Returns the UCN iff this != NULL, else -1
		*/
		int getUCN()
		{
			if(this!=NULL)
				return this->UCN;
			else
				return -1;
		}
		
		/** Store an interior point of the facet */
		void setInteriorPoint(intvec *iv)
		{
			this->interiorPoint = ivCopy(iv);
		}
		
		intvec *getInteriorPoint()
		{
			return this->interiorPoint;
		}	
		
		/** \brief Debugging function
		* prints the facet normal an all (codim-2)-facets that belong to it
		*/
		void fDebugPrint()
		{			
			facet *codim2Act;			
			codim2Act = this->codim2Ptr;
			intvec *fNormal;
			fNormal = this->getFacetNormal();
			intvec *f2Normal;
			cout << "=======================" << endl;
			cout << "Facet normal = (";
			fNormal->show(1,1);
			cout << ")"<<endl;
			/*for(int ii=0;ii<pVariables;ii++)
			{
				cout << (*fNormal)[ii] << ",";
			}
			if(this->isFlippable==TRUE)
				cout << ")" << endl;
			else
				cout << ")*" << endl;*/	//This case should never happen in SLA!
			cout << "-----------------------" << endl;
			cout << "Codim2 facets:" << endl;
			while(codim2Act!=NULL)
			{
				f2Normal = codim2Act->getFacetNormal();
				cout << "(";
				f2Normal->show(1,0);
// 				for(int jj=0;jj<pVariables;jj++)
// 				{
// 					cout << (*f2Normal)[jj] << ",";
// 				}
				cout << ")" << endl;
				codim2Act = codim2Act->next;
			}
			cout << "=======================" << endl;
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

facet::~facet()
{
	idDelete((ideal *)this->flipGB);
	rDelete(this->flipRing);
	//this=NULL;
}

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
		static int counter;
		
	public:	
		/** \brief Pointer to the first facet */
		facet *facetPtr;	//Will hold the adress of the first facet; set by gcone::getConeNormals
		
		/** # of variables in the ring */
		int numVars;		//#of variables in the ring
		
		/** # of facets of the cone
		* This value is set by gcone::getConeNormals
		*/
		int numFacets;		//#of facets of the cone
		
		/**
		* At least as a workaround we store the irredundant facets of a matrix here.
		* Otherwise, since we throw away non-flippable facets, facets2Matrix will not 
		* yield all the necessary information
		*/
		dd_MatrixPtr ddFacets;	//Matrix to store irredundant facets of the cone
		
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
			this->facetPtr=NULL;	//maybe this->facetPtr = new facet();			
			this->baseRing=currRing;
			this->counter++;
			this->UCN=this->counter;			
			this->numFacets=0;
			this->ivIntPt=NULL;
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
			this->counter++;
			this->UCN=this->counter;			
			this->numFacets=0;
			this->ivIntPt=NULL;
		}
		
		/** \brief Copy constructor 
		*
		* Copies a cone, sets this->gcBasis to the flipped GB
		* Call this only after a successful call to gcone::flip which sets facet::flipGB
		*/		
		gcone(const gcone& gc, const facet &f)
		{
			this->next=NULL;
			this->numVars=gc.numVars;						
			this->counter++;
			this->UCN=this->counter;			
			this->facetPtr=NULL;
 			this->gcBasis=idCopy(f.flipGB);
			this->inputIdeal=idCopy(this->gcBasis);
			this->baseRing=rCopy(f.flipRing);
			this->numFacets=0;
			this->ivIntPt=NULL;
			//rComplete(this->baseRing);
			//rChangeCurrRing(this->baseRing);
		}
		
		/** \brief Default destructor 
		*/
		~gcone()
		{
			//NOTE SAVE THE FACET STRUCTURE!!!
		}			
		
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
		
		/** \brief Print the interior point */
		void showIntPoint()
		{
			ivIntPt->show();
		}
		
		/** \brief Print facets
		* This is mainly for debugging purposes. Usually called from within gdb
		*/
		void showFacets(short codim=1)
		{
			facet *f;
			switch(codim)
			{
				case 1:
					f = this->facetPtr;
					break;
				case 2:
					f = this->facetPtr->codim2Ptr;
					break;
			}			
			
			intvec *iv;			
			while(f!=NULL)
			{
				iv = f->getFacetNormal();
				cout << "(";
				iv->show(1,0);				
				if(f->isFlippable==FALSE)
					cout << ")* ";
				else
					cout << ") ";
				f=f->next;
			}
			cout << endl;
		}
		
		/** For debugging purposes only */
		void showSLA(facet &f)
		{
			facet *fAct;
			fAct = &f;
			facet *codim2Act;
			codim2Act = fAct->codim2Ptr;
			intvec *fNormal;// = new intvec(this->numVars);
			intvec *f2Normal;
			cout << endl;
			while(fAct!=NULL)
			{
				fNormal=fAct->getFacetNormal();
				cout << "(";
				fNormal->show(1,0);
				if(fAct->isFlippable==TRUE)
					cout << ") ";
				else
					cout << ")* ";
				codim2Act = fAct->codim2Ptr;
				cout << " Codim2: ";
				while(codim2Act!=NULL)
				{
					f2Normal = codim2Act->getFacetNormal();
					cout << "(";
					f2Normal->show(1,0);
					cout << ") ";
					codim2Act = codim2Act->next;
				}
				cout << "UCN = " << fAct->getUCN() << endl;				
				fAct = fAct->next;
			}
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
			if(this!=NULL)
				return this->UCN;
			else
				return -1;
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
// 			cout << "The Groebner basis has " << lengthGB << " elements" << endl;
// 			cout << "The current ring has " << numvar << " variables" << endl;
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
// 			cout << "rows=" << rows << endl;
// 			cout << "Will create a " << rows << " x " << cols << " matrix to store inequalities" << endl;
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
// 				cout << "Poly No. " << i << " has " << pCompCount << " components" << endl;
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
//   			cout << "The inequality matrix is" << endl;
//   			dd_WriteMatrix(stdout, ddineq);
#endif

			// The inequalities are now stored in ddineq
			// Next we check for superflous rows
			ddredrows = dd_RedundantRows(ddineq, &dderr);
			if (dderr!=dd_NoError)			// did an error occur?
			{
				dd_WriteErrorMessages(stderr,dderr);	//if so tell us
			} 
#ifdef gfan_DEBUG
			else
			{
// 				cout << "Redundant rows: ";
// 				set_fwrite(stdout, ddredrows);		//otherwise print the redundant rows
			}//if dd_Error
#endif
			//Remove reduntant rows here!
			dd_MatrixCanonicalize(&ddineq, &ddlinset, &ddredrows, &ddnewpos, &dderr);
			ddrows = ddineq->rowsize;	//Size of the matrix with redundancies removed
			ddcols = ddineq->colsize;
			
			//ddCreateMatrix(ddrows,ddcols+1);
			ddFacets = dd_CopyMatrix(ddineq);
#ifdef gfan_DEBUG
//   			cout << "Having removed redundancies, the normals now read:" << endl;
//   			dd_WriteMatrix(stdout,ddineq);
//  			cout << "Rows = " << ddrows << endl;
//  			cout << "Cols = " << ddcols << endl;
#endif
			
			/*Write the normals into class facet*/
#ifdef gfan_DEBUG
// 			cout << "Creating list of normals" << endl;
#endif
			/*The pointer *fRoot should be the return value of this function*/
			//facet *fRoot = new facet();	//instantiate new facet
			//fRoot->setUCN(this->getUCN());
			//this->facetPtr = fRoot;		//set variable facetPtr of class gcone to first facet
			facet *fAct; 			//pointer to active facet
			//fAct = fRoot;			//Seems to do the trick. fRoot and fAct have to point to the same adress!
			//std::cout << "fRoot = " << fRoot << ", fAct = " << fAct << endl;
			int numNonFlip=0;
			for (int kk = 0; kk<ddrows; kk++)
			{
				intvec *load = new intvec(this->numVars);	//intvec to store a single facet normal that will then be stored via setFacetNormal
				for (int jj = 1; jj <ddcols; jj++)
				{
					double foo;
					foo = mpq_get_d(ddineq->matrix[kk][jj]);
#ifdef gfan_DEBUG
// 					std::cout << "fAct is " << foo << " at " << fAct << std::endl;
#endif
					(*load)[jj-1] = (int)foo;		//store typecasted entry at pos jj-1 of load		
				}//for (int jj = 1; jj <ddcols; jj++)
				
				/*Quick'n'dirty hack for flippability*/	
				bool isFlip=FALSE;
								
				for (int jj = 0; jj<load->length(); jj++)
				{
					intvec *ivCanonical = new intvec(load->length());
					(*ivCanonical)[jj]=1;
// 					cout << "dotProd=" << dotProduct(*load,*ivCanonical) << endl;
					if (dotProduct(*load,*ivCanonical)<0)	
					//if (ivMult(load,ivCanonical)<0)
					{
						isFlip=TRUE;
						break;	//URGHS
					}
				}	
				if (isFlip==FALSE)
				{
					this->numFacets++;
					numNonFlip++;
					if(this->numFacets==1)
					{
						facet *fRoot = new facet();
						this->facetPtr = fRoot;
						fAct = fRoot;
						
					}
					else
					{
						fAct->next = new facet();
						fAct = fAct->next;
					}
					fAct->isFlippable=FALSE;
					fAct->setFacetNormal(load);
					fAct->setUCN(this->getUCN());
#ifdef gfan_DEBUG
					cout << "Marking facet (";
					load->show(1,0);
					cout << ") as non flippable" << endl;				
#endif
				}
				else
				{	/*Now load should be full and we can call setFacetNormal*/					
					this->numFacets++;
					if(this->numFacets==1)
					{
						facet *fRoot = new facet();
						this->facetPtr = fRoot;
						fAct = fRoot;
					}
					else
					{
						fAct->next = new facet();
						fAct = fAct->next;
					}
					fAct->isFlippable=TRUE;
					fAct->setFacetNormal(load);
					fAct->setUCN(this->getUCN());					
				}//if (isFlippable==FALSE)
				//delete load;				
			}//for (int kk = 0; kk<ddrows; kk++)
			
			//In cases like I=<x-1,y-1> there are only non-flippable facets...
			if(numNonFlip==this->numFacets)
			{					
				cerr << "Only non-flippable facets. Terminating..." << endl;
			}
			
			/*
			Now we should have a linked list containing the facet normals of those facets that are
			-irredundant
			-flipable
			Adressing is done via *facetPtr
			*/
			
			if (compIntPoint==TRUE)
			{
				intvec *iv = new intvec(this->numVars);
				dd_MatrixPtr posRestr=dd_CreateMatrix(this->numVars,this->numVars+1);
				int jj=1;
				for (int ii=0;ii<=this->numVars;ii++)
				{
					dd_set_si(posRestr->matrix[ii][jj],1);
					jj++;							
				}
				dd_MatrixAppendTo(&ddineq,posRestr);
				interiorPoint(ddineq, *iv);	//NOTE ddineq contains non-flippable facets
				this->setIntPoint(iv);	//stores the interior point in gcone::ivIntPt
				//delete iv;
			}
			
			//Compute the number of facets
			// wrong because ddineq->rowsize contains only irredundant facets. There can still be
			// non-flippable ones!
			//this->numFacets=ddineq->rowsize;
			
			//Clean up but don't delete the return value! (Whatever it will turn out to be)			
			//dd_FreeMatrix(ddineq);
			//set_free(ddredrows);
			//free(ddnewpos);
			//set_free(ddlinset);
			//NOTE Commented out. Solved the bug that after facet2Matrix there were facets lost
			//THIS SUCKS
			//dd_free_global_constants();

		}//method getConeNormals(ideal I)	
		
		/** \brief Compute the (codim-2)-facets of a given cone
		* This method is used during noRevS
		* Additionally we check whether the codim2-facet normal is strictly positive. Otherwise
		* the facet is marked as non-flippable.
		*/
		void getCodim2Normals(gcone const &gc)
		{
			//this->facetPtr->codim2Ptr = new facet(2);	//instantiate a (codim-2)-facet
			facet *fAct;
			fAct = this->facetPtr;		
			facet *codim2Act;
			//codim2Act = this->facetPtr->codim2Ptr;
			
			dd_set_global_constants();
			
			dd_MatrixPtr ddineq,P,ddakt;
			dd_rowset impl_linset, redset;
			dd_ErrorType err;
			dd_rowindex newpos;		

			//ddineq = facets2Matrix(gc);	//get a matrix representation of the cone
			ddineq = dd_CopyMatrix(gc.ddFacets);
				
			/*Now set appropriate linearity*/
			dd_PolyhedraPtr ddpolyh;
			for (int ii=0; ii<this->numFacets; ii++)			
			{				
				ddakt = dd_CopyMatrix(ddineq);
				ddakt->representation=dd_Inequality;
				set_addelem(ddakt->linset,ii+1);				
				dd_MatrixCanonicalize(&ddakt, &impl_linset, &redset, &newpos, &err);			
					
#ifdef gfan_DEBUG
//   				cout << "Codim2 matrix"<<endl;
//     				dd_WriteMatrix(stdout,ddakt);
#endif
				ddpolyh=dd_DDMatrix2Poly(ddakt, &err);
				P=dd_CopyGenerators(ddpolyh);				
#ifdef gfan_DEBUG
//   				cout << "Codim2 facet:" << endl;
//      				dd_WriteMatrix(stdout,P);
//   				cout << endl;
#endif
					
				/* We loop through each row of P
				* normalize it by making all entries integer ones
				* and add the resulting vector to the int matrix facet::codim2Facets
				*/
				for (int jj=1;jj<=P->rowsize;jj++)
				{					
					fAct->numCodim2Facets++;
					if(fAct->numCodim2Facets==1)					
					{						
						fAct->codim2Ptr = new facet(2);						
						codim2Act = fAct->codim2Ptr;
					}
					else
					{
						codim2Act->next = new facet(2);
						codim2Act = codim2Act->next;
					}
					intvec *n = new intvec(this->numVars);
					makeInt(P,jj,*n);
					codim2Act->setFacetNormal(n);
					delete n;					
				}		
				/*We check whether the facet spanned by the codim-2 facets
				* intersects with the positive orthant. Otherwise we define this
				* facet to be non-flippable
				*/
				intvec *iv_intPoint = new intvec(this->numVars);
				dd_MatrixPtr shiftMatrix;
				dd_MatrixPtr intPointMatrix;
				shiftMatrix = dd_CreateMatrix(this->numVars,this->numVars+1);
				for(int kk=0;kk<this->numVars;kk++)
				{
					dd_set_si(shiftMatrix->matrix[kk][0],1);
					dd_set_si(shiftMatrix->matrix[kk][kk+1],1);
				}
				intPointMatrix=dd_MatrixAppend(ddakt,shiftMatrix);
				//dd_WriteMatrix(stdout,intPointMatrix);
				interiorPoint(intPointMatrix,*iv_intPoint);
				for(int ll=0;ll<this->numVars;ll++)
				{
					if( (*iv_intPoint)[ll] < 0 )
					{
						fAct->isFlippable=FALSE;
						break;
					}
				}
				/*End of check*/					
				fAct = fAct->next;	
				dd_FreeMatrix(ddakt);
				dd_FreePolyhedra(ddpolyh);
			}//while
		}
		
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
			//std::cout << "===" << std::endl;
			std::cout << "running gcone::flip" << std::endl;
			std::cout << "flipping UCN " << this->getUCN() << endl;
			for(int ii=0;ii<IDELEMS(gb);ii++)
			{
				pWrite((poly)gb->m[ii]);
			}
			cout << "over facet (";
 			fNormal->show(1,0);
			cout << ") with UCN " << f->getUCN();
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
//  				cout << "Initial Form=";				
//  				pWrite(initialFormElement[ii]);
//  				cout << "---" << endl;
#endif
				/*Now initialFormElement must be added to (ideal)initialForm */
				initialForm->m[ii]=initialFormElement[ii];
			}//for			
#ifdef gfan_DEBUG
/*			cout << "Initial ideal is: " << endl;
			idShow(initialForm);
			//f->printFlipGB();*/
// 			cout << "===" << endl;
#endif
			//delete check;
			
			/*2nd step: lift initial ideal to a GB of the neighbouring cone using minus alpha as weight*/
			/*Substep 2.1
			compute $G_{-\alpha}(in_v(I)) 
			see journal p. 66
			NOTE Check for different rings. Prolly it will not always be necessary to add a weight, if the
			srcRing already has a weighted ordering
			*/
			ring srcRing=currRing;
			ring tmpRing;
			
			if( (srcRing->order[0]!=ringorder_a))
			{
				tmpRing=rCopyAndAddWeight(srcRing,ivNeg(fNormal));
			}
			else
			{
				tmpRing=rCopy0(srcRing);
				int length=fNormal->length();
				int *A=(int *)omAlloc0(length*sizeof(int));
				for(int jj=0;jj<length;jj++)
				{
					A[jj]=-(*fNormal)[jj];
				}
                                omFree(tmpRing->wvhdl[0]);
				tmpRing->wvhdl[0]=(int*)A;
                                tmpRing->block1[0]=length;
				rComplete(tmpRing);	
			}
			rChangeCurrRing(tmpRing);
			
			//rWrite(currRing); cout << endl;
			
			ideal ina;			
			ina=idrCopyR(initialForm,srcRing);			
#ifdef gfan_DEBUG
//   			cout << "ina=";
//   			idShow(ina); cout << endl;
#endif
			ideal H;
			//H=kStd(ina,NULL,isHomog,NULL);	//we know it is homogeneous
			H=kStd(ina,NULL,testHomog,NULL);
			idSkipZeroes(H);
#ifdef gfan_DEBUG
//   			cout << "H="; idShow(H); cout << endl;
#endif
			/*Substep 2.2
			do the lifting and mark according to H
			*/
			rChangeCurrRing(srcRing);
			ideal srcRing_H;
			ideal srcRing_HH;			
			srcRing_H=idrCopyR(H,tmpRing);
#ifdef gfan_DEBUG
//   			cout << "srcRing_H = ";
//   			idShow(srcRing_H); cout << endl;
#endif
			srcRing_HH=ffG(srcRing_H,this->gcBasis);		
#ifdef gfan_DEBUG
//   			cout << "srcRing_HH = ";
//   			idShow(srcRing_HH); cout << endl;
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
			intPointMatrix = dd_CreateMatrix(iPMatrixRows+10,this->numVars+1); //iPMatrixRows+10;
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
			//Let's make sure we compute interior points from the positive orthant
			dd_MatrixPtr posRestr=dd_CreateMatrix(this->numVars,this->numVars+1);
			int jj=1;
			for (int ii=0;ii<this->numVars;ii++)
			{
				dd_set_si(posRestr->matrix[ii][jj],1);
				jj++;							
			}
			dd_MatrixAppendTo(&intPointMatrix,posRestr);
#ifdef gfan_DEBUG
// 			dd_WriteMatrix(stdout,intPointMatrix);
#endif
			intvec *iv_weight = new intvec(this->numVars);
			interiorPoint(intPointMatrix, *iv_weight);	//iv_weight now contains the interior point
			dd_FreeMatrix(intPointMatrix);
			dd_free_global_constants();
			
			/*Step 3
			turn the minimal basis into a reduced one
			*/
			//ring dstRing=rCopyAndAddWeight(tmpRing,iv_weight);	
			
			// NOTE May assume that at this point srcRing already has 3 blocks of orderins, starting with a
			// Thus: 
			//ring dstRing=rCopyAndChangeWeight(srcRing,iv_weight);
			//cout << "PLING" << endl;
			/*ring dstRing=rCopy0(srcRing);
			for (int ii=0;ii<this->numVars;ii++)
			{				
				dstRing->wvhdl[0][ii]=(*iv_weight)[ii];				
			}*/
			ring dstRing=rCopy0(tmpRing);
			int length=iv_weight->length();
			int *A=(int *)omAlloc0(length*sizeof(int));
			for(int jj=0;jj<length;jj++)
			{
				A[jj]=(*iv_weight)[jj];
			}
			dstRing->wvhdl[0]=(int*)A;
			rComplete(dstRing);					
			rChangeCurrRing(dstRing);
			
//#ifdef gfan_DEBUG
			rWrite(dstRing); cout << endl;
//#endif
			ideal dstRing_I;			
			dstRing_I=idrCopyR(srcRing_HH,srcRing);
			//dstRing_I=idrCopyR(inputIdeal,srcRing);
			//validOpts<1>=TRUE;
#ifdef gfan_DEBUG
			//idShow(dstRing_I);
#endif			
			BITSET save=test;
			test|=Sy_bit(OPT_REDSB);
			test|=Sy_bit(OPT_REDTAIL);
#ifdef gfan_DEBUG
			test|=Sy_bit(6);	//OPT_DEBUG
#endif
			ideal tmpI;
			//NOTE Any of the two variants of tmpI={idrCopy(),dstRing_I} does the trick
			//tmpI = idrCopyR(this->inputIdeal,this->baseRing);
			tmpI = dstRing_I;
			dstRing_I=kStd(tmpI,NULL,testHomog,NULL);			
			//kInterRed(dstRing_I);
			idSkipZeroes(dstRing_I);
			test=save;
			/*End of step 3 - reduction*/
			
			f->setFlipGB(dstRing_I);//store the flipped GB
			f->flipRing=rCopy(dstRing);	//store the ring on the other side
//#ifdef gfan_DEBUG
			cout << "Flipped GB is UCN " << counter+1 << ":" << endl;
			f->printFlipGB();
			cout << endl;
//#endif			
			rChangeCurrRing(srcRing);	//return to the ring we started the computation of flipGB in
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
// 			cout << "Entering restOfDiv" << endl;
			poly p=f;
			//pWrite(p);			
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
// 			cout << "Entering ffG" << endl;
			int size=IDELEMS(H);
			ideal res=idInit(size,1);
			poly temp1, temp2, temp3;	//polys to temporarily store values for pSub
			for (int ii=0;ii<size;ii++)
			{
				res->m[ii]=restOfDiv(H->m[ii],G);
				//res->m[ii]=kNF(G, NULL,H->m[ii]);
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
			BITSET save=test;
			test|=Sy_bit(OPT_REDSB);
			test|=Sy_bit(OPT_REDTAIL);
			ideal gb;
			gb=kStd(inputIdeal,NULL,testHomog,NULL);
			idSkipZeroes(gb);
			this->gcBasis=gb;	//write the GB into gcBasis
			test=save;
		}//void getGB
		
		/** \brief The Generic Groebner Walk due to FJLT
		* Needed for computing the search facet
		*/
		ideal GenGrbWlk(ideal, ideal)
		{
		}//GGW
		
		/** \brief Compute the negative of a given intvec
		*/		
		intvec *ivNeg(const intvec *iv)
		{
			intvec *res = new intvec(iv->length());
			res=ivCopy(iv);
			*res *= (int)-1;
			//for(int ii=0;ii<this->numVars;ii++)
			//{			
				//(res)[ii] = (*res[ii])*(int)(-1);
			//}
			//res->show(1,0);			
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
		* Result will be written into intvec iv. 
		* Any rational point is automatically converted into an integer.
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
									
			/*NOTE: Leave the following line commented out!
			* Otherwise it will cause interior points that are not strictly positive on some examples
			* 
			*/
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
									
			//lpSol=dd_CopyLPSolution(lpInt);
			if (err!=dd_NoError){cout << "Error during dd_CopyLPSolution" << endl;}			
#ifdef gfan_DEBUG
			cout << "Interior point: ";
			for (int ii=1; ii<(lpSol->d)-1;ii++)
			{
				dd_WriteNumber(stdout,lpSol->sol[ii]);
			}
			cout << endl;
#endif
			
			//NOTE The following strongly resembles parts of makeInt. 
			//Maybe merge sometimes
			mpz_t kgV; mpz_init(kgV);
			mpz_set_str(kgV,"1",10);
			mpz_t den; mpz_init(den);
			mpz_t tmp; mpz_init(tmp);
			mpq_get_den(tmp,lpSol->sol[1]);
			for (int ii=1;ii<(lpSol->d)-1;ii++)
			{
				mpq_get_den(den,lpSol->sol[ii+1]);
				mpz_lcm(kgV,tmp,den);
				mpz_set(tmp, kgV);
			}
			mpq_t qkgV;
			mpq_init(qkgV);
			mpq_set_z(qkgV,kgV);
			for (int ii=1;ii<(lpSol->d)-1;ii++)
			{
				mpq_t product;
				mpq_init(product);
				mpq_mul(product,qkgV,lpSol->sol[ii]);
				iv[ii-1]=(int)mpz_get_d(mpq_numref(product));
				mpq_clear(product);
			}
#ifdef gfan_DEBUG
// 			iv.show();
// 			cout << endl;
#endif
			mpq_clear(qkgV);
			mpz_clear(tmp);
			mpz_clear(den);
			mpz_clear(kgV);			
			
			dd_FreeLPSolution(lpSol);
			dd_FreeLPData(lpInt);
			dd_FreeLPData(lp);
			set_free(ddlinset);
			set_free(ddredrows);			
			
		}//void interiorPoint(dd_MatrixPtr const &M)
		
		/** \brief Copy a ring and add a weighted ordering in first place
		* 
		*/
		ring rCopyAndAddWeight(ring const &r, intvec const *ivw)				
		{
			ring res=rCopy0(r);
			int jj;
			
                        omFree(res->order);
			res->order =(int *)omAlloc0(4*sizeof(int));
                        omFree(res->block0);
			res->block0=(int *)omAlloc0(4*sizeof(int));
                        omFree(res->block1);
			res->block1=(int *)omAlloc0(4*sizeof(int));
                        omfree(res->wvhdl);
			res->wvhdl =(int **)omAlloc0(4*sizeof(int**));
			
			res->order[0]=ringorder_a;
                        res->block0[0]=1;
                        res->block1[0]=res->N;;
			res->order[1]=ringorder_dp;	//basically useless, since that should never be used			
                        res->block0[1]=1;
                        res->block1[1]=res->N;;
			res->order[2]=ringorder_C;

			int length=ivw->length();
			int *A=(int *)omAlloc0(length*sizeof(int));
			for (jj=0;jj<length;jj++)
			{				
				A[jj]=(*ivw)[jj];				
			}			
			res->wvhdl[0]=(int *)A;
			res->block1[0]=length;
			
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
			
			while(fAct!=NULL) 
			{
				cout << "======================"<< endl;
				gcAct->flip(gcAct->gcBasis,gcAct->facetPtr);
				gcone *gcTmp = new gcone(*gcAct);
				//idShow(gcTmp->gcBasis);
				gcTmp->getConeNormals(gcTmp->gcBasis, TRUE);
#ifdef gfan_DEBUG
				facet *f = new facet();
				f=gcTmp->facetPtr;
				while(f!=NULL)
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
		void noRevS(gcone &gcRoot, bool usingIntPoint=FALSE)
		{	
			facet *SearchListRoot = new facet(); //The list containing ALL facets we come across
			facet *SearchListAct;
			SearchListAct = NULL;
			//SearchListAct = SearchListRoot;
			
			gcone *gcAct;
			gcAct = &gcRoot;
			gcone *gcPtr;	//Pointer to end of linked list of cones
			gcPtr = &gcRoot;
			gcone *gcNext;	//Pointer to next cone to be visited
			gcNext = &gcRoot;
			gcone *gcHead;
			gcHead = &gcRoot;
			
			facet *fAct;
			fAct = gcAct->facetPtr;			
			
			ring rAct;
			rAct = currRing;
						
			int UCNcounter=gcAct->getUCN();
#ifdef gfan_DEBUG
			cout << "NoRevs" << endl;
			cout << "Facets are:" << endl;
 			gcAct->showFacets();
#endif			
			
			gcAct->getCodim2Normals(*gcAct);
				
			//Compute unique representation of codim-2-facets
			gcAct->normalize();
			
			/*Make a copy of the facet list of first cone
			Since the operations getCodim2Normals and normalize affect the facets
			we must not memcpy them before these ops!
			*/
			intvec *fNormal;// = new intvec(this->numVars);
			intvec *f2Normal;// = new intvec(this->numVars);
			facet *codim2Act; codim2Act = NULL;			
			facet *sl2Root; //sl2Root = new facet(2);
			facet *sl2Act;	//sl2Act = sl2Root;
			
			for(int ii=0;ii<this->numFacets;ii++)
			{
				//only copy flippable facets!
				if(fAct->isFlippable==TRUE)
				{
					fNormal = fAct->getFacetNormal();
					if( ii==0 || (ii>0 && SearchListAct==NULL) ) //1st facet may be non-flippable
					{						
						SearchListAct = SearchListRoot;
						//memcpy(SearchListAct,fAct,sizeof(facet));					
					}
					else
					{			
						SearchListAct->next = new facet();
						SearchListAct = SearchListAct->next;						
						//memcpy(SearchListAct,fAct,sizeof(facet));				
					}
					SearchListAct->setFacetNormal(fNormal);
					SearchListAct->setUCN(this->getUCN());
					SearchListAct->numCodim2Facets=fAct->numCodim2Facets;
					SearchListAct->isFlippable=TRUE;
					//Copy codim2-facets				
					codim2Act=fAct->codim2Ptr;
					SearchListAct->codim2Ptr = new facet(2);
					sl2Root = SearchListAct->codim2Ptr;
					sl2Act = sl2Root;
					//while(codim2Act!=NULL)
					for(int jj=0;jj<fAct->numCodim2Facets;jj++)
					{
						f2Normal = codim2Act->getFacetNormal();
						if(jj==0)
						{						
							sl2Act = sl2Root;
							sl2Act->setFacetNormal(f2Normal);
						}
						else
						{
							sl2Act->next = new facet(2);
							sl2Act = sl2Act->next;
							sl2Act->setFacetNormal(f2Normal);
						}					
						codim2Act = codim2Act->next;
					}
					fAct = fAct->next;
				}//if(fAct->isFlippable==TRUE)
				else {fAct = fAct->next;}
			}//End of copying facets into SLA				
			
			SearchListAct = SearchListRoot;	//Set to beginning of list
			/*Make SearchList doubly linked*/
			while(SearchListAct!=NULL)
			{
				if(SearchListAct->next!=NULL)
				{
					SearchListAct->next->prev = SearchListAct;					
				}
				SearchListAct = SearchListAct->next;
			}
			SearchListAct = SearchListRoot;	//Set to beginning of List
			
			fAct = gcAct->facetPtr;
			//NOTE Disabled until code works as expected
			//gcAct->writeConeToFile(*gcAct);
			
			/*End of initialisation*/
			fAct = SearchListAct;
			/*2nd step
			Choose a facet from fListPtr, flip it and forget the previous cone
			We always choose the first facet from fListPtr as facet to be flipped
			*/			
			while((SearchListAct!=NULL) )
			{//NOTE See to it that the cone is only changed after ALL facets have been flipped!				
				fAct = SearchListAct;
				//while( ( (fAct->next!=NULL) && (fAct->getUCN()==fAct->next->getUCN() ) ) )
				//do
				while(fAct!=NULL)
				{	//Since SLA should only contain flippables there should be no need to check for that
					gcAct->flip(gcAct->gcBasis,fAct);
					ring rTmp=rCopy(fAct->flipRing);
					rComplete(rTmp);
 					rChangeCurrRing(rTmp);
 					gcone *gcTmp = new gcone::gcone(*gcAct,*fAct);
 					gcTmp->getConeNormals(gcTmp->gcBasis, FALSE);					
 					gcTmp->getCodim2Normals(*gcTmp);					
					gcTmp->normalize();
#ifdef gfan_DEBUG
					gcTmp->showFacets(1);
#endif
					//gcTmp->writeConeToFile(*gcTmp);
					/*add facets to SLA here*/
					SearchListRoot=gcTmp->enqueueNewFacets(*SearchListRoot);
#ifdef gfan_DEBUG
					if(SearchListRoot!=NULL)
						gcTmp->showSLA(*SearchListRoot);
#endif
 					rChangeCurrRing(gcAct->baseRing);
 					gcPtr->next=gcTmp;
 					gcPtr=gcPtr->next;
					if(fAct->getUCN() == fAct->next->getUCN())
					{
						fAct=fAct->next;
					}
					else
						break;
				}//while( ( (fAct->next!=NULL) && (fAct->getUCN()==fAct->next->getUCN() ) ) );
				//Search for cone with smallest UCN
				gcNext = gcHead;
				while(gcNext!=NULL)
				{
					//if( gcNext->getUCN() == UCNcounter+1 )
					if( gcNext->getUCN() == SearchListRoot->getUCN() )
					{
						gcAct = gcNext;
						rAct=rCopy(gcAct->baseRing);
						rComplete(rAct);
						rChangeCurrRing(rAct);						
						break;						
					}
					gcNext = gcNext->next;
				}
				UCNcounter++;
				//SearchListAct = SearchListAct->next;
				//SearchListAct = fAct->next;
				SearchListAct = SearchListRoot;
			}
			cout << endl << "Found " << counter << " cones - terminating" << endl;
		
			//NOTE Hm, comment in and get a crash for free...
			//dd_free_global_constants();				
			//gc.writeConeToFile(gc);
			
			
			//fAct = fListPtr;
			//gcone *gcTmp = new gcone(gc);	//copy
			//gcTmp->flip(gcTmp->gcBasis,fAct);
			//NOTE: gcTmp may be deleted, gcRoot from main proc should probably not!
			
		}//void noRevS(gcone &gc)	
		
		
		/** \brief Make a set of rational vectors into integers
		*
		* We compute the lcm of the denominators and multiply with this to get integer values.		
		* \param dd_MatrixPtr,intvec
		*/
		void makeInt(dd_MatrixPtr const &M, int const line, intvec &n)
		{			
			mpz_t denom[this->numVars];
			for(int ii=0;ii<this->numVars;ii++)
			{
				mpz_init_set_str(denom[ii],"0",10);
			}
		
			mpz_t kgV,tmp;
			mpz_init(kgV);
			mpz_init(tmp);
			
			for (int ii=0;ii<(M->colsize)-1;ii++)
			{
				mpz_t z;
				mpz_init(z);
				mpq_get_den(z,M->matrix[line-1][ii+1]);
				mpz_set( denom[ii], z);
				mpz_clear(z);				
			}
			
			//Check whether denom is all ones, in which case we will divide out the gcd of the nominators
// 			mpz_t checksum; mpz_t rop;
// 			mpz_init(checksum);
// 			mpz_init(rop);
// 			bool divideOutGcd=FALSE;
// 			for(int ii=0;ii<this->numVars;ii++)
// 			{
// 				mpz_add(rop, checksum, denom[ii]);
// 				mpz_set(checksum, rop);
// 			}
// 			if( (int)mpz_get_ui(checksum)==this->numVars)
// 			{
// 				divideOutGcd=TRUE;
// 			}
			
			/*Compute lcm of the denominators*/
			mpz_set(tmp,denom[0]);
			for (int ii=0;ii<(M->colsize)-1;ii++)
			{
				mpz_lcm(kgV,tmp,denom[ii]);
				mpz_set(tmp,kgV);				
			}
			
			/*Multiply the nominators by kgV*/
			mpq_t qkgV,res;
			mpq_init(qkgV);
			mpq_set_str(qkgV,"1",10);
 			mpq_canonicalize(qkgV);
			
			mpq_init(res);
			mpq_set_str(res,"1",10);
 			mpq_canonicalize(res);
			
			mpq_set_num(qkgV,kgV);
			
// 			mpq_canonicalize(qkgV);
			for (int ii=0;ii<(M->colsize)-1;ii++)
			{
				mpq_mul(res,qkgV,M->matrix[line-1][ii+1]);
				n[ii]=(int)mpz_get_d(mpq_numref(res));
			}
			//mpz_clear(denom[this->numVars]);
			mpz_clear(kgV);
			mpq_clear(qkgV); mpq_clear(res);			
			
		}
		/**
		 * For all codim-2-facets we compute the gcd of the components of the facet normal and 
		 * divide it out. Thus we get a normalized representation of each
		 * (codim-2)-facet normal, i.e. a primitive vector
		*/
		void normalize()
		{
			int ggT=1;
			facet *fAct;
			facet *codim2Act;
			fAct = this->facetPtr;
			codim2Act = fAct->codim2Ptr;
			intvec *n = new intvec(this->numVars);
			
			//while(codim2Act->next!=NULL)
			while(fAct!=NULL)
			{
				while(codim2Act!=NULL)
				{				
					n=codim2Act->getFacetNormal();
					for(int ii=0;ii<this->numVars;ii++)
					{
						ggT = intgcd(ggT,(*n)[ii]);
					}
					for(int ii=0;ii<this->numVars;ii++)
					{
						(*n)[ii] = ((*n)[ii])/ggT;
					}
					codim2Act->setFacetNormal(n);
					codim2Act = codim2Act->next;				
				}
				fAct = fAct->next;
			}
		
			//delete n;
			/*codim2Act = this->facetPtr->codim2Ptr;	//reset to start of linked list			
			while(codim2Act!=NULL)
			{				
				n=codim2Act->getFacetNormal();
				intvec *new_n = new intvec(this->numVars);
				for(int ii=0;ii<this->numVars;ii++)
				{
					(*new_n)[ii] = (*n)[ii]*ggT;
				}
				codim2Act->setFacetNormal(new_n);
				codim2Act = codim2Act->next;
				//delete n;
				//delete new_n;
			}	*/		
		}
		/** \brief Enqueue new facets into the searchlist 
		* The searchlist (SLA for short) is implemented as a FIFO
		* Checks are done as follows:
		* 1) Check facet fAct against all facets in SLA for parallelity. If it is not parallel to any one add it.
		* 2) If it is parallel compare the codim2 facets. If they coincide the facets are equal and we delete the 
		*	facet from SLA and do not add fAct.
		* It may very well happen, that SLA=\f$ \emptyset \f$ but we do not have checked all fActs yet. In this case we
		* can be sure, that none of the facets that are still there already were in SLA once, so we just dump them into SLA.
		* Takes ptr to search list root
		* Returns a pointer to new first element of Searchlist
		*/
		//void enqueueNewFacets(facet &f)
		facet * enqueueNewFacets(facet &f)
		{
			facet *slHead;
			slHead = &f;
			facet *slAct;	//called with f=SearchListRoot
			slAct = &f;
			facet *slEnd;	//Pointer to end of SLA
			slEnd = &f;
			facet *slEndStatic;	//marks the end before a new facet is added		
			facet *fAct;
			fAct = this->facetPtr;
			facet *codim2Act;
			codim2Act = this->facetPtr->codim2Ptr;
			facet *sl2Act;
			sl2Act = f.codim2Ptr;
			/** Pointer to a facet that will be deleted */
			facet *deleteMarker;
			deleteMarker = NULL;
			
			/** Flag to indicate a facet that should be added to SLA*/
			bool doNotAdd=FALSE;
			/** \brief  Flag to mark a facet that might be added
			* The following case may occur:
			* A facet fAct is found to be parallel but not equal to the current facet slAct from SLA.
			* This does however not mean that there does not exist a facet behind the current slAct that is actually equal
			* to fAct. In this case we set the boolean flag maybe to TRUE. If we encounter an equality lateron, it is reset to
			* FALSE and the according slAct is deleted. 
			* If slAct->next==NULL AND maybe==TRUE we know, that fAct must be added
			*/
			volatile bool maybe=FALSE;
			/**A facet was removed, lengthOfSearchlist-- thus we must not rely on 
			* if(notParallelCtr==lengthOfSearchList) but rather
			* if( (notParallelCtr==lengthOfSearchList && removalOccured==FALSE)
			*/
			volatile bool removalOccured=FALSE;
			int ctr=0;	//encountered equalities in SLA
			int notParallelCtr=0;
			int lengthOfSearchList=1;
			while(slEnd->next!=NULL)
			{
				slEnd=slEnd->next;
				lengthOfSearchList++;
			}
			slEndStatic = slEnd;
			/*1st step: compare facetNormals*/
			intvec *fNormal=NULL; //= new intvec(this->numVars);
			intvec *f2Normal=NULL; //= new intvec(this->numVars);
			intvec *slNormal=NULL; //= new intvec(this->numVars);
			intvec *sl2Normal=NULL; //= new intvec(this->numVars);
			
			while(fAct!=NULL)
			{						
				if(fAct->isFlippable==TRUE)
				{
					maybe=FALSE;
					doNotAdd=TRUE;
					fNormal=fAct->getFacetNormal();
					slAct = slHead;
					notParallelCtr=0;
					/*If slAct==NULL and fAct!=NULL 
					we just copy all remaining facets into SLA*/
					if(slAct==NULL)
					{
						facet *fCopy;
						fCopy = fAct;
						while(fCopy!=NULL)
						{
							if(slAct==NULL)
							{
								slAct = new facet();
								slHead = slAct;								
							}
							else
							{
								slAct->next = new facet();
								slAct = slAct->next;
							}							
							slAct->setFacetNormal(fAct->getFacetNormal());
							slAct->setUCN(fAct->getUCN());
							slAct->isFlippable=fAct->isFlippable;
							facet *f2Copy;
							f2Copy = fCopy->codim2Ptr;
							sl2Act = slAct->codim2Ptr;
							while(f2Copy!=NULL)
							{
								if(sl2Act==NULL)
								{
									sl2Act = new facet(2);
									slAct->codim2Ptr = sl2Act;					
								}
								else
								{
									facet *marker;
									marker = sl2Act;
									sl2Act->next = new facet(2);
									sl2Act = sl2Act->next;
									sl2Act->prev = marker;
								}
								sl2Act->setFacetNormal(f2Copy->getFacetNormal());
								sl2Act->setUCN(f2Copy->getUCN());
								f2Copy = f2Copy->next;
							}
							fCopy = fCopy->next;
						}
						break;
					}
					/*End of */
					while(slAct!=NULL)
					//while(slAct!=slEndStatic->next)
					{
						removalOccured=FALSE;
						slNormal = slAct->getFacetNormal();
#ifdef gfan_DEBUG
						cout << "Checking facet (";
						fNormal->show(1,1);
						cout << ") against (";
						slNormal->show(1,1);
						cout << ")" << endl;
#endif
						if(!isParallel(fNormal, slNormal))
						{
							notParallelCtr++;
// 							slAct = slAct->next;
						}
						else	//fN and slN are parallel
						{
							//We check whether the codim-2-facets coincide
							codim2Act = fAct->codim2Ptr;
							ctr=0;
							while(codim2Act!=NULL)
							{
								f2Normal = codim2Act->getFacetNormal();
								//sl2Act = f.codim2Ptr;
								sl2Act = slAct->codim2Ptr;
								while(sl2Act!=NULL)
								{
									sl2Normal = sl2Act->getFacetNormal();
									if(areEqual(f2Normal, sl2Normal))
											ctr++;
									sl2Act = sl2Act->next;
								}//while(sl2Act!=NULL)
								codim2Act = codim2Act->next;
							}//while(codim2Act!=NULL)
							if(ctr==fAct->numCodim2Facets)	//facets ARE equal
							{
#ifdef gfan_DEBUG
								cout << "Removing facet (";
								slNormal->show(1,0);
								cout << ") from SLA:" << endl;
 								fAct->fDebugPrint();
 								slAct->fDebugPrint();
#endif
								removalOccured=TRUE;
								slAct->isFlippable=FALSE;
								doNotAdd=TRUE;
								maybe=FALSE;								
								if(slAct==slHead)	//We want to delete the first element of SearchList
								{
									deleteMarker = slHead;				
									slHead = slAct->next;						
									if(slHead!=NULL)
										slHead->prev = NULL;					
									//set a bool flag to mark slAct as to be deleted
								}//NOTE find a way to delete without affecting slAct = slAct->next
								else if(slAct==slEndStatic)
									{
										deleteMarker = slAct;					
										if(slEndStatic->next==NULL)
										{							
											slEndStatic = slEndStatic->prev;
											slEndStatic->next = NULL;
											slEnd = slEndStatic;
										}
										else	//we already added a facet after slEndStatic
										{
											slEndStatic->prev->next = slEndStatic->next;
											slEndStatic = slEndStatic->prev;
											//slEnd = slEndStatic;
										}
									} 								
									else
									{
										deleteMarker = slAct;
										slAct->prev->next = slAct->next;
										slAct->next->prev = slAct->prev;
									}
								
								//update lengthOfSearchList					
 								lengthOfSearchList--;
								//slAct = slAct->next; //not needed, since facets are equal
								//delete deleteMarker;
								deleteMarker=NULL;
								//fAct = fAct->next;
 								break;
							}//if(ctr==fAct->numCodim2Facets)
							else	//facets are parallel but NOT equal. But this does not imply that there
								//is no other facet later in SLA that might be equal.
							{
								maybe=TRUE;
//       								if(slAct->next==NULL && maybe==TRUE)
//       								{
//  									doNotAdd=FALSE;
//    									slAct = slAct->next;
//  									break;
//       								}
//       								else
//    									slAct=slAct->next;
							}
							//slAct = slAct->next;
							//delete deleteMarker;							
						}//if(!isParallel(fNormal, slNormal))
 						if( (slAct->next==NULL) && (maybe==TRUE) )
 						{
 							doNotAdd=FALSE;
 							break;
 						}
 						slAct = slAct->next;
						/* NOTE The following lines must not be here but rather called inside the if-block above.
						Otherwise results get crappy. Unfortunately there are two slAct=slAct->next calls now,
						(not nice!) but since they are in seperate branches of the if-statement there should not
						be a way it gets called twice thus ommiting one facet:
 						slAct = slAct->next;						
						delete deleteMarker;*/
						deleteMarker=NULL;
						//if slAct was marked as to be deleted, delete it here!
					}//while(slAct!=NULL)									
					if( (notParallelCtr==lengthOfSearchList && removalOccured==FALSE) || (doNotAdd==FALSE) )
 					//if( (notParallelCtr==lengthOfSearchList ) || doNotAdd==FALSE ) 
					{
#ifdef gfan_DEBUG
						cout << "Adding facet (";
						fNormal->show(1,0);
						cout << ") to SLA " << endl;
#endif
						//Add fAct to SLA
						facet *marker;
						marker = slEnd;
						facet *f2Act;
						f2Act = fAct->codim2Ptr;
						
						slEnd->next = new facet();
						slEnd = slEnd->next;
						facet *slEndCodim2Root;
						facet *slEndCodim2Act;
						slEndCodim2Root = slEnd->codim2Ptr;
						slEndCodim2Act = slEndCodim2Root;
								
						slEnd->setUCN(this->getUCN());
						slEnd->isFlippable = TRUE;
						slEnd->setFacetNormal(fNormal);
						slEnd->prev = marker;
						//Copy codim2-facets
						intvec *f2Normal;// = new intvec(this->numVars);
						while(f2Act!=NULL)
						{
							f2Normal=f2Act->getFacetNormal();
							if(slEndCodim2Root==NULL)
							{
								slEndCodim2Root = new facet(2);
								slEnd->codim2Ptr = slEndCodim2Root;			
								slEndCodim2Root->setFacetNormal(f2Normal);
								slEndCodim2Act = slEndCodim2Root;
							}
							else					
							{
								slEndCodim2Act->next = new facet(2);
								slEndCodim2Act = slEndCodim2Act->next;
								slEndCodim2Act->setFacetNormal(f2Normal);
							}
							f2Act = f2Act->next;
						}
						lengthOfSearchList++;
						//delete f2Normal;						
					}//if( (notParallelCtr==lengthOfSearchList && removalOccured==FALSE) ||
					fAct = fAct->next;
				}//if(fAct->isFlippable==TRUE)
				else
				{
					fAct = fAct->next;
				}
			}//while(fAct!=NULL)						
			return slHead;
// 			delete sl2Normal;
// 			delete slNormal;
// 			delete f2Normal;
// 			delete fNormal;
 		}//addC2N
		
		/** \brief Compute the gcd of two ints
		*/
		int intgcd(int a, int b)
		{
			int r, p=a, q=b;
			if(p < 0)
				p = -p;
			if(q < 0)
				q = -q;
			while(q != 0)
			{
				r = p % q;
				p = q;
				q = r;
			}
			return p;
		}		
		
		/** \brief Construct a dd_MatrixPtr from a cone's list of facets
		* 
		*/
		dd_MatrixPtr facets2Matrix(gcone const &gc)
		{
			facet *fAct;
			fAct = gc.facetPtr;
	
			dd_MatrixPtr M;
			dd_rowrange ddrows;
			dd_colrange ddcols;
			ddcols=(this->numVars)+1;
			ddrows=this->numFacets;
			dd_NumberType numb = dd_Integer;
			M=dd_CreateMatrix(ddrows,ddcols);			
			
			int jj=0;
			//while(fAct->next!=NULL)
			while(fAct!=NULL)
			{
				intvec *comp;
				comp = fAct->getFacetNormal();
				for(int ii=0;ii<this->numVars;ii++)
				{
					dd_set_si(M->matrix[jj][ii+1],(*comp)[ii]);
				}
				jj++;
				fAct=fAct->next;				
			}			
			
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
 			int UCN=gc.UCN;
			stringstream ss;
			ss << UCN;
			string UCNstr = ss.str();		
			
			char prefix[]="/tmp/cone";
			char *UCNstring;
			strcpy(UCNstring,UCNstr.c_str());
			char suffix[]=".sg";
			char *filename=strcat(prefix,UCNstring);
			strcat(filename,suffix);
					
			ofstream gcOutputFile(filename);
			facet *fAct = new facet(); //NOTE Why new?
			fAct = gc.facetPtr;			
			
			char *ringString = rString(currRing);
			
			if (!gcOutputFile)
			{
				cout << "Error opening file for writing in writeConeToFile" << endl;
			}
			else
			{	gcOutputFile << "UCN" << endl;
				gcOutputFile << this->UCN << endl;
				gcOutputFile << "RING" << endl;	
				gcOutputFile << ringString << endl;			
				//Write this->gcBasis into file
				gcOutputFile << "GCBASIS" << endl;				
				for (int ii=0;ii<IDELEMS(gc.gcBasis);ii++)
				{					
 					gcOutputFile << p_String((poly)gc.gcBasis->m[ii],gc.baseRing) << endl;
				}				
				
				gcOutputFile << "FACETS" << endl;								
				//while(fAct->next!=NULL)
				while(fAct!=NULL)
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

int gcone::counter=0;

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
		cout << "GB of input ideal is:" << endl;
		idShow(gcAct->gcBasis);
		cout << endl;
		gcAct->getConeNormals(gcAct->gcBasis);		
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
