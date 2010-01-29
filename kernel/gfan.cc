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
#include "kutil.h"	//ksCreateSpoly
// #include "intvec.h"
#include "polys.h"
#include "ideals.h"
#include "kmatrix.h"
#include "GMPrat.h"
//#include "fast_maps.h"	//Mapping of ideals
// #include "maps.h"
// #include "ring.h"	//apparently not needed
// #include "structs.h"
#include "../Singular/lists.h"
#include "prCopy.h"	//Needed for idrCopyR
#include <iostream>
// #include <bitset>
#include <fstream>	//read-write cones to files
// #include <gmp.h>
#include <string>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
//#include <gmpxx.h>
// #include <vector>


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
// #define gfan_DEBUG
#ifndef gfan_DEBUGLEVEL
#define gfan_DEBUGLEVEL 1
#endif
#endif
#define gfanp
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
	this->numRays=0;
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
	this->numRays=0;
	this->flipGB=NULL;
	this->isIncoming=FALSE;
	this->next=NULL;
	this->prev=NULL;
	this->flipRing=NULL;
	this->isFlippable=FALSE;
}
		
/** \brief The copy constructor
* By default only copies the fNormal, f2Normals and UCN
*/
facet::facet(const facet& f)
{
	this->fNormal=ivCopy(f.fNormal);
	this->UCN=f.UCN;
	this->isFlippable=f.isFlippable;
	facet* f2Copy;
	f2Copy=f.codim2Ptr;
	facet* f2Act;
	f2Act=this->codim2Ptr;
	while(f2Copy!=NULL)
	{
		if(f2Act==NULL)
		{
			f2Act=new facet(2);
			this->codim2Ptr=f2Act;			
		}
		else
		{
			facet* marker;
			marker = f2Act;
			f2Act->next = new facet(2);
			f2Act = f2Act->next;
			f2Act->prev = marker;
		}
		intvec *f2Normal;
		f2Normal = f2Copy->getFacetNormal();
// 		f2Act->setFacetNormal(f2Copy->getFacetNormal());
		f2Act->setFacetNormal(f2Normal);
		delete f2Normal;
		f2Act->setUCN(f2Copy->getUCN());
		f2Copy = f2Copy->next;
	}	
}
		
/** The default destructor */
facet::~facet()
{
	if(this->fNormal!=NULL)
		delete this->fNormal;
	if(this->interiorPoint!=NULL)
		delete this->interiorPoint;
	/* Cleanup the codim2-structure */
	if(this->codim==2)
	{
		facet *codim2Ptr;
		codim2Ptr = this->codim2Ptr;
		while(codim2Ptr!=NULL)
		{
			if(codim2Ptr->fNormal!=NULL)
			{
				delete codim2Ptr->fNormal;
				codim2Ptr = codim2Ptr->next;
			}
		}
	}
	if(this->codim2Ptr!=NULL)
		delete this->codim2Ptr;
	if(this->flipGB!=NULL)
		idDelete((ideal *)&this->flipGB);
	if(this->flipRing!=NULL && this->flipRing->idroot!=(idhdl)0xfbfbfbfbfbfbfbfb)
// 		rDelete(this->flipRing);
// 	this->flipRing=NULL;
	this->prev=NULL;
	this->next=NULL;
	//this=NULL;
}
		
		
/** \brief Comparison of facets*/
inline bool gcone::areEqual(facet *f, facet *s)
{
#ifdef gfanp
	gcone::numberOfFacetChecks++;
#endif
	bool res = TRUE;
	int notParallelCtr=0;
	int ctr=0;
	intvec* fNormal; //No new since ivCopy and therefore getFacetNormal return a new
	intvec* sNormal;
	fNormal = f->getFacetNormal();
	sNormal = s->getFacetNormal();
	//Do not need parallelity. Too time consuming
	if(!isParallel(fNormal,sNormal))
		notParallelCtr++;
// 	intvec *foo=ivNeg(sNormal);
// 	if(fNormal->compare(foo)!=0) //facet normals
// 		return TRUE;	
	else//parallelity, so we check the codim2-facets
	{
		facet* f2Act;
		facet* s2Act;
		f2Act = f->codim2Ptr;		
		ctr=0;
		while(f2Act!=NULL)
		{
			intvec* f2Normal;
			f2Normal = f2Act->getFacetNormal();
			s2Act = s->codim2Ptr;
			while(s2Act!=NULL)
			{
				intvec* s2Normal;
				s2Normal = s2Act->getFacetNormal();
// 				bool foo=areEqual(f2Normal,s2Normal);
				int foo=f2Normal->compare(s2Normal);
				if(foo==0)
					ctr++;
				s2Act = s2Act->next;
				delete s2Normal;
			}
			delete f2Normal;
			f2Act = f2Act->next;
		}		
	}
	delete fNormal;
	delete sNormal;
	if(ctr==f->numCodim2Facets)
		res=TRUE;
	else
	{
#ifdef gfanp
		gcone::parallelButNotEqual++;
#endif
		res=FALSE;
	}
	return res;
}	
		
/** Stores the facet normal \param intvec*/
inline void facet::setFacetNormal(intvec *iv)
{
	this->fNormal = ivCopy(iv);			
}
		
/** Hopefully returns the facet normal 
* Mind: ivCopy returns a new intvec, so use this in the following way:
* intvec *iv;
* iv = this->getFacetNormal();
* [...]
* delete(iv);
*/
inline intvec *facet::getFacetNormal()
{				
	return ivCopy(this->fNormal);
// 	return this->fNormal;
}
		
/** Method to print the facet normal*/
inline void facet::printNormal()
{
	fNormal->show();
}
		
/** Store the flipped GB*/
inline void facet::setFlipGB(ideal I)
{
	this->flipGB=idCopy(I);
}
		
/** Returns a pointer to the flipped GB
Seems not be used
Anyhow idCopy would make sense here.
*/
inline ideal facet::getFlipGB()
{
	return this->flipGB;
}
		
/** Print the flipped GB*/
inline void facet::printFlipGB()
{
#ifndef NDEBUG
	idShow(this->flipGB);
#endif
}
		
/** Set the UCN */
inline void facet::setUCN(int n)
{
	this->UCN=n;
}
		
/** \brief Get the UCN 
* Returns the UCN iff this != NULL, else -1
*/
inline int facet::getUCN()
{
	if(this!=NULL)//  && this!=(facet *)0xfbfbfbfbfbfbfbfb)// || this!=(facet *)0xfbfbfbfb) )
// 	if(this!=NULL && ( this->fNormal!=(intvec *)0xfbfbfbfb || this->fNormal!=(intvec *)0xfbfbfbfbfbfbfbfb) )
		return this->UCN;
	else
		return -1;
}
		
/** Store an interior point of the facet */
inline void facet::setInteriorPoint(intvec *iv)
{
	this->interiorPoint = ivCopy(iv);
}
		
/** Returns a pointer to this->interiorPoint
* MIND: ivCopy returns a new intvec
* @see facet::getFacetNormal
*/
inline intvec *facet::getInteriorPoint()
{
	return ivCopy(this->interiorPoint);
}	
		
/** \brief Debugging function
* prints the facet normal an all (codim-2)-facets that belong to it
*/
inline void facet::fDebugPrint()
{			
	facet *codim2Act;			
	codim2Act = this->codim2Ptr;
	intvec *fNormal;
	fNormal = this->getFacetNormal();	
	cout << "=======================" << endl;
	cout << "Facet normal = (";
	fNormal->show(1,1);
	cout << ")"<<endl;	
	cout << "-----------------------" << endl;
	cout << "Codim2 facets:" << endl;
	while(codim2Act!=NULL)
	{
		intvec *f2Normal;
		f2Normal = codim2Act->getFacetNormal();
		cout << "(";
		f2Normal->show(1,0);
		cout << ")" << endl;
		codim2Act = codim2Act->next;
		delete f2Normal;
	}
	cout << "=======================" << endl;
 	delete fNormal;	
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
//     	this->gcBasis=idCopy(f.flipGB);
//     	ring saveRing=currRing;
// 	ring tmpRing=f.flipRing;
// 	rComplete(f.flipRing);
//     	rChangeCurrRing(f.flipRing);
   	this->gcBasis=idrCopyR(f.flipGB, f.flipRing);
//  	this->inputIdeal=idCopy(this->gcBasis);
	this->baseRing=rCopy(f.flipRing);
// 	rComplete(this->baseRing);
//     	rChangeCurrRing(saveRing);
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
// 	if(this->inputIdeal!=NULL)
// 		idDelete((ideal *)&this->inputIdeal);
// 	if (this->rootRing!=NULL && this->rootRing!=(ip_sring *)0xfefefefefefefefe)
// 		rDelete(this->rootRing);
// 	if(this->UCN!=1)
// 		rDelete(this->baseRing);
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
	this->counter--;
	//should be deleted in noRevS
// 	dd_FreeMatrix(this->ddFacets);
	//dd_FreeMatrix(this->ddFacets);
}			
	
inline int gcone::getCounter()
{
	return this->counter;
}
	
/** \brief Set the interior point of a cone */
inline void gcone::setIntPoint(intvec *iv)
{
	this->ivIntPt=ivCopy(iv);
}
		
/** \brief Return the interior point */
inline intvec *gcone::getIntPoint()
{
	return ivCopy(this->ivIntPt);
}
		
/** \brief Print the interior point */
inline void gcone::showIntPoint()
{
	ivIntPt->show();
}
		
/** \brief Print facets
 * This is mainly for debugging purposes. Usually called from within gdb
 */
volatile void gcone::showFacets(const short codim)
{
	facet *f=this->facetPtr;
	facet *f2=NULL;
	if(codim==2)
		f2=this->facetPtr->codim2Ptr;
	/*switch(codim)
	{
		case 1:
			f = this->facetPtr;
			break;
		case 2:
			f2 = this->facetPtr->codim2Ptr;
			break;
	}*/	
	while(f!=NULL)
	{
		intvec *iv;
		iv = f->getFacetNormal();
		cout << "(";
		iv->show(1,0);				
		if(f->isFlippable==FALSE)
			cout << ")* ";
		else
			cout << ") ";
		delete iv;
		if(codim==2)
		{
			f2=f->codim2Ptr;
			while(f2!=NULL)
			{
				cout << "[";
				f2->getFacetNormal()->show(1,0);
				cout << "]";
				f2 = f2->next;
			}
			cout << endl;
		}
		f=f->next;
	}
	cout << endl;
}
		
/** For debugging purposes only */
inline volatile void gcone::showSLA(facet &f)
{
	facet *fAct;
	fAct = &f;
	facet *codim2Act;
	codim2Act = fAct->codim2Ptr;
	
	cout << endl;
	while(fAct!=NULL)
	{
		intvec *fNormal;		
		fNormal=fAct->getFacetNormal();
		cout << "(";
		fNormal->show(1,0);
		if(fAct->isFlippable==TRUE)
			cout << ") ";
		else
			cout << ")* ";
		delete fNormal;
		codim2Act = fAct->codim2Ptr;
		cout << " Codim2: ";
		while(codim2Act!=NULL)
		{
			intvec *f2Normal;
			f2Normal = codim2Act->getFacetNormal();
			cout << "(";
			f2Normal->show(1,0);
			cout << ") ";
			delete f2Normal;
			codim2Act = codim2Act->next;
		}
		cout << "UCN = " << fAct->getUCN() << endl;				
		fAct = fAct->next;
	}
}
		
inline void gcone::idDebugPrint(const ideal &I)
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

inline void gcone::invPrint(const ideal &I)
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

inline bool gcone::isMonomial(const ideal &I)
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
inline void gcone::setNumFacets()
{
}
		
/** \brief Get gcone::numFacets */
inline int gcone::getNumFacets()
{
	return this->numFacets;
}
		
inline int gcone::getUCN()
{
	if( this!=NULL)// && ( this!=(gcone * const)0xfbfbfbfbfbfbfbfb && this!=(gcone * const)0xfbfbfbfb ) )
		return this->UCN;
	else
		return -1;
}

inline int gcone::getPredUCN()
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
inline void gcone::getConeNormals(const ideal &I, bool compIntPoint)
{
#ifdef gfanp
	timeval start, end;
	gettimeofday(&start, 0);
#endif
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
// 		int *tailexpv=(int*)omAlloc(((this->numVars)+1)*sizeof(int));
		pGetExpV(aktpoly,leadexpv);
		while(pNext(aktpoly)!=NULL)
		{
			aktpoly=pNext(aktpoly);
			int *tailexpv=(int*)omAlloc(((this->numVars)+1)*sizeof(int));
			pGetExpV(aktpoly,tailexpv);			
			for(int kk=1;kk<=this->numVars;kk++)
			{				
				dd_set_si(ddineq->matrix[(dd_rowrange)aktmatrixrow][kk],leadexpv[kk]-tailexpv[kk]);
			}
			aktmatrixrow += 1;
			omFree(tailexpv);
		}		
		omFree(leadexpv);	
	} //for
#if true
	/*Let's make the preprocessing here. This could already be done in the above for-loop,
	* but for a start it is more convenient here.
	* We check the necessary condition of FJT p.18
	* Quote: [...] every non-zero spoly should have at least one of its terms in inv(G)
	*/
// 	ideal initialForm=idInit(IDELEMS(I),1);
// 	intvec *gamma=new intvec(this->numVars);
	int falseGammaCounter=0;
	int *redRowsArray=NULL;
	int num_alloc=0;
	int num_elts=0;	
	for(int ii=0;ii<ddineq->rowsize;ii++)
	{
		ideal initialForm=idInit(IDELEMS(I),1);
		//read row ii into gamma
		double tmp;
		intvec *gamma=new intvec(this->numVars);
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
					//TODO: Suffices to check LTs here
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
					pDelete(&p);
					pDelete(&q);
					if(isMaybeFacet==FALSE)
					{
						dd_set_si(ddineq->matrix[ii][0],1);						
						if(num_alloc==0)
							num_alloc += 1;
						else
							num_alloc += 1;
						void *tmp = realloc(redRowsArray,(num_alloc*sizeof(int)));
						if(!tmp)
						{
							WerrorS("Woah dude! Couldn't realloc memory\n");
							exit(-1);
						}
						redRowsArray = (int*)tmp;
						redRowsArray[num_elts]=ii;
						num_elts++;
						//break;//for(int kk, since we have found one that is not in L	
						goto _start;	//mea culpa, mea culpa, mea maxima culpa
					}
				}//if(P->p!=NULL)				
			}//for k
		}//for jj
		_start:;
		idDelete(&L);
		delete P;
		idDelete(&initialForm);
		//idDelete(L);
		delete gamma;
	}//for(ii<ddineq-rowsize
// 	delete gamma;
	int offset=0;//needed for correction of redRowsArray[ii]
	for( int ii=0;ii<num_elts;ii++ )
	{
		dd_MatrixRowRemove(&ddineq,redRowsArray[ii]+1-offset);//cddlib sucks at enumeration
		offset++;
	}
	free(redRowsArray);
	/*And now for the strictly positive rows
	* Doesn't gain significant speedup
	*/
	/*int *posRowsArray=NULL;
	num_alloc=0;
	num_elts=0;
	for(int ii=0;ii<ddineq->rowsize;ii++)
	{
		intvec *ivPos = new intvec(this->numVars);
		for(int jj=0;jj<this->numVars;jj++)
			(*ivPos)[jj]=(int)mpq_get_d(ddineq->matrix[ii][jj+1]);
		bool isStrictlyPos=FALSE;
		int posCtr=0;		
		for(int jj=0;jj<this->numVars;jj++)
		{
			intvec *ivCanonical = new intvec(this->numVars);
			jj==0 ? (*ivCanonical)[ivPos->length()-1]=1 : (*ivCanonical)[jj-1]=1;
			if(dotProduct(*ivCanonical,*ivPos)!=0)
			{
				if ((*ivPos)[jj]>=0)
				{				
					posCtr++;				
				}
			}			
			delete ivCanonical;
		}
		if(posCtr==ivPos->length())
			isStrictlyPos=TRUE;
		if(isStrictlyPos==TRUE)
		{
			if(num_alloc==0)
				num_alloc += 1;
			else
				num_alloc += 1;
			void *tmp = realloc(posRowsArray,(num_alloc*sizeof(int)));
			if(!tmp)
			{
				WerrorS("Woah dude! Couldn't realloc memory\n");
				exit(-1);
			}
			posRowsArray = (int*)tmp;
			posRowsArray[num_elts]=ii;
			num_elts++;	
		}
		delete ivPos;
	}
	offset=0;
	for(int ii=0;ii<num_elts;ii++)
	{
		dd_MatrixRowRemove(&ddineq,posRowsArray[ii]+1-offset);
		offset++;
	}
	free(posRowsArray);*/
#endif

	dd_MatrixCanonicalize(&ddineq, &ddlinset, &ddredrows, &ddnewpos, &dderr);	
	ddrows = ddineq->rowsize;	//Size of the matrix with redundancies removed
	ddcols = ddineq->colsize;
	
	this->ddFacets = dd_CopyMatrix(ddineq);
			
	/*Write the normals into class facet*/	
	facet *fAct; 	//pointer to active facet	
	int numNonFlip=0;
	for (int kk = 0; kk<ddrows; kk++)
	{
		intvec *load = new intvec(this->numVars);//intvec to store a single facet normal that will then be stored via setFacetNormal
		for (int jj = 1; jj <ddcols; jj++)
		{
			double foo;
			foo = mpq_get_d(ddineq->matrix[kk][jj]);
			(*load)[jj-1] = (int)foo;	//store typecasted entry at pos jj-1 of load		
		}//for (int jj = 1; jj <ddcols; jj++)
				
		/*Quick'n'dirty hack for flippability. Executed only if gcone::hasHomInput==FALSE
		* Otherwise every facet intersects the positive orthant
		*/	
		if(gcone::hasHomInput==FALSE)
		{
			//TODO: No dP needed
			bool isFlip=FALSE;
			for(int jj = 0; jj<load->length(); jj++)
			{
				intvec *ivCanonical = new intvec(load->length());
				(*ivCanonical)[jj]=1;
				if (dotProduct(*load,*ivCanonical)<0)	
				{
					isFlip=TRUE;
					break;	//URGHS
				}
				delete ivCanonical;
			}/*End of check for flippability*/
			if(isFlip==FALSE)
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
			{
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
			}
		}//hasHomInput==FALSE
		else	//Every facet is flippable
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
// 		exit(-1);//Bit harsh maybe...
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
		dd_FreeMatrix(posRestr);
	}	
	//Clean up but don't delete the return value!	
	dd_FreeMatrix(ddineq);
	set_free(ddredrows);
	set_free(ddlinset);
	free(ddnewpos);
#ifdef gfanp
	gettimeofday(&end, 0);
	time_getConeNormals += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif

}//gcone::getConeNormals(ideal I)
		
/** \brief Compute the (codim-2)-facets of a given cone
 * This method is used during noRevS
 * Additionally we check whether the codim2-facet normal is strictly positive. Otherwise
 * the facet is marked as non-flippable.
 */
inline void gcone::getCodim2Normals(const gcone &gc)
{
#ifdef gfanp
	timeval start, end;
  	gettimeofday(&start, 0);
#endif
	//this->facetPtr->codim2Ptr = new facet(2);	//instantiate a (codim-2)-facet
	facet *fAct;
	fAct = this->facetPtr;		
	facet *codim2Act;
	//codim2Act = this->facetPtr->codim2Ptr;
	dd_MatrixPtr ddineq;//,P,ddakt;
	dd_ErrorType err;
	//ddineq = facets2Matrix(gc);	//get a matrix representation of the cone
	ddineq = dd_CopyMatrix(gc.ddFacets);
	/*Now set appropriate linearity*/
	for (int ii=0; ii<this->numFacets; ii++)			
	{	
		dd_rowset impl_linset, redset;
		dd_rowindex newpos;
		dd_MatrixPtr ddakt;
  		ddakt = dd_CopyMatrix(ddineq);
// 		ddakt->representation=dd_Inequality;	//Not using this makes it faster. But why does the quick check below still work?
// 		ddakt->representation=dd_Generator;
		set_addelem(ddakt->linset,ii+1);/*Now set appropriate linearity*/
#ifdef gfanp
		timeval t_ddMC_start, t_ddMC_end;
		gettimeofday(&t_ddMC_start,0);
#endif				
 		//dd_MatrixCanonicalize(&ddakt, &impl_linset, &redset, &newpos, &err);
		dd_PolyhedraPtr ddpolyh;
		ddpolyh=dd_DDMatrix2Poly(ddakt, &err);
// 		ddpolyh=dd_DDMatrix2Poly2(ddakt, dd_MaxCutoff, &err);
		dd_MatrixPtr P;
		P=dd_CopyGenerators(ddpolyh);		
		dd_FreePolyhedra(ddpolyh);
		//TODO Call for one cone , normalize - check equalities - plus lineality -done
#ifdef gfanp
		gettimeofday(&t_ddMC_end,0);
		t_ddMC += (t_ddMC_end.tv_sec - t_ddMC_start.tv_sec + 1e-6*(t_ddMC_end.tv_usec - t_ddMC_start.tv_usec));
#endif	
		/* We loop through each row of P normalize it by making all
		* entries integer ones and add the resulting vector to the
		* int matrix facet::codim2Facets */
		for (int jj=1;jj<=/*ddakt*/P->rowsize;jj++)
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
#ifdef gfanp
			timeval t_mI_start, t_mI_end;
			gettimeofday(&t_mI_start,0);
#endif
			makeInt(P,jj,*n);
			/*for(int kk=0;kk<this->numVars;kk++)
			{
				int foo;
				foo = (int)mpq_get_d(ddakt->matrix[ii][kk+1]);
				(*n)[kk]=foo;
			}*/
#ifdef gfanp
			gettimeofday(&t_mI_end,0);
			t_mI += (t_mI_end.tv_sec - t_mI_start.tv_sec + 1e-6*(t_mI_end.tv_usec - t_mI_start.tv_usec));
#endif
			codim2Act->setFacetNormal(n);
			delete n;					
		}		
		/*We check whether the facet spanned by the codim-2 facets
		* intersects with the positive orthant. Otherwise we define this
		* facet to be non-flippable. Works since we set the appropriate 
		* linearity for ddakt above.
		*/
		//TODO It might be faster to compute jus the implied equations instead of a relative interior point
// 		intvec *iv_intPoint = new intvec(this->numVars);
// 		dd_MatrixPtr shiftMatrix;
// 		dd_MatrixPtr intPointMatrix;
// 		shiftMatrix = dd_CreateMatrix(this->numVars,this->numVars+1);
// 		for(int kk=0;kk<this->numVars;kk++)
// 		{
// 			dd_set_si(shiftMatrix->matrix[kk][0],1);
// 			dd_set_si(shiftMatrix->matrix[kk][kk+1],1);
// 		}
// 		intPointMatrix=dd_MatrixAppend(ddakt,shiftMatrix);
// #ifdef gfanp
// 		timeval t_iP_start, t_iP_end;
// 		gettimeofday(&t_iP_start, 0);
// #endif		
// 		interiorPoint(intPointMatrix,*iv_intPoint);
// // 		dd_rowset impl_linste,lbasis;
// // 		dd_LPSolutionPtr lps=NULL;
// // 		dd_ErrorType err;
// // 		dd_FindRelativeInterior(intPointMatrix, &impl_linset, &lbasis, &lps, &err);
// #ifdef gfanp
// 		gettimeofday(&t_iP_end, 0);
// 		t_iP += (t_iP_end.tv_sec - t_iP_start.tv_sec + 1e-6*(t_iP_end.tv_usec - t_iP_start.tv_usec));
// #endif
// 		for(int ll=0;ll<this->numVars;ll++)
// 		{
// 			if( (*iv_intPoint)[ll] < 0 )
// 			{
// 				fAct->isFlippable=FALSE;
// 				break;
// 			}
// 		}
		/*End of check*/
		/*This test should be way less time consuming*/
#ifdef gfanp
		timeval t_iP_start, t_iP_end;
		gettimeofday(&t_iP_start, 0);
#endif
		bool containsStrictlyPosRay=TRUE;
		for(int ii=0;ii<ddakt->rowsize;ii++)
		{
			containsStrictlyPosRay=TRUE;
			for(int jj=1;jj<this->numVars;jj++)
			{
				if(ddakt->matrix[ii][jj]<=0)
				{
					containsStrictlyPosRay=FALSE;
					break;
				}
			}
			if(containsStrictlyPosRay==TRUE)
				break;
		}
		if(containsStrictlyPosRay==FALSE)
		//TODO Not sufficient. Intersect with pos orthant for pos int
			fAct->isFlippable=FALSE;
#ifdef gfanp
		gettimeofday(&t_iP_end, 0);
		t_iP += (t_iP_end.tv_sec - t_iP_start.tv_sec + 1e-6*(t_iP_end.tv_usec - t_iP_start.tv_usec));
#endif
		/**/
		fAct = fAct->next;	
		dd_FreeMatrix(ddakt);
		dd_FreeMatrix(P);
	}//for 	
	dd_FreeMatrix(ddineq);
#ifdef gfanp
	gettimeofday(&end, 0);
	time_getCodim2Normals += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif
}
		
/** Really extremal rays this time ;)
* Extremal rays are unique modulo the homogeneity space.
* Therefore we dd_MatrixAppend gc->ddFacets and gcone::dd_LinealitySpace
* into ddineq. Next we compute the extremal rays of the so given subspace.
* Figuring out whether a ray belongs to a given facet(normal) is done by
* checking whether the inner product of the ray with the normal is zero.
*/
void gcone::getExtremalRays(const gcone &gc)
{
#ifdef gfanp
	timeval start, end;
	gettimeofday(&start, 0);
#endif
	//Add lineality space - dd_LinealitySpace
	dd_MatrixPtr ddineq;
	dd_ErrorType err;
	ddineq = dd_AppendMatrix(gc.ddFacets,gcone::dd_LinealitySpace);
	dd_PolyhedraPtr ddPolyh;
	ddPolyh = dd_DDMatrix2Poly(ddineq, &err);
	dd_MatrixPtr P;
	P=dd_CopyGenerators(ddPolyh);
	dd_FreePolyhedra(ddPolyh);	
	//Loop through the rows of P and check whether fNormal*row[i]=0 => row[i] belongs to fNormal
	int rows=P->rowsize;
	facet *fAct=gc.facetPtr;
	while(fAct!=NULL)
	{
		intvec *fNormal;// = new intvec(this->numVars);
		fNormal = fAct->getFacetNormal();
		for(int ii=0;ii<rows;ii++)
		{
			intvec *rowvec = new intvec(this->numVars);
			makeInt(P,ii+1,*rowvec);//get an integer entry instead of rational
			if(dotProduct(*fNormal,*rowvec)==0)
			{
				fAct->numCodim2Facets++;
				facet *codim2Act;
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
				codim2Act->setFacetNormal(rowvec);
				fAct->numRays++;			
			}
			delete(rowvec);
		}
		//Now (if we have at least 3 variables) do a bubblesort on the rays
		/*if(this->numVars>2)
		{
			facet *A[fAct->numRays-1];
			facet *f2Act=fAct->codim2Ptr;
			for(unsigned ii=0;ii<fAct->numRays;ii++)
			{
				A[ii]=f2Act;
				f2Act=f2Act->next;
			}
			bool exchanged=FALSE;
			unsigned n=fAct->numRays-1;
			do
			{
				exchanged=FALSE;//n=fAct->numRays-1;				
				for(unsigned ii=0;ii<=n-1;ii++)
				{					
					if((A[ii]->fNormal)->compare((A[ii+1]->fNormal))==1)
					{
						//Swap rays
						cout << "Swapping ";
						A[ii]->fNormal->show(1,0); cout << " with "; A[ii+1]->fNormal->show(1,0); cout << endl;
						A[ii]->next=A[ii+1]->next;
						if(ii>0)
							A[ii-1]->next=A[ii+1];
						A[ii+1]->next=A[ii];
						if(ii==0)
							fAct->codim2Ptr=A[ii+1];
						//end swap
						facet *tmp=A[ii];//swap in list
						A[ii+1]=A[ii];
						A[ii]=tmp;
// 						tmp=NULL;			
					}					
				}
				n--;			
			}while(exchanged==TRUE && n>=0);
		}*///if pVariables>2
		delete fNormal;		
		fAct = fAct->next;
	}
	//Now all extremal rays should be set w.r.t their respective fNormal
	//TODO Not sufficient -> vol2 II/125&127
	//NOTE Sufficient according to cddlibs doc. These ARE rays
	fAct=gc.facetPtr;
	while(fAct!=NULL)
	{
		bool containsStrictlyPosRay=TRUE;
		facet *codim2Act;
		codim2Act = fAct->codim2Ptr;
		while(codim2Act!=NULL)
		{
			containsStrictlyPosRay==TRUE;
			intvec *rayvec;
			rayvec = codim2Act->getFacetNormal();//Mind this is no normal but a ray!
			int negCtr=0;
			for(int ii=0;ii<rayvec->length();ii++)
			{
				if((*rayvec)[ii]<=0)
				{
					containsStrictlyPosRay==FALSE;
					break;
				}
			}
			if(containsStrictlyPosRay==TRUE)
			{
				delete(rayvec);
				break;
			}
			delete(rayvec);
			codim2Act = codim2Act->next;
		}
		if(containsStrictlyPosRay==FALSE)
			fAct->isFlippable=FALSE;
		fAct = fAct->next;
	}
	
	
#ifdef gfanp
	gettimeofday(&end, 0);
	t_getExtremalRays += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif	
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
inline void gcone::flip(ideal gb, facet *f)		//Compute "the other side"
{	
#ifdef gfanp
	timeval start, end;
	gettimeofday(&start, 0);
#endif		
	intvec *fNormal;// = new intvec(this->numVars);	//facet normal, check for parallelity			
	fNormal = f->getFacetNormal();	//read this->fNormal;

// 	std::cout << "running gcone::flip" << std::endl;
// 	std::cout << "flipping UCN " << this->getUCN() << endl;
// 	cout << "over facet (";
// 	fNormal->show(1,0);
// 	cout << ") with UCN " << f->getUCN();
// 	std::cout << std::endl;
	if(this->getUCN() != f->getUCN())
	{
		WerrorS("Uh oh... Trying to flip over facet with incompatible UCN");
		exit(-1);
	}
	/*1st step: Compute the initial ideal*/
	/*poly initialFormElement[IDELEMS(gb)];*/	//array of #polys in GB to store initial form
	ideal initialForm=idInit(IDELEMS(gb),this->gcBasis->rank);
	
	computeInv(gb,initialForm,*fNormal);

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
		intvec *iv = new intvec(this->numVars);
		iv = ivNeg(fNormal);
// 		tmpRing=rCopyAndAddWeight(srcRing,ivNeg(fNormal));
		tmpRing=rCopyAndAddWeight(srcRing,iv);
		delete iv;
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
		//omFree(A);
	}
  	delete fNormal; 
	rChangeCurrRing(tmpRing);	
			
	ideal ina;			
	ina=idrCopyR(initialForm,srcRing);
	idDelete(&initialForm);
	ideal H;
//  	H=kStd(ina,NULL,isHomog,NULL);	//we know it is homogeneous
#ifdef gfanp
	timeval t_kStd_start, t_kStd_end;
	gettimeofday(&t_kStd_start,0);
#endif
	H=kStd(ina,NULL,testHomog,NULL);	//This is \mathcal(G)_{>_-\alpha}(in_v(I))
#ifdef gfanp
	gettimeofday(&t_kStd_end, 0);
	t_kStd += (t_kStd_end.tv_sec - t_kStd_start.tv_sec + 1e-6*(t_kStd_end.tv_usec - t_kStd_start.tv_usec));
#endif
	idSkipZeroes(H);
	idDelete(&ina);

	/*Substep 2.2
	do the lifting and mark according to H
	*/
	rChangeCurrRing(srcRing);
	ideal srcRing_H;
	ideal srcRing_HH;			
	srcRing_H=idrCopyR(H,tmpRing);
	//H is needed further below, so don't idDelete here
 	srcRing_HH=ffG(srcRing_H,this->gcBasis);
 	idDelete(&srcRing_H);
		
	/*Substep 2.2.1
	 * Mark according to G_-\alpha
	 * Here we have a minimal basis srcRing_HH. In order to turn this basis into a reduced basis
	 * we have to compute an interior point of C(srcRing_HH). For this we need to know the cone
	 * represented by srcRing_HH MARKED ACCORDING TO G_{-\alpha}
	 * Thus we check whether the leading monomials of srcRing_HH and srcRing_H coincide. If not we 
	 * compute the difference accordingly
	*/
#ifdef gfanp
	timeval t_markings_start, t_markings_end;
	gettimeofday(&t_markings_start, 0);
#endif		
	bool markingsAreCorrect=FALSE;
	dd_MatrixPtr intPointMatrix;
	int iPMatrixRows=0;
	dd_rowrange aktrow=0;			
	for (int ii=0;ii<IDELEMS(srcRing_HH);ii++)
	{
		poly aktpoly=(poly)srcRing_HH->m[ii];//This is a pointer, so don't pDelete
		iPMatrixRows = iPMatrixRows+pLength(aktpoly);		
	}
	/* additionally one row for the standard-simplex and another for a row that becomes 0 during
	 * construction of the differences
	 */
	intPointMatrix = dd_CreateMatrix(iPMatrixRows+2,this->numVars+1); 
	intPointMatrix->numbtype=dd_Integer;	//NOTE: DO NOT REMOVE OR CHANGE TO dd_Rational
	
	for (int ii=0;ii<IDELEMS(srcRing_HH);ii++)
	{
		markingsAreCorrect=FALSE;	//crucial to initialise here
		poly aktpoly=srcRing_HH->m[ii]; //Only a pointer, so don't pDelete
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
			int *v=(int *)omAlloc((this->numVars+1)*sizeof(int));
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
			int ctr=0;
			for (int jj=0;jj<this->numVars;jj++)
			{				
				/*Store into ddMatrix*/				
				if(leadExpV[jj+1]-v[jj+1])
					ctr++;
				dd_set_si(intPointMatrix->matrix[aktrow][jj+1],leadExpV[jj+1]-v[jj+1]);
			}
			/*It ought to be more sensible to avoid 0-rows in the first place*/
// 			if(ctr==this->numVars)//We have a 0-row
// 				dd_MatrixRowRemove(&intPointMatrix,aktrow);
// 			else
				aktrow +=1;
			omFree(v);
		}
// 		omFree(v);
		omFree(leadExpV);
	}//for (int ii=0;ii<IDELEMS(srcRing_HH);ii++)
#ifdef gfanp
	gettimeofday(&t_markings_end, 0);
	t_markings += (t_markings_end.tv_sec - t_markings_start.tv_sec + 1e-6*(t_markings_end.tv_usec - t_markings_start.tv_usec));
#endif
	/*Now it is safe to idDelete(H)*/
	idDelete(&H);
	/*Preprocessing goes here since otherwise we would delete the constraint
	* for the standard simplex.
	*/
	preprocessInequalities(intPointMatrix);
	/*Now we add the constraint for the standard simplex*/
// 	dd_set_si(intPointMatrix->matrix[aktrow][0],-1);
// 	for (int jj=1;jj<=this->numVars;jj++)
// 	{
// 		dd_set_si(intPointMatrix->matrix[aktrow][jj],1);
// 	}
	//Let's make sure we compute interior points from the positive orthant
// 	dd_MatrixPtr posRestr=dd_CreateMatrix(this->numVars,this->numVars+1);
// 	
// 	int jj=1;
// 	for (int ii=0;ii<this->numVars;ii++)
// 	{
// 		dd_set_si(posRestr->matrix[ii][jj],1);
// 		jj++;							
// 	}
	/*We create a matrix containing the standard simplex
	* and constraints to assure a strictly positive point
	* is computed */
	dd_MatrixPtr posRestr = dd_CreateMatrix(this->numVars+1, this->numVars+1);
	for(int ii=0;ii<posRestr->rowsize;ii++)
	{
		if(ii==0)
		{
			dd_set_si(posRestr->matrix[ii][0],-1);
			for(int jj=1;jj<=this->numVars;jj++)			
				dd_set_si(posRestr->matrix[ii][jj],1);			
		}
		else
		{
			/** Set all variables to \geq 1/10. YMMV but this choice is pretty equal*/
			dd_set_si2(posRestr->matrix[ii][0],-1,2);
			dd_set_si(posRestr->matrix[ii][ii],1);
		}
	}
	dd_MatrixAppendTo(&intPointMatrix,posRestr);
	dd_FreeMatrix(posRestr);

	intvec *iv_weight = new intvec(this->numVars);
#ifdef gfanp
	timeval t_dd_start, t_dd_end;
	gettimeofday(&t_dd_start, 0);
#endif
	dd_ErrorType err;
	dd_rowset implLin, redrows;
	dd_rowindex newpos;

	interiorPoint(intPointMatrix, *iv_weight);	//iv_weight now contains the interior point
	dd_FreeMatrix(intPointMatrix);
	/*Crude attempt for interior point */
	/*dd_PolyhedraPtr ddpolyh;
	dd_ErrorType err;
	dd_rowset impl_linset,redset;
	dd_rowindex newpos;
	dd_MatrixCanonicalize(&intPointMatrix, &impl_linset, &redset, &newpos, &err);
	ddpolyh=dd_DDMatrix2Poly(intPointMatrix, &err);
	dd_MatrixPtr P;
	P=dd_CopyGenerators(ddpolyh);
	dd_FreePolyhedra(ddpolyh);
	for(int ii=0;ii<P->rowsize;ii++)
	{
 		intvec *iv_row=new intvec(this->numVars);
		makeInt(P,ii+1,*iv_row);
		iv_weight =ivAdd(iv_weight, iv_row);
		delete iv_row;
	}
	dd_FreeMatrix(P);
	dd_FreeMatrix(intPointMatrix);*/
#ifdef gfanp
	gettimeofday(&t_dd_end, 0);
	t_dd += (t_dd_end.tv_sec - t_dd_start.tv_sec + 1e-6*(t_dd_end.tv_usec - t_dd_start.tv_usec));
#endif			
	/*Step 3
	* turn the minimal basis into a reduced one */			
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
 	rDelete(tmpRing);
	delete iv_weight;

	ideal dstRing_I;			
	dstRing_I=idrCopyR(srcRing_HH,srcRing);
	idDelete(&srcRing_HH); //Hmm.... causes trouble - no more
	//dstRing_I=idrCopyR(inputIdeal,srcRing);
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
#ifdef gfanp
	gettimeofday(&t_kStd_start,0);
#endif
	dstRing_I=kStd(tmpI,NULL,testHomog,NULL);
#ifdef gfanp
	gettimeofday(&t_kStd_end, 0);
	t_kStd += (t_kStd_end.tv_sec - t_kStd_start.tv_sec + 1e-6*(t_kStd_end.tv_usec - t_kStd_start.tv_usec));
#endif
	idDelete(&tmpI);
	idNorm(dstRing_I);			
// 	kInterRed(dstRing_I);
	idSkipZeroes(dstRing_I);
	test=save;
	/*End of step 3 - reduction*/
			
	f->setFlipGB(dstRing_I);//store the flipped GB
	idDelete(&dstRing_I);
	f->flipRing=rCopy(dstRing);	//store the ring on the other side
//#ifdef gfan_DEBUG
//  	cout << "Flipped GB is UCN " << counter+1 << ":" << endl;
//  	this->idDebugPrint(dstRing_I);
// 	cout << endl;
//#endif			
	rChangeCurrRing(srcRing);	//return to the ring we started the computation of flipGB in
 	rDelete(dstRing);
#ifdef gfanp
	gettimeofday(&end, 0);
	time_flip += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif
}//void flip(ideal gb, facet *f)

/** \brief Compute initial ideal
 * Compute the initial ideal in_v(G) wrt a (possible) facet normal
 * used in gcone::getFacetNormal in order to preprocess possible facet normals
 * and in gcone::flip for obvious reasons.
*/
inline void gcone::computeInv(ideal &gb, ideal &initialForm, intvec &fNormal)
{
#ifdef gfanp
	timeval start, end;
	gettimeofday(&start, 0);
#endif
	for (int ii=0;ii<IDELEMS(gb);ii++)
	{
		poly initialFormElement;
		poly aktpoly = (poly)gb->m[ii];//Ptr, so don't pDelete(aktpoly)
		int *leadExpV=(int *)omAlloc((this->numVars+1)*sizeof(int));
		pGetExpV(aktpoly,leadExpV);	//find the leading exponent in leadExpV[1],...,leadExpV[n], use pNext(p)
		initialFormElement=pHead(aktpoly);
		while(pNext(aktpoly)!=NULL)	/*loop trough terms and check for parallelity*/
		{
			intvec *check = new intvec(this->numVars);
			aktpoly=pNext(aktpoly);	//next term
// 			pSetm(aktpoly);
			int *v=(int *)omAlloc((this->numVars+1)*sizeof(int));
			pGetExpV(aktpoly,v);		
			/* Convert (int)v into (intvec)check */			
			for (int jj=0;jj<this->numVars;jj++)
			{
				(*check)[jj]=v[jj+1]-leadExpV[jj+1];
			}
			if (isParallel(*check,fNormal)) //pass *check when 
// 			if(fNormal.compare(check)==0)
			{
				//Found a parallel vector. Add it
				initialFormElement = pAdd((initialFormElement),(poly)pHead(aktpoly));
			}
			omFree(v);
			delete check;
		}//while
#ifdef gfan_DEBUG
//  		cout << "Initial Form=";				
//  		pWrite(initialFormElement[ii]);
//  		cout << "---" << endl;
#endif
		/*Now initialFormElement must be added to (ideal)initialForm */
		initialForm->m[ii]=pCopy(initialFormElement);
		pDelete(&initialFormElement);
		omFree(leadExpV);
	}//for
#ifdef gfanp
	gettimeofday(&end, 0);
	time_computeInv += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif
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
inline ideal gcone::ffG(const ideal &H, const ideal &G)
{
	int size=IDELEMS(H);
	ideal res=idInit(size,1);
	for (int ii=0;ii<size;ii++)
	{
		poly temp1=pInit();
		poly temp2=pInit();
		poly temp3=pInit();//polys to temporarily store values for pSub
// 		res->m[ii]=pCopy(kNF(G, NULL,H->m[ii],0,0));
		temp1=pCopy(H->m[ii]);
// 		temp2=pCopy(res->m[ii]);
		//NOTE if gfanHeuristic=0 (sic!) this results in dPolyErrors - mon from wrong ring
		temp2=pCopy(kNF(G, NULL,H->m[ii],0,0));
		temp3=pSub(temp1, temp2);
		res->m[ii]=pCopy(temp3);
		//res->m[ii]=pSub(temp1,temp2); //buggy		
		//cout << "res->m["<<ii<<"]=";pWrite(res->m[ii]);
		pDelete(&temp1);
// 		pDelete(&temp2);
// 		pDelete(&temp3); //NOTE does not work, so commented out
	}
	return res;
}
	
/** \brief Preprocessing of inequlities
* Do some preprocessing on the matrix of inequalities
* 1) Replace several constraints on the pos. orthants by just one for each orthant
* 2) Remove duplicates of inequalities
* 3) Remove inequalities that arise as sums of other inequalities
*/
void gcone::preprocessInequalities(dd_MatrixPtr &ddineq)
{
/*	int *posRowsArray=NULL;
	int num_alloc=0;
	int num_elts=0;
 	int offset=0;*/
	//Remove zeroes (and strictly pos rows?)
	for(int ii=0;ii<ddineq->rowsize;ii++)
	{
		intvec *iv = new intvec(this->numVars);
		int posCtr=0;
		for(int jj=0;jj<this->numVars;jj++)
		{
			(*iv)[jj]=(int)mpq_get_d(ddineq->matrix[ii][jj+1]);
			if((*iv)[jj]>0)//check for strictly pos rows
				posCtr++;
			//Behold! This will delete the row for the standard simplex!
		}
// 		if( (iv->compare(0)==0) || (posCtr==iv->length()) )
		if( (posCtr==iv->length()) || (iv->compare(0)==0) )		
		{
			dd_MatrixRowRemove(&ddineq,ii+1);
			ii--;//Yes. This is on purpose
		}
		delete iv;
	}
	//Remove duplicates of rows
// 	posRowsArray=NULL;
// 	num_alloc=0;
// 	num_elts=0;
// 	offset=0;
// 	int num_newRows = ddineq->rowsize;
// 	for(int ii=0;ii<ddineq->rowsize-1;ii++)
// 	for(int ii=0;ii<num_newRows-1;ii++)
// 	{
// 		intvec *iv = new intvec(this->numVars);//1st vector to check against
// 		for(int jj=0;jj<this->numVars;jj++)
// 			(*iv)[jj]=(int)mpq_get_d(ddineq->matrix[ii][jj+1]);
// 		for(int jj=ii+1;jj</*ddineq->rowsize*/num_newRows;jj++)
// 		{
// 			intvec *ivCheck = new intvec(this->numVars);//Checked against iv
// 			for(int kk=0;kk<this->numVars;kk++)
// 				(*ivCheck)[kk]=(int)mpq_get_d(ddineq->matrix[jj][kk+1]);
// 			if (iv->compare(ivCheck)==0)
// 			{
// // 				cout << "=" << endl;
// // 				num_alloc++;
// // 				void *tmp=realloc(posRowsArray,(num_alloc*sizeof(int)));
// // 				if(!tmp)
// // 				{
// // 					WerrorS("Woah dude! Couldn't realloc memory\n");
// // 					exit(-1);
// // 				}
// // 				posRowsArray = (int*)tmp;
// // 				posRowsArray[num_elts]=jj;
// // 				num_elts++;
// 				dd_MatrixRowRemove(&ddineq,jj+1);
// 				num_newRows = ddineq->rowsize;
// 			}
// 			delete ivCheck;
// 		}
// 		delete iv;
// 	}
// 	for(int ii=0;ii<num_elts;ii++)
// 	{
// 		dd_MatrixRowRemove(&ddineq,posRowsArray[ii]+1-offset);
// 		offset++;
// 	}
// 	free(posRowsArray);
	//Apply Thm 2.1 of JOTA Vol 53 No 1 April 1987*/	
}//preprocessInequalities
	
/** \brief Compute a Groebner Basis
 *
 * Computes the Groebner basis and stores the result in gcone::gcBasis
 *\param ideal
 *\return void
 */
inline void gcone::getGB(const ideal &inputIdeal)		
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
inline intvec *gcone::ivNeg(const intvec *iv)
{
	//NOTE: Can't this be done without new?
	intvec *res;// = new intvec(iv->length());
	res=ivCopy(iv);
	*res *= (int)-1;						
	return res;
}


/** \brief Compute the dot product of two intvecs
*
*/
inline int gcone::dotProduct(const intvec &iva, const intvec &ivb)				
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
inline bool gcone::isParallel(const intvec &a, const intvec &b)
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
inline void gcone::interiorPoint( dd_MatrixPtr &M, intvec &iv) //no const &M here since we want to remove redundant rows
{
	dd_LPPtr lp,lpInt;
	dd_ErrorType err=dd_NoError;
	dd_LPSolverType solver=dd_DualSimplex;
	dd_LPSolutionPtr lpSol=NULL;
	dd_rowset ddlinset,ddredrows;	//needed for dd_FindRelativeInterior
 	dd_rowindex ddnewpos;
	dd_NumberType numb;	
	//M->representation=dd_Inequality;
			
	//NOTE: Make this n-dimensional!
	//dd_set_si(M->rowvec[0],1);dd_set_si(M->rowvec[1],1);dd_set_si(M->rowvec[2],1);
									
	/*NOTE: Leave the following line commented out!
	* Otherwise it will slow down computations a great deal
	* */
// 	dd_MatrixCanonicalizeLinearity(&M, &ddlinset, &ddnewpos, &err);
	//if (err!=dd_NoError){cout << "Error during dd_MatrixCanonicalize" << endl;}
	dd_MatrixPtr posRestr=dd_CreateMatrix(this->numVars,this->numVars+1);
	int jj=1;
	for (int ii=0;ii<this->numVars;ii++)
	{
		dd_set_si(posRestr->matrix[ii][jj],1);
		jj++;							
	}
	dd_MatrixAppendTo(&M,posRestr);
	dd_FreeMatrix(posRestr);
	lp=dd_Matrix2LP(M, &err);
	if (err!=dd_NoError){WerrorS("Error during dd_Matrix2LP in gcone::interiorPoint");}
	if (lp==NULL){WerrorS("LP is NULL");}
#ifdef gfan_DEBUG
//			dd_WriteLP(stdout,lp);
#endif	
					
	lpInt=dd_MakeLPforInteriorFinding(lp);
	if (err!=dd_NoError){WerrorS("Error during dd_MakeLPForInteriorFinding in gcone::interiorPoint");}
#ifdef gfan_DEBUG
// 			dd_WriteLP(stdout,lpInt);
#endif			
// 	dd_FindRelativeInterior(M,&ddlinset,&ddredrows,&lpSol,&err);
	if (err!=dd_NoError)
	{
		WerrorS("Error during dd_FindRelativeInterior in gcone::interiorPoint");
		dd_WriteErrorMessages(stdout, err);
	}			
	dd_LPSolve(lpInt,solver,&err);	//This will not result in a point from the relative interior
// 	if (err!=dd_NoError){WerrorS("Error during dd_LPSolve");}									
	lpSol=dd_CopyLPSolution(lpInt);
// 	if (err!=dd_NoError){WerrorS("Error during dd_CopyLPSolution");}
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
// 	set_free(ddlinset);
// 	set_free(ddredrows); 	
			
}//void interiorPoint(dd_MatrixPtr const &M)

inline void gcone::interiorPoint2(const dd_MatrixPtr &M, intvec &iv)
{
	KMatrix<Rational> mMat(M->rowsize+1,M->colsize);
	for(int ii=0;ii<M->rowsize;ii++)
	{
		for(int jj=0;jj<M->colsize-1;jj++)
		{
			if(mpq_sgn(M->matrix[ii][jj+1])<-1)
			{
				mMat.set(ii,jj,-(Rational)mpq_get_d(M->matrix[ii][jj+1]));
			}
			else
				mMat.set(ii,jj,(Rational)mpq_get_d(M->matrix[ii][jj+1]));
				
// 			mMat.set(ii,jj,&(M->matrix[ii][jj+1]) );
			cout << mpq_get_d(M->matrix[ii][jj+1]) << " ";
// 			int val=(int)mMat.get(ii,jj);
// 			cout << ii << "," << jj << endl;;
// 			mpq_out_str (NULL, 10, (__mpq_struct)mMat.get(ii,jj));
		}
		cout << endl;
		mMat.set(ii,M->colsize-1,1);
	}
	dd_WriteMatrix(stdout,M);
//  	for(int ii=0;ii<M->rowsize;ii++)
// 	{
// 		cout << mMat.get(ii,ii+M->colsize) << " ";
// 		if((ii+M->colsize)%M->colsize==0)
// 			cout << endl;
// 	}
	
	Rational* mSol;
	int rank;
	int c;
// 	dd_WriteMatrix(stdout,M);
 	rank=mMat.solve(&mSol,&c);
// 	for(int ii=0;ii<c;ii++)	
// 		iv[ii]=mSol[ii];
// 		cout << mSol[ii].get_den() << "/" << mSol[ii].get_num() << endl;
	int gcd=1;
	for(int ii=0;ii<c-1;ii++)
		gcd += intgcd(mSol[ii].get_den_si(),mSol[ii+1].get_den_si());
	cout << gcd << endl;
	for(int ii=0;ii<iv.length();ii++)
		iv[ii]=(int)((mSol[ii].get_num_si()*gcd)/mSol[ii].get_den_si());

}
	
	
/** \brief Copy a ring and add a weighted ordering in first place
 * 
 */
ring gcone::rCopyAndAddWeight(const ring &r, const intvec *ivw)				
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
inline bool gcone::areEqual(const intvec &a, const intvec &b)
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
/** \brief Compute the lineality/homogeneity space
* It is the kernel of the inequality matrix Ax=0
*/
dd_MatrixPtr gcone::computeLinealitySpace()
{
	dd_MatrixPtr res;
	dd_MatrixPtr ddineq;
	ddineq=dd_CopyMatrix(this->ddFacets);	
	//Add a row of 0s in 0th place
	dd_MatrixPtr ddAppendRowOfZeroes=dd_CreateMatrix(1,this->numVars+1);
	dd_MatrixPtr ddFoo=dd_AppendMatrix(ddAppendRowOfZeroes,ddineq);
	dd_FreeMatrix(ddAppendRowOfZeroes);
	dd_FreeMatrix(ddineq);
	ddineq=dd_CopyMatrix(ddFoo);
	dd_FreeMatrix(ddFoo);
	//Cohen starts here
	int dimKer=0;//Cohen calls this r
	int m=ddineq->rowsize;//Rows
	int n=ddineq->colsize;//Cols
	int c[m+1];
	int d[n+1];
	for(int ii=0;ii<m;ii++)
		c[ii]=0;
	for(int ii=0;ii<n;ii++)
		d[ii]=0;	
	
	for(int k=1;k<n;k++)
	{
		//Let's find a j s.t. m[j][k]!=0 && c[j]=0		
		int condCtr=0;//Check each row for zeroness
		for(int j=1;j<m;j++)
		{
			if(mpq_sgn(ddineq->matrix[j][k])!=0 && c[j]==0)
			{
				mpq_t quot; mpq_init(quot);
				mpq_t one; mpq_init(one); mpq_set_str(one,"-1",10);
				mpq_t ratd; mpq_init(ratd);
				if((int)mpq_get_d(ddineq->matrix[j][k])!=0)
					mpq_div(quot,one,ddineq->matrix[j][k]);
				mpq_set(ratd,quot);
				mpq_canonicalize(ratd);
		
				mpq_set_str(ddineq->matrix[j][k],"-1",10);
				for(int ss=k+1;ss<n;ss++)
				{
					mpq_t prod; mpq_init(prod);
					mpq_mul(prod, ratd, ddineq->matrix[j][ss]);				
					mpq_set(ddineq->matrix[j][ss],prod);
					mpq_canonicalize(ddineq->matrix[j][ss]);
					mpq_clear(prod);
				}		
				for(int ii=1;ii<m;ii++)
				{
					if(ii!=j)
					{
						mpq_set(ratd,ddineq->matrix[ii][k]);
						mpq_set_str(ddineq->matrix[ii][k],"0",10);			
						for(int ss=k+1;ss<m;ss++)
						{
							mpq_t prod; mpq_init(prod);
							mpq_mul(prod, ratd, ddineq->matrix[j][ss]);
							mpq_t sum; mpq_init(sum);
							mpq_add(sum, ddineq->matrix[ii][ss], prod);
							mpq_set(ddineq->matrix[ii][ss],	sum);
							mpq_canonicalize(ddineq->matrix[ii][ss]);
							mpq_clear(prod);
							mpq_clear(sum);
						}
					}
				}
				c[j]=k;		
				d[k]=j;
				mpq_clear(quot); mpq_clear(ratd); mpq_clear(one);	
			}
			else
				condCtr++;
		}			
		if(condCtr==m-1)	//Why -1 ???
		{
			dimKer++;
			d[k]=0;
// 			break;//goto _4;
		}//else{
		/*Eliminate*/
	}//for(k
	/*Output kernel, i.e. set gcone::dd_LinealitySpace here*/	
// 	gcone::dd_LinealitySpace = dd_CreateMatrix(dimKer,this->numVars+1);
	res = dd_CreateMatrix(dimKer,this->numVars+1);
	int row=-1;
	for(int kk=1;kk<n;kk++)
	{
		if(d[kk]==0)
		{
			row++;
			for(int ii=1;ii<n;ii++)
			{
				if(d[ii]>0)
					mpq_set(res->matrix[row][ii],ddineq->matrix[d[ii]][kk]);
				else if(ii==kk)				
					mpq_set_str(res->matrix[row][ii],"1",10);
				mpq_canonicalize(res->matrix[row][ii]);
			}
		}
	}
	dd_FreeMatrix(ddineq);
	return(res);
	//Better safe than sorry:
	//NOTE dd_LinealitySpace contains RATIONAL ENTRIES
	//Therefore if you need integer ones: CALL gcone::makeInt(...) method
}	

/** Compute the kernel of a given mxn-matrix */
// dd_MatrixPtr gcone::kernel(const dd_MatrixPtr &M)
// {
// }

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
// 	vector<facet> stlRoot;
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
	/*Compute lineality space here
	* Afterwards static dd_MatrixPtr gcone::dd_LinealitySpace is set
	*/
	dd_LinealitySpace = gcAct->computeLinealitySpace();
	/*End of lineality space computation*/		
// 	gcAct->getCodim2Normals(*gcAct);
	gcAct->getExtremalRays(*gcAct);
				
	//Compute unique representation of codim-2-facets
	gcAct->normalize();
			
	/* Make a copy of the facet list of first cone
	   Since the operations getCodim2Normals and normalize affect the facets
	   we must not memcpy them before these ops!
	*/
	
	facet *codim2Act; codim2Act = NULL;			
	facet *sl2Root; //sl2Root = new facet(2);
	facet *sl2Act;	//sl2Act = sl2Root;
			
	for(int ii=0;ii<this->numFacets;ii++)
	{
		//only copy flippable facets! NOTE: We do not need flipRing or any such information.
		if(fAct->isFlippable==TRUE)
		{
			intvec *fNormal;
			fNormal = fAct->getFacetNormal();
			if( ii==0 || (ii>0 && SearchListAct==NULL) ) //1st facet may be non-flippable
			{						
				SearchListAct = SearchListRoot;				
			}
			else
			{			
				SearchListAct->next = new facet();
				SearchListAct = SearchListAct->next;						
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
// 			for(int jj=0;jj<fAct->numRays-1;jj++)
			{
				intvec *f2Normal;
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
				delete f2Normal;				
				codim2Act = codim2Act->next;
			}
			fAct = fAct->next;
			delete fNormal;			
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
	while((SearchListAct!=NULL))// && counter<10)
	{//NOTE See to it that the cone is only changed after ALL facets have been flipped!				
		fAct = SearchListAct;
				
 		while(fAct!=NULL)
// 		while( (fAct->getUCN() == fAct->next->getUCN()) )		
		{	//Since SLA should only contain flippables there should be no need to check for that			
			gcAct->flip(gcAct->gcBasis,fAct);			
			//NOTE rCopy needed?
			ring rTmp=rCopy(fAct->flipRing);
// 			ring rTmp=fAct->flipRing; //segfaults
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
 			idDelete((ideal *)&fAct->flipGB);
			rDelete(fAct->flipRing);
			
			gcTmp->getConeNormals(gcTmp->gcBasis, FALSE);	
// 			gcTmp->getCodim2Normals(*gcTmp);
			gcTmp->getExtremalRays(*gcTmp);
			gcTmp->normalize();// will be done in g2c 
			//gcTmp->ddFacets should not be needed anymore, so
			dd_FreeMatrix(gcTmp->ddFacets);
#ifdef gfan_DEBUG
// 			gcTmp->showFacets(1);
#endif
			/*add facets to SLA here*/
			SearchListRoot=gcTmp->enqueueNewFacets(SearchListRoot);
 			if(gfanHeuristic==1)
 			{
				gcTmp->writeConeToFile(*gcTmp);
				//The for loop is no longer needed
// 				for(int ii=0;ii<IDELEMS(gcTmp->gcBasis);ii++)
// 				{
// 					pDelete(&gcTmp->gcBasis->m[ii]);
// 				}
 				idDelete((ideal*)&gcTmp->gcBasis);//Whonder why?
				//If you use the following make sure it is uncommented in readConeFromFile
// 				rDelete(gcTmp->baseRing);
 			}			
#ifdef gfan_DEBUG
// 			if(SearchListRoot!=NULL)
// 				gcTmp->showSLA(*SearchListRoot);
#endif
			rChangeCurrRing(gcAct->baseRing);
			rDelete(rTmp);
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
// 			fAct=fAct->next;
		}//while( ( (fAct->next!=NULL) && (fAct->getUCN()==fAct->next->getUCN() ) ) );		
		//Search for cone with smallest UCN
		gcNext = gcHead;
		while(gcNext!=NULL && SearchListRoot!=NULL && gcNext!=(gcone * const)0xfbfbfbfbfbfbfbfb && gcNext!=(gcone * const)0xfbfbfbfb)
		{			
			if( gcNext->getUCN() == SearchListRoot->getUCN() )
			{//NOTE: Implement heuristic to be used!
				if(gfanHeuristic==0)
				{
					gcAct = gcNext;
					//Seems better to not to use rCopy here
// 					rAct=rCopy(gcAct->baseRing);
					rAct=gcAct->baseRing;
					rComplete(rAct);
					rChangeCurrRing(rAct);						
					break;
				}
				else if(gfanHeuristic==1)
				{
					gcone *gcDel;
					gcDel = gcAct;					
					gcAct = gcNext;
// 					rDelete(gcDel->baseRing);
					//Read st00f from file
					//implant the GB into gcAct
					readConeFromFile(gcAct->getUCN(), gcAct);
// 					rAct=rCopy(gcAct->baseRing);
					/*The ring change occurs in readConeFromFile, so as to 
					assure that gcAct->gcBasis belongs to the right ring*/
					rAct=gcAct->baseRing;
					rComplete(rAct);
					rChangeCurrRing(rAct);
					break;
				}				
			}			
 			gcNext = gcNext->next;
		}
		UCNcounter++;
		//SearchListAct = SearchListAct->next;
		//SearchListAct = fAct->next;
		SearchListAct = SearchListRoot;
	}
	cout << endl << "Found " << counter << " cones - terminating" << endl;
}//void noRevS(gcone &gc)	
		
		
/** \brief Make a set of rational vectors into integers
 *
 * We compute the lcm of the denominators and multiply with this to get integer values.		
 * \param dd_MatrixPtr,intvec
 */
inline void gcone::makeInt(const dd_MatrixPtr &M, const int line, intvec &n)
{			
// 	mpz_t denom[this->numVars];
	mpz_t *denom = new mpz_t[this->numVars];
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
	mpz_clear(tmp);	
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
	delete [] denom;
	mpz_clear(kgV);
	mpq_clear(qkgV); mpq_clear(res);			
			
}
/**
 * For all codim-2-facets we compute the gcd of the components of the facet normal and 
 * divide it out. Thus we get a normalized representation of each
 * (codim-2)-facet normal, i.e. a primitive vector
 * Actually we now also normalize the facet normals.
 */
inline void gcone::normalize()
{
	int *ggT = new int;
		*ggT=1;
	facet *fAct;
	facet *codim2Act;
	fAct = this->facetPtr;
	codim2Act = fAct->codim2Ptr;
	while(fAct!=NULL)
	{
		intvec *fNormal;
		fNormal = fAct->getFacetNormal();
		for(int ii=0;ii<this->numVars;ii++)
		{
			*ggT=intgcd((*ggT),(*fNormal)[ii]);
		}
		for(int ii=0;ii<this->numVars;ii++)
			(*fNormal)[ii] = ((*fNormal)[ii])/(*ggT);
		fAct->setFacetNormal(fNormal);
		delete fNormal;
		/*And now for the codim2*/
		while(codim2Act!=NULL)
		{				
			intvec *n;
			n=codim2Act->getFacetNormal();
			for(int ii=0;ii<this->numVars;ii++)
			{
				*ggT = intgcd((*ggT),(*n)[ii]);
			}
			for(int ii=0;ii<this->numVars;ii++)
			{
				(*n)[ii] = ((*n)[ii])/(*ggT);
			}
			codim2Act->setFacetNormal(n);
			codim2Act = codim2Act->next;
			delete n;
		}
		fAct = fAct->next;
	}
	delete ggT;				
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
facet * gcone::enqueueNewFacets(facet *f)
{
#ifdef gfanp
	timeval start, end;
	gettimeofday(&start, 0);
#endif
	facet *slHead;
	slHead = f;
	facet *slAct;	//called with f=SearchListRoot
	slAct = f;
	facet *slEnd;	//Pointer to end of SLA
	slEnd = f;
// 	facet *slEndStatic;	//marks the end before a new facet is added		
	facet *fAct;
	fAct = this->facetPtr;
	facet *codim2Act;
	codim2Act = this->facetPtr->codim2Ptr;
	facet *sl2Act;
	sl2Act = f->codim2Ptr;
	/** Pointer to a facet that will be deleted */
	volatile facet *deleteMarker;
	deleteMarker = NULL;
			
	/** Flag to indicate a facet that should be added to SLA*/
// 	bool doNotAdd=FALSE;
	/** \brief  Flag to mark a facet that might be added
	 * The following case may occur:
	 * A facet fAct is found to be parallel but not equal to the current facet slAct from SLA.
	 * This does however not mean that there does not exist a facet behind the current slAct that is actually equal
	 * to fAct. In this case we set the boolean flag maybe to TRUE. If we encounter an equality lateron, it is reset to
	 * FALSE and the according slAct is deleted. 
	 * If slAct->next==NULL AND maybe==TRUE we know, that fAct must be added
	 */
// 	volatile bool maybe=FALSE;
	/**A facet was removed, lengthOfSearchlist-- thus we must not rely on 
	 * if(notParallelCtr==lengthOfSearchList) but rather
	 * if( (notParallelCtr==lengthOfSearchList && removalOccured==FALSE)
	 */
	volatile bool removalOccured=FALSE;
	int ctr=0;	//encountered equalities in SLA
	int notParallelCtr=0;
// 	gcone::lengthOfSearchList=1;
	while(slEnd->next!=NULL)
	{
		slEnd=slEnd->next;
// 		gcone::lengthOfSearchList++;
	}
	/*1st step: compare facetNormals*/
//  	intvec *fNormal=NULL;gcone::
//  	intvec *slNormal=NULL;
			
	while(fAct!=NULL)
	{						
		if(fAct->isFlippable==TRUE)
		{
			intvec *fNormal=NULL;
// 			intvec *slNormal=NULL;
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
						slAct = new facet(*fCopy);
						slHead = slAct;								
					}
					else
					{
						slAct->next = new facet(*fCopy);
						slAct = slAct->next;
					}
					fCopy = fCopy->next;
				}
				break;
			}
			/*End of dumping into SLA*/
			while(slAct!=NULL)
			//while(slAct!=slEndStatic->next)
			{
				intvec *slNormal=NULL;
				removalOccured=FALSE;
				slNormal = slAct->getFacetNormal();
#ifdef gfan_DEBUG
				cout << "Checking facet (";
				fNormal->show(1,1);
				cout << ") against (";
				slNormal->show(1,1);
				cout << ")" << endl;
#endif				
 				if(areEqual(fAct,slAct))
				{
					deleteMarker = slAct;
					if(slAct==slHead)
					{						
						slHead = slAct->next;						
						if(slHead!=NULL)
							slHead->prev = NULL;						
					}
					else if (slAct==slEnd)
					{
						slEnd=slEnd->prev;
						slEnd->next = NULL;						
					}								
					else
					{
						slAct->prev->next = slAct->next;
						slAct->next->prev = slAct->prev;						
					}
					removalOccured=TRUE;
// 					cout << gcone::lengthOfSearchList << endl;
					gcone::lengthOfSearchList--;
// 					cout << gcone::lengthOfSearchList << endl;
					if(deleteMarker!=NULL)
					{
// 						delete deleteMarker;
// 						deleteMarker=NULL;
					}
#ifdef gfan_DEBUG
					cout << "Removing (";
					fNormal->show(1,1);
					cout << ") from list" << endl;
#endif
					delete slNormal;
					break;//leave the while loop, since we found fAct=slAct thus delete slAct and do not add fAct
				}		
				
				slAct = slAct->next;
				/* NOTE The following lines must not be here but rather called inside the if-block above.
				Otherwise results get crappy. Unfortunately there are two slAct=slAct->next calls now,
				(not nice!) but since they are in seperate branches of the if-statement there should not
				be a way it gets called twice thus ommiting one facet:
				slAct = slAct->next;*/
				if(deleteMarker!=NULL)
				{						
//  					delete deleteMarker;
//  					deleteMarker=NULL;
				}
				delete slNormal;
						//if slAct was marked as to be deleted, delete it here!
			}//while(slAct!=NULL)
			if(removalOccured==FALSE)
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
// 				intvec *f2Normal=new intvec(this->numVars);
				while(f2Act!=NULL)
				{
					intvec *f2Normal;
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
					delete f2Normal;
				}
				gcone::lengthOfSearchList++;							
// 				cout << gcone::lengthOfSearchList << endl;
			}//if( (notParallelCtr==lengthOfSearchList && removalOccured==FALSE) ||
			fAct = fAct->next;
 			delete fNormal;
//  			delete slNormal;
		}//if(fAct->isFlippable==TRUE)
		else
		{
			fAct = fAct->next;
		}
		if(gcone::maxSize<gcone::lengthOfSearchList)
			gcone::maxSize= gcone::lengthOfSearchList;
	}//while(fAct!=NULL)
#ifdef gfanp
	gettimeofday(&end, 0);
	time_enqueue += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif						
	return slHead;
}//addC2N

//A try with the STL
facet * gcone::enqueue2(facet *f)
{
// 	vector<facet> searchList;
// 	facet fAct;
// 	fAct=this->facetPtr;
// 	facet *slAct;
// 	slAct = f;
// 	while(fAct!=NULL)
// 	{
// 		if(fAct->isFlippable==TRUE)
// 		{
// 			if(slAct==NULL)
// 		}
// 	}
}

/** \brief Compute the gcd of two ints
 */
inline int gcone::intgcd(const int a, const int b)
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
 * NO LONGER USED
 */
inline dd_MatrixPtr gcone::facets2Matrix(const gcone &gc)
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
		delete comp;
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
inline void gcone::writeConeToFile(const gcone &gc, bool usingIntPoints)
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
			delete iv;
			while(f2Act!=NULL)
			{
				intvec *iv2;
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
				delete iv2;
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
inline void gcone::readConeFromFile(int UCN, gcone *gc)
{
	//int UCN=gc.UCN;
	stringstream ss;
	ss << UCN;
	string UCNstr = ss.str();
// 	string line;
// 	string strGcBasisLength;
// 	string strMonom, strCoeff, strCoeffNom, strCoeffDenom;
	int gcBasisLength=0;
// 	int intCoeff=1;
// 	int intCoeffNom=1;		//better (number) but that's not compatible with stringstream;
// 	int intCoeffDenom=1;
	
// 	bool hasCoeffInQ = FALSE;	//does the polynomial have rational coeff?
// 	bool hasNegCoeff = FALSE;	//or a negative one?
	size_t found;			//used for find_first_(not)_of

	string prefix="/tmp/cone";
	string suffix=".sg";
	string filename;
	filename.append(prefix);
	filename.append(UCNstr);
	filename.append(suffix);
					
	ifstream gcInputFile(filename.c_str(), ifstream::in);
	
	ring saveRing=currRing;	
	//Comment the following if you uncomment the if(line=="RING") part below
 	rChangeCurrRing(gc->baseRing);
	
	while( !gcInputFile.eof() )
	{
		string line;
		getline(gcInputFile,line);
// 		hasCoeffInQ = FALSE;
// 		hasNegCoeff = FALSE;
		
		if(line=="RING")
		{
// 			getline(gcInputFile,line);
// 			found = line.find("a(");
// 			line.erase(0,found+2);
// 			string strweight;
// 			strweight=line.substr(0,line.find_first_of(")"));
// 			intvec *iv=new intvec(this->numVars);
// 			for(int ii=0;ii<this->numVars;ii++)
// 			{
// 				string weight;
// 				weight=line.substr(0,line.find_first_of(",)"));				
// 				(*iv)[ii]=atoi(weight.c_str());
// 				line.erase(0,line.find_first_of(",)")+1);
// 			}
// 			ring newRing;
// 			if(currRing->order[0]!=ringorder_a)
// 			{
// 				newRing=rCopyAndAddWeight(currRing,iv);
// 			}
// 			else
// 			{ 			
// 				newRing=rCopy0(currRing);
// 				int length=this->numVars;
// 				int *A=(int *)omAlloc0(length*sizeof(int));
// 				for(int jj=0;jj<length;jj++)
// 				{
// 					A[jj]=-(*iv)[jj];
// 				}
// 				omFree(newRing->wvhdl[0]);
// 				newRing->wvhdl[0]=(int*)A;
// 				newRing->block1[0]=length;
// 			}
// 			delete iv;
//  			rComplete(newRing);
// 			gc->baseRing=rCopy(newRing);
// // 			rDelete(newRing);
// 			rComplete(gc->baseRing);
// 			if(currRing!=gc->baseRing)
// 				rChangeCurrRing(gc->baseRing);
		}
		
		if(line=="GCBASISLENGTH")
		{
			string strGcBasisLength;
			getline(gcInputFile, line);
			strGcBasisLength = line;
			int size=atoi(strGcBasisLength.c_str());
			gcBasisLength=size;		
			gc->gcBasis=idInit(size,1);
		}
		if(line=="GCBASIS")
		{
			for(int jj=0;jj<gcBasisLength;jj++)
			{
				getline(gcInputFile,line);
				//magically convert strings into polynomials
				//polys.cc:p_Read
				//check until first occurance of + or -
				//data or c_str	
				poly strPoly=pInit();//Ought to be inside the while loop, but that will eat your memory
				poly resPoly=pInit();	//The poly to be read in							
				while(!line.empty())
				{
// 					poly strPoly=pInit();
					number nCoeff=nInit(1);
					number nCoeffNom=nInit(1);
					number nCoeffDenom=nInit(1);
					string strMonom, strCoeff, strCoeffNom, strCoeffDenom;
					bool hasCoeffInQ = FALSE;	//does the polynomial have rational coeff?
					bool hasNegCoeff = FALSE;	//or a negative one?
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
						strCoeffDenom=strMonom.substr(found+1,strMonom.find_first_not_of("1234567890",found+1));
						strMonom.erase(0,found);
						strMonom.erase(0,strMonom.find_first_not_of("1234567890/"));			
						nRead(strCoeffNom.c_str(), &nCoeffNom);
						nRead(strCoeffDenom.c_str(), &nCoeffDenom);
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
						
					p_Read(monom,strPoly,currRing);	//strPoly:=monom				
					switch (hasCoeffInQ)
					{
						case TRUE:
							if(hasNegCoeff)
								nCoeffNom=nNeg(nCoeffNom);
							pSetCoeff(strPoly, nDiv(nCoeffNom, nCoeffDenom));
							break;
						case FALSE:
							if(hasNegCoeff)
								nCoeff=nNeg(nCoeff);							
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
					nDelete(&nCoeff);
					nDelete(&nCoeffNom);
					nDelete(&nCoeffDenom);
// 					pDelete(&strPoly);
				}//while(!line.empty())			
				gc->gcBasis->m[jj]=pCopy(resPoly);
				pDelete(&resPoly);	//reset
// 				pDelete(&strPoly);	//NOTE Crashes				
			}
			break;
		}//if(line=="GCBASIS")		
	}//while(!gcInputFile.eof())	
	gcInputFile.close();
	rChangeCurrRing(saveRing);
}
	
ring gcone::getBaseRing()
{
	return rCopy(this->baseRing);
}
/** \brief Sort the rays of a facet lexicographically
*/
void gcone::sortRays(gcone *gc)
{
	facet *fAct;
	fAct = this->facetPtr->codim2Ptr;
// 	while(fAct->next!=NULL)
// 	{
// 		if(fAct->fNormal->compare(fAct->fNormal->next)==-1
// 	}
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
		/*The following is necessary for leaves in the tree of cones
		* Since we don't use them in the computation and gcBasis is 
		* set to (poly)NULL in noRevS we need to get this back here.
		*/
// 		if(gcAct->gcBasis->m[0]==(poly)NULL)
		if(gfanHeuristic==1)
			gcAct->readConeFromFile(gcAct->getUCN(),gcAct);
// 		ring saveRing=currRing;
// 		ring tmpRing=gcAct->getBaseRing;
// 		rChangeCurrRing(tmpRing);
// 		l->m[1].data=(void*)idrCopyR_NoSort(gcAct->gcBasis,gcAct->getBaseRing());
		l->m[1].data=(void*)idrCopyR(gcAct->gcBasis,gcAct->getBaseRing());
// 		rChangeCurrRing(saveRing);

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
inline intvec gcone::f2M(gcone *gc, facet *f, int n)
{
	facet *fAct;
	intvec *res;//=new intvec(this->numVars);	
// 	int codim=n;
// 	int bound;
// 	if(f==gc->facetPtr)
	if(n==1)
	{
		intvec *m1Res=new intvec(gc->numFacets,gc->numVars,0);
		res = ivCopy(m1Res);
		fAct = gc->facetPtr;
		delete m1Res;
// 		bound = gc->numFacets*(this->numVars);		
	}
	else
	{
		fAct = f->codim2Ptr;
		intvec *m2Res = new intvec(f->numCodim2Facets,gc->numVars,0);
		res = ivCopy(m2Res);
		delete m2Res;	
// 		bound = fAct->numCodim2Facets*(this->numVars);

	}
	int ii=0;
	while(fAct!=NULL )//&& ii < bound )
	{
		intvec *fNormal;
		fNormal = fAct->getFacetNormal();
		for(int jj=0;jj<this->numVars;jj++)
		{
			(*res)[ii]=(*fNormal)[jj];
			ii++;
		}
		fAct = fAct->next;
		delete fNormal;
	}	
	return *res;
}

int gcone::counter=0;
int gfanHeuristic;
int gcone::lengthOfSearchList;
int gcone::maxSize;
dd_MatrixPtr gcone::dd_LinealitySpace;
#ifdef gfanp
// int gcone::lengthOfSearchList=0;
float gcone::time_getConeNormals;
float gcone::time_getCodim2Normals;
float gcone::t_getExtremalRays;
float gcone::time_flip;
float gcone::t_markings;
float gcone::t_dd;
float gcone::t_kStd;
float gcone::time_enqueue;
float gcone::time_computeInv;
float gcone::t_ddMC;
float gcone::t_mI;
float gcone::t_iP;
unsigned gcone::parallelButNotEqual=0;
unsigned gcone::numberOfFacetChecks=0;
#endif
bool gcone::hasHomInput=FALSE;
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
	
		ring inputRing=currRing;	// The ring the user entered
		ring rootRing;			// The ring associated to the target ordering
// 		ideal res;
		dd_set_global_constants();
		if(method==noRevS)
		{
			gcone *gcRoot = new gcone(currRing,inputIdeal);
			gcone *gcAct;
			gcAct = gcRoot;
			gcAct->numVars=pVariables;
			gcAct->getGB(inputIdeal);
			/*Check whether input is homogeneous
			if TRUE each facet intersects the positive orthant, so we don't need the
			flippability test in getConeNormals & getExtremalRays
			*/
			if(idHomIdeal(gcAct->gcBasis,NULL))
				gcone::hasHomInput=TRUE;
	#ifndef NDEBUG
	// 		cout << "GB of input ideal is:" << endl;
	// 		idShow(gcAct->gcBasis);
	#endif
			if(gcAct->isMonomial(gcAct->gcBasis))
			{
				WerrorS("Monomial input - terminating");
				exit(-1);
				gcAct->getConeNormals(gcAct->gcBasis);
				lResList=lprepareResult(gcAct,1);
				dd_free_global_constants;
				//This is filthy
				return lResList;
			}			
			gcAct->getConeNormals(gcAct->gcBasis);
			gcAct->noRevS(*gcAct);	//Here we go!
			rChangeCurrRing(inputRing);
			//res=gcAct->gcBasis;
			//Below is a workaround, since gcAct->gcBasis gets deleted in noRevS
// 			res = inputIdeal; 
			lResList=lprepareResult(gcRoot,gcRoot->getCounter());
			/*Cleanup*/
			gcone *gcDel;	
			gcDel = gcRoot;
			gcAct = gcRoot;
			while(gcAct!=NULL)
			{
				gcDel = gcAct;
				gcAct = gcAct->next;
				delete gcDel;
			}
		}//method==noRevS
		dd_FreeMatrix(gcone::dd_LinealitySpace);
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
	//gcone::counter=0;
	/*Return result*/
#ifdef gfanp
	cout << "t_getConeNormals:" << gcone::time_getConeNormals << endl;
// 	cout << "t_getCodim2Normals:" << gcone::time_getCodim2Normals << endl;
// 	cout << "  t_ddMC:" << gcone::t_ddMC << endl;
// 	cout << "  t_mI:" << gcone::t_mI << endl;
// 	cout << "  t_iP:" << gcone::t_iP << endl;
	cout << "t_getExtremalRays:" << gcone::t_getExtremalRays << endl;
	cout << "t_Flip:" << gcone::time_flip << endl;
	cout << "  t_markings:" << gcone::t_markings << endl;
	cout << "  t_dd:" << gcone::t_dd << endl;
	cout << " t_kStd:" << gcone::t_kStd << endl;
	cout << "t_computeInv:" << gcone::time_computeInv << endl;
	cout << "t_enqueue:" << gcone::time_enqueue << endl;
	cout << endl;
	cout << "Checked " << gcone::numberOfFacetChecks << " Facets" << endl;
	cout << " out of which there were " << gcone::parallelButNotEqual << " parallel but not equal." << endl;
#endif
	cout << "Maximum lenght of list of facets: " << gcone::maxSize << endl;

	return lResList;
}

#endif
