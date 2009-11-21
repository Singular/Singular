/*
Compute the Groebner fan of an ideal
$Author: monerjan $
$Date: 2009/11/03 06:57:32 $
$Header: /usr/local/Singular/cvsroot/kernel/gfan.cc,v 1.103 2009/11/03 06:57:32 monerjan Exp $
$Id$
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
#include "structs.h"
#include "../Singular/lists.h"
#include "prCopy.h"
#include <iostream>
#include <bitset>
#include <fstream>	//read-write cones to files
#include <gmp.h>
#include <string>
#include <sstream>
#include <time.h>

//#include <gmpxx.h>

/*DO NOT REMOVE THIS*/
#ifndef GMPRATIONAL
#define GMPRATIONAL
#endif

//Hacks for different working places
#define p800

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
//#define gfan_DEBUG
#ifndef gfan_DEBUGLEVEL
#define gfan_DEBUGLEVEL 1
#endif
#endif

#include <gfan.h>
using namespace std;

/**
*\brief Class facet
*	Implements the facet structure as a linked list
*
*/
		
/** \brief The default constructor for facets
*/
facet::facet()			
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
	this->flipRing=NULL;	//the ring on the other side
	this->isFlippable=FALSE;
}
		
/** \brief Constructor for facets of codim >= 2
* Note that as of now the code of the constructors is only for facets and codim2-faces. One
* could easily change that by renaming numCodim2Facets to numCodimNminusOneFacets or similar
*/
facet::facet(int const &n)
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
facet::facet(const facet& f)
{
}
		
/** The default destructor */
facet::~facet()
{
	delete this->fNormal;
	delete this->interiorPoint;
	/* Cleanup the codim2-structure */
	if(this->codim==2)
	{
		facet *codim2Ptr;
		codim2Ptr = this->codim2Ptr;
		while(codim2Ptr!=NULL)
		{
			delete codim2Ptr->fNormal;
			codim2Ptr = codim2Ptr->next;
		}
	}
	delete this->codim2Ptr;
	idDelete((ideal *)&this->flipGB);
	rDelete(this->flipRing);
	this->flipRing=NULL;
	//this=NULL;
}
		
		
/** \brief Comparison of facets*/
bool facet::areEqual(facet &f, facet &g)
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
void facet::setFacetNormal(intvec *iv)
{
	this->fNormal = ivCopy(iv);			
}
		
/** Hopefully returns the facet normal */
intvec *facet::getFacetNormal()
{				
	return ivCopy(this->fNormal);
}
		
/** Method to print the facet normal*/
void facet::printNormal()
{
	fNormal->show();
}
		
/** Store the flipped GB*/
void facet::setFlipGB(ideal I)
{
	this->flipGB=idCopy(I);
}
		
/** Return the flipped GB*/
ideal facet::getFlipGB()
{
	return this->flipGB;
}
		
/** Print the flipped GB*/
void facet::printFlipGB()
{
#ifndef NDEBUG
	idShow(this->flipGB);
#endif
}
		
/** Set the UCN */
void facet::setUCN(int n)
{
	this->UCN=n;
}
		
/** \brief Get the UCN 
* Returns the UCN iff this != NULL, else -1
*/
int facet::getUCN()
{
 	if(this!=NULL && ( this!=(facet *)0xfbfbfbfb || this!=(facet *)0xfbfbfbfbfbfbfbfb) )
// 	if(this!=NULL && ( this->fNormal!=(intvec *)0xfbfbfbfb || this->fNormal!=(intvec *)0xfbfbfbfbfbfbfbfb) )
		return this->UCN;
	else
		return -1;
}
		
/** Store an interior point of the facet */
void facet::setInteriorPoint(intvec *iv)
{
	this->interiorPoint = ivCopy(iv);
}
		
intvec *facet::getInteriorPoint()
{
	return this->interiorPoint;
}	
		
/** \brief Debugging function
* prints the facet normal an all (codim-2)-facets that belong to it
*/
void facet::fDebugPrint()
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
	cout << "-----------------------" << endl;
	cout << "Codim2 facets:" << endl;
	while(codim2Act!=NULL)
	{
		f2Normal = codim2Act->getFacetNormal();
		cout << "(";
		f2Normal->show(1,0);
		cout << ")" << endl;
		codim2Act = codim2Act->next;
	}
	cout << "=======================" << endl;
}		
		
//friend class gcone;	//Bad style


/**
*\brief Implements the cone structure
*
* A cone is represented by a linked list of facet normals
* @see facet
*/


/** \brief Default constructor. 
 *
 * Initialises this->next=NULL and this->facetPtr=NULL
 */
gcone::gcone()
{
	this->next=NULL;
	this->prev=NULL;
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
 * This constructor is only called once in the computation of the Gröbner fan,
 * namely for the very first cone. Therefore pred is set to 1.
 * Might set it to this->UCN though...
 * Since knowledge of the root ring is only needed when using reverse search,
 * this constructor is not needed when using the "second" method
*/
gcone::gcone(ring r, ideal I)
{
	this->next=NULL;
	this->prev=NULL;
	this->facetPtr=NULL;			
// 	this->rootRing=r;
	this->inputIdeal=I;
	this->baseRing=currRing;
	this->counter++;
	this->UCN=this->counter;
	this->pred=1;
	this->numFacets=0;
	this->ivIntPt=NULL;
}
		
/** \brief Copy constructor 
 *
 * Copies a cone, sets this->gcBasis to the flipped GB
 * Call this only after a successful call to gcone::flip which sets facet::flipGB
*/		
gcone::gcone(const gcone& gc, const facet &f)
{
	this->next=NULL;
// 	this->prev=(gcone *)&gc; //comment in to get a tree
	this->prev=NULL;
	this->numVars=gc.numVars;						
	this->counter++;
	this->UCN=this->counter;
	this->pred=gc.UCN;			
	this->facetPtr=NULL;
	this->gcBasis=idCopy(f.flipGB);
	this->inputIdeal=idCopy(this->gcBasis);
	this->baseRing=rCopy(f.flipRing);
	this->numFacets=0;
	this->ivIntPt=NULL;
// 	this->rootRing=NULL;
	//rComplete(this->baseRing);
	//rChangeCurrRing(this->baseRing);
}
		
/** \brief Default destructor 
*/
gcone::~gcone()
{
	if(this->gcBasis!=NULL)
		idDelete((ideal *)&this->gcBasis);
	if(this->inputIdeal!=NULL)
		idDelete((ideal *)&this->inputIdeal);
// 	if (this->rootRing!=NULL && this->rootRing!=(ip_sring *)0xfefefefefefefefe)
// 		rDelete(this->rootRing);
	//rDelete(this->baseRing);
	facet *fAct;
	facet *fDel;
	/*Delete the facet structure*/
	fAct=this->facetPtr;
	fDel=fAct;
	while(fAct!=NULL)
	{
		fDel=fAct;
		fAct=fAct->next;
		delete fDel;
	}
	//dd_FreeMatrix(this->ddFacets);
}			
	
int gcone::getCounter()
{
	return this->counter;
}
	
/** \brief Set the interior point of a cone */
void gcone::setIntPoint(intvec *iv)
{
	this->ivIntPt=ivCopy(iv);
}
		
/** \brief Return the interior point */
intvec *gcone::getIntPoint()
{
	return this->ivIntPt;
}
		
/** \brief Print the interior point */
void gcone::showIntPoint()
{
	ivIntPt->show();
}
		
/** \brief Print facets
 * This is mainly for debugging purposes. Usually called from within gdb
 */
void gcone::showFacets(short codim)
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
void gcone::showSLA(facet &f)
{
	facet *fAct;
	fAct = &f;
	facet *codim2Act;
	codim2Act = fAct->codim2Ptr;
	intvec *fNormal;
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
		
void gcone::idDebugPrint(ideal const &I)
{
	int numElts=IDELEMS(I);
	cout << "Ideal with " << numElts << " generators" << endl;
	cout << "Leading terms: ";
	for (int ii=0;ii<numElts;ii++)
	{
		pWrite0(pHead(I->m[ii]));
		cout << ",";
	}
	cout << endl;
}

void gcone::invPrint(ideal const &I)
{
// 	int numElts=IDELEMS(I);
// 	cout << "inv = ";
// 	for(int ii=0;ii<numElts;ii++);
// 	{
// 		pWrite0(pHead(I->m[ii]));
// 		cout << ",";
// 	}
// 	cout << endl;
}

bool gcone::isMonomial(ideal const &I)
{
	bool res = TRUE;
	for(int ii=0;ii<IDELEMS(I);ii++)
	{
		if(pLength((poly)I->m[ii])>1)
		{
			res = FALSE;
			break;
		}						 
	}
	return res;
}
		
/** \brief Set gcone::numFacets */
void gcone::setNumFacets()
{
}
		
/** \brief Get gcone::numFacets */
int gcone::getNumFacets()
{
	return this->numFacets;
}
		
int gcone::getUCN()
{
	if( this!=NULL && ( this!=(gcone * const)0xfbfbfbfbfbfbfbfb && this!=(gcone * const)0xfbfbfbfb ) )
		return this->UCN;
	else
		return -1;
}

int gcone::getPredUCN()
{
	return this->pred;
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
void gcone::getConeNormals(ideal const &I, bool compIntPoint)
{
	poly aktpoly;
	int rows; 			// will contain the dimensions of the ineq matrix - deprecated by
	dd_rowrange ddrows;
	dd_colrange ddcols;
	dd_rowset ddredrows;		// # of redundant rows in ddineq
	dd_rowset ddlinset;		// the opposite
	dd_rowindex ddnewpos;		  // all to make dd_Canonicalize happy
	dd_NumberType ddnumb=dd_Integer; //Number type
	dd_ErrorType dderr=dd_NoError;		
	//Compute the # inequalities i.e. rows of the matrix
	rows=0; //Initialization
	for (int ii=0;ii<IDELEMS(I);ii++)
	{
		aktpoly=(poly)I->m[ii];
		rows=rows+pLength(aktpoly)-1;
	}

	dd_rowrange aktmatrixrow=0;	// needed to store the diffs of the expvects in the rows of ddineq
	dd_set_global_constants();
	ddrows=rows;
	ddcols=this->numVars;
	dd_MatrixPtr ddineq; 		//Matrix to store the inequalities			
	ddineq=dd_CreateMatrix(ddrows,ddcols+1); //The first col has to be 0 since cddlib checks for additive consts there
		
	// We loop through each g\in GB and compute the resulting inequalities
	for (int i=0; i<IDELEMS(I); i++)
	{
		aktpoly=(poly)I->m[i];		//get aktpoly as i-th component of I
		//simpler version of storing expvect diffs
		int *leadexpv=(int*)omAlloc(((this->numVars)+1)*sizeof(int));
		int *tailexpv=(int*)omAlloc(((this->numVars)+1)*sizeof(int));
		pGetExpV(aktpoly,leadexpv);
		while(pNext(aktpoly)!=NULL)
		{
			aktpoly=pNext(aktpoly);
			pGetExpV(aktpoly,tailexpv);
			for(int kk=1;kk<=this->numVars;kk++)
			{
				dd_set_si(ddineq->matrix[(dd_rowrange)aktmatrixrow][kk],leadexpv[kk]-tailexpv[kk]);
			}
			aktmatrixrow += 1;
		}
		omFree(tailexpv);
		omFree(leadexpv);	
	} //for
#if true
	/*Let's make the preprocessing here. This could already be done in the above for-loop,
	* but for a start it is more convenient here.
	* We check the necessary condition of FJT p.18
	* Quote: [...] every non-zero spoly should have at least one of its terms in inv(G)
	*/
	ideal initialForm=idInit(IDELEMS(I),1);
	intvec *gamma=new intvec(this->numVars);
	int falseGammaCounter=0;
	int *redRowsArray=NULL;
	int num_alloc=0;
	int num_elts=0;	
	for(int ii=0;ii<ddineq->rowsize;ii++)
	{
		//read row ii into gamma
		double tmp;	
		for(int jj=1;jj<=this->numVars;jj++)
		{
			tmp=mpq_get_d(ddineq->matrix[ii][jj]);
			(*gamma)[jj-1]=(int)tmp;
		}
		computeInv((ideal&)I,initialForm,*gamma);
		//Create leading ideal
		ideal L=idInit(IDELEMS(initialForm),1);
		for(int jj=0;jj<IDELEMS(initialForm);jj++)
		{
			L->m[jj]=pCopy(pHead(initialForm->m[jj]));
		}		
		
		LObject *P = new sLObject();
		memset(P,0,sizeof(LObject));

		for(int jj=0;jj<=IDELEMS(initialForm)-2;jj++)
		{
			bool isMaybeFacet=FALSE;
			P->p1=initialForm->m[jj];	//build spolys of initialForm in_v

			for(int kk=jj+1;kk<=IDELEMS(initialForm)-1;kk++)
			{
				P->p2=initialForm->m[kk];
				ksCreateSpoly(P);							
				if(P->p!=NULL)	//spoly non zero=?
				{	
					poly p=pInit();			
					poly q=pInit();
					p=pCopy(P->p);
					q=pHead(p);	//Monomial q
					isMaybeFacet=FALSE;
					while(p!=NULL)
					{
						q=pHead(p);						
// 						unsigned long sevSpoly=pGetShortExpVector(q);
// 						unsigned long not_sevL;						
						for(int ll=0;ll<IDELEMS(L);ll++)
						{
// 							not_sevL=~pGetShortExpVector(L->m[ll]);// 					
							//if(!(sevSpoly & not_sevL) && pLmDivisibleBy(L->m[ll],q) )//i.e. spoly is in L	
							if(pLmEqual(L->m[ll],q) || pDivisibleBy(L->m[ll],q))
							{							
								isMaybeFacet=TRUE;
 								break;//for
							}
						}
						if(isMaybeFacet==TRUE)
						{ 							
							break;//while(p!=NULL)
						}
						p=pNext(p);						
					}//while
					if(isMaybeFacet==FALSE)
					{
						dd_set_si(ddineq->matrix[ii][0],1);						
						if(num_alloc==0)
							num_alloc += 1;
						else
							num_alloc += 1;
						void *_tmp = realloc(redRowsArray,(num_alloc*sizeof(int)));
						if(!_tmp)
							WerrorS("Woah dude! Couldn't realloc memory\n");
						redRowsArray = (int*)_tmp;
						redRowsArray[num_elts]=ii;
						num_elts++;
						//break;//for(int kk, since we have found one that is not in L	
						goto _start;	//mea culpa, mea culpa, mea maxima culpa
					}
				}//if(P->p!=NULL)				
			}//for k
		}//for jj
		_start:;
		delete P;
		//idDelete(L);		
	}//for(ii<ddineq-rowsize
	
	int offset=0;//needed for correction of redRowsArray[ii]
	for( int ii=0;ii<num_elts;ii++ )
	{
		dd_MatrixRowRemove(&ddineq,redRowsArray[ii]+1-offset);//cddlib sucks at enumeration
		offset++;
	}
	free(redRowsArray);
#endif
// 	cout << "Found " << falseGammaCounter << " false in " << rows << endl;
	//Maybe add another row to contain the constraints of the standard simplex?


	//Remove redundant rows here!
	//Necessary check here! C.f. FJT p18		
	dd_MatrixCanonicalize(&ddineq, &ddlinset, &ddredrows, &ddnewpos, &dderr);
	//time(&canonicalizeTac);
	//cout << "dd_MatrixCanonicalize time: " << difftime(canonicalizeTac,canonicalizeTic) << "sec" << endl;
	ddrows = ddineq->rowsize;	//Size of the matrix with redundancies removed
	ddcols = ddineq->colsize;
			
	//ddCreateMatrix(ddrows,ddcols+1);
	ddFacets = dd_CopyMatrix(ddineq);
			
	/*Write the normals into class facet*/
	/*The pointer *fRoot should be the return value of this function*/
	//facet *fRoot = new facet();	//instantiate new facet
	//fRoot->setUCN(this->getUCN());
	//this->facetPtr = fRoot;		//set variable facetPtr of class gcone to first facet
	facet *fAct; 	//pointer to active facet
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
			(*load)[jj-1] = (int)foo;	//store typecasted entry at pos jj-1 of load		
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
			delete ivCanonical;
		}/*End of check for flippability*/
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
		delete load;				
	}//for (int kk = 0; kk<ddrows; kk++)
			
	//In cases like I=<x-1,y-1> there are only non-flippable facets...
	if(numNonFlip==this->numFacets)
	{					
		WerrorS ("Only non-flippable facets. Terminating...\n");
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
		delete iv;
	}
			
	//Compute the number of facets
	// wrong because ddineq->rowsize contains only irredundant facets. There can still be
	// non-flippable ones!
	//this->numFacets=ddineq->rowsize;
	
	//Clean up but don't delete the return value! (Whatever it will turn out to be)			
	dd_FreeMatrix(ddineq);
	set_free(ddredrows);
	set_free(ddlinset);
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
void gcone::getCodim2Normals(gcone const &gc)
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
		ddpolyh=dd_DDMatrix2Poly(ddakt, &err);
		P=dd_CopyGenerators(ddpolyh);				
		/* We loop through each row of P normalize it by making all
		* entries integer ones and add the resulting vector to the
		* int matrix facet::codim2Facets */
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
//  		dd_FreeMatrix(ddineq);
		dd_FreeMatrix(shiftMatrix);
		dd_FreeMatrix(intPointMatrix);
		dd_FreePolyhedra(ddpolyh);
		delete iv_intPoint;
	}//for
	dd_FreeMatrix(ddineq);
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
void gcone::flip(ideal gb, facet *f)		//Compute "the other side"
{			
	intvec *fNormal = new intvec(this->numVars);	//facet normal, check for parallelity			
	fNormal = f->getFacetNormal();	//read this->fNormal;

	std::cout << "running gcone::flip" << std::endl;
	std::cout << "flipping UCN " << this->getUCN() << endl;
	cout << "over facet (";
	fNormal->show(1,0);
	cout << ") with UCN " << f->getUCN();
	std::cout << std::endl;

	/*1st step: Compute the initial ideal*/
	/*poly initialFormElement[IDELEMS(gb)];*/	//array of #polys in GB to store initial form
	ideal initialForm=idInit(IDELEMS(gb),this->gcBasis->rank);
	poly aktpoly;	
	computeInv(gb,initialForm,*fNormal);
	//NOTE The code below went into gcone::computeInv
#ifdef gfan_DEBUG
/*	cout << "Initial ideal is: " << endl;
	idShow(initialForm);
	//f->printFlipGB();*/
// 	cout << "===" << endl;
#endif			
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
 	delete fNormal; //NOTE: Why does this crash?
	rChangeCurrRing(tmpRing);
			
	//rWrite(currRing); cout << endl;
			
	ideal ina;			
	ina=idrCopyR(initialForm,srcRing);
#ifndef NDEBUG			
#ifdef gfan_DEBUG
   	cout << "ina=";
   	idShow(ina); cout << endl;
#endif
#endif
	ideal H;
	//H=kStd(ina,NULL,isHomog,NULL);	//we know it is homogeneous
	H=kStd(ina,NULL,testHomog,NULL);	//This is \mathcal(G)_{>_-\alpha}(in_v(I))
	idSkipZeroes(H);
#ifndef NDEBUG
#ifdef gfan_DEBUG
   			cout << "H="; idShow(H); cout << endl;
#endif
#endif
	/*Substep 2.2
	do the lifting and mark according to H
	*/
	rChangeCurrRing(srcRing);
	ideal srcRing_H;
	ideal srcRing_HH;			
	srcRing_H=idrCopyR(H,tmpRing);
#ifndef NDEBUG
#ifdef gfan_DEBUG
   	cout << "srcRing_H = ";
   	idShow(srcRing_H); cout << endl;
#endif
#endif
 	srcRing_HH=ffG(srcRing_H,this->gcBasis);	
#ifndef NDEBUG
#ifdef gfan_DEBUG
   	cout << "srcRing_HH = ";
   	idShow(srcRing_HH); cout << endl;
#endif
#endif
	/*Substep 2.2.1
	 * Mark according to G_-\alpha
	 * Here we have a minimal basis srcRing_HH. In order to turn this basis into a reduced basis
	 * we have to compute an interior point of C(srcRing_HH). For this we need to know the cone
	 * represented by srcRing_HH MARKED ACCORDING TO G_{-\alpha}
	 * Thus we check whether the leading monomials of srcRing_HH and srcRing_H coincide. If not we 
	 * compute the difference accordingly
	*/
	dd_set_global_constants();
	bool markingsAreCorrect=FALSE;
	dd_MatrixPtr intPointMatrix;
	int iPMatrixRows=0;
	dd_rowrange aktrow=0;			
	for (int ii=0;ii<IDELEMS(srcRing_HH);ii++)
	{
		poly aktpoly=(poly)srcRing_HH->m[ii];
		iPMatrixRows = iPMatrixRows+pLength(aktpoly);
	}
	/* additionally one row for the standard-simplex and another for a row that becomes 0 during
	 * construction of the differences
	 */
	intPointMatrix = dd_CreateMatrix(iPMatrixRows+2,this->numVars+1); //iPMatrixRows+10;
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
// 						cout << src_ExpV[kk] << "," << dst_ExpV[kk] << endl;
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
// 				cout << "correct markings" << endl;
#endif
			}//if (pHead(aktpoly)==pHead(H->m[jj])
			omFree(src_ExpV);
			omFree(dst_ExpV);
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
		omFree(v);
		omFree(leadExpV);
	}//for (int ii=0;ii<IDELEMS(srcRing_HH);ii++)
	/*Now we add the constraint for the standard simplex*/
	/*NOTE:Might actually work without the standard simplex
	* No, it won't. MM 
	*/
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
	dd_FreeMatrix(posRestr);
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
	//rDelete(tmpRing);
	delete iv_weight;

	//rWrite(dstRing); cout << endl;

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
	idNorm(dstRing_I);			
			//kInterRed(dstRing_I);
	idSkipZeroes(dstRing_I);
	test=save;
	/*End of step 3 - reduction*/
			
	f->setFlipGB(dstRing_I);//store the flipped GB
	f->flipRing=rCopy(dstRing);	//store the ring on the other side
//#ifdef gfan_DEBUG
	cout << "Flipped GB is UCN " << counter+1 << ":" << endl;
//  	this->idDebugPrint(dstRing_I);
// 	cout << endl;
//#endif			
	rChangeCurrRing(srcRing);	//return to the ring we started the computation of flipGB in
// 	rDelete(dstRing);
}//void flip(ideal gb, facet *f)

/** \brief Compute initial ideal
 * Compute the initial ideal in_v(G) wrt a (possible) facet normal
 * used in gcone::getFacetNormal in order to preprocess possible facet normals
 * and in gcone::flip for obvious reasons.
*/
void gcone::computeInv(ideal &gb, ideal &initialForm, intvec &fNormal)
{
	intvec *check = new intvec(this->numVars);
	poly initialFormElement[IDELEMS(gb)];
	poly aktpoly;
	
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
// 			cout << "check=";			
// 			check->show();
// 			cout << endl;
#endif
			if (isParallel(*check,fNormal)) //pass *check when 
			{
// 				cout << "Parallel vector found, adding to initialFormElement" << endl;			
				initialFormElement[ii] = pAdd(pCopy(initialFormElement[ii]),(poly)pHead(aktpoly));
			}						
		}//while
#ifdef gfan_DEBUG
//  		cout << "Initial Form=";				
//  		pWrite(initialFormElement[ii]);
//  		cout << "---" << endl;
#endif
		/*Now initialFormElement must be added to (ideal)initialForm */
		initialForm->m[ii]=initialFormElement[ii];
		omFree(leadExpV);
		omFree(v);		
	}//for
	delete check;
}

/** \brief Compute the remainder of a polynomial by a given ideal
 *
 * Compute \f$ f^{\mathcal{G}} \f$
 * Algorithm is taken from Cox, Little, O'Shea, IVA 2nd Ed. p 62
 * However, since we are only interested in the remainder, there is no need to
 * compute the factors \f$ a_i \f$
 */
//NOTE: Should be replaced by kNF or kNF2
//NOTE: Done
//NOTE: removed with r12286
		
/** \brief Compute \f$ f-f^{\mathcal{G}} \f$
*/
//NOTE: use kNF or kNF2 instead of restOfDivision
ideal gcone::ffG(ideal const &H, ideal const &G)
{
// 			cout << "Entering ffG" << endl;
	int size=IDELEMS(H);
	ideal res=idInit(size,1);
	poly temp1, temp2, temp3;	//polys to temporarily store values for pSub
	for (int ii=0;ii<size;ii++)
	{
// 		res->m[ii]=restOfDiv(H->m[ii],G);
		res->m[ii]=kNF(G, NULL,H->m[ii],0,0);
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
void gcone::getGB(ideal const &inputIdeal)		
{
	BITSET save=test;
	test|=Sy_bit(OPT_REDSB);
	test|=Sy_bit(OPT_REDTAIL);
	ideal gb;
	gb=kStd(inputIdeal,NULL,testHomog,NULL);
	idNorm(gb);
	idSkipZeroes(gb);
	this->gcBasis=gb;	//write the GB into gcBasis
	test=save;
}//void getGB
		
/** \brief Compute the negative of a given intvec
	*/		
intvec *gcone::ivNeg(const intvec *iv)
{
	//NOTE: Can't this be done without new?
	intvec *res = new intvec(iv->length());
	res=ivCopy(iv);
	*res *= (int)-1;						
	return res;
}


/** \brief Compute the dot product of two intvecs
*
*/
int gcone::dotProduct(intvec const &iva, intvec const &ivb)				
{			
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
bool gcone::isParallel(intvec const &a, intvec const &b)
{			
	int lhs,rhs;
	bool res;
	lhs=dotProduct(a,b)*dotProduct(a,b);
	rhs=dotProduct(a,a)*dotProduct(b,b);
			//cout << "LHS="<<lhs<<", RHS="<<rhs<<endl;
	if (lhs==rhs)
	{
		res = TRUE;
	}
	else
	{
		res = FALSE;
	}
	return res;
}//bool isParallel
		
/** \brief Compute an interior point of a given cone
 * Result will be written into intvec iv. 
 * Any rational point is automatically converted into an integer.
 */
void gcone::interiorPoint(dd_MatrixPtr const &M, intvec &iv) //no const &M here since we want to remove redundant rows
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
ring gcone::rCopyAndAddWeight(ring const &r, intvec const *ivw)				
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
//removed with r12286
		
/** \brief Check for equality of two intvecs
 */
bool gcone::areEqual(intvec const &a, intvec const &b)
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
//removed with r12286
		
/** \brief The new method of Markwig and Jensen
 * Compute gcBasis and facets for the arbitrary starting cone. Store \f$(codim-1)\f$-facets as normals.
 * In order to represent a facet uniquely compute also the \f$(codim-2)\f$-facets and norm by the gcd of the components.
 * Keep a list of facets as a linked list containing an intvec and an integer matrix.
 * Since a \f$(codim-1)\f$-facet belongs to exactly two full dimensional cones, we remove a facet from the list as
 * soon as we compute this facet again. Comparison of facets is done by...
 */
void gcone::noRevS(gcone &gcRoot, bool usingIntPoint)
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
	
	/* Use pure SLA or writeCone2File */
// 	enum heuristic {
// 		ram,
// 		hdd
// 	};
// 	heuristic h;
// 	h=hdd;
	
#ifdef gfan_DEBUG
	cout << "NoRevs" << endl;
	cout << "Facets are:" << endl;
	gcAct->showFacets();
#endif			
			
	gcAct->getCodim2Normals(*gcAct);
				
	//Compute unique representation of codim-2-facets
	gcAct->normalize();
			
	/* Make a copy of the facet list of first cone
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
		//only copy flippable facets! NOTE: We do not need flipRing or any such information.
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
	//NOTE Disabled until code works as expected. Reenabled 2.11.2009
	gcAct->writeConeToFile(*gcAct);
			
	/*End of initialisation*/
	fAct = SearchListAct;
	/*2nd step
	  Choose a facet from fListPtr, flip it and forget the previous cone
	  We always choose the first facet from fListPtr as facet to be flipped
	*/			
// 	time_t tic, tac;
	while((SearchListAct!=NULL))// && counter<10)
	{//NOTE See to it that the cone is only changed after ALL facets have been flipped!				
		fAct = SearchListAct;
				
		while(fAct!=NULL)
		{	//Since SLA should only contain flippables there should be no need to check for that
			
			gcAct->flip(gcAct->gcBasis,fAct);
			
			
			ring rTmp=rCopy(fAct->flipRing);
			rComplete(rTmp);			
			rChangeCurrRing(rTmp);
			gcone *gcTmp = new gcone::gcone(*gcAct,*fAct);//copy constructor!
			/* Now gcTmp->gcBasis and gcTmp->baseRing are set from fAct->flipGB and fAct->flipRing.
			 * Since we come at most once across a given facet from gcAct->facetPtr we can delete them.
			 * NOTE: Can this cause trouble with the destructor?
			 * Answer: Yes it bloody well does!
			 * However I'd like to delete this data here, since if we have a cone with many many facets it
			 * should pay to get rid of the flipGB as soon as possible.
			 * Destructor must be equipped with necessary checks.
			*/
// 			idDelete((ideal *)&fAct->flipGB);
// 			rDelete(fAct->flipRing);
			
			gcTmp->getConeNormals(gcTmp->gcBasis, FALSE);	
			gcTmp->getCodim2Normals(*gcTmp);
			gcTmp->normalize();
#ifdef gfan_DEBUG
			gcTmp->showFacets(1);
#endif
 			if(gfanHeuristic==1)
 			{
				gcTmp->writeConeToFile(*gcTmp);
 			}
			/*add facets to SLA here*/
			SearchListRoot=gcTmp->enqueueNewFacets(*SearchListRoot);
#ifdef gfan_DEBUG
			if(SearchListRoot!=NULL)
				gcTmp->showSLA(*SearchListRoot);
#endif
			rChangeCurrRing(gcAct->baseRing);
			//rDelete(rTmp);
			//doubly linked for easier removal
			gcTmp->prev = gcPtr;
			gcPtr->next=gcTmp;
			gcPtr=gcPtr->next;
			if(fAct->getUCN() == fAct->next->getUCN())
			{
				fAct=fAct->next;
			}
			else
				break;
		}//while( ( (fAct->next!=NULL) && (fAct->getUCN()==fAct->next->getUCN() ) ) );
		//NOTE Now all neighbouring cones of gcAct have been computed, so we may delete gcAct
		gcone *deleteMarker;
		deleteMarker=gcAct;		
// 		delete deleteMarker;
// 		deleteMarker=NULL;
		//Search for cone with smallest UCN
		gcNext = gcHead;
		while(gcNext!=NULL && SearchListRoot!=NULL && gcNext!=(gcone * const)0xfbfbfbfbfbfbfbfb && gcNext!=(gcone * const)0xfbfbfbfb)
		{			
			/*NOTE if gcNext->getUCN is smaller than SearchListRoot->getUCN it follows, that the cone
			gcNext will not be used in any more computation. So -> delete
			BAD IDEA!!! Luckily the delete statement below is never executed since noRevs is 
			invoked from gcRoot which has UCN==1. And apparently the else below is also never executed!
			*/
			if (gcNext->getUCN() < SearchListRoot->getUCN() )
			{
				//idDelete((ideal *)&gcNext->gcBasis);	//at least drop huge gröbner bases
				if(gcNext == gcHead)
				{
 					deleteMarker = gcHead;
 					gcHead = gcNext->next;
					//gcNext->next->prev = NULL;
				}
				else
				{
					deleteMarker = gcNext;
					gcNext->prev->next = gcNext->next;
					gcNext->next->prev = gcNext->prev;
				}
// 				gcNext = gcNext->next;
//  				delete deleteMarker;
// 				deleteMarker = NULL;
			}
			else if( gcNext->getUCN() == SearchListRoot->getUCN() )
			{//NOTE: Implement heuristic to be used!
				if(gfanHeuristic==0)
				{
					gcAct = gcNext;
					rAct=rCopy(gcAct->baseRing);
					rComplete(rAct);
					rChangeCurrRing(rAct);						
					break;
				}
				else if(gfanHeuristic==1)
				{
					//Read st00f from file
					gcAct = gcNext;
					//implant the GB into gcAct
					readConeFromFile(gcNext->getUCN(), gcAct);
					rAct=rCopy(gcAct->baseRing);
					rComplete(rAct);
					rChangeCurrRing(rAct);
					break;
				}				
			}			
 			gcNext = gcNext->next;
//   			if(deleteMarker!=NULL && deleteMarker!=(gcone *)0xfbfbfbfbfbfbfbfb)
			if(this->getUCN()!=1)	//workaround to avoid unpredictable behaviour towards end of program
 				delete deleteMarker;
// 			deleteMarker = NULL;
// 			gcNext = gcNext->next;
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
void gcone::makeInt(dd_MatrixPtr const &M, int const line, intvec &n)
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
void gcone::normalize()
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
	delete n;
				
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
facet * gcone::enqueueNewFacets(facet &f)
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
// 			delete deleteMarker;
// 			deleteMarker=NULL;
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
			/*End of dumping into SLA*/
			while(slAct!=NULL)
					//while(slAct!=slEndStatic->next)
			{
// 				if(deleteMarker!=NULL)
// 				{
// 					delete deleteMarker;
// 					deleteMarker=NULL;
// 				}
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
							/*delete deleteMarker*/;					
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
								slEndStatic->next->prev = slEndStatic->prev;
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
						//delete slAct;
						//slAct=NULL;
						//slAct = slAct->next; //not needed, since facets are equal
// 						delete deleteMarker;
						//deleteMarker=NULL;
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
				slAct = slAct->next;*/						
						//delete deleteMarker;
						//deleteMarker=NULL;
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
int gcone::intgcd(int a, int b)
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
dd_MatrixPtr gcone::facets2Matrix(gcone const &gc)
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
void gcone::writeConeToFile(gcone const &gc, bool usingIntPoints)
{
	int UCN=gc.UCN;
	stringstream ss;
	ss << UCN;
	string UCNstr = ss.str();		
			
 	string prefix="/tmp/cone";
// 	string prefix="./";	//crude hack -> run tests in separate directories
	string suffix=".sg";
	string filename;
	filename.append(prefix);
	filename.append(UCNstr);
	filename.append(suffix);
	
	
// 	int thisPID = getpid();
// 	ss << thisPID;
// 	string strPID = ss.str();
// 	string prefix="./";
						
	ofstream gcOutputFile(filename.c_str());
	facet *fAct;
	fAct = gc.facetPtr;			
	facet *f2Act;
	f2Act=fAct->codim2Ptr;
	
	char *ringString = rString(currRing);
			
	if (!gcOutputFile)
	{
		cout << "Error opening file for writing in writeConeToFile" << endl;
	}
	else
	{	
		gcOutputFile << "UCN" << endl;
		gcOutputFile << this->UCN << endl;
		gcOutputFile << "RING" << endl;	
		gcOutputFile << ringString << endl;
		gcOutputFile << "GCBASISLENGTH" << endl;
		gcOutputFile << IDELEMS(gc.gcBasis) << endl;			
		//Write this->gcBasis into file
		gcOutputFile << "GCBASIS" << endl;				
		for (int ii=0;ii<IDELEMS(gc.gcBasis);ii++)
		{					
			gcOutputFile << p_String((poly)gc.gcBasis->m[ii],gc.baseRing) << endl;
		}				
					
		gcOutputFile << "FACETS" << endl;								
		
		while(fAct!=NULL)
		{	
			intvec *iv;
			intvec *iv2;
			iv=fAct->getFacetNormal();
			f2Act=fAct->codim2Ptr;
			for (int ii=0;ii<iv->length();ii++)
			{
				if (ii<iv->length()-1)
				{
					gcOutputFile << (*iv)[ii] << ",";
				}
				else
				{
					gcOutputFile << (*iv)[ii] << " ";
				}
			}
			while(f2Act!=NULL)
			{
				iv2=f2Act->getFacetNormal();	
				for(int jj=0;jj<iv2->length();jj++)
				{
					if (jj<iv2->length()-1)
					{
						gcOutputFile << (*iv2)[jj] << ",";
					}
					else
					{
						gcOutputFile << (*iv2)[jj] << " ";
					}
				}
				f2Act = f2Act->next;
			}
			gcOutputFile << endl;
			fAct=fAct->next;
		}			
	gcOutputFile.close();
	}
			
}//writeConeToFile(gcone const &gc)
		
/** \brief Reads a cone from a file identified by its number
 */
void gcone::readConeFromFile(int UCN, gcone *gc)
{
	//int UCN=gc.UCN;
	stringstream ss;
	ss << UCN;
	string UCNstr = ss.str();
	string line;
	string strGcBasisLength;
	string strMonom, strCoeff, strCoeffNom, strCoeffDenom;		
	int gcBasisLength=0;
	int intCoeff=1;
	int intCoeffNom=1;		//better (number) but that's not compatible with stringstream;
	int intCoeffDenom=1;
	number nCoeff=nInit(1);
	number nCoeffNom=nInit(1);
	number nCoeffDenom=nInit(1);
	bool hasCoeffInQ = FALSE;	//does the polynomial have rational coeff?
	bool hasNegCoeff = FALSE;	//or a negative one?
	size_t found;			//used for find_first_(not)_of

	string prefix="/tmp/cone";
	string suffix=".sg";
	string filename;
	filename.append(prefix);
	filename.append(UCNstr);
	filename.append(suffix);
					
	ifstream gcInputFile(filename.c_str(), ifstream::in);
	
	ring saveRing=currRing;	
	rChangeCurrRing(gc->baseRing);
	poly strPoly=pInit();
	poly resPoly=pInit();	//The poly to be read in
	
	string::iterator EOL;
	int terms=1;	//#Terms in the poly
	
	while( !gcInputFile.eof() )
	{
		getline(gcInputFile,line);
		hasCoeffInQ = FALSE;
		hasNegCoeff = FALSE;
		
		if(line=="GCBASISLENGTH")
		{
			getline(gcInputFile, line);
			strGcBasisLength = line;
			gcBasisLength=atoi(strGcBasisLength.c_str());
		}
		if(line=="GCBASIS")
		{
			for(int jj=0;jj<gcBasisLength;jj++)
			{
				getline(gcInputFile,line);
				//find out how many terms the poly consists of
				for(EOL=line.begin(); EOL!=line.end();++EOL)
				{
					string s;
					s=*EOL;
					if(s=="+" || s=="-")
						terms++;
				}
				//magically convert strings into polynomials
				//polys.cc:p_Read
				//check until first occurance of + or -
				//data or c_str
				while(!line.empty())
				{
					hasNegCoeff = FALSE;
					found = line.find_first_of("+-");	//get the first monomial
					string tmp;
					tmp=line[found];
// 					if(found!=0 && (tmp.compare("-")==0) )
// 						hasNegCoeff = TRUE;	//We have a coeff < 0
					if(found==0)
					{
						if(tmp.compare("-")==0)
							hasNegCoeff = TRUE;
						line.erase(0,1);	//remove leading + or -
						found = line.find_first_of("+-");	//adjust found
					}
					strMonom = line.substr(0,found);
					line.erase(0,found);
					found = strMonom.find_first_of("/");
					if(found!=string::npos)	//i.e. "/" exists in strMonom
					{
						hasCoeffInQ = TRUE;
						strCoeffNom=strMonom.substr(0,found);						
						strCoeffDenom=strMonom.substr(found+1,strMonom.find_first_not_of("1234567890"));
						strMonom.erase(0,found);
						strMonom.erase(0,strMonom.find_first_not_of("1234567890/"));			
//  						ss << strCoeffNom;
//  						ss >> intCoeffNom;
// 						nCoeffNom=(snumber*)strCoeffNom.c_str();
						nRead(strCoeffNom.c_str(), &nCoeffNom);
						nRead(strCoeffDenom.c_str(), &nCoeffDenom);
// 						nCoeffNom=nInit(intCoeffNom);
//  						ss << strCoeffDenom;
//  						ss >> intCoeffDenom;
// 						nCoeffDenom=nInit(intCoeffDenom);
// 						nCoeffDenom=(snumber*)strCoeffNom.c_str();
						//NOTE NOT SURE WHETHER THIS WILL WORK!
  						//nCoeffNom=nInit(intCoeffNom);
//   						nCoeffDenom=(number)strCoeffDenom.c_str();
// 						nCoeffDenom=(number)strCoeffDenom.c_str();
					}
					else
					{
						found = strMonom.find_first_not_of("1234567890");
						strCoeff = strMonom.substr(0,found);
						if(!strCoeff.empty())
						{
 							nRead(strCoeff.c_str(),&nCoeff);
						}
						else
						{
// 							intCoeff = 1;
							nCoeff = nInit(1);
						}
												
					}
					const char* monom = strMonom.c_str();
						
					p_Read(monom,strPoly,currRing);
					switch (hasCoeffInQ)
					{
						case TRUE:
							if(hasNegCoeff)
								nCoeffNom=nNeg(nCoeffNom);
// 								intCoeffNom *= -1;
// 							pSetCoeff(strPoly, nDiv((number)intCoeffNom, (number)intCoeffDenom));
							pSetCoeff(strPoly, nDiv(nCoeffNom, nCoeffDenom));
							break;
						case FALSE:
							if(hasNegCoeff)
								nCoeff=nNeg(nCoeff);
// 								intCoeff *= -1;
							if(!nIsOne(nCoeff))
							{
// 								if(hasNegCoeff)
// 									intCoeff *= -1;
// 								pSetCoeff(strPoly,(number) intCoeff);
								pSetCoeff(strPoly, nCoeff );
							}
							break;
													
					}
						//pSetCoeff(strPoly, (number) intCoeff);//Why is this set to zero instead of 1???
					if(pIsConstantComp(resPoly))
						resPoly=pCopy(strPoly);							
					else
						resPoly=pAdd(resPoly,strPoly);
					
					
				}//while(!line.empty())		
			
				gcBasis->m[jj]=pCopy(resPoly);
				resPoly=NULL;	//reset
			}
			break;
		}		
	}//while(!gcInputFile.eof())
	
	gcInputFile.close();
	rChangeCurrRing(saveRing);
}
	
// static void gcone::idPrint(ideal &I)
// {
// 	for(int ii=0;ii<IDELEMS(I);ii++)
// 	{
// 		cout << "_[" << ii << "]=" << I->m[ii] << endl;
// 	}
// }
ring gcone::getBaseRing()
{
	return this->baseRing;
}
/** \brief Gather the output
* List of lists
*\param *gc Pointer to gcone, preferably gcRoot ;-)
*\param n the number of cones
*
*/
lists lprepareResult(gcone *gc, int n)
{
	gcone *gcAct;
	gcAct = gc;	
	facet *fAct;
	fAct = gc->facetPtr;
	
	lists res=(lists)omAllocBin(slists_bin);
	res->Init(n);	//initialize to store n cones
	for(int ii=0;ii<n;ii++)
	{
		res->m[ii].rtyp=LIST_CMD;
		lists l=(lists)omAllocBin(slists_bin);
		l->Init(6);
		l->m[0].rtyp=INT_CMD;
		l->m[0].data=(void*)gcAct->getUCN();
		l->m[1].rtyp=IDEAL_CMD;
		l->m[1].data=(void*)idrCopyR_NoSort(gcAct->gcBasis,gcAct->getBaseRing());
		
		l->m[2].rtyp=INTVEC_CMD;
		intvec iv=(gcAct->f2M(gcAct,gcAct->facetPtr));
		l->m[2].data=(void*)ivCopy(&iv);
		
		l->m[3].rtyp=LIST_CMD;
			lists lCodim2List = (lists)omAllocBin(slists_bin);
			lCodim2List->Init(gcAct->numFacets);		
 
			fAct = gcAct->facetPtr;//fAct->codim2Ptr;
			int jj=0;
			while(fAct!=NULL && jj<gcAct->numFacets)
			{
				lCodim2List->m[jj].rtyp=INTVEC_CMD;
				intvec ivC2=(gcAct->f2M(gcAct,fAct,2));
				lCodim2List->m[jj].data=(void*)ivCopy(&ivC2);
				jj++;
				fAct = fAct->next;
			} 		
		l->m[3].data=(void*)lCodim2List;
		
		
		l->m[4].rtyp=RING_CMD;
		l->m[4].data=(void*)(gcAct->getBaseRing()); 		
		l->m[5].rtyp=INT_CMD;
		l->m[5].data=(void*)gcAct->getPredUCN();
		res->m[ii].data=(void*)l;
		gcAct = gcAct->next;
	}		
	
	return res;
}
/** \brief Write facets of a cone into a matrix
* Takes a pointer to a facet as 2nd arg 
* f should always point to gc->facetPtr
* param n is used to determine whether it operates in codim 1 or 2
*
*/
intvec gcone::f2M(gcone *gc, facet *f, int n)
{
	facet *fAct;
	intvec *res;
	intvec *fNormal = new intvec(gc->numVars);
// 	int codim=n;
// 	int bound;
// 	if(f==gc->facetPtr)
	if(n==1)
	{
		intvec *m1Res=new intvec(gc->numFacets,gc->numVars,0);
		res = m1Res;
		fAct = gc->facetPtr;
// 		bound = gc->numFacets*(this->numVars);		
	}
	else
	{
		fAct = f->codim2Ptr;
		intvec *m2Res = new intvec(f->numCodim2Facets,gc->numVars,0);
		res = m2Res;		
// 		bound = fAct->numCodim2Facets*(this->numVars);

	}
	int ii=0;
	while(fAct!=NULL )//&& ii < bound )
	{
		fNormal = fAct->getFacetNormal();
		for(int jj=0;jj<this->numVars;jj++)
		{
			(*res)[ii]=(*fNormal)[jj];
			ii++;
		}
		fAct = fAct->next;
	}

	delete fNormal;
	return *res;
}

int gcone::counter=0;
int gfanHeuristic;
// ideal gfan(ideal inputIdeal, int h)
lists gfan(ideal inputIdeal, int h)
{
	lists lResList; //this is the object we return
	
	if(rHasGlobalOrdering(currRing))
	{	
	int numvar = pVariables; 
	gfanHeuristic = h;
	
	enum searchMethod {
		reverseSearch,
		noRevS
	};
	
	searchMethod method;
	method = noRevS;

	
#ifdef gfan_DEBUG
	cout << "Now in subroutine gfan" << endl;
#endif
	ring inputRing=currRing;	// The ring the user entered
	ring rootRing;			// The ring associated to the target ordering
	ideal res;	
	facet *fRoot;	
	
	if(method==noRevS)
	{
		gcone *gcRoot = new gcone(currRing,inputIdeal);
		gcone *gcAct;
		gcAct = gcRoot;
		gcAct->numVars=pVariables;
		gcAct->getGB(inputIdeal);		
#ifndef NDEBUG
		cout << "GB of input ideal is:" << endl;
		idShow(gcAct->gcBasis);
#endif
		if(gcAct->isMonomial(gcAct->gcBasis))
		{
			WerrorS("Monomial input - terminating");
			res=gcAct->gcBasis;
			//break;
		}
		cout << endl;
		gcAct->getConeNormals(gcAct->gcBasis);		
		gcAct->noRevS(*gcAct);		
		//res=gcAct->gcBasis;
		//Below is a workaround, since gcAct->gcBasis gets deleted in noRevS
		res = inputIdeal; 
		lResList=lprepareResult(gcRoot,gcRoot->getCounter());
// 		res=lResList;
	}
	dd_free_global_constants();
	}//rHasGlobalOrdering
	else
	{
		WerrorS("Ring has non-global ordering - terminating");
		lResList->Init(1);
		lResList->m[0].rtyp=INT_CMD;
		int ires=0;
		lResList->m[0].data=(void*)&ires;
	}
	
	return lResList;
}

#endif
