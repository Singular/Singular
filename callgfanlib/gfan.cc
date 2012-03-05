/*
Compute the Groebner fan of an ideal
Author: monerjan 
Date: 2009/11/03 06:57:32
*/

#include <kernel/mod2.h>

#ifdef HAVE_FANS
#include <kernel/options.h>
#include <kernel/kstd1.h>
#include <kernel/kutil.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/kmatrix.h>
#include <kernel/GMPrat.h>

//#include "ring.h"        //apparently not needed
#include <Singular/lists.h>
#include <kernel/prCopy.h>
#include <kernel/stairc.h>
#include <fstream>        //read-write cones to files
#include <string>
#include <sstream>
#include <stdlib.h>
#include <assert.h>
#include <gfanlib/gfanlib.h>

/*DO NOT REMOVE THIS*/
#ifndef GMPRATIONAL
#define GMPRATIONAL
#endif

#include <setoper.h>
#include <cdd.h>
#include <cddmp.h>

#ifndef gfan_DEBUG
// #define gfan_DEBUG
#ifndef gfan_DEBUGLEVEL
#define gfan_DEBUGLEVEL 1
#endif
#endif

//NOTE Defining this will slow things down!
//Only good for very coarse profiling
// #define gfanp
#ifdef gfanp
  #include <sys/time.h>
  #include <iostream>
#endif

//NOTE DO NOT REMOVE THIS
#ifndef SHALLOW
  #define SHALLOW
#endif

#ifndef USE_ZFAN
  #define USE_ZFAN
#endif

#include <callgfanlib/gfan.h>
using namespace std;

#define ivIsStrictlyPositive iv64isStrictlyPositive

/**
*\brief Class facet
*        Implements the facet structure as a linked list
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
        this->codim=1;                //default for (codim-1)-facets
        this->numCodim2Facets=0;
        this->numRays=0;
        this->flipGB=NULL;
        this->next=NULL;
        this->prev=NULL;
        this->flipRing=NULL;        //the ring on the other side
        this->isFlippable=FALSE;
}

/** \brief Constructor for facets of codim == 2
* Note that as of now the code of the constructors is only for facets and codim2-faces. One
* could easily change that by renaming numCodim2Facets to numCodimNminusOneFacets or similar
*/
facet::facet(const int &n)
{
        this->fNormal=NULL;
        this->interiorPoint=NULL;
        this->UCN=0;
        this->codim2Ptr=NULL;
        if(n==2)
        {
                this->codim=n;
        }//NOTE Handle exception here!
        this->numCodim2Facets=0;
        this->numRays=0;
        this->flipGB=NULL;
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
        this->fNormal=iv64Copy(f.fNormal);
        this->UCN=f.UCN;
        this->isFlippable=f.isFlippable;
        //Needed for flip2
        //NOTE ONLY REFERENCE
        this->interiorPoint=iv64Copy(f.interiorPoint);//only referencing is prolly not the best thing to do in a copy constructor
        facet* f2Copy;
        f2Copy=f.codim2Ptr;
        facet* f2Act;
        f2Act=this->codim2Ptr;
        while(f2Copy!=NULL)
        {
                if(f2Act==NULL
#ifndef NDEBUG
  #if SIZEOF_LONG==8
                                 || f2Act==(facet*)0xfefefefefefefefe
  #elif SIZEOF_LONG==4
                                 || f2Act==(facet*)0xfefefefe
  #endif
#endif
                  )
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
                int64vec *f2Normal;
                f2Normal = f2Copy->getFacetNormal();
//                 f2Act->setFacetNormal(f2Copy->getFacetNormal());
                f2Act->setFacetNormal(f2Normal);
                delete f2Normal;
                f2Act->setUCN(f2Copy->getUCN());
                f2Copy = f2Copy->next;
        }
}

/** \brief Shallow copy constructor for facets
* We only need the interior point for equality testing
*/
facet* facet::shallowCopy(const facet& f)
{
        facet *res = new facet();
        res->fNormal=(int64vec * const)f.fNormal;
        res->UCN=f.UCN;
        res->isFlippable=f.isFlippable;
        res->interiorPoint=(int64vec * const)f.interiorPoint;
        res->codim2Ptr=(facet * const)f.codim2Ptr;
        res->prev=NULL;
        res->next=NULL;
        res->flipGB=NULL;
        res->flipRing=NULL;
        return res;
}

void facet::shallowDelete()
{
#ifndef NDEBUG
//         printf("shallowdel@UCN %i\n", this->getUCN());
#endif
        this->fNormal=NULL;
//         this->UCN=0;
        this->interiorPoint=NULL;
        this->codim2Ptr=NULL;
        this->prev=NULL;
        this->next=NULL;
        this->flipGB=NULL;
        this->flipRing=NULL;
        assert(this->fNormal==NULL);
//         delete(this);
}

/** The default destructor */
facet::~facet()
{
#ifndef NDEBUG
//         printf("~facet@UCN %i\n",this->getUCN());
#endif
        if(this->fNormal!=NULL)
                delete this->fNormal;
        if(this->interiorPoint!=NULL)
                delete this->interiorPoint;
        /* Cleanup the codim2-structure */
//         if(this->codim==2)
//         {
//                 facet *codim2Ptr;
//                 codim2Ptr = this->codim2Ptr;
//                 while(codim2Ptr!=NULL)
//                 {
//                         if(codim2Ptr->fNormal!=NULL)
//                         {
//                                 delete codim2Ptr->fNormal;//NOTE Do not want this anymore since the rays are now in gcone!
//                                 codim2Ptr = codim2Ptr->next;
//                         }
//                 }
//         }
        //The rays are stored in the cone!
        if(this->flipGB!=NULL)
                idDelete((ideal *)&this->flipGB);
//         if(this->flipRing!=NULL && this->flipRing->idroot!=(idhdl)0xfbfbfbfbfbfbfbfb)
//                 rDelete(this->flipRing); //See vol II/134
//         this->flipRing=NULL;
        this->prev=NULL;
        this->next=NULL;
}

inline const int64vec *facet::getRef2FacetNormal() const
{
        return(this->fNormal);
}

/** Equality check for facets based on unique interior points*/
static bool areEqual2(facet* f, facet *g)
{
#ifdef gfanp
        gcone::numberOfFacetChecks++;
        timeval start, end;
        gettimeofday(&start, 0);
#endif
        bool res = TRUE;
        const int64vec *fIntP = f->getRef2InteriorPoint();
        const int64vec *gIntP = g->getRef2InteriorPoint();
        for(int ii=0;ii<pVariables;ii++)
        {
                if( (*fIntP)[ii] != (*gIntP)[ii] )
                {
                        res=FALSE;
                        break;
                }
        }
//         if( fIntP->compare(gIntP)!=0) res=FALSE;
#ifdef gfanp
        gettimeofday(&end, 0);
        gcone::t_areEqual += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif
        return res;
}

/** \brief Comparison of facets
 * called from enqueueNewFacets
* The facet normals are primitve vectors since we call gcone::normalize() on all cones.
* Hence it should suffice to check whether facet normal f equals minus facet normal s.
* If so we check the extremal rays
*
* BEWARE: It would be better to use const int64vec* but that will lead to call something like
* int foo=((int64vec*)f2Normal)->compare((int64vec*)s2Normal) resulting in much higher memory usage
*/
static bool areEqual(facet *f, facet *s)
{
#ifdef gfanp
        gcone::numberOfFacetChecks++;
        timeval start, end;
        gettimeofday(&start, 0);
#endif
        bool res = TRUE;
        int notParallelCtr=0;
        int ctr=0;
        const int64vec* fNormal; //No new since iv64Copy and therefore getFacetNormal return a new
        const int64vec* sNormal;
        fNormal = f->getRef2FacetNormal();
        sNormal = s->getRef2FacetNormal();
#include <kernel/intvec.h>
        //Do not need parallelity. Too time consuming
//          if(!isParallel(*fNormal,*sNormal))
//         if(fNormal->compare(ivNeg(sNormal))!=0)//This results in a Mandelbug
 //                 notParallelCtr++;
//         else//parallelity, so we check the codim2-facets
        int64vec *fNRef=const_cast<int64vec*>(fNormal);
        int64vec *sNRef=const_cast<int64vec*>(sNormal);
        if(isParallel(*fNRef,*sNRef))
//         if(fNormal->compare((sNormal))!=0)//Behold! Teh definitive Mandelbug
        {
                facet* f2Act;
                facet* s2Act;
                f2Act = f->codim2Ptr;
                ctr=0;
                while(f2Act!=NULL)
                {
                        const int64vec* f2Normal;
                        f2Normal = f2Act->getRef2FacetNormal();
//                         int64vec *f2Ref=const_cast<int64vec*>(f2Normal);
                        s2Act = s->codim2Ptr;
                        while(s2Act!=NULL)
                        {
                                const int64vec* s2Normal;
                                s2Normal = s2Act->getRef2FacetNormal();
//                                 bool foo=areEqual(f2Normal,s2Normal);
//                                 int64vec *s2Ref=const_cast<int64vec*>(s2Normal);
                                int foo=f2Normal->compare(s2Normal);
                                if(foo==0)
                                        ctr++;
                                s2Act = s2Act->next;
                        }
                        f2Act = f2Act->next;
                }
        }
        if(ctr==f->numCodim2Facets)
                res=TRUE;
        else
        {
#ifdef gfanp
                gcone::parallelButNotEqual++;
#endif
                res=FALSE;
        }
#ifdef gfanp
        gettimeofday(&end, 0);
        gcone::t_areEqual += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif
        return res;
}

/** Stores the facet normal \param int64vec*/
inline void facet::setFacetNormal(int64vec *iv)
{
        if(this->fNormal!=NULL)
                delete this->fNormal;
        this->fNormal = iv64Copy(iv);
}

/** Hopefully returns the facet normal
* Mind: iv64Copy returns a new int64vec, so use this in the following way:
* int64vec *iv;
* iv = this->getFacetNormal();
* [...]
* delete(iv);
*/
inline int64vec *facet::getFacetNormal() const
{
        return iv64Copy(this->fNormal);
}

/** Method to print the facet normal*/
inline void facet::printNormal() const
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
#ifndef NDEBUG
  #if SIZEOF_LONG==8
        if((this!=NULL && this!=(facet * const)0xfbfbfbfbfbfbfbfb))
  #elif SIZEOF_LONG==4
        if((this!=NULL && this!=(facet * const)0xfbfbfbfb))
  #endif
#endif
#ifdef NDEBUG
        if(this!=NULL)
#endif
                return this->UCN;
        else
                return -1;
}

/** Store an interior point of the facet */
inline void facet::setInteriorPoint(int64vec *iv)
{
        if(this->interiorPoint!=NULL)
                delete this->interiorPoint;
        this->interiorPoint = iv64Copy(iv);
}

/** Returns a pointer to this->interiorPoint
* MIND: iv64Copy returns a new int64vec
* @see facet::getFacetNormal
*/
inline int64vec *facet::getInteriorPoint()
{
        return iv64Copy(this->interiorPoint);
}

inline const int64vec *facet::getRef2InteriorPoint()
{
        return (this->interiorPoint);
}

/** \brief Debugging function
* prints the facet normal an all (codim-2)-facets that belong to it
*/
volatile void facet::fDebugPrint()
{
  #ifndef NDEBUG
        facet *codim2Act;
        codim2Act = this->codim2Ptr;
        int64vec *fNormal;
        fNormal = this->getFacetNormal();
        printf("=======================\n");
        printf("Facet normal = (");fNormal->show(1,1);printf(")\n");
        printf("-----------------------\n");
        printf("Codim2 facets:\n");
        while(codim2Act!=NULL)
        {
                int64vec *f2Normal;
                f2Normal = codim2Act->getFacetNormal();
                printf("(");f2Normal->show(1,0);printf(")\n");
                codim2Act = codim2Act->next;
                delete f2Normal;
        }
        printf("=======================\n");
         delete fNormal;
  #endif
}


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
        this->facetPtr=NULL;        //maybe this->facetPtr = new facet();
        this->baseRing=currRing;
        this->counter++;
        this->UCN=this->counter;
        this->numFacets=0;
        this->ivIntPt=NULL;
        this->gcRays=NULL;
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
        this->inputIdeal=I;
        this->baseRing=currRing;
        this->counter++;
        this->UCN=this->counter;
        this->pred=1;
        this->numFacets=0;
        this->ivIntPt=NULL;
        this->gcRays=NULL;
}

/** \brief Copy constructor
 *
 * Copies a cone, sets this->gcBasis to the flipped GB
 * Call this only after a successful call to gcone::flip which sets facet::flipGB
*/
gcone::gcone(const gcone& gc, const facet &f)
{
        this->next=NULL;
//         this->prev=(gcone *)&gc; //comment in to get a tree
        this->prev=NULL;
        this->numVars=gc.numVars;
        this->counter++;
        this->UCN=this->counter;
        this->pred=gc.UCN;
        this->facetPtr=NULL;
           this->gcBasis=idrCopyR(f.flipGB, f.flipRing);
//          this->inputIdeal=idCopy(this->gcBasis);
        this->baseRing=rCopy(f.flipRing);
        this->numFacets=0;
        this->ivIntPt=NULL;
        this->gcRays=NULL;
}

/** \brief Default destructor
*/
gcone::~gcone()
{
#ifndef NDEBUG
  #if SIZEOF_LONG==8
        if( ( this->gcBasis!=(ideal)(0xfbfbfbfbfbfbfbfb) ) && (this->gcBasis!=NULL) )
                idDelete((ideal*)&this->gcBasis);
  #elif SIZEOF_LONG!=8
        if(this->gcBasis!=(ideal)0xfbfbfbfb)
                idDelete((ideal *)&this->gcBasis);
  #endif
#else
        if(this->gcBasis!=NULL)
                idDelete((ideal *)&this->gcBasis);
#endif
//         idDelete((ideal *)&this->gcBasis);
//         if(this->inputIdeal!=NULL)
//                 idDelete((ideal *)&this->inputIdeal);
//         if (this->rootRing!=NULL && this->rootRing!=(ip_sring *)0xfefefefefefefefe)
//                 rDelete(this->rootRing);
        if(this->UCN!=1 && this->baseRing!=NULL)
                rDelete(this->baseRing);
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
//         dd_FreeMatrix(this->ddFacets);
        //dd_FreeMatrix(this->ddFacets);
        for(int ii=0;ii<this->numRays;ii++)
                delete(gcRays[ii]);
        omFree(gcRays);
}

/** Returns the number of cones existing at the time*/
inline int gcone::getCounter()
{
        return this->counter;
}

/** \brief Set the interior point of a cone */
inline void gcone::setIntPoint(int64vec *iv)
{
        if(this->ivIntPt!=NULL)
                delete this->ivIntPt;
        this->ivIntPt=iv64Copy(iv);
}

/** \brief Returns either a physical copy the interior point of a cone or just a reference to it.*/
inline int64vec *gcone::getIntPoint(bool shallow)
{
        if(shallow==TRUE)
                return this->ivIntPt;
        else
                return iv64Copy(this->ivIntPt);
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
  #ifndef NDEBUG
        facet *f=this->facetPtr;
        facet *f2=NULL;
        if(codim==2)
                f2=this->facetPtr->codim2Ptr;
        while(f!=NULL)
        {
                int64vec *iv;
                iv = f->getFacetNormal();
                printf("(");iv->show(1,0);
                if(f->isFlippable==FALSE)
                        printf(")* ");
                else
                        printf(") ");
                delete iv;
                if(codim==2)
                {
                        f2=f->codim2Ptr;
                        while(f2!=NULL)
                        {
                                printf("[");f2->getFacetNormal()->show(1,0);printf("]");
                                f2 = f2->next;
                        }
                        printf("\n");
                }
                f=f->next;
        }
        printf("\n");
  #endif
}

/** For debugging purposes only */
static volatile void showSLA(facet &f)
{
  #ifndef NDEBUG
        facet *fAct;
        fAct = &f;
        if(fAct!=NULL)
        {
                facet *codim2Act;
                codim2Act = fAct->codim2Ptr;

                printf("\n");
                while(fAct!=NULL)
                {
                        int64vec *fNormal;
                        fNormal=fAct->getFacetNormal();
                        printf("(");fNormal->show(1,0);
                        if(fAct->isFlippable==TRUE)
                                printf(") ");
                        else
                                printf(")* ");
                        delete fNormal;
                        codim2Act = fAct->codim2Ptr;
                        printf(" Codim2: ");
                        while(codim2Act!=NULL)
                        {
                                int64vec *f2Normal;
                                f2Normal = codim2Act->getFacetNormal();
                                printf("(");f2Normal->show(1,0);printf(") ");
                                delete f2Normal;
                                codim2Act = codim2Act->next;
                        }
                        printf("UCN = %i\n",fAct->getUCN());
                        fAct = fAct->next;
                }
        }
  #endif
}

static void idDebugPrint(const ideal &I)
{
  #ifndef NDEBUG
        int numElts=IDELEMS(I);
        printf("Ideal with %i generators\n", numElts);
        printf("Leading terms: ");
        for (int ii=0;ii<numElts;ii++)
        {
                pWrite0(pHead(I->m[ii]));
                printf(",");
        }
        printf("\n");
  #endif
}

static void invPrint(const ideal &I)
{
//         int numElts=IDELEMS(I);
//         cout << "inv = ";
//         for(int ii=0;ii<numElts;ii++);
//         {
//                 pWrite0(pHead(I->m[ii]));
//                 cout << ",";
//         }
//         cout << endl;
}

static bool isMonomial(const ideal &I)
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
/** Returns a copy of the this->baseRing */
inline ring gcone::getBaseRing()
{
        return rCopy(this->baseRing);
}

inline void gcone::setBaseRing(ring r)
{
        this->baseRing=rCopy(r);
}

inline ring gcone::getRef2BaseRing()
{
        return this->baseRing;
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
void gcone::getConeNormals(const ideal &I, bool compIntPoint)
{
#ifdef gfanp
        timeval start, end;
        gettimeofday(&start, 0);
#endif
        poly aktpoly;
        int rows;                         // will contain the dimensions of the ineq matrix - deprecated by
        dd_rowrange ddrows;
        dd_colrange ddcols;
        dd_rowset ddredrows;                // # of redundant rows in ddineq
        dd_rowset ddlinset;                // the opposite
        dd_rowindex ddnewpos=NULL;                  // all to make dd_Canonicalize happy
        dd_NumberType ddnumb=dd_Integer; //Number type
        dd_ErrorType dderr=dd_NoError;
        //Compute the # inequalities i.e. rows of the matrix
        rows=0; //Initialization
        for (int ii=0;ii<IDELEMS(I);ii++)
        {
//                 aktpoly=(poly)I->m[ii];
//                 rows=rows+pLength(aktpoly)-1;
                rows=rows+pLength((poly)I->m[ii])-1;
        }

        dd_rowrange aktmatrixrow=0;        // needed to store the diffs of the expvects in the rows of ddineq
        ddrows=rows;
        ddcols=this->numVars;
        dd_MatrixPtr ddineq;                 //Matrix to store the inequalities
        ddineq=dd_CreateMatrix(ddrows,ddcols+1); //The first col has to be 0 since cddlib checks for additive consts there

        // We loop through each g\in GB and compute the resulting inequalities
        for (int i=0; i<IDELEMS(I); i++)
        {
                aktpoly=(poly)I->m[i];                //get aktpoly as i-th component of I
                //simpler version of storing expvect diffs
                int *leadexpv=(int*)omAlloc(((this->numVars)+1)*sizeof(int));
                pGetExpV(aktpoly,leadexpv);
                poly pNextTerm=aktpoly;
                while(pNext(pNextTerm)/*pNext(aktpoly)*/!=NULL)
                {
                        pNextTerm/*aktpoly*/=pNext(pNextTerm);
                        int *tailexpv=(int*)omAlloc(((this->numVars)+1)*sizeof(int));
                        pGetExpV(pNextTerm,tailexpv);
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
//         ideal initialForm=idInit(IDELEMS(I),1);
//         int64vec *gamma=new int64vec(this->numVars);
        int falseGammaCounter=0;
        int *redRowsArray=NULL;
        int num_alloc=0;
        int num_elts=0;
        for(int ii=0;ii<ddineq->rowsize;ii++)
        {
                ideal initialForm=idInit(IDELEMS(I),I->rank);
                //read row ii into gamma
//                 int64 tmp;
                int64vec *gamma=new int64vec(this->numVars);
                for(int jj=1;jj<=this->numVars;jj++)
                {
                        int64 tmp;
                        tmp=(int64)mpq_get_d(ddineq->matrix[ii][jj]);
                        (*gamma)[jj-1]=(int64)tmp;
                }
                computeInv((ideal&)I,initialForm,*gamma);
                delete gamma;
                //Create leading ideal
                ideal L=idInit(IDELEMS(initialForm),1);
                for(int jj=0;jj<IDELEMS(initialForm);jj++)
                {
                        poly p=pHead(initialForm->m[jj]);
                        L->m[jj]=pCopy(/*pHead(initialForm->m[jj]))*/p);
                        pDelete(&p);
                }

                LObject *P = new sLObject();//TODO What's the difference between sLObject and LObject?
                memset(P,0,sizeof(LObject));

                for(int jj=0;jj<=IDELEMS(initialForm)-2;jj++)
                {
                        bool isMaybeFacet=FALSE;
                        P->p1=initialForm->m[jj];        //build spolys of initialForm in_v

                        for(int kk=jj+1;kk<=IDELEMS(initialForm)-1;kk++)
                        {
                                P->p2=initialForm->m[kk];
                                ksCreateSpoly(P);
                                if(P->p!=NULL)        //spoly non zero=?
                                {
                                        poly p;//NOTE Don't use pInit here. Evil memleak will follow
                                        poly q;
                                        poly pDel,qDel;
                                        p=pCopy(P->p);
                                        q=pHead(p);        //Monomial q
                                        pDelete(&q);
                                        pDel=p; qDel=q;
                                        isMaybeFacet=FALSE;
                                        //TODO: Suffices to check LTs here
                                        while(p!=NULL)
                                        {
                                                q=pHead(p);
                                                for(int ll=0;ll<IDELEMS(L);ll++)
                                                {
                                                        if(pLmEqual(L->m[ll],q) || pDivisibleBy(L->m[ll],q))
                                                        {
                                                                isMaybeFacet=TRUE;
                                                                 break;//for
                                                        }
                                                }
                                                pDelete(&q);
                                                if(isMaybeFacet==TRUE)
                                                {
                                                        break;//while(p!=NULL)
                                                }
                                                p=pNext(p);
                                        }//while
//                                         pDelete(&p);//NOTE Better to use pDel and qDel. Commenting in this line will not work!
                                        if(q!=NULL) pDelete(&q);
                                        pDelete(&pDel);
                                        pDelete(&qDel);
                                        if(isMaybeFacet==FALSE)
                                        {
                                                dd_set_si(ddineq->matrix[ii][0],1);
//                                                 if(num_alloc==0)
//                                                         num_alloc += 1;
//                                                 else
//                                                         num_alloc += 1;
                                                if(num_alloc==num_elts)        num_alloc==0 ? num_alloc=1 : num_alloc*=2;

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
                                                goto _start;        //mea culpa, mea culpa, mea maxima culpa
                                        }
                                }//if(P->p!=NULL)
                                pDelete(&(P->p));
                        }//for k
                }//for jj
                _start:;
                idDelete(&L);
                delete P;
                idDelete(&initialForm);
        }//for(ii<ddineq-rowsize
//         delete gamma;
        int offset=0;//needed for correction of redRowsArray[ii]
#ifndef NDEBUG
        printf("Removed %i of %i in preprocessing step\n",num_elts,ddineq->rowsize);
#endif
        for( int ii=0;ii<num_elts;ii++ )
        {
                dd_MatrixRowRemove(&ddineq,redRowsArray[ii]+1-offset);//cddlib sucks at enumeration
                offset++;
        }
        free(redRowsArray);//NOTE May crash on some machines.
        /*And now for the strictly positive rows
        * Doesn't gain significant speedup
        */
        /*int *posRowsArray=NULL;
        num_alloc=0;
        num_elts=0;
        for(int ii=0;ii<ddineq->rowsize;ii++)
        {
                int64vec *ivPos = new int64vec(this->numVars);
                for(int jj=0;jj<this->numVars;jj++)
                        (*ivPos)[jj]=(int)mpq_get_d(ddineq->matrix[ii][jj+1]);
                bool isStrictlyPos=FALSE;
                int posCtr=0;
                for(int jj=0;jj<this->numVars;jj++)
                {
                        int64vec *ivCanonical = new int64vec(this->numVars);
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
        ddrows = ddineq->rowsize;        //Size of the matrix with redundancies removed
        ddcols = ddineq->colsize;

        this->ddFacets = dd_CopyMatrix(ddineq);

        /*Write the normals into class facet*/
        facet *fAct;         //pointer to active facet
        int numNonFlip=0;
        for (int kk = 0; kk<ddrows; kk++)
        {
                int64 ggT=1;//NOTE Why does (int)mpq_get_d(ddineq->matrix[kk][1]) not work?
                int64vec *load = new int64vec(this->numVars);//int64vec to store a single facet normal that will then be stored via setFacetNormal
                for (int jj = 1; jj <ddcols; jj++)
                {
                        int64 val;
                        val = (int64)mpq_get_d(ddineq->matrix[kk][jj]);
                        (*load)[jj-1] = val;        //store typecasted entry at pos jj-1 of load
                        ggT = int64gcd(ggT,/*(int64&)foo*/val);
                }//for (int jj = 1; jj <ddcols; jj++)
                if(ggT>1)
                {
                        for(int ll=0;ll<this->numVars;ll++)
                                (*load)[ll] /= ggT;//make primitive vector
                }
                /*Quick'n'dirty hack for flippability. Executed only if gcone::hasHomInput==FALSE
                * Otherwise every facet intersects the positive orthant
                */
                if(gcone::hasHomInput==FALSE)
                {
                        //TODO: No dP needed
                        bool isFlip=FALSE;
                        for(int jj = 0; jj<load->length(); jj++)
                        {
//                                 int64vec *ivCanonical = new int64vec(load->length());
//                                 (*ivCanonical)[jj]=1;
//                                 if (dotProduct(*load,*ivCanonical)<0)
//                                 {
//                                         isFlip=TRUE;
//                                         break;        //URGHS
//                                 }
//                                 delete ivCanonical;
                                if((*load)[jj]<0)
                                {
                                        isFlip=TRUE;
                                        break;
                                }
                        }/*End of check for flippability*/
//                         if(iv64isStrictlyPositive(load))
//                                 isFlip=TRUE;
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
#ifndef NDEBUG
                                printf("Marking facet (");load->show(1,0);printf(") as non flippable\n");
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
                else        //Every facet is flippable
                {        /*Now load should be full and we can call setFacetNormal*/
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
//                 exit(-1);//Bit harsh maybe...
        }

        /*
        Now we should have a linked list containing the facet normals of those facets that are
        -irredundant
        -flipable
        Adressing is done via *facetPtr
        */
        if (compIntPoint==TRUE)
        {
                int64vec *iv = new int64vec(this->numVars);
                dd_MatrixPtr posRestr=dd_CreateMatrix(this->numVars,this->numVars+1);
                int jj=1;
                for (int ii=0;ii<=this->numVars;ii++)
                {
                        dd_set_si(posRestr->matrix[ii][jj],1);
                        jj++;
                }
                dd_MatrixAppendTo(&ddineq,posRestr);
                interiorPoint(ddineq, *iv);        //NOTE ddineq contains non-flippable facets
                this->setIntPoint(iv);        //stores the interior point in gcone::ivIntPt
                delete iv;
                dd_FreeMatrix(posRestr);
        }
        //Clean up but don't delete the return value!
        //dd_FreeMatrix(ddineq);
        set_free(ddredrows);//check
        set_free(ddlinset);//check
        //free(ddnewpos);//<-- NOTE Here the crash occurs omAlloc issue?
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
void gcone::getCodim2Normals(const gcone &gc)
{
#ifdef gfanp
        timeval start, end;
          gettimeofday(&start, 0);
#endif
        //this->facetPtr->codim2Ptr = new facet(2);        //instantiate a (codim-2)-facet
        facet *fAct;
        fAct = this->facetPtr;
        facet *codim2Act;
        //codim2Act = this->facetPtr->codim2Ptr;
        dd_MatrixPtr ddineq;//,P,ddakt;
        dd_ErrorType err;
        //ddineq = facets2Matrix(gc);        //get a matrix representation of the cone
        ddineq = dd_CopyMatrix(gc.ddFacets);
        /*Now set appropriate linearity*/
        for (int ii=0; ii<this->numFacets; ii++)
        {
                dd_rowset impl_linset, redset;
                dd_rowindex newpos;
                dd_MatrixPtr ddakt;
                  ddakt = dd_CopyMatrix(ddineq);
//                 ddakt->representation=dd_Inequality;        //Not using this makes it faster. But why does the quick check below still work?
//                 ddakt->representation=dd_Generator;
                set_addelem(ddakt->linset,ii+1);/*Now set appropriate linearity*/
#ifdef gfanp
                timeval t_ddMC_start, t_ddMC_end;
                gettimeofday(&t_ddMC_start,0);
#endif
                 //dd_MatrixCanonicalize(&ddakt, &impl_linset, &redset, &newpos, &err);
                dd_PolyhedraPtr ddpolyh;
                ddpolyh=dd_DDMatrix2Poly(ddakt, &err);
//                 ddpolyh=dd_DDMatrix2Poly2(ddakt, dd_MaxCutoff, &err);
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
                        int64vec *n = new int64vec(this->numVars);
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
//                 int64vec *iv_intPoint = new int64vec(this->numVars);
//                 dd_MatrixPtr shiftMatrix;
//                 dd_MatrixPtr intPointMatrix;
//                 shiftMatrix = dd_CreateMatrix(this->numVars,this->numVars+1);
//                 for(int kk=0;kk<this->numVars;kk++)
//                 {
//                         dd_set_si(shiftMatrix->matrix[kk][0],1);
//                         dd_set_si(shiftMatrix->matrix[kk][kk+1],1);
//                 }
//                 intPointMatrix=dd_MatrixAppend(ddakt,shiftMatrix);
// #ifdef gfanp
//                 timeval t_iP_start, t_iP_end;
//                 gettimeofday(&t_iP_start, 0);
// #endif
//                 interiorPoint(intPointMatrix,*iv_intPoint);
// //                 dd_rowset impl_linste,lbasis;
// //                 dd_LPSolutionPtr lps=NULL;
// //                 dd_ErrorType err;
// //                 dd_FindRelativeInterior(intPointMatrix, &impl_linset, &lbasis, &lps, &err);
// #ifdef gfanp
//                 gettimeofday(&t_iP_end, 0);
//                 t_iP += (t_iP_end.tv_sec - t_iP_start.tv_sec + 1e-6*(t_iP_end.tv_usec - t_iP_start.tv_usec));
// #endif
//                 for(int ll=0;ll<this->numVars;ll++)
//                 {
//                         if( (*iv_intPoint)[ll] < 0 )
//                         {
//                                 fAct->isFlippable=FALSE;
//                                 break;
//                         }
//                 }
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
* We use ivAdd here which returns a new int64vec. Therefore we need to avoid
* a memory leak which would be cause by the line
* iv=ivAdd(iv,b)
* So we keep pointer tmp to iv and delete(tmp), so there should not occur a
* memleak
* TODO normalization
*/
void gcone::getExtremalRays(const gcone &gc)
{
#ifdef gfanp
        timeval start, end;
        gettimeofday(&start, 0);
        timeval poly_start, poly_end;
        gettimeofday(&poly_start,0);
#endif
        //Add lineality space - dd_LinealitySpace
        dd_MatrixPtr ddineq;
        dd_ErrorType err;
        ddineq = (dd_LinealitySpace->rowsize>0) ? dd_AppendMatrix(gc.ddFacets,gcone::dd_LinealitySpace) : dd_CopyMatrix(gc.ddFacets);
        /* In case the input is non-homogeneous we add constrains for the positive orthant.
        * This is justified by the fact that for non-homog ideals we only consider the
        * restricted fan. This way we can be sure to find strictly positive interior points.
        * This in turn makes life easy when checking for flippability!
        * Drawback: Makes the LP larger so probably slows down computations a wee bit.
        */
        dd_MatrixPtr ddPosRestr;
        if(hasHomInput==FALSE)
        {
                dd_MatrixPtr tmp;
                ddPosRestr=dd_CreateMatrix(this->numVars,this->numVars+1);
                for(int ii=0;ii<this->numVars;ii++)
                        dd_set_si(ddPosRestr->matrix[ii][ii+1],1);
                dd_MatrixAppendTo(&ddineq,ddPosRestr);
                assert(ddineq);
                dd_FreeMatrix(ddPosRestr);
        }
        dd_PolyhedraPtr ddPolyh;
        ddPolyh = dd_DDMatrix2Poly(ddineq, &err);
        dd_MatrixPtr P;
        P=dd_CopyGenerators(ddPolyh);//Here we actually compute the rays!
        dd_FreePolyhedra(ddPolyh);
        dd_FreeMatrix(ddineq);
#ifdef gfanp
        gettimeofday(&poly_end,0);
        t_ddPolyh += (poly_end.tv_sec - poly_start.tv_sec + 1e-6*(poly_end.tv_usec - poly_start.tv_usec));
#endif
        /* Compute interior point on the fly*/
        int64vec *ivIntPointOfCone = new int64vec(this->numVars);
        int64vec *foo = new int64vec(this->numVars);
        for(int ii=0;ii<P->rowsize;ii++)
        {
                int64vec *tmp = ivIntPointOfCone;
                makeInt(P,ii+1,*foo);
                ivIntPointOfCone = iv64Add(ivIntPointOfCone,foo);
                delete tmp;
        }
        delete foo;
        int64 ggT=(*ivIntPointOfCone)[0];
        for (int ii=0;ii<(this->numVars);ii++)
        {
                if( (*ivIntPointOfCone)[ii]>INT_MAX )
                        WarnS("Interior point exceeds INT_MAX!\n");
                //Compute intgcd
                ggT=int64gcd(ggT,(*ivIntPointOfCone)[ii]);
        }

        //Divide out a common gcd > 1
        if(ggT>1)
        {
                for(int ii=0;ii<this->numVars;ii++)
                {
                        (*ivIntPointOfCone)[ii] /= ggT;
                        if( (*ivIntPointOfCone)[ii]>INT_MAX ) WarnS("Interior point still exceeds INT_MAX after GCD!\n");
                }
        }

        /*For homogeneous input (like Det3,3,5) the int points may be negative. So add a suitable multiple of (1,_,1)*/
        if(hasHomInput==TRUE && iv64isStrictlyPositive(ivIntPointOfCone)==FALSE)
        {
                int64vec *ivOne = new int64vec(this->numVars);
                int maxNegEntry=0;
                for(int ii=0;ii<this->numVars;ii++)
                {
//                         (*ivOne)[ii]=1;
                        if ((*ivIntPointOfCone)[ii]<maxNegEntry) maxNegEntry=(*ivIntPointOfCone)[ii];
                }
                maxNegEntry *= -1;
                maxNegEntry++;//To be on the safe side
                for(int ii=0;ii<this->numVars;ii++)
                        (*ivOne)[ii]=maxNegEntry;
                int64vec *tmp=ivIntPointOfCone;
                ivIntPointOfCone=iv64Add(ivIntPointOfCone,ivOne);
                delete(tmp);
//                 while( !iv64isStrictlyPositive(ivIntPointOfCone) )
//                 {
//                         int64vec *tmp = ivIntPointOfCone;
//                         for(int jj=0;jj<this->numVars;jj++)
//                                 (*ivOne)[jj] = (*ivOne)[jj] << 1; //times 2
//                         ivIntPointOfCone = ivAdd(ivIntPointOfCone,ivOne);
//                         delete tmp;
//                 }
                delete ivOne;
                int64 ggT=(*ivIntPointOfCone)[0];
                for(int ii=0;ii<this->numVars;ii++)
                        ggT=int64gcd( ggT, (*ivIntPointOfCone)[ii]);
                if(ggT>1)
                {
                        for(int jj=0;jj<this->numVars;jj++)
                                (*ivIntPointOfCone)[jj] /= ggT;
                }
        }
//         assert(iv64isStrictlyPositive(ivIntPointOfCone));

        this->setIntPoint(ivIntPointOfCone);
        delete(ivIntPointOfCone);
        /* end of interior point computation*/

        //Loop through the rows of P and check whether fNormal*row[i]=0 => row[i] belongs to fNormal
        int rows=P->rowsize;
        facet *fAct=gc.facetPtr;
        //Construct an array to hold the extremal rays of the cone
        this->gcRays = (int64vec**)omAlloc0(sizeof(int64vec*)*P->rowsize);
        for(int ii=0;ii<P->rowsize;ii++)
        {
                int64vec *rowvec = new int64vec(this->numVars);
                makeInt(P,ii+1,*rowvec);//get an integer entry instead of rational, rowvec is primitve
                this->gcRays[ii] = iv64Copy(rowvec);
                delete rowvec;
        }
        this->numRays=P->rowsize;
        //Check which rays belong to which facet
        while(fAct!=NULL)
        {
                const int64vec *fNormal;// = new int64vec(this->numVars);
                fNormal = fAct->getRef2FacetNormal();//->getFacetNormal();
                int64vec *ivIntPointOfFacet = new int64vec(this->numVars);
                for(int ii=0;ii<rows;ii++)
                {
                         if(dotProduct(*fNormal,this->gcRays[ii])==0)
                        {
                                int64vec *tmp = ivIntPointOfFacet;//Prevent memleak
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
                                //codim2Act->setFacetNormal(rowvec);
                                //Rather just let codim2Act point to the corresponding int64vec of gcRays
                                codim2Act->fNormal=this->gcRays[ii];
                                fAct->numRays++;
                                //Memleak avoided via tmp
                                ivIntPointOfFacet=iv64Add(ivIntPointOfFacet,this->gcRays[ii]);
                                //Now tmp still points to the OLD address of ivIntPointOfFacet
                                delete(tmp);

                        }
                }//For non-homog input ivIntPointOfFacet should already be >0 here
//                 if(!hasHomInput) {assert(iv64isStrictlyPositive(ivIntPointOfFacet));}
                //if we have no strictly pos ray but the input is homogeneous
                //then add a suitable multiple of (1,...,1)
                if( !iv64isStrictlyPositive(ivIntPointOfFacet) && hasHomInput==TRUE)
                {
                        int64vec *ivOne = new int64vec(this->numVars);
                        for(int ii=0;ii<this->numVars;ii++)
                                (*ivOne)[ii]=1;
                        while( !iv64isStrictlyPositive(ivIntPointOfFacet) )
                        {
                                int64vec *tmp = ivIntPointOfFacet;
                                for(int jj=0;jj<this->numVars;jj++)
                                {
                                        (*ivOne)[jj] = (*ivOne)[jj] << 1; //times 2
                                }
                                ivIntPointOfFacet = iv64Add(ivIntPointOfFacet/*diff*/,ivOne);
                                delete tmp;
                        }
                        delete ivOne;
                }
                int64 ggT=(*ivIntPointOfFacet)[0];
                for(int ii=0;ii<this->numVars;ii++)
                        ggT=int64gcd(ggT,(*ivIntPointOfFacet)[ii]);
                if(ggT>1)
                {
                        for(int ii=0;ii<this->numVars;ii++)
                                (*ivIntPointOfFacet)[ii] /= ggT;
                }
                fAct->setInteriorPoint(ivIntPointOfFacet);

                delete(ivIntPointOfFacet);
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
//                                                 tmp=NULL;
                                        }
                                }
                                n--;
                        }while(exchanged==TRUE && n>=0);
                }*///if pVariables>2
//                 delete fNormal;
                fAct = fAct->next;
        }//end of facet checking
        dd_FreeMatrix(P);
        //Now all extremal rays should be set w.r.t their respective fNormal
        //TODO Not sufficient -> vol2 II/125&127
        //NOTE Sufficient according to cddlibs doc. These ARE rays
        //What the hell... let's just take interior points
        if(gcone::hasHomInput==FALSE)
        {
                fAct=gc.facetPtr;
                while(fAct!=NULL)
                {
//                         bool containsStrictlyPosRay=FALSE;
//                         facet *codim2Act;
//                         codim2Act = fAct->codim2Ptr;
//                         while(codim2Act!=NULL)
//                         {
//                                 int64vec *rayvec;
//                                 rayvec = codim2Act->getFacetNormal();//Mind this is no normal but a ray!
//                                 //int negCtr=0;
//                                 if(iv64isStrictlyPositive(rayvec))
//                                 {
//                                         containsStrictlyPosRay=TRUE;
//                                         delete(rayvec);
//                                         break;
//                                 }
//                                 delete(rayvec);
//                                 codim2Act = codim2Act->next;
//                         }
//                         if(containsStrictlyPosRay==FALSE)
//                                 fAct->isFlippable=FALSE;
                        if(!iv64isStrictlyPositive(fAct->interiorPoint))
                                fAct->isFlippable=FALSE;
                        fAct = fAct->next;
                }
        }//hasHomInput?
#ifdef gfanp
        gettimeofday(&end, 0);
        t_getExtremalRays += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif
}

/** Order the spanning rays in a lex way hopefully using qsort()*/
void gcone::orderRays()
{
//   qsort(gcRays,sizeof(int64vec),int64vec::compare);
}

inline bool gcone::iv64isStrictlyPositive(const int64vec * iv64)
{
        bool res=TRUE;
        for(int ii=0;ii<iv64->length();ii++)
        {
                if((*iv64)[ii]<=0)
                {
                        res=FALSE;
                        break;
                }
        }
        return res;
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
inline void gcone::flip(ideal gb, facet *f)                //Compute "the other side"
{
#ifdef gfanp
        timeval start, end;
        gettimeofday(&start, 0);
#endif
        int64vec *fNormal;// = new int64vec(this->numVars);        //facet normal, check for parallelity
        fNormal = f->getFacetNormal();        //read this->fNormal;
#ifndef NDEBUG
//         std::cout << "running gcone::flip" << std::endl;
        printf("flipping UCN %i over facet",this->getUCN());
        fNormal->show(1,0);
        printf(") with UCN %i\n",f->getUCN() );
#endif
        if(this->getUCN() != f->getUCN())
        {
                WerrorS("Uh oh... Trying to flip over facet with incompatible UCN");
                exit(-1);
        }
        /*1st step: Compute the initial ideal*/
        /*poly initialFormElement[IDELEMS(gb)];*/        //array of #polys in GB to store initial form
        ideal initialForm=idInit(IDELEMS(gb),this->gcBasis->rank);

        computeInv(gb,initialForm,*fNormal);

#ifndef NDEBUG
/*        cout << "Initial ideal is: " << endl;
        idShow(initialForm);
        //f->printFlipGB();*/
//         cout << "===" << endl;
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
                int64vec *iv;// = new int64vec(this->numVars);
                iv = ivNeg(fNormal);//ivNeg uses iv64Copy -> new
//                 tmpRing=rCopyAndAddWeight(srcRing,ivNeg(fNormal));
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
//          H=kStd(ina,NULL,isHomog,NULL);        //we know it is homogeneous
#ifdef gfanp
        timeval t_kStd_start, t_kStd_end;
        gettimeofday(&t_kStd_start,0);
#endif
        if(gcone::hasHomInput==TRUE)
                H=kStd(ina,NULL,isHomog,NULL/*,gcone::hilbertFunction*/);
        else
                H=kStd(ina,NULL,isNotHomog,NULL);        //This is \mathcal(G)_{>_-\alpha}(in_v(I))
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
        intPointMatrix->numbtype=dd_Integer;        //NOTE: DO NOT REMOVE OR CHANGE TO dd_Rational

        for (int ii=0;ii<IDELEMS(srcRing_HH);ii++)
        {
                markingsAreCorrect=FALSE;        //crucial to initialise here
                poly aktpoly=srcRing_HH->m[ii]; //Only a pointer, so don't pDelete
                /*Comparison of leading monomials is done via exponent vectors*/
                for (int jj=0;jj<IDELEMS(H);jj++)
                {
                        int *src_ExpV = (int *)omAlloc((this->numVars+1)*sizeof(int));
                        int *dst_ExpV = (int *)omAlloc((this->numVars+1)*sizeof(int));
                        pGetExpV(aktpoly,src_ExpV);
                        rChangeCurrRing(tmpRing);        //this ring change is crucial!
                        poly p=pCopy(H->m[ii]);
                        pGetExpV(p/*pCopy(H->m[ii])*/,dst_ExpV);
                        pDelete(&p);
                        rChangeCurrRing(srcRing);
                        bool expVAreEqual=TRUE;
                        for (int kk=1;kk<=this->numVars;kk++)
                        {
#ifndef NDEBUG
//                                 cout << src_ExpV[kk] << "," << dst_ExpV[kk] << endl;
#endif
                                if (src_ExpV[kk]!=dst_ExpV[kk])
                                {
                                        expVAreEqual=FALSE;
                                }
                        }
                        if (expVAreEqual==TRUE)
                        {
                                markingsAreCorrect=TRUE; //everything is fine
#ifndef NDEBUG
//                                 cout << "correct markings" << endl;
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
//                         if(ctr==this->numVars)//We have a 0-row
//                                 dd_MatrixRowRemove(&intPointMatrix,aktrow);
//                         else
                                aktrow +=1;
                        omFree(v);
                }
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
//         dd_set_si(intPointMatrix->matrix[aktrow][0],-1);
//         for (int jj=1;jj<=this->numVars;jj++)
//         {
//                 dd_set_si(intPointMatrix->matrix[aktrow][jj],1);
//         }
        //Let's make sure we compute interior points from the positive orthant
//         dd_MatrixPtr posRestr=dd_CreateMatrix(this->numVars,this->numVars+1);
//
//         int jj=1;
//         for (int ii=0;ii<this->numVars;ii++)
//         {
//                 dd_set_si(posRestr->matrix[ii][jj],1);
//                 jj++;
//         }
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

        int64vec *iv_weight = new int64vec(this->numVars);
#ifdef gfanp
        timeval t_dd_start, t_dd_end;
        gettimeofday(&t_dd_start, 0);
#endif
        dd_ErrorType err;
        dd_rowset implLin, redrows;
        dd_rowindex newpos;

        //NOTE Here we should remove interiorPoint and instead
        // create and ordering like (a(omega),a(fNormal),dp)
//         if(this->ivIntPt==NULL)
                interiorPoint(intPointMatrix, *iv_weight);        //iv_weight now contains the interior point
//         else
//                 iv_weight=this->getIntPoint();
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
                 int64vec *iv_row=new int64vec(this->numVars);
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
#ifndef NDEBUG
//         test|=Sy_bit(6);        //OPT_DEBUG
#endif
        ideal tmpI;
        //NOTE Any of the two variants of tmpI={idrCopy(),dstRing_I} does the trick
        //tmpI = idrCopyR(this->inputIdeal,this->baseRing);
        tmpI = dstRing_I;
#ifdef gfanp
        gettimeofday(&t_kStd_start,0);
#endif
        if(gcone::hasHomInput==TRUE)
                dstRing_I=kStd(tmpI,NULL,isHomog,NULL/*,gcone::hilbertFunction*/);
        else
                dstRing_I=kStd(tmpI,NULL,isNotHomog,NULL);
#ifdef gfanp
        gettimeofday(&t_kStd_end, 0);
        t_kStd += (t_kStd_end.tv_sec - t_kStd_start.tv_sec + 1e-6*(t_kStd_end.tv_usec - t_kStd_start.tv_usec));
#endif
        idDelete(&tmpI);
        idNorm(dstRing_I);
//         kInterRed(dstRing_I);
        idSkipZeroes(dstRing_I);
        test=save;
        /*End of step 3 - reduction*/

        f->setFlipGB(dstRing_I);//store the flipped GB
//         idDelete(&dstRing_I);
        f->flipRing=rCopy(dstRing);        //store the ring on the other side
#ifndef NDEBUG
         printf("Flipped GB is UCN %i:\n",counter+1);
         idDebugPrint(dstRing_I);
        printf("\n");
#endif
        idDelete(&dstRing_I);
        rChangeCurrRing(srcRing);        //return to the ring we started the computation of flipGB in
         rDelete(dstRing);
#ifdef gfanp
        gettimeofday(&end, 0);
        time_flip += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif
}//void flip(ideal gb, facet *f)

/** \brief A slightly different approach to flipping
* Here we use the fact that in_v(in_u(I))=in_(u+eps*v)(I). Therefore, we do no longer
* need to compute an interior point and run BBA on the minimal basis but we can rather
* use the ordering (a(omega),a(fNormal),dp)
* The second parameter facet *f must not be const since we need to store f->flipGB
* Problem: Assume we start in a cone with ordering (dp,C). Then \f$ in_\omega(I) \f$
* will be from a ring with (a(),dp,C) and our resulting cone from (a(),a(),dp,C). Hence a way
* must be found to circumvent the sequence of a()'s growing to a ridiculous size.
* Therefore: We use (a(),a(),dp,C) for the computation of the reduced basis. But then we
* do have an interior point of the cone by adding the extremal rays. So we replace
* the latter cone by a cone with (a(sum_of_rays),dp,C).
* Con: It's incredibly ugly
* Pro: No messing around with readConeFromFile()
* Is there a way to construct a vector from \f$ \omega \f$ and the facet normal?
*/
inline void gcone::flip2(const ideal &gb, facet *f)
{
#ifdef gfanp
        timeval start, end;
        gettimeofday(&start, 0);
#endif
        const int64vec *fNormal;
        fNormal = f->getRef2FacetNormal();/*->getFacetNormal();*/        //read this->fNormal;
#ifndef NDEBUG
//         printf("flipping UCN %i over facet(",this->getUCN());
//         fNormal->show(1,0);
//         printf(") with UCN %i\n",f->getUCN());
#endif
        if(this->getUCN() != f->getUCN())
        {        printf("%i vs %i\n",this->getUCN(), f->getUCN() );
                WerrorS("Uh oh... Trying to flip over facet with incompatible UCN");
                exit(-1);
        }
        /*1st step: Compute the initial ideal*/
        ideal initialForm=idInit(IDELEMS(gb),this->gcBasis->rank);
        computeInv( gb, initialForm, *fNormal );
        ring srcRing=currRing;
        ring tmpRing;

        const int64vec *intPointOfFacet;
        intPointOfFacet=f->getInteriorPoint();
        //Now we need two blocks of ringorder_a!
        //May assume the same situation as in flip() here
        if( (srcRing->order[0]!=ringorder_a/*64*/) && (srcRing->order[1]!=ringorder_a/*64*/) )
        {
                int64vec *iv = new int64vec(this->numVars);//init with 1s, since we do not need a 2nd block here but later
//                 int64vec *iv_foo = new int64vec(this->numVars,1);//placeholder
                int64vec *ivw = ivNeg(const_cast<int64vec*>(fNormal));
                tmpRing=rCopyAndAddWeight2(srcRing,ivw/*intPointOfFacet*/,iv);
                delete iv;delete ivw;
//                 delete iv_foo;
        }
        else
        {
                int64vec *iv=new int64vec(this->numVars);
                int64vec *ivw=ivNeg(const_cast<int64vec*>(fNormal));
                tmpRing=rCopyAndAddWeight2(srcRing,ivw,iv);
                delete iv; delete ivw;
                /*tmpRing=rCopy0(srcRing);
                int length=fNormal->length();
                int *A1=(int *)omAlloc0(length*sizeof(int));
                int *A2=(int *)omAlloc0(length*sizeof(int));
                for(int jj=0;jj<length;jj++)
                {
                        A1[jj] = -(*fNormal)[jj];
                        A2[jj] = 1;//-(*fNormal)[jj];//NOTE Do we need this here? This is only the facet ideal
                }
                omFree(tmpRing->wvhdl[0]);
                if(tmpRing->wvhdl[1]!=NULL)
                        omFree(tmpRing->wvhdl[1]);
                tmpRing->wvhdl[0]=(int*)A1;
                tmpRing->block1[0]=length;
                tmpRing->wvhdl[1]=(int*)A2;
                tmpRing->block1[1]=length;
                rComplete(tmpRing);*/
        }
//         delete fNormal; //NOTE Do not delete when using getRef2FacetNormal();
        rChangeCurrRing(tmpRing);
        //Now currRing should have (a(),a(),dp,C)
        ideal ina;
        ina=idrCopyR(initialForm,srcRing);
        idDelete(&initialForm);
        ideal H;
#ifdef gfanp
        timeval t_kStd_start, t_kStd_end;
        gettimeofday(&t_kStd_start,0);
#endif
        BITSET save=test;
        test|=Sy_bit(OPT_REDSB);
        test|=Sy_bit(OPT_REDTAIL);
//         if(gcone::hasHomInput==TRUE)
                H=kStd(ina,NULL,testHomog/*isHomog*/,NULL/*,gcone::hilbertFunction*/);
//         else
//                 H=kStd(ina,NULL,isNotHomog,NULL);        //This is \mathcal(G)_{>_-\alpha}(in_v(I))
        test=save;
#ifdef gfanp
        gettimeofday(&t_kStd_end, 0);
        t_kStd += (t_kStd_end.tv_sec - t_kStd_start.tv_sec + 1e-6*(t_kStd_end.tv_usec - t_kStd_start.tv_usec));
#endif
        idSkipZeroes(H);
        idDelete(&ina);

        rChangeCurrRing(srcRing);
        ideal srcRing_H;
        ideal srcRing_HH;
        srcRing_H=idrCopyR(H,tmpRing);
        //H is needed further below, so don't idDelete here
        srcRing_HH=ffG(srcRing_H,this->gcBasis);
        idDelete(&srcRing_H);
        //Now BBA(srcRing_HH) with (a(),a(),dp)
        /* Evil modification of currRing */
        ring dstRing=rCopy0(tmpRing);
        int length=this->numVars;
        int *A1=(int *)omAlloc0(length*sizeof(int));
        int *A2=(int *)omAlloc0(length*sizeof(int));
        const int64vec *ivw=f->getRef2FacetNormal();
        for(int jj=0;jj<length;jj++)
        {
                A1[jj] = (*intPointOfFacet)[jj];
                A2[jj] = -(*ivw)[jj];//TODO Or minus (*ivw)[ii] ??? NOTE minus
        }
        omFree(dstRing->wvhdl[0]);
        if(dstRing->wvhdl[1]!=NULL)
                omFree(dstRing->wvhdl[1]);
        dstRing->wvhdl[0]=(int*)A1;
        dstRing->block1[0]=length;
        dstRing->wvhdl[1]=(int*)A2;
        dstRing->block1[1]=length;
        rComplete(dstRing);
        rChangeCurrRing(dstRing);
        ideal dstRing_I;
        dstRing_I=idrCopyR(srcRing_HH,srcRing);
        idDelete(&srcRing_HH); //Hmm.... causes trouble - no more
        save=test;
        test|=Sy_bit(OPT_REDSB);
        test|=Sy_bit(OPT_REDTAIL);
        ideal tmpI;
        tmpI = dstRing_I;
#ifdef gfanp
//         timeval t_kStd_start, t_kStd_end;
        gettimeofday(&t_kStd_start,0);
#endif
//         if(gcone::hasHomInput==TRUE)
//                 dstRing_I=kStd(tmpI,NULL,isHomog,NULL/*,gcone::hilbertFunction*/);
//         else
                dstRing_I=kStd(tmpI,NULL,testHomog,NULL);
#ifdef gfanp
        gettimeofday(&t_kStd_end, 0);
        t_kStd += (t_kStd_end.tv_sec - t_kStd_start.tv_sec + 1e-6*(t_kStd_end.tv_usec - t_kStd_start.tv_usec));
#endif
        idDelete(&tmpI);
        idNorm(dstRing_I);
        idSkipZeroes(dstRing_I);
        test=save;
        /*End of step 3 - reduction*/

        f->setFlipGB(dstRing_I);
        f->flipRing=rCopy(dstRing);
        rDelete(tmpRing);
        rDelete(dstRing);
        //Now we should have dstRing with (a(),a(),dp,C)
        //This must be replaced with (a(),dp,C) BEFORE gcTmp is actually added to the list
        //of cones in noRevS
        rChangeCurrRing(srcRing);
#ifdef gfanp
        gettimeofday(&end, 0);
        time_flip2 += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif
}//flip2

/** \brief Compute initial ideal
 * Compute the initial ideal in_v(G) wrt a (possible) facet normal
 * used in gcone::getFacetNormal in order to preprocess possible facet normals
 * and in gcone::flip for obvious reasons.
*/
/*inline*/ void gcone::computeInv(const ideal &gb, ideal &initialForm, const int64vec &fNormal)
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
                pGetExpV(aktpoly,leadExpV);        //find the leading exponent in leadExpV[1],...,leadExpV[n], use pNext(p)
                initialFormElement=pHead(aktpoly);
//                 int *v=(int *)omAlloc((this->numVars+1)*sizeof(int));
                while(pNext(aktpoly)!=NULL)        /*loop trough terms and check for parallelity*/
                {
                        int64vec *check = new int64vec(this->numVars);
                        aktpoly=pNext(aktpoly);        //next term
                        int *v=(int *)omAlloc((this->numVars+1)*sizeof(int));
                        pGetExpV(aktpoly,v);
                        /* Convert (int)v into (int64vec)check */
//                         bool notPar=FALSE;
                        for (int jj=0;jj<this->numVars;jj++)
                        {
                                (*check)[jj]=v[jj+1]-leadExpV[jj+1];
//                                 register int64 foo=(fNormal)[jj];
//                                 if( ( (*check)[jj]  == /*fNormal[jj]*/foo )
//                                  || ( (/*fNormal[jj]*/foo!=0) && ( ( (*check)[jj] % /*fNormal[jj]*/foo ) !=0 ) ) )
//                                 {
//                                         notPar=TRUE;
//                                         break;
//                                 }
                        }
                        omFree(v);
                        if (isParallel(*check,fNormal))//Found a parallel vector. Add it
//                          if(notPar==FALSE)
                        {
                                initialFormElement = pAdd((initialFormElement),(poly)pHead(aktpoly));//pAdd = p_Add_q destroys args
                        }
                        delete check;
                }//while
//                 omFree(v);
#ifndef NDEBUG
//                  cout << "Initial Form=";
//                  pWrite(initialFormElement[ii]);
//                  cout << "---" << endl;
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
//                 poly temp1;//=pInit();
//                 poly temp2;//=pInit();
                poly temp3;//=pInit();//polys to temporarily store values for pSub
//                 res->m[ii]=pCopy(kNF(G, NULL,H->m[ii],0,0));
//                  temp1=pCopy(H->m[ii]);//TRY
//                 temp2=pCopy(res->m[ii]);
                //NOTE if gfanHeuristic=0 (sic!) this results in dPolyErrors - mon from wrong ring
//                  temp2=pCopy(kNF(G, NULL,H->m[ii],0,0));//TRY
//                  temp3=pSub(temp1, temp2);//TRY
                temp3=pSub(pCopy(H->m[ii]),pCopy(kNF(G,NULL,H->m[ii],0,0)));//NOTRY
                res->m[ii]=pCopy(temp3);
                //res->m[ii]=pSub(temp1,temp2); //buggy
                //cout << "res->m["<<ii<<"]=";pWrite(res->m[ii]);
//                 pDelete(&temp1);//TRY
//                 pDelete(&temp2);
                pDelete(&temp3);
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
/*        int *posRowsArray=NULL;
        int num_alloc=0;
        int num_elts=0;
         int offset=0;*/
        //Remove zeroes (and strictly pos rows?)
        for(int ii=0;ii<ddineq->rowsize;ii++)
        {
                int64vec *iv = new int64vec(this->numVars);
                int64vec *ivNull = new int64vec(this->numVars);//Needed for intvec64::compare(*int64vec)
                int posCtr=0;
                for(int jj=0;jj<this->numVars;jj++)
                {
                        (*iv)[jj]=(int)mpq_get_d(ddineq->matrix[ii][jj+1]);
                        if((*iv)[jj]>0)//check for strictly pos rows
                                posCtr++;
                        //Behold! This will delete the row for the standard simplex!
                }
//                 if( (iv->compare(0)==0) || (posCtr==iv->length()) )
                if( (posCtr==iv->length()) || (iv->compare(ivNull)==0) )
                {
                        dd_MatrixRowRemove(&ddineq,ii+1);
                        ii--;//Yes. This is on purpose
                }
                delete iv;
                delete ivNull;
        }
        //Remove duplicates of rows
//         posRowsArray=NULL;
//         num_alloc=0;
//         num_elts=0;
//         offset=0;
//         int num_newRows = ddineq->rowsize;
//         for(int ii=0;ii<ddineq->rowsize-1;ii++)
//         for(int ii=0;ii<num_newRows-1;ii++)
//         {
//                 int64vec *iv = new int64vec(this->numVars);//1st vector to check against
//                 for(int jj=0;jj<this->numVars;jj++)
//                         (*iv)[jj]=(int)mpq_get_d(ddineq->matrix[ii][jj+1]);
//                 for(int jj=ii+1;jj</*ddineq->rowsize*/num_newRows;jj++)
//                 {
//                         int64vec *ivCheck = new int64vec(this->numVars);//Checked against iv
//                         for(int kk=0;kk<this->numVars;kk++)
//                                 (*ivCheck)[kk]=(int)mpq_get_d(ddineq->matrix[jj][kk+1]);
//                         if (iv->compare(ivCheck)==0)
//                         {
// //                                 cout << "=" << endl;
// //                                 num_alloc++;
// //                                 void *tmp=realloc(posRowsArray,(num_alloc*sizeof(int)));
// //                                 if(!tmp)
// //                                 {
// //                                         WerrorS("Woah dude! Couldn't realloc memory\n");
// //                                         exit(-1);
// //                                 }
// //                                 posRowsArray = (int*)tmp;
// //                                 posRowsArray[num_elts]=jj;
// //                                 num_elts++;
//                                 dd_MatrixRowRemove(&ddineq,jj+1);
//                                 num_newRows = ddineq->rowsize;
//                         }
//                         delete ivCheck;
//                 }
//                 delete iv;
//         }
//         for(int ii=0;ii<num_elts;ii++)
//         {
//                 dd_MatrixRowRemove(&ddineq,posRowsArray[ii]+1-offset);
//                 offset++;
//         }
//         free(posRowsArray);
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
        this->gcBasis=gb;        //write the GB into gcBasis
        test=save;
}//void getGB

/** \brief Compute the negative of a given int64vec
        */
static int64vec* ivNeg(/*const*/int64vec *iv)
{        //Hm, switching to int64vec const int64vec does no longer work
        int64vec *res;// = new int64vec(iv->length());
        res=iv64Copy(iv);
        *res *= (int)-1;
        return res;
}


/** \brief Compute the dot product of two intvecs
*
*/
static int dotProduct(const int64vec &iva, const int64vec &ivb)
{
        int res=0;
        for (int i=0;i<pVariables;i++)
        {
// #ifndef NDEBUG
//         (const_cast<int64vec*>(&iva))->show(1,0); (const_cast<int64vec*>(&ivb))->show(1,0);
// #endif
                res = res+(iva[i]*ivb[i]);
        }
        return res;
}
/** \brief Check whether two intvecs are parallel
 *
 * \f$ \alpha\parallel\beta\Leftrightarrow\langle\alpha,\beta\rangle^2=\langle\alpha,\alpha\rangle\langle\beta,\beta\rangle \f$
 */
static bool isParallel(const int64vec &a,const int64vec &b)
{
        bool res;
        int lhs=dotProduct(a,b)*dotProduct(a,b);
        int rhs=dotProduct(a,a)*dotProduct(b,b);
        return res = (lhs==rhs)?TRUE:FALSE;
}

/** \brief Compute an interior point of a given cone
 * Result will be written into int64vec iv.
 * Any rational point is automatically converted into an integer.
 */
void gcone::interiorPoint( dd_MatrixPtr &M, int64vec &iv) //no const &M here since we want to remove redundant rows
{
        dd_LPPtr lp,lpInt;
        dd_ErrorType err=dd_NoError;
        dd_LPSolverType solver=dd_DualSimplex;
        dd_LPSolutionPtr lpSol=NULL;
//         dd_rowset ddlinset,ddredrows;        //needed for dd_FindRelativeInterior
//          dd_rowindex ddnewpos;
        dd_NumberType numb;
        //M->representation=dd_Inequality;

        //NOTE: Make this n-dimensional!
        //dd_set_si(M->rowvec[0],1);dd_set_si(M->rowvec[1],1);dd_set_si(M->rowvec[2],1);

        /*NOTE: Leave the following line commented out!
        * Otherwise it will slow down computations a great deal
        * */
//         dd_MatrixCanonicalizeLinearity(&M, &ddlinset, &ddnewpos, &err);
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
#ifndef NDEBUG
//                        dd_WriteLP(stdout,lp);
#endif

        lpInt=dd_MakeLPforInteriorFinding(lp);
        if (err!=dd_NoError){WerrorS("Error during dd_MakeLPForInteriorFinding in gcone::interiorPoint");}
#ifndef NDEBUG
//                         dd_WriteLP(stdout,lpInt);
#endif
//         dd_FindRelativeInterior(M,&ddlinset,&ddredrows,&lpSol,&err);
        if (err!=dd_NoError)
        {
                WerrorS("Error during dd_FindRelativeInterior in gcone::interiorPoint");
                dd_WriteErrorMessages(stdout, err);
        }
        dd_LPSolve(lpInt,solver,&err);        //This will not result in a point from the relative interior
//         if (err!=dd_NoError){WerrorS("Error during dd_LPSolve");}
        lpSol=dd_CopyLPSolution(lpInt);
//         if (err!=dd_NoError){WerrorS("Error during dd_CopyLPSolution");}
#ifndef NDEBUG
        printf("Interior point: ");
        for (int ii=1; ii<(lpSol->d)-1;ii++)
        {
                dd_WriteNumber(stdout,lpSol->sol[ii]);
        }
        printf("\n");
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
#ifndef NDEBUG
//                         iv.show();
//                         cout << endl;
#endif
        mpq_clear(qkgV);
        mpz_clear(tmp);
        mpz_clear(den);
        mpz_clear(kgV);

        dd_FreeLPSolution(lpSol);
        dd_FreeLPData(lpInt);
        dd_FreeLPData(lp);
//         set_free(ddlinset);
//         set_free(ddredrows);

}//void interiorPoint(dd_MatrixPtr const &M)

/** Computes an interior point of a cone by taking two interior points a,b from two different facets
* and then computing b+(a-b)/2
* Of course this only works for flippable facets
* Two cases may occur:
* 1st: There are only two facets who share the only strictly positive ray
* 2nd: There are at least two facets which have a distinct positive ray
* In the former case we use linear algebra to determine an interior point,
* in the latter case we simply add up the two rays
*
* Way too bad! The case may occur that the cone is spanned by three rays, of which only two are strictly
* positive => these lie in a plane and thus their sum is not from relative interior.
* So let's just sum up all rays, find one strictly positive and shift the point along that ray
*
* Used by noRevS
*NOTE no longer used nor maintained. MM Mar 9, 2010
*/
// void gcone::interiorPoint2()
// {//idPrint(this->gcBasis);
// #ifndef NDEBUG
//         if(this->ivIntPt!=NULL)
//                 WarnS("Interior point already exists - ovrewriting!");
// #endif
//         facet *f1 = this->facetPtr;
//         facet *f2 = NULL;
//         int64vec *intF1=NULL;
//         while(f1!=NULL)
//         {
//                 if(f1->isFlippable)
//                 {
//                         facet *f1Ray = f1->codim2Ptr;
//                         while(f1Ray!=NULL)
//                         {
//                                 const int64vec *check=f1Ray->getRef2FacetNormal();
//                                 if(iv64isStrictlyPositive(check))
//                                 {
//                                         intF1=iv64Copy(check);
//                                         break;
//                                 }
//                                 f1Ray=f1Ray->next;
//                         }
//                 }
//                 if(intF1!=NULL)
//                         break;
//                 f1=f1->next;
//         }
//         if(f1!=NULL && f1->next!=NULL)//Choose another facet, different from f1
//                 f2=f1->next;
//         else
//                 f2=this->facetPtr;
//         if(intF1==NULL && hasHomInput==TRUE)
//         {
//                 intF1 = new int64vec(this->numVars);
//                 for(int ii=0;ii<this->numVars;ii++)
//                         (*intF1)[ii]=1;
//         }
//         assert(f1); assert(f2);
//         int64vec *intF2=f2->getInteriorPoint();
//         mpq_t *qPosRay = new mpq_t[this->numVars];//The positive ray from above
//         mpq_t *qIntPt = new mpq_t[this->numVars];//starting vector a+((b-a)/2)
//         mpq_t *qPosIntPt = new mpq_t[this->numVars];//This should be >0 eventually
//         for(int ii=0;ii<this->numVars;ii++)
//         {
//                 mpq_init(qPosRay[ii]);
//                 mpq_init(qIntPt[ii]);
//                 mpq_init(qPosIntPt[ii]);
//         }
//         //Compute a+((b-a)/2) && Convert intF1 to mpq
//         for(int ii=0;ii<this->numVars;ii++)
//         {
//                 mpq_t a,b;
//                 mpq_init(a); mpq_init(b);
//                 mpq_set_si(a,(*intF1)[ii],1);
//                 mpq_set_si(b,(*intF2)[ii],1);
//                 mpq_t diff;
//                 mpq_init(diff);
//                 mpq_sub(diff,b,a);        //diff=b-a
//                 mpq_t quot;
//                 mpq_init(quot);
//                 mpq_div_2exp(quot,diff,1);        //quot=diff/2=(b-a)/2
//                 mpq_clear(diff);
//                 //Don't be clever and reuse diff here
//                 mpq_t sum; mpq_init(sum);
//                 mpq_add(sum,b,quot);        //sum=b+quot=a+(b-a)/2
//                 mpq_set(qIntPt[ii],sum);
//                 mpq_clear(sum);
//                 mpq_clear(quot);
//                 mpq_clear(a); mpq_clear(b);
//                 //Now for intF1
//                 mpq_set_si(qPosRay[ii],(*intF1)[ii],1);
//         }
//         //Now add: qPosIntPt=qPosRay+qIntPt until qPosIntPt >0
//         while(TRUE)
//         {
//                 bool success=FALSE;
//                 int posCtr=0;
//                 for(int ii=0;ii<this->numVars;ii++)
//                 {
//                         mpq_t sum; mpq_init(sum);
//                         mpq_add(sum,qPosRay[ii],qIntPt[ii]);
//                         mpq_set(qPosIntPt[ii],sum);
//                         mpq_clear(sum);
//                         if(mpq_sgn(qPosIntPt[ii])==1)
//                                 posCtr++;
//                 }
//                 if(posCtr==this->numVars)//qPosIntPt > 0
//                         break;
//                 else
//                 {
//                         mpq_t qTwo; mpq_init(qTwo);
//                         mpq_set_ui(qTwo,2,1);
//                         for(int jj=0;jj<this->numVars;jj++)
//                         {
//                                 mpq_t tmp; mpq_init(tmp);
//                                 mpq_mul(tmp,qPosRay[jj],qTwo);
//                                 mpq_set( qPosRay[jj], tmp);
//                                 mpq_clear(tmp);
//                         }
//                         mpq_clear(qTwo);
//                 }
//         }//while
//         //Now qPosIntPt ought to be >0, so convert back to int :D
//         /*Compute lcm of the denominators*/
//         mpz_t *denom = new mpz_t[this->numVars];
//         mpz_t tmp,kgV;
//         mpz_init(tmp); mpz_init(kgV);
//         for (int ii=0;ii<this->numVars;ii++)
//         {
//                 mpz_t z;
//                 mpz_init(z);
//                 mpq_get_den(z,qPosIntPt[ii]);
//                 mpz_init(denom[ii]);
//                 mpz_set( denom[ii], z);
//                 mpz_clear(z);
//         }
//
//         mpz_set(tmp,denom[0]);
//         for (int ii=0;ii<this->numVars;ii++)
//         {
//                 mpz_lcm(kgV,tmp,denom[ii]);
//                 mpz_set(tmp,kgV);
//         }
//         mpz_clear(tmp);
//         /*Multiply the nominators by kgV*/
//         mpq_t qkgV,res;
//         mpq_init(qkgV);
//         mpq_canonicalize(qkgV);
//         mpq_init(res);
//         mpq_canonicalize(res);
//
//         mpq_set_num(qkgV,kgV);
//         int64vec *n=new int64vec(this->numVars);
//         for (int ii=0;ii<this->numVars;ii++)
//         {
//                 mpq_canonicalize(qPosIntPt[ii]);
//                 mpq_mul(res,qkgV,qPosIntPt[ii]);
//                 (*n)[ii]=(int)mpz_get_d(mpq_numref(res));
//         }
//         this->setIntPoint(n);
//         delete n;
//         delete [] qPosIntPt;
//         delete [] denom;
//         delete [] qPosRay;
//         delete [] qIntPt;
//         mpz_clear(kgV);
//         mpq_clear(qkgV); mpq_clear(res);
// }

/** \brief Copy a ring and add a weighted ordering in first place
 *
 */
ring gcone::rCopyAndAddWeight(const ring &r, int64vec *ivw)
{
        ring res=rCopy0(r);
        int jj;

        omFree(res->order);
        res->order =(int *)omAlloc0(4*sizeof(int/*64*/));
        omFree(res->block0);
        res->block0=(int *)omAlloc0(4*sizeof(int/*64*/));
        omFree(res->block1);
        res->block1=(int *)omAlloc0(4*sizeof(int/*64*/));
        omfree(res->wvhdl);
        res->wvhdl =(int **)omAlloc0(4*sizeof(int/*64*/**));

        res->order[0]=ringorder_a/*64*/;
        res->block0[0]=1;
        res->block1[0]=res->N;
        res->order[1]=ringorder_dp;        //basically useless, since that should never be used
        res->block0[1]=1;
        res->block1[1]=res->N;
        res->order[2]=ringorder_C;

        int length=ivw->length();
        int/*64*/ *A=(int/*64*/ *)omAlloc0(length*sizeof(int/*64*/));
        for (jj=0;jj<length;jj++)
        {
                A[jj]=(*ivw)[jj];
                if((*ivw)[jj]>=INT_MAX) WarnS("A[jj] exceeds INT_MAX in gcone::rCopyAndAddWeight!\n");
        }
        res->wvhdl[0]=(int *)A;
        res->block1[0]=length;

        rComplete(res);
        return res;
}//rCopyAndAdd

ring gcone::rCopyAndAddWeight2(const ring &r,const int64vec *ivw, const int64vec *fNormal)
{
        ring res=rCopy0(r);

        omFree(res->order);
        res->order =(int *)omAlloc0(5*sizeof(int/*64*/));
        omFree(res->block0);
        res->block0=(int *)omAlloc0(5*sizeof(int/*64*/));
        omFree(res->block1);
        res->block1=(int *)omAlloc0(5*sizeof(int/*64*/));
        omfree(res->wvhdl);
        res->wvhdl =(int **)omAlloc0(5*sizeof(int/*64*/**));

        res->order[0]=ringorder_a/*64*/;
        res->block0[0]=1;
        res->block1[0]=res->N;
        res->order[1]=ringorder_a/*64*/;
        res->block0[1]=1;
        res->block1[1]=res->N;

        res->order[2]=ringorder_dp;
        res->block0[2]=1;
        res->block1[2]=res->N;

        res->order[3]=ringorder_C;

        int length=ivw->length();
        int/*64*/ *A1=(int/*64*/ *)omAlloc0(length*sizeof(int/*64*/));
        int/*64*/ *A2=(int/*64*/ *)omAlloc0(length*sizeof(int/*64*/));
        for (int jj=0;jj<length;jj++)
        {
                A1[jj]=(*ivw)[jj];
                A2[jj]=-(*fNormal)[jj];
                if((*ivw)[jj]>=INT_MAX || (*fNormal)[jj]>=INT_MAX) WarnS("A[jj] exceeds INT_MAX in gcone::rCopyAndAddWeight2!\n");
        }
        res->wvhdl[0]=(int *)A1;
        res->block1[0]=length;
        res->wvhdl[1]=(int *)A2;
        res->block1[1]=length;
        rComplete(res);
        return res;
}

//NOTE not needed anywhere
// ring rCopyAndChangeWeight(ring const &r, int64vec *ivw)
// {
//         ring res=rCopy0(currRing);
//         rComplete(res);
//         rSetWeightVec(res,(int64*)ivw);
//         //rChangeCurrRing(rnew);
//         return res;
// }

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
static bool ivAreEqual(const int64vec &a, const int64vec &b)
{
        bool res=TRUE;
        for(int ii=0;ii<pVariables;ii++)
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
* As a result gcone::dd_LinealitySpace is set
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
                                                for(int ss=k+1;ss<n;ss++)
                                                {
                                                        mpq_t prod; mpq_init(prod);
                                                        mpq_mul(prod, ratd, ddineq->matrix[j][ss]);
                                                        mpq_t sum; mpq_init(sum);
                                                        mpq_add(sum, ddineq->matrix[ii][ss], prod);
                                                        mpq_set(ddineq->matrix[ii][ss],        sum);
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
                if(condCtr==m-1)        //Why -1 ???
                {
                        dimKer++;
                        d[k]=0;
//                         break;//goto _4;
                }//else{
                /*Eliminate*/
        }//for(k
        /*Output kernel, i.e. set gcone::dd_LinealitySpace here*/
//         gcone::dd_LinealitySpace = dd_CreateMatrix(dimKer,this->numVars+1);
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


/** \brief The new method of Markwig and Jensen
 * Compute gcBasis and facets for the arbitrary starting cone. Store \f$(codim-1)\f$-facets as normals.
 * In order to represent a facet uniquely compute also the \f$(codim-2)\f$-facets and norm by the gcd of the components.
 * Keep a list of facets as a linked list containing an int64vec and an integer matrix.
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
        gcone *gcPtr;        //Pointer to end of linked list of cones
        gcPtr = &gcRoot;
        gcone *gcNext;        //Pointer to next cone to be visited
        gcNext = &gcRoot;
        gcone *gcHead;
        gcHead = &gcRoot;
        facet *fAct;
        fAct = gcAct->facetPtr;
        ring rAct;
        rAct = currRing;
        int UCNcounter=gcAct->getUCN();
#ifndef NDEBUG
        printf("NoRevs\n");
        printf("Facets are:\n");
        gcAct->showFacets();
#endif
        /*Compute lineality space here
        * Afterwards static dd_MatrixPtr gcone::dd_LinealitySpace is set*/
        if(dd_LinealitySpace==NULL)
                dd_LinealitySpace = gcAct->computeLinealitySpace();
        /*End of lineality space computation*/
        //gcAct->getCodim2Normals(*gcAct);
        if(fAct->codim2Ptr==NULL)
                gcAct->getExtremalRays(*gcAct);
        /* Make a copy of the facet list of first cone
           Since the operations getCodim2Normals and normalize affect the facets
           we must not memcpy them before these ops! */
        /*facet *codim2Act; codim2Act = NULL;
        facet *sl2Root;
        facet *sl2Act;*/
        for(int ii=0;ii<this->numFacets;ii++)
        {
                //only copy flippable facets! NOTE: We do not need flipRing or any such information.
                if(fAct->isFlippable==TRUE)
                {
                        /*Using shallow copy here*/
#ifdef SHALLOW
                        if( ii==0 || (ii>0 && SearchListAct==NULL) ) //1st facet may be non-flippable
                        {
                                if(SearchListRoot!=NULL) delete(SearchListRoot);
                                SearchListRoot = fAct->shallowCopy(*fAct);
                                SearchListAct = SearchListRoot;        //SearchListRoot is already 'new'ed at beginning of method!
                        }
                        else
                        {
                                SearchListAct->next = fAct->shallowCopy(*fAct);
                                SearchListAct = SearchListAct->next;
                        }
                        fAct=fAct->next;
#else
                        /*End of shallow copy*/
                        int64vec *fNormal;
                        fNormal = fAct->getFacetNormal();
                        if( ii==0 || (ii>0 && SearchListAct==NULL) ) //1st facet may be non-flippable
                        {
                                SearchListAct = SearchListRoot;        //SearchListRoot is already 'new'ed at beginning of method!
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
                        //Copy int point as well
                        int64vec *ivIntPt;
                        ivIntPt = fAct->getInteriorPoint();
                        SearchListAct->setInteriorPoint(ivIntPt);
                        delete(ivIntPt);
                        //Copy codim2-facets
                        facet *codim2Act;
                        codim2Act = NULL;
                        facet *sl2Root;
                        facet *sl2Act;
                        codim2Act=fAct->codim2Ptr;
                        SearchListAct->codim2Ptr = new facet(2);
                        sl2Root = SearchListAct->codim2Ptr;
                        sl2Act = sl2Root;
                        for(int jj=0;jj<fAct->numCodim2Facets;jj++)
//                         for(int jj=0;jj<fAct->numRays-1;jj++)
                        {
                                int64vec *f2Normal;
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
#endif
                }//if(fAct->isFlippable==TRUE)
                else {fAct = fAct->next;}
        }//End of copying facets into SLA

        SearchListAct = SearchListRoot;        //Set to beginning of list
        /*Make SearchList doubly linked*/
#ifndef NDEBUG
  #if SIZEOF_LONG==8
        while(SearchListAct!=(facet*)0xfefefefefefefefe && SearchListAct!=NULL)
        {
                if(SearchListAct->next!=(facet*)0xfefefefefefefefe && SearchListAct->next!=NULL){
  #elif SIZEOF_LONG!=8
        while(SearchListAct!=(facet*)0xfefefefe)
        {
                if(SearchListAct->next!=(facet*)0xfefefefe){
  #endif
#else
        while(SearchListAct!=NULL)
        {
                if(SearchListAct->next!=NULL){
#endif
                        SearchListAct->next->prev = SearchListAct;
                }
                SearchListAct = SearchListAct->next;
        }
        SearchListAct = SearchListRoot;        //Set to beginning of List

//         fAct = gcAct->facetPtr;//???
        gcAct->writeConeToFile(*gcAct);
        /*End of initialisation*/

        fAct = SearchListAct;
        /*2nd step
          Choose a facet from SearchList, flip it and forget the previous cone
          We always choose the first facet from SearchList as facet to be flipped
        */
        while( (SearchListAct!=NULL))//&& counter<490)
        {//NOTE See to it that the cone is only changed after ALL facets have been flipped!
                fAct = SearchListAct;
                 while(fAct!=NULL)
//                 while( (fAct->getUCN() == fAct->next->getUCN()) )
                {        //Since SLA should only contain flippables there should be no need to check for that
                        gcAct->flip2(gcAct->gcBasis,fAct);
                        //NOTE rCopy needed?
                        ring rTmp=rCopy(fAct->flipRing);
                        rComplete(rTmp);
                        rChangeCurrRing(rTmp);
                        gcone *gcTmp = new gcone(*gcAct,*fAct);//copy constructor!
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
                        gcTmp->getConeNormals(gcTmp->gcBasis/*, FALSE*/);
//                         gcTmp->getCodim2Normals(*gcTmp);
                        gcTmp->getExtremalRays(*gcTmp);
                        //NOTE Order rays lex here
                        gcTmp->orderRays();
//                         //NOTE If flip2 is used we need to get an interior point of gcTmp
//                         // and replace gcTmp->baseRing with an appropriate ring with only
//                         // one weight
//                         gcTmp->interiorPoint2();
                        gcTmp->replaceDouble_ringorder_a_ByASingleOne();
                        //gcTmp->ddFacets should not be needed anymore, so
                        dd_FreeMatrix(gcTmp->ddFacets);
#ifndef NDEBUG
//                         gcTmp->showFacets(1);
#endif
                        /*add facets to SLA here*/
#ifdef SHALLOW
  #ifndef NDEBUG
                        printf("fActUCN before enq2: %i\n",fAct->getUCN());
  #endif
                        facet *tmp;
                        tmp=gcTmp->enqueue2(SearchListRoot);
  #ifndef NDEBUG
                        printf("\nheadUCN=%i\n",tmp->getUCN());
                        printf("fActUCN after enq2: %i\n",fAct->getUCN());
  #endif
                        SearchListRoot=tmp;
                        //SearchListRoot=gcTmp->enqueue2/*NewFacets*/(SearchListRoot);
#else
                        SearchListRoot=gcTmp->enqueueNewFacets(SearchListRoot);
#endif //ifdef SHALLOW
//                         gcTmp->writeConeToFile(*gcTmp);
                         if(gfanHeuristic==1)
                         {
                                gcTmp->writeConeToFile(*gcTmp);
                                 idDelete((ideal*)&gcTmp->gcBasis);//Whonder why?
                                rDelete(gcTmp->baseRing);
                         }
#ifndef NDEBUG
                        if(SearchListRoot!=NULL)
                                showSLA(*SearchListRoot);
#endif
                        rChangeCurrRing(gcAct->baseRing);
                        rDelete(rTmp);
                        //doubly linked for easier removal
                        gcTmp->prev = gcPtr;
                        gcPtr->next=gcTmp;
                        gcPtr=gcPtr->next;
                        //Cleverly disguised exit condition follows
                        if(fAct->getUCN() == fAct->next->getUCN())
                        {
                                printf("Switching UCN from %i to %i\n",fAct->getUCN(),fAct->next->getUCN());
                                fAct=fAct->next;
                        }
                        else
                        {
                                //rDelete(gcAct->baseRing);
//                                 printf("break\n");
                                break;
                        }
//                         fAct=fAct->next;
                }//while( ( (fAct->next!=NULL) && (fAct->getUCN()==fAct->next->getUCN() ) ) );
                //Search for cone with smallest UCN
#ifndef NDEBUG
  #if SIZEOF_LONG==8        //64 bit
                while(gcNext!=(gcone * const)0xfbfbfbfbfbfbfbfb && SearchListRoot!=NULL)
  #elif SIZEOF_LONG == 4
                while(gcNext!=(gcone * const)0xfbfbfbfb && SearchListRoot!=NULL)
  #endif
#endif
#ifdef NDEBUG
                while(gcNext!=NULL && SearchListRoot!=NULL)
#endif
                {
                        if( gcNext->getUCN() == SearchListRoot->getUCN() )
                        {
                                if(gfanHeuristic==0)
                                {
                                        gcAct = gcNext;
                                        //Seems better to not to use rCopy here
//                                         rAct=rCopy(gcAct->baseRing);
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
                                        //Read st00f from file &
                                        //implant the GB into gcAct
                                        readConeFromFile(gcAct->getUCN(), gcAct);
                                        //Kill the baseRing but ONLY if it is not the ring the computation started in!
//                                         if(gcDel->getUCN()!=1)//WTF!? This causes the Mandelbug in gcone::areEqual(facet, facet)
//                                                 rDelete(gcDel->baseRing);
//                                         rAct=rCopy(gcAct->baseRing);
                                        /*The ring change occurs in readConeFromFile, so as to
                                        assure that gcAct->gcBasis belongs to the right ring*/
                                        rAct=gcAct->baseRing;
                                        rComplete(rAct);
                                        rChangeCurrRing(rAct);
                                        break;
                                }
                        }
                        else if(gcNext->getUCN() < SearchListRoot->getUCN() )
                        {
                                idDelete( (ideal*)&gcNext->gcBasis );
//                                 rDelete(gcNext->baseRing);//TODO Why does this crash?
                        }
                        /*else
                        {
                                if(gfanHeuristic==1)
                                {
                                        gcone *gcDel;
                                        gcDel = gcNext;
                                        if(gcDel->getUCN()!=1)
                                                rDelete(gcDel->baseRing);
                                }
                        }*/
                         gcNext = gcNext->next;
                }
                UCNcounter++;
                SearchListAct = SearchListRoot;
        }
        printf("\nFound %i cones - terminating\n", counter);
}//void noRevS(gcone &gc)


/** \brief Make a set of rational vectors into integers
 *
 * We compute the lcm of the denominators and multiply with this to get integer values.
 * If the gcd of the nominators > 1 we divide by the gcd => primitive vector.
 * Expects a new int64vec as 3rd parameter
 * \param dd_MatrixPtr,int64vec
 */
void gcone::makeInt(const dd_MatrixPtr &M, const int line, int64vec &n)
{
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

//        mpq_canonicalize(qkgV);
//         int ggT=1;
        for (int ii=0;ii<(M->colsize)-1;ii++)
        {
                mpq_mul(res,qkgV,M->matrix[line-1][ii+1]);
                n[ii]=(int64)mpz_get_d(mpq_numref(res));
//                 ggT=int64gcd(ggT,n[ii]);
        }
        int64 ggT=n[0];
        for(int ii=0;ii<this->numVars;ii++)
                ggT=int64gcd(ggT,n[ii]);
        //Normalisation
        if(ggT>1)
        {
                for(int ii=0;ii<this->numVars;ii++)
                        n[ii] /= ggT;
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
// void gcone::normalize()
// {
//         int *ggT = new int;
//                 *ggT=1;
//         facet *fAct;
//         facet *codim2Act;
//         fAct = this->facetPtr;
//         codim2Act = fAct->codim2Ptr;
//         while(fAct!=NULL)
//         {
//                 int64vec *fNormal;
//                 fNormal = fAct->getFacetNormal();
//                 int *ggT = new int;
//                 *ggT=1;
//                 for(int ii=0;ii<this->numVars;ii++)
//                 {
//                         *ggT=intgcd((*ggT),(*fNormal)[ii]);
//                 }
//                 if(*ggT>1)//We only need to do this if the ggT is non-trivial
//                 {
// //                         int64vec *fCopy = fAct->getFacetNormal();
//                         for(int ii=0;ii<this->numVars;ii++)
//                                 (*fNormal)[ii] = ((*fNormal)[ii])/(*ggT);
//                         fAct->setFacetNormal(fNormal);
//                 }
//                 delete fNormal;
//                 delete ggT;
//                 /*And now for the codim2*/
//                 while(codim2Act!=NULL)
//                 {
//                         int64vec *n;
//                         n=codim2Act->getFacetNormal();
//                         int *ggT=new int;
//                         *ggT=1;
//                         for(int ii=0;ii<this->numVars;ii++)
//                         {
//                                 *ggT = intgcd((*ggT),(*n)[ii]);
//                         }
//                         if(*ggT>1)
//                         {
//                                 for(int ii=0;ii<this->numVars;ii++)
//                                 {
//                                         (*n)[ii] = ((*n)[ii])/(*ggT);
//                                 }
//                                 codim2Act->setFacetNormal(n);
//                         }
//                         codim2Act = codim2Act->next;
//                         delete n;
//                         delete ggT;
//                 }
//                 fAct = fAct->next;
//         }
// }

/** \brief Enqueue new facets into the searchlist
 * The searchlist (SLA for short) is implemented as a FIFO
 * Checks are done as follows:
 * 1) Check facet fAct against all facets in SLA for parallelity. If it is not parallel to any one add it.
 * 2) If it is parallel compare the codim2 facets. If they coincide the facets are equal and we delete the
 *        facet from SLA and do not add fAct.
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
        facet *slAct;        //called with f=SearchListRoot
        slAct = f;
        facet *slEnd;        //Pointer to end of SLA
        slEnd = f;
//         facet *slEndStatic;        //marks the end before a new facet is added
        facet *fAct;
        fAct = this->facetPtr;
        facet *codim2Act;
        codim2Act = this->facetPtr->codim2Ptr;
        facet *sl2Act;
        sl2Act = f->codim2Ptr;
        /** Pointer to a facet that will be deleted */
        volatile facet *deleteMarker;
        deleteMarker = NULL;

        /** \brief  Flag to mark a facet that might be added
         * The following case may occur:
         * A facet fAct is found to be parallel but not equal to the current facet slAct from SLA.
         * This does however not mean that there does not exist a facet behind the current slAct that is actually equal
         * to fAct. In this case we set the boolean flag maybe to TRUE. If we encounter an equality lateron, it is reset to
         * FALSE and the according slAct is deleted.
         * If slAct->next==NULL AND maybe==TRUE we know, that fAct must be added
         */

        /**A facet was removed, lengthOfSearchlist-- thus we must not rely on
         * if(notParallelCtr==lengthOfSearchList) but rather
         * if( (notParallelCtr==lengthOfSearchList && removalOccured==FALSE)
         */
        volatile bool removalOccured=FALSE;
        while(slEnd->next!=NULL)
        {
                slEnd=slEnd->next;
        }
        /*1st step: compare facetNormals*/
        while(fAct!=NULL)
        {
                if(fAct->isFlippable==TRUE)
                {
                        int64vec *fNormal=NULL;
                        fNormal=fAct->getFacetNormal();
                        slAct = slHead;
                        /*If slAct==NULL and fAct!=NULL
                        we just copy all remaining facets into SLA*/
                        if(slAct==NULL)
                        {
                                facet *fCopy;
                                fCopy = fAct;
                                while(fCopy!=NULL)
                                {
                                        if(fCopy->isFlippable==TRUE)//We must assure fCopy is also flippable
                                        {
                                                if(slAct==NULL)
                                                {
                                                        slAct = new facet(*fCopy/*,TRUE*/);//copy constructor
                                                        slHead = slAct;
                                                }
                                                else
                                                {
                                                        slAct->next = new facet(*fCopy/*,TRUE*/);
                                                        slAct = slAct->next;
                                                }
                                        }
                                        fCopy = fCopy->next;
                                }
                                break;//Where does this lead to?
                        }
                        /*End of dumping into SLA*/
                        while(slAct!=NULL)
                        {
                                int64vec *slNormal=NULL;
                                removalOccured=FALSE;
                                slNormal = slAct->getFacetNormal();
#ifndef NDEBUG
                                printf("Checking facet (");fNormal->show(1,1);printf(") against (");slNormal->show(1,1);printf(")\n");
#endif
//                                 if( (areEqual(fAct,slAct) && (!areEqual2(fAct,slAct)) ))
//                                         exit(-1);
                                 if(areEqual2(fAct,slAct))
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
                                        gcone::lengthOfSearchList--;
                                        if(deleteMarker!=NULL)
                                        {
//                                                 delete deleteMarker;
//                                                 deleteMarker=NULL;
                                        }
#ifndef NDEBUG
                                        printf("Removing (");fNormal->show(1,1);printf(") from list\n");
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
//                                          delete deleteMarker;
//                                          deleteMarker=NULL;
                                }
                                delete slNormal;
                                                //if slAct was marked as to be deleted, delete it here!
                        }//while(slAct!=NULL)
                        if(removalOccured==FALSE)
                        {
#ifndef NDEBUG
//                                 cout << "Adding facet (";fNormal->show(1,0);cout << ") to SLA " << endl;
#endif
                                //Add fAct to SLA
                                facet *marker;
                                marker = slEnd;
                                facet *f2Act;
                                f2Act = fAct->codim2Ptr;

                                slEnd->next = new facet();
                                slEnd = slEnd->next;//Update slEnd
                                facet *slEndCodim2Root;
                                facet *slEndCodim2Act;
                                slEndCodim2Root = slEnd->codim2Ptr;
                                slEndCodim2Act = slEndCodim2Root;

                                slEnd->setUCN(this->getUCN());
                                slEnd->isFlippable = TRUE;
                                slEnd->setFacetNormal(fNormal);
                                //NOTE Add interior point here
                                //This is ugly but needed for flip2
                                //Better: have slEnd->interiorPoint point to fAct->interiorPoint
                                //NOTE Only reference -> c.f. copy constructor
                                //slEnd->setInteriorPoint(fAct->getInteriorPoint());
                                slEnd->interiorPoint = fAct->interiorPoint;
                                slEnd->prev = marker;
                                //Copy codim2-facets
                                //int64vec *f2Normal=new int64vec(this->numVars);
                                while(f2Act!=NULL)
                                {
                                        int64vec *f2Normal;
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
                        }//if( (notParallelCtr==lengthOfSearchList && removalOccured==FALSE) ||
                        fAct = fAct->next;
                         delete fNormal;
//                          delete slNormal;
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

/** Enqueuing using shallow copies*/
facet * gcone::enqueue2(facet *f)
{
        assert(f!=NULL);
#ifdef gfanp
        timeval start, end;
        gettimeofday(&start, 0);
#endif
        facet *slHead;
        slHead = f;
        facet *slAct;        //called with f=SearchListRoot
        slAct = f;
        static facet *slEnd;        //Pointer to end of SLA
        if(slEnd==NULL)
                slEnd = f;

        facet *fAct;
        fAct = this->facetPtr;//New facets to compare
        facet *codim2Act;
        codim2Act = this->facetPtr->codim2Ptr;
        volatile bool removalOccured=FALSE;
        while(slEnd->next!=NULL)
        {
                slEnd=slEnd->next;
        }
        while(fAct!=NULL)
        {
                if(fAct->isFlippable)
                {
                        facet *fDeleteMarker=NULL;
                        slAct = slHead;
                        if(slAct==NULL)
                        {
                                printf("Zero length SLA\n");
                                facet *fCopy;
                                fCopy = fAct;
                                while(fCopy!=NULL)
                                {
                                        if(fCopy->isFlippable==TRUE)//We must assure fCopy is also flippable
                                        {
                                                if(slAct==NULL)
                                                {
                                                        slAct = slAct->shallowCopy(*fCopy);//shallow copy constructor
                                                        slHead = slAct;
                                                }
                                                else
                                                {
                                                        slAct->next = slAct->shallowCopy(*fCopy);
                                                        slAct = slAct->next;
                                                }
                                        }
                                        fCopy = fCopy->next;
                                }
                                break;        //WTF?
                        }
                        /*Comparison starts here*/
                        while(slAct!=NULL)
                        {
                                removalOccured=FALSE;
#ifndef NDEBUG
        printf("Checking facet (");fAct->fNormal->show(1,1);printf(") against (");slAct->fNormal->show(1,1);printf(")\n");
#endif
                                if(areEqual2(fAct,slAct))
                                {
                                        fDeleteMarker=slAct;
                                        if(slAct==slHead)
                                        {
//                                                 fDeleteMarker=slHead;
//                                                 printf("headUCN@enq=%i\n",slHead->getUCN());
                                                slHead = slAct->next;
//                                                 printf("headUCN@enq=%i\n",slHead->getUCN());
                                                if(slHead!=NULL)
                                                {
                                                        slHead->prev = NULL;
                                                }
                                                fDeleteMarker->shallowDelete();
                                                 //delete fDeleteMarker;//NOTE this messes up fAct in noRevS!
//                                                 printf("headUCN@enq=%i\n",slHead->getUCN());
                                        }
                                        else if (slAct==slEnd)
                                        {
                                                slEnd=slEnd->prev;
                                                slEnd->next = NULL;
                                                fDeleteMarker->shallowDelete();
                                                delete(fDeleteMarker);
                                        }
                                        else
                                        {
                                                slAct->prev->next = slAct->next;
                                                slAct->next->prev = slAct->prev;
                                                fDeleteMarker->shallowDelete();
                                                delete(fDeleteMarker);
                                        }
                                        removalOccured=TRUE;
                                        gcone::lengthOfSearchList--;
#ifndef NDEBUG
printf("Removing (");fAct->fNormal->show(1,1);printf(") from list\n");
#endif
                                        break;//leave the while loop, since we found fAct=slAct thus delete slAct and do not add fAct
                                }
                                slAct = slAct->next;
                        }//while(slAct!=NULL)
                        if(removalOccured==FALSE)
                        {
                                facet *marker=slEnd;
                                slEnd->next = fAct->shallowCopy(*fAct);
                                slEnd = slEnd->next;
                                slEnd->prev=marker;
                                gcone::lengthOfSearchList++;
                        }
                        fAct = fAct->next;
//                         if(fDeleteMarker!=NULL)
//                         {
//                                 fDeleteMarker->shallowDelete();
//                                 delete(fDeleteMarker);
//                                 fDeleteMarker=NULL;
//                         }
                }
                else
                        fAct = fAct->next;
        }//while(fAct!=NULL)

#ifdef gfanp
        gettimeofday(&end, 0);
        time_enqueue += (end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec));
#endif
//         printf("headUCN@enq=%i\n",slHead->getUCN());
        return slHead;
}

/**
* During flip2 every gc->baseRing gets two ringorder_a
* To avoid having an awful lot of those in the end we endow
* gc->baseRing by a suitable ring with (a,dp,C) and copy all
* necessary stuff over
* But first we will try to just do an inplace exchange and copying only the
* gc->gcBasis
*/
void gcone::replaceDouble_ringorder_a_ByASingleOne()
{
        ring srcRing=currRing;
        ring replacementRing=rCopy0((ring)this->baseRing);
        /*We assume we have (a(),a(),dp) here*/
        omFree(replacementRing->order);
        replacementRing->order =(int *)omAlloc0(4*sizeof(int/*64*/));
        omFree(replacementRing->block0);
        replacementRing->block0=(int *)omAlloc0(4*sizeof(int/*64*/));
        omFree(replacementRing->block1);
        replacementRing->block1=(int *)omAlloc0(4*sizeof(int/*64*/));
        omfree(replacementRing->wvhdl);
        replacementRing->wvhdl =(int **)omAlloc0(4*sizeof(int/*64*/**));

        replacementRing->order[0]=ringorder_a/*64*/;
        replacementRing->block0[0]=1;
        replacementRing->block1[0]=replacementRing->N;

        replacementRing->order[1]=ringorder_dp;
        replacementRing->block0[1]=1;
        replacementRing->block1[1]=replacementRing->N;

        replacementRing->order[2]=ringorder_C;

        int64vec *ivw = this->getIntPoint(TRUE);//returns a reference
//         assert(this->ivIntPt);
        int length=ivw->length();
        int/*64*/ *A=(int/*64*/ *)omAlloc0(length*sizeof(int/*64*/));
        for (int jj=0;jj<length;jj++)
        {
                A[jj]=(*ivw)[jj];
                if((*ivw)[jj]>=INT_MAX) WarnS("A[jj] exceeds INT_MAX in gcone::replaceDouble_ringorder_a_ByASingleOne!\n");
        }
        //delete ivw; //Not needed if this->getIntPoint(TRUE)
        replacementRing->wvhdl[0]=(int *)A;
        replacementRing->block1[0]=length;
        /*Finish*/
        rComplete(replacementRing);
        rChangeCurrRing(replacementRing);
        ideal temporaryGroebnerBasis=idrCopyR(this->gcBasis,this->baseRing);
        rDelete(this->baseRing);
        this->baseRing=rCopy(replacementRing);
        this->gcBasis=idCopy(temporaryGroebnerBasis);
        /*And back to where we came from*/
        rChangeCurrRing(srcRing);
        idDelete( (ideal*)&temporaryGroebnerBasis );
        rDelete(replacementRing);
}

/** \brief Compute the gcd of two ints
 */
static int64 int64gcd(const int64 &a, const int64 &b)
{
        int64 r, p=a, q=b;
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

static int intgcd(const int &a, const int &b)
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
// inline dd_MatrixPtr gcone::facets2Matrix(const gcone &gc)
// {
//         facet *fAct;
//         fAct = gc.facetPtr;
//
//         dd_MatrixPtr M;
//         dd_rowrange ddrows;
//         dd_colrange ddcols;
//         ddcols=(this->numVars)+1;
//         ddrows=this->numFacets;
//         dd_NumberType numb = dd_Integer;
//         M=dd_CreateMatrix(ddrows,ddcols);
//
//         int jj=0;
//
//         while(fAct!=NULL)
//         {
//                 int64vec *comp;
//                 comp = fAct->getFacetNormal();
//                 for(int ii=0;ii<this->numVars;ii++)
//                 {
//                         dd_set_si(M->matrix[jj][ii+1],(*comp)[ii]);
//                 }
//                 jj++;
//                 delete comp;
//                 fAct=fAct->next;
//         }
//         return M;
// }

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
void gcone::writeConeToFile(const gcone &gc, bool usingIntPoints)
{
        int UCN=gc.UCN;
        stringstream ss;
        ss << UCN;
        string UCNstr = ss.str();

         string prefix="/tmp/Singular/cone";
//         string prefix="./";        //crude hack -> run tests in separate directories
        string suffix=".sg";
        string filename;
        filename.append(prefix);
        filename.append(UCNstr);
        filename.append(suffix);

//         int thisPID = getpid();
//         ss << thisPID;
//         string strPID = ss.str();
//         string prefix="./";

        ofstream gcOutputFile(filename.c_str());
        assert(gcOutputFile);
        facet *fAct;
        fAct = gc.facetPtr;
        facet *f2Act;
        f2Act=fAct->codim2Ptr;

        char *ringString = rString(gc.baseRing);

        if (!gcOutputFile)
        {
                WerrorS("Error opening file for writing in writeConeToFile\n");
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
                        const int64vec *iv=fAct->getRef2FacetNormal();
//                         iv=fAct->getRef2FacetNormal();//->getFacetNormal();
                        f2Act=fAct->codim2Ptr;
                        for (int ii=0;ii<iv->length();ii++)
                        {
//                                 if (ii<iv->length()-1)
//                                         gcOutputFile << (*iv)[ii] << ",";
//                                 else
//                                         gcOutputFile << (*iv)[ii] << " ";
                                gcOutputFile << (*iv)[ii];
                                (ii<iv->length()-1) ? gcOutputFile << "," : gcOutputFile << " ";
                        }
                        //delete iv;
                        while(f2Act!=NULL)
                        {
                                const int64vec *iv2;
                                iv2=f2Act->getRef2FacetNormal();
                                for(int jj=0;jj<iv2->length();jj++)
                                {
//                                         if (jj<iv2->length()-1)
//                                                 gcOutputFile << (*iv2)[jj] << ",";
//                                         else
//                                                 gcOutputFile << (*iv2)[jj] << " ";
                                        gcOutputFile << (*iv2)[jj];
                                        (jj<iv2->length()-1) ? gcOutputFile << "," : gcOutputFile << " ";
                                }
                                f2Act = f2Act->next;
                        }
                        gcOutputFile << endl;
                        fAct=fAct->next;
                }
        gcOutputFile.close();
        }
        delete [] ringString;

}//writeConeToFile(gcone const &gc)

/** \brief Reads a cone from a file identified by its number
* ||depending on whether flip or flip2 is used, switch the flag flipFlag
* ||defaults to 0 => flip
* ||1 => flip2
*/
void gcone::readConeFromFile(int UCN, gcone *gc)
{
        //int UCN=gc.UCN;
        stringstream ss;
        ss << UCN;
        string UCNstr = ss.str();
        int gcBasisLength=0;
        size_t found;        //used for find_first_(not)_of

        string prefix="/tmp/Singular/cone";
        string suffix=".sg";
        string filename;
        filename.append(prefix);
        filename.append(UCNstr);
        filename.append(suffix);

        ifstream gcInputFile(filename.c_str(), ifstream::in);

        ring saveRing=currRing;
        //Comment the following if you uncomment the if(line=="RING") part below
//          rChangeCurrRing(gc->baseRing);

        while( !gcInputFile.eof() )
        {
                string line;
                getline(gcInputFile,line);
                if(line=="RING")
                {
                        getline(gcInputFile,line);
                        found = line.find("a(");
                        line.erase(0,found+2);
                        string strweight;
                        strweight=line.substr(0,line.find_first_of(")"));

                        int64vec *iv=new int64vec(this->numVars);//
                        for(int ii=0;ii<this->numVars;ii++)
                        {
                                string weight;
                                weight=line.substr(0,line.find_first_of(",)"));
                                char *w=new char[weight.size()+1];
                                strcpy(w,weight.c_str());
                                (*iv)[ii]=atol(w/*weight.c_str()*/);//Better to long. Weight bound in Singular:2147483647
                                delete[] w;
                                line.erase(0,line.find_first_of(",)")+1);
                        }
                        found = line.find("a(");

                        ring newRing;
                        if(currRing->order[0]!=ringorder_a/*64*/)
                        {
                                        newRing=rCopyAndAddWeight(currRing,iv);
                        }
                        else
                        {
                                        newRing=rCopy0(currRing);
                                        int length=this->numVars;
                                        int *A=(int *)omAlloc0(length*sizeof(int));
                                        for(int jj=0;jj<length;jj++)
                                        {
                                                A[jj]=(*iv)[jj];
                                        }
                                        omFree(newRing->wvhdl[0]);
                                        newRing->wvhdl[0]=(int*)A;
                                        newRing->block1[0]=length;
                        }
                        delete iv;
                         rComplete(newRing);
                        gc->baseRing=rCopy(newRing);
                        rDelete(newRing);
                        rComplete(gc->baseRing);
                        if(currRing!=gc->baseRing)
                                rChangeCurrRing(gc->baseRing);
                }

                if(line=="GCBASISLENGTH")
                {
                        string strGcBasisLength;
                        getline(gcInputFile, line);
                        strGcBasisLength = line;
                        char *s=new char[strGcBasisLength.size()+1];
                        strcpy(s,strGcBasisLength.c_str());
                        int size=atoi(s/*strGcBasisLength.c_str()*/);
                        delete[] s;
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
//                                 poly strPoly;//=pInit();//Ought to be inside the while loop, but that will eat your memory
                                poly resPoly=pInit();        //The poly to be read in
                                while(!line.empty())
                                {
                                        poly strPoly;//=pInit();

                                        string strMonom, strCoeff, strCoeffNom, strCoeffDenom;
                                        bool hasCoeffInQ = FALSE;        //does the polynomial have rational coeff?
                                        bool hasNegCoeff = FALSE;        //or a negative one?
                                        found = line.find_first_of("+-");        //get the first monomial
                                        string tmp;
                                        tmp=line[found];
//                                         if(found!=0 && (tmp.compare("-")==0) )
//                                                 hasNegCoeff = TRUE;        //We have a coeff < 0
                                        if(found==0)
                                        {
                                                if(tmp.compare("-")==0)
                                                        hasNegCoeff = TRUE;
                                                line.erase(0,1);        //remove leading + or -
                                                found = line.find_first_of("+-");        //adjust found
                                        }
                                        strMonom = line.substr(0,found);
                                        line.erase(0,found);
                                        number nCoeff=nInit(1);
                                        number nCoeffNom=nInit(1);
                                        number nCoeffDenom=nInit(1);
                                        found = strMonom.find_first_of("/");
                                        if(found!=string::npos)        //i.e. "/" exists in strMonom
                                        {
                                                hasCoeffInQ = TRUE;
                                                strCoeffNom=strMonom.substr(0,found);
                                                strCoeffDenom=strMonom.substr(found+1,strMonom.find_first_not_of("1234567890",found+1));
                                                strMonom.erase(0,found);
                                                strMonom.erase(0,strMonom.find_first_not_of("1234567890/"));
                                                char *Nom=new char[strCoeffNom.size()+1];
                                                char *Denom=new char[strCoeffDenom.size()+1];
                                                strcpy(Nom,strCoeffNom.c_str());
                                                strcpy(Denom,strCoeffDenom.c_str());
                                                nRead(Nom/*strCoeffNom.c_str()*/, &nCoeffNom);
                                                nRead(Denom/*strCoeffDenom.c_str()*/, &nCoeffDenom);
                                                delete[] Nom;
                                                delete[] Denom;
                                        }
                                        else
                                        {
                                                found = strMonom.find_first_not_of("1234567890");
                                                strCoeff = strMonom.substr(0,found);
                                                if(!strCoeff.empty())
                                                {
                                                        char *coeff = new char[strCoeff.size()+1];
                                                        strcpy(coeff, strCoeff.c_str());
                                                         nRead(coeff/*strCoeff.c_str()*/,&nCoeff);
                                                        delete[] coeff;
                                                }
                                        }
                                        char* monom = new char[strMonom.size()+1];
                                        strcpy(monom, strMonom.c_str());
                                        p_Read(monom,strPoly,currRing);        //strPoly:=monom
                                        delete[] monom;
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
                                                                pSetCoeff(strPoly, nCoeff );
                                                        }
                                                        break;
                                        }
                                                //pSetCoeff(strPoly, (number) intCoeff);//Why is this set to zero instead of 1???
                                        if(pIsConstantComp(resPoly))
                                        {
                                                resPoly=pCopy(strPoly);
                                                pDelete(&strPoly);
                                        }
                                        else
                                        {
//                                                 poly tmp=pAdd(pCopy(resPoly),strPoly);//foo is destroyed
//                                                 pDelete(&resPoly);
//                                                 resPoly=tmp;
//                                                 pDelete(&tmp);
                                                resPoly=pAdd(resPoly,strPoly);//pAdd = p_Add_q, destroys args
                                        }
                                        /*if(nCoeff!=NULL)
                                                nDelete(&nCoeff);*/ //NOTE This may cause a crash on certain examples...
                                        nDelete(&nCoeffNom);
                                        nDelete(&nCoeffDenom);
                                }//while(!line.empty())
                                gc->gcBasis->m[jj]=pCopy(resPoly);
                                pDelete(&resPoly);        //reset
                        }
//                         break;
                }//if(line=="GCBASIS")
                if(line=="FACETS")
                {
                        facet *fAct=gc->facetPtr;
                        while(fAct!=NULL)
                        {
                                getline(gcInputFile,line);
                                found = line.find("\t");
                                string normalString=line.substr(0,found);
                                int64vec *fN = new int64vec(this->numVars);
                                for(int ii=0;ii<this->numVars;ii++)
                                {
                                        string component;
                                        found = normalString.find(",");
                                        component=normalString.substr(0,found);
                                        char *sComp = new char[component.size()+1];
                                        strcpy(sComp,component.c_str());
                                        (*fN)[ii]=atol(sComp/*component.c_str()*/);
                                        delete[] sComp;
                                        normalString.erase(0,found+1);
                                }
                                /*Only the following line needs to be commented out if you decide not to delete fNormals*/
//                                 fAct->setFacetNormal(fN);
                                delete(fN);
                                fAct = fAct->next;        //Booh, this is ugly
                        }
                        break; //NOTE Must always be in the last if-block!
                }
        }//while(!gcInputFile.eof())
        gcInputFile.close();
        rChangeCurrRing(saveRing);
}


/** \brief Sort the rays of a facet lexicographically
*/
// void gcone::sortRays(gcone *gc)
// {
//         facet *fAct;
//         fAct = this->facetPtr->codim2Ptr;
//         while(fAct->next!=NULL)
//         {
//                 if(fAct->fNormal->compare(fAct->fNormal->next)==-1
//         }
// }

/** \brief Gather the output
* List of lists
* If heuristic==1 readConeFromFile() is called once more on every cone. This may slow down the computation but it also
* allows us to rDelete(gcDel->baseRing) and the such in gcone::noRevS.
*\param *gc Pointer to gcone, preferably gcRoot ;-)
*\param n the number of cones as determined by gcRoot->getCounter()
*
*/
lists lprepareResult(gcone *gc, const int n)
{
        gcone *gcAct;
        gcAct = gc;
        facet *fAct;
        fAct = gc->facetPtr;

        lists res=(lists)omAllocBin(slists_bin);
        res->Init(n);        //initialize to store n cones
        for(int ii=0;ii<n;ii++)
        {
                if(gfanHeuristic==1)// && gcAct->getUCN()>1)
                {
                        gcAct->readConeFromFile(gcAct->getUCN(),gcAct);
//                         rChangeCurrRing(gcAct->getBaseRing());//NOTE memleak?
                }
                rChangeCurrRing(gcAct->getRef2BaseRing());
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
//                 if(gcAct->gcBasis->m[0]==(poly)NULL)
//                 if(gfanHeuristic==1 && gcAct->getUCN()>1)
//                         gcAct->readConeFromFile(gcAct->getUCN(),gcAct);
//                 ring saveRing=currRing;
//                 ring tmpRing=gcAct->getBaseRing;
//                 rChangeCurrRing(tmpRing);
//                 l->m[1].data=(void*)idrCopyR_NoSort(gcAct->gcBasis,gcAct->getBaseRing());
//                 l->m[1].data=(void*)idrCopyR(gcAct->gcBasis,gcAct->getBaseRing());//NOTE memleak?
                l->m[1].data=(void*)idrCopyR(gcAct->gcBasis,gcAct->getRef2BaseRing());
//                 rChangeCurrRing(saveRing);

                l->m[2].rtyp=INTVEC_CMD;
                int64vec iv=(gcAct->f2M(gcAct,gcAct->facetPtr));//NOTE memleak?
                l->m[2].data=(void*)iv64Copy(&iv);

                l->m[3].rtyp=LIST_CMD;
                        lists lCodim2List = (lists)omAllocBin(slists_bin);
                        lCodim2List->Init(gcAct->numFacets);
                        fAct = gcAct->facetPtr;//fAct->codim2Ptr;
                        int jj=0;
                        while(fAct!=NULL && jj<gcAct->numFacets)
                        {
                                lCodim2List->m[jj].rtyp=INTVEC_CMD;
                                int64vec ivC2=(gcAct->f2M(gcAct,fAct,2));
                                lCodim2List->m[jj].data=(void*)iv64Copy(&ivC2);
                                jj++;
                                fAct = fAct->next;
                        }
                l->m[3].data=(void*)lCodim2List;
                l->m[4].rtyp=INTVEC_CMD/*RING_CMD*/;
                l->m[4].data=(void*)(gcAct->getIntPoint/*BaseRing*/());
                l->m[5].rtyp=INT_CMD;
                l->m[5].data=(void*)gcAct->getPredUCN();
                res->m[ii].data=(void*)l;
                gcAct = gcAct->next;
        }
        return res;
}

/** Convert gc->gcRays into an intvec in order to be used with bbcone stuff*/
intvec *gcRays2Intmat(gcone *gc)
{
  int r = gc->numRays;
  int c = gc->numVars;  //Spalten
  intvec *res = new intvec(r,c,(int)0);

  int offset=0;
  for(int ii=0;ii<gc->numRays;ii++)
  {
    int64vec *ivTmp=iv64Copy(gc->gcRays[ii]);
    for(int jj=0;jj<pVariables;jj++)
      (*res)[offset+jj]=(int)(*ivTmp)[jj];
    offset += pVariables;
    delete ivTmp;
  }
  return res;
}

/** \brief Put stuff in gfanlib's datatype gfan::ZFan
*/
void prepareGfanLib(gcone *gc, gfan::ZFan *fan)
{
  using namespace gfan;
  int ambientDimension = gc->numVars;
  gcone *gcAct;
  gcAct = gc;

  //Iterate over all cones and adjoin to PolyhedralFan
   while(gcAct!=NULL)
  {
    intvec *rays=gcRays2Intmat(gcAct);
    ZMatrix zm = intmat2ZMatrix(rays);
    delete rays;
    ZCone *zc = new ZCone();
    *zc = ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
//     delete &zm;
    zc->canonicalize();//As per Anders' hint
    fan->insert(*zc);
//     delete zc;
    gcAct=gcAct->next;
  }
}

/** \brief Write facets of a cone into a matrix
* Takes a pointer to a facet as 2nd arg
* f should always point to gc->facetPtr
* param n is used to determine whether it operates in codim 1 or 2
* We have to cast the int64vecs to int64vec due to issues with list structure
*/
inline int64vec gcone::f2M(gcone *gc, facet *f, int n)
{
        facet *fAct;
        int64vec *res;//=new int64vec(this->numVars);
//         int codim=n;
//         int bound;
//         if(f==gc->facetPtr)
        if(n==1)
        {
                int64vec *m1Res=new int64vec(gc->numFacets,gc->numVars,0);
                res = iv64Copy(m1Res);
                fAct = gc->facetPtr;
                delete m1Res;
//                 bound = gc->numFacets*(this->numVars);
        }
        else
        {
                fAct = f->codim2Ptr;
                int64vec *m2Res = new int64vec(f->numCodim2Facets,gc->numVars,0);
                res = iv64Copy(m2Res);
                delete m2Res;
//                 bound = fAct->numCodim2Facets*(this->numVars);

        }
        int ii=0;
        while(fAct!=NULL )//&& ii < bound )
        {
                const int64vec *fNormal;
                fNormal = fAct->getRef2FacetNormal();//->getFacetNormal();
                for(int jj=0;jj<this->numVars;jj++)
                {
                        (*res)[ii]=(int)(*fNormal)[jj];//This is ugly and prone to overflow
                        ii++;
                }
                fAct = fAct->next;
        }
        return *res;
}

int gcone::counter=0;
int gfanHeuristic;
int gcone::lengthOfSearchList;
int gcone::maxSize;
dd_MatrixPtr gcone::dd_LinealitySpace;
int64vec *gcone::hilbertFunction;
#ifdef gfanp
// int gcone::lengthOfSearchList=0;
float gcone::time_getConeNormals;
float gcone::time_getCodim2Normals;
float gcone::t_getExtremalRays;
float gcone::t_ddPolyh;
float gcone::time_flip;
float gcone::time_flip2;
float gcone::t_areEqual;
float gcone::t_markings;
float gcone::t_dd;
float gcone::t_kStd=0;
float gcone::time_enqueue;
float gcone::time_computeInv;
float gcone::t_ddMC;
float gcone::t_mI;
float gcone::t_iP;
float gcone::t_isParallel;
unsigned gcone::parallelButNotEqual=0;
unsigned gcone::numberOfFacetChecks=0;
#endif
int gcone::numVars;
bool gcone::hasHomInput=FALSE;
int64vec *gcone::ivZeroVector;
// ideal gfan(ideal inputIdeal, int h)
/** Main routine
 * The first and second parameter are mandatory. The third (and maybe fourth) parameter is for Janko :)
 */
#ifndef USE_ZFAN
lists grfan(ideal inputIdeal, int h, bool singleCone=FALSE)
#else
gfan::ZFan* grfan(ideal inputIdeal, int h, bool singleCone=FALSE)
#endif
{
        lists lResList; //this is the object we return
        gfan::ZFan *zResFan = new gfan::ZFan(pVariables);

        if(rHasGlobalOrdering(currRing))
        {
//                 int numvar = pVariables;
                gfanHeuristic = h;

                enum searchMethod {
                        reverseSearch,
                        noRevS
                };

                searchMethod method;
                method = noRevS;

                ring inputRing=currRing;        // The ring the user entered
//                 ring rootRing;                        // The ring associated to the target ordering

                dd_set_global_constants();
                if(method==noRevS)
                {
                        gcone *gcRoot = new gcone(currRing,inputIdeal);
                        gcone *gcAct;
                        gcAct = gcRoot;
                        gcone::numVars=pVariables;
                        //gcAct->numVars=pVariables;//NOTE is now static
                        gcAct->getGB(inputIdeal);
                        /*Check whether input is homogeneous
                        if TRUE each facet intersects the positive orthant, so we don't need the
                        flippability test in getConeNormals & getExtremalRays
                        */
                        if(idHomIdeal(gcAct->gcBasis,NULL))//disabled for tests
                        {
                                gcone::hasHomInput=TRUE;
//                                 gcone::hilbertFunction=hHstdSeries(inputIdeal,NULL,NULL,NULL,currRing);
                        }
                        else
                        {
                                gcone::ivZeroVector = new int64vec(pVariables);
                                for(int ii=0;ii<pVariables;ii++)
                                        (*gcone::ivZeroVector)[ii]=0;
                        }

                        if(isMonomial(gcAct->gcBasis))
                        {//FIXME
                                WerrorS("Monomial input - terminating");
                                dd_free_global_constants();
                                //This is filthy
                                goto pointOfNoReturn;
                        }
                        gcAct->getConeNormals(gcAct->gcBasis);
                        gcone::dd_LinealitySpace = gcAct->computeLinealitySpace();
                        gcAct->getExtremalRays(*gcAct);
                        if(singleCone==FALSE)//Is Janko here?
                        {//Compute the whole fan
                          gcAct->noRevS(*gcAct);        //Here we go!
                        }
                        //Switch back to the ring the computation was started in
                        rChangeCurrRing(inputRing);
                        //res=gcAct->gcBasis;
                        //Below is a workaround, since gcAct->gcBasis gets deleted in noRevS
#ifndef USE_ZFAN
                        lResList=lprepareResult(gcRoot,gcRoot->getCounter());
#else
                        prepareGfanLib(gcRoot,zResFan);
#endif
                        /*Cleanup*/
                        gcone *gcDel;
                        gcDel = gcRoot;
                        gcAct = gcRoot;
                        while(gcAct!=NULL)
                        {
                                gcDel = gcAct;
                                gcAct = gcAct->next;
//                                 delete gcDel;
                        }
                }//method==noRevS
                dd_FreeMatrix(gcone::dd_LinealitySpace);
                dd_free_global_constants();
        }//rHasGlobalOrdering
        else
        {
                //Simply return an empty list
                WerrorS("Ring has non-global ordering.\nThis function requires your current ring to be endowed with a global ordering.\n Now terminating!");
//                 gcone *gcRoot=new gcone();
//                 gcone *gcPtr = gcRoot;
//                 for(int ii=0;ii<10000;ii++)
//                 {
//                         gcPtr->setBaseRing(currRing);
//                         facet *fPtr=gcPtr->facetPtr=new facet();
//                         for(int jj=0;jj<5;jj++)
//                         {
//                                 int64vec *iv=new int64vec(pVariables);
//                                 fPtr->setFacetNormal(iv);
//                                 delete(iv);
//                                 fPtr->next=new facet();
//                                 fPtr=fPtr->next;
//                         }
//                         gcPtr->next=new gcone();
//                         gcPtr->next->prev=gcPtr;
//                         gcPtr=gcPtr->next;
//                 }
//                 gcPtr=gcRoot;
//                 while(gcPtr!=NULL)
//                 {
//                         gcPtr=gcPtr->next;
// //                         delete(gcPtr->prev);
//                 }
                goto pointOfNoReturn;
        }
        /*Return result*/
#ifdef gfanp
        cout << endl << "t_getConeNormals:" << gcone::time_getConeNormals << endl;
        /*cout << "t_getCodim2Normals:" << gcone::time_getCodim2Normals << endl;
        cout << "  t_ddMC:" << gcone::t_ddMC << endl;
        cout << "  t_mI:" << gcone::t_mI << endl;
        cout << "  t_iP:" << gcone::t_iP << endl;*/
        cout << "t_getExtremalRays:" << gcone::t_getExtremalRays << endl;
        cout << "  t_ddPolyh:" << gcone::t_ddPolyh << endl;
        cout << "t_Flip:" << gcone::time_flip << endl;
        cout << "  t_markings:" << gcone::t_markings << endl;
        cout << "  t_dd:" << gcone::t_dd << endl;
        cout << "  t_kStd:" << gcone::t_kStd << endl;
        cout << "t_Flip2:" << gcone::time_flip2 << endl;
        cout << "  t_dd:" << gcone::t_dd << endl;
        cout << "  t_kStd:" << gcone::t_kStd << endl;
        cout << "t_computeInv:" << gcone::time_computeInv << endl;
        cout << "t_enqueue:" << gcone::time_enqueue << endl;
        cout << "  t_areEqual:" <<gcone::t_areEqual << endl;
        cout << "t_isParallel:" <<gcone::t_isParallel << endl;
        cout << endl;
        cout << "Checked " << gcone::numberOfFacetChecks << " Facets" << endl;
        cout << " out of which there were " << gcone::parallelButNotEqual << " parallel but not equal." << endl;
#endif
        printf("Maximum length of list of facets: %i", gcone::maxSize);
pointOfNoReturn:
#ifndef USE_ZFAN
        return lResList;
#else
        return zResFan;
#endif
}

/** Compute a single Gröbner cone by specifying an ideal and a weight vector.
 * NOTE: We do NOT check whether the vector is from the relative interior of the cone.
 * That is upon the user to assure.
 */
// lists grcone_by_intvec(ideal inputIdeal)
// {
//   if( (rRingOrder_t)currRing->order[0] == ringorder_wp)
//   {
//     lists lResList;
//     lResList=grfan(inputIdeal, 0, TRUE);
//   }
//   else
//     WerrorS("Need wp ordering");
// }
#endif
