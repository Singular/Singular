/*
gfan.h Interface to gfan.cc

Author: monerjan
*/
#ifndef GFAN_H
#define GFAN_H

#include "kernel/mod2.h"

#if HAVE_GFANLIB

#include "misc/int64vec.h"

#include "gfanlib/config.h"
#ifdef HAVE_CDD_SETOPER_H
#include <cdd/setoper.h>
#include <cdd/cdd.h>
#include <cdd/cddmp.h>
#elif HAVE_CDDLIB_SETOPER_H
#include <cddlib/setoper.h>
#include <cddlib/cdd.h>
#include <cddlib/cddmp.h>
#else
#include <setoper.h>
#include <cdd.h>
#include <cddmp.h>
#endif
#include "bbfan.h"
#include "bbcone.h"
THREAD_VAR extern int gfanHeuristic;

#ifndef USE_ZFAN
#define USE_ZFAN
#endif
#ifndef USE_ZFAN
  lists grfan(ideal inputIdeal, int heuristic, bool singleCone);
#else
  #include "gfanlib/gfanlib.h"
  gfan::ZFan *grfan(ideal inputIdeal, int h, bool singleCone);
#endif
// lists grcone_by_intvec(ideal inputIdeal);

class facet
{
        private:
                /** \brief Inner normal of the facet, describing it uniquely up to isomorphism */
                int64vec *fNormal;

                /** \brief An interior point of the facet*/
                int64vec *interiorPoint;

                /** \brief Universal Cone Number
                 * The number of the cone the facet belongs to, Set in getConeNormals()
                 */
                int UCN;

                /** \brief The codim of the facet
                 */
                short codim;

                /** \brief The Groebner basis on the other side of a shared facet
                 *
                 * In order not to have to compute the flipped GB twice we store the basis we already get
                 * when identifying search facets. Thus in the next step of the reverse search we can
                 * just copy the old cone and update the facet and the gcBasis.
                 * facet::flibGB is set via facet::setFlipGB() and printed via facet::printFlipGB
                 */
                ideal flipGB;                //The Groebner Basis on the other side, computed via gcone::flip

        public:
                /** \brief Boolean value to indicate whether a facet is flippable or not
                 * This is also used to mark facets that nominally are flippable but which do
                 * not intersect with the positive orthant. This check is done in gcone::getCodim2Normals
                 */
                bool isFlippable;        //**flippable facet? */
                //bool isIncoming;        //Is the facet incoming or outgoing in the reverse search? No longer in use
                facet *next;                //Pointer to next facet
                facet *prev;                //Pointer to predecessor. Needed for the SearchList in noRevS
                facet *codim2Ptr;        //Pointer to (codim-2)-facet. Bit of recursion here ;-)
                int numCodim2Facets;        //#of (codim-2)-facets of this facet. Set in getCodim2Normals()
                unsigned numRays;        //Number of spanning rays of the facet
                ring flipRing;                //the ring on the other side of the facet
//                 int64vec **fRays;

                /** The default constructor. */
                facet();
                /** Constructor for lower dimensional faces*/
                facet(const int &n);
                /**  The copy constructor */
                facet(const facet& f);
                /** A shallow copy of facets*/
                facet* shallowCopy(const facet& f);
                void shallowDelete();
                /** The default destructor */
                ~facet();
                /** Comparison operator*/
//                 inline bool operator==(const facet *f,const facet *g);
                /** \brief Comparison of facets*/
//                 inline bool areEqual(facet *f, facet *g);//Now static
                /** Stores the facet normal \param int64vec*/
                inline void setFacetNormal(int64vec *iv);
                /** Returns the facet normal */
                inline int64vec *getFacetNormal() const;
                /** Return a reference to the facet normal*/
                inline const int64vec *getRef2FacetNormal() const;
                /** Method to print the facet normal*/
                inline void printNormal() const;
                /** Store the flipped GB*/
                inline void setFlipGB(ideal I);
                /** Return the flipped GB*/
                inline ideal getFlipGB();
                /** Print the flipped GB*/
                inline void printFlipGB();
                /** Set the UCN */
                inline void setUCN(int n);
                /** \brief Get the UCN
                 * Returns the UCN iff this != NULL, else -1
                 */
                inline int getUCN();
                /** Store an interior point of the facet */
                inline void setInteriorPoint(int64vec *iv);
                inline int64vec *getInteriorPoint();
                inline const int64vec *getRef2InteriorPoint();
                /** \brief Debugging function
                 * prints the facet normal an all (codim-2)-facets that belong to it
                 */
                volatile void fDebugPrint();
                friend class gcone;
};


/**
 *\brief Implements the cone structure
 *
 * A cone is represented by a linked list of facet normals
 * @see facet
 */

class gcone
{
        private:
                ideal inputIdeal;        //the original
                ring baseRing;                //the basering of the cone
                int64vec *ivIntPt;        //an interior point of the cone
                int UCN;                //unique number of the cone
                int pred;                //UCN of the cone this one is derived from
                 THREAD_VAR static int counter;

        public:
                /** \brief Pointer to the first facet */
                facet *facetPtr;        //Will hold the adress of the first facet; set by gcone::getConeNormals
#ifdef gfanp
                THREAD_VAR static float time_getConeNormals;
                THREAD_VAR static float time_getCodim2Normals;
                THREAD_VAR static float t_getExtremalRays;
                THREAD_VAR static float t_ddPolyh;
                THREAD_VAR static float time_flip;
                THREAD_VAR static float time_flip2;
                THREAD_VAR static float t_areEqual;
                THREAD_VAR static float t_ffG;
                THREAD_VAR static float t_markings;
                THREAD_VAR static float t_dd;
                THREAD_VAR static float t_kStd;
                THREAD_VAR static float time_enqueue;
                THREAD_VAR static float time_computeInv;
                THREAD_VAR static float t_ddMC;
                THREAD_VAR static float t_mI;
                THREAD_VAR static float t_iP;
                THREAD_VAR static float t_isParallel;
                THREAD_VAR static unsigned parallelButNotEqual;
                THREAD_VAR static unsigned numberOfFacetChecks;
#endif
                /** Matrix to contain the homogeneity/lineality space */
                THREAD_VAR static dd_MatrixPtr dd_LinealitySpace;
                THREAD_VAR static int lengthOfSearchList;
                /** Maximum size of the searchlist*/
                THREAD_VAR static int maxSize;
                /** is the ideal homogeneous? */
                THREAD_VAR static bool hasHomInput;
                /** # of variables in the ring */
                THREAD_VAR static int numVars;                //#of variables in the ring
                /** The hilbert function - for the homogeneous case*/
                THREAD_VAR static int64vec *hilbertFunction;
                /** The zero vector. Needed in case of fNormal mismatch*/
                THREAD_VAR static int64vec *ivZeroVector;

                /** # of facets of the cone
                 * This value is set by gcone::getConeNormals
                 */
                int numFacets;                //#of facets of the cone

                /**
                 * At least as a workaround we store the irredundant facets of a matrix here.
                 * This is needed to compute an interior points of a cone. Note that there
                 * will be non-flippable facets in it!
                 */
                dd_MatrixPtr ddFacets;        //Matrix to store irredundant facets of the cone

                /** Array of intvecs representing the rays of the cone*/
                int64vec **gcRays;
                unsigned numRays;        //#rays of the cone
                /** Contains the Groebner basis of the cone. Is set by gcone::getGB(ideal I)*/
                ideal gcBasis;                //GB of the cone, set by gcone::getGB();
                gcone *next;                //Pointer to next cone
                gcone *prev;

                gcone();
                gcone(ring r, ideal I);
                gcone(const gcone& gc, const facet &f);
                ~gcone();
                inline int getCounter();
                inline ring getBaseRing();
                inline ring getRef2BaseRing();
                inline void setBaseRing(ring r);
                inline void setIntPoint(int64vec *iv);
                inline int64vec *getIntPoint(bool shallow=FALSE);
                inline void showIntPoint();
                inline void setNumFacets();
                inline int getNumFacets();
                inline int getUCN();
                inline int getPredUCN();
                volatile void showFacets(short codim=1);
//                 volatile void showSLA(facet &f);
//                 void idDebugPrint(const ideal &I);
//                 void invPrint(const ideal &I);
//                 bool isMonomial(const ideal &I);
//                 int64vec *ivNeg(const int64vec *iv);
//                 inline int dotProduct(int64vec &iva, int64vec &ivb);
//                 inline int dotProduct(const int64vec &iva, const int64vec &ivb);
//                 inline bool isParallel(const int64vec &a, const int64vec &b);
                void noRevS(gcone &gcRoot, bool usingIntPoint=FALSE);
//                 inline int intgcd(const int &a, const int &b);
                void writeConeToFile(const gcone &gc, bool usingIntPoints=FALSE);
                void readConeFromFile(int gcNum, gcone *gc);
                int64vec f2M(gcone *gc, facet *f, int n=1);
//                 inline void sortRays(gcone *gc);
                //The real stuff
                void getConeNormals(const ideal &I, bool compIntPoint=FALSE);
                void getCodim2Normals(const gcone &gc);
                void getExtremalRays(const gcone &gc);
                void orderRays();
                void flip(ideal gb, facet *f);
                void flip2(const ideal &gb, facet *f);
                void computeInv(const ideal &gb, ideal &inv, const int64vec &f);
                //poly restOfDiv(poly const &f, ideal const &I); removed with r12286
                inline ideal ffG(const ideal &H, const ideal &G);
                inline void getGB(ideal const &inputIdeal);
                void interiorPoint( dd_MatrixPtr &M, int64vec &iv);//used from flip and optionally from getConeNormals
//                 void interiorPoint2(); //removed Feb 8th, 2010, new method Feb 19th, 2010, again removed Mar 16th, 2010
                void preprocessInequalities(dd_MatrixPtr &M);
                ring rCopyAndAddWeight(const ring &r, int64vec *ivw);
                ring rCopyAndAddWeight2(const ring &, const int64vec *, const int64vec *);
//                 ring rCopyAndChangeWeight(const ring &r, int64vec *ivw);        //NOTE remove
//                 void reverseSearch(gcone *gcAct); //NOTE both removed from r12286
//                 bool isSearchFacet(gcone &gcTmp, facet *testfacet); //NOTE remove
                void makeInt(const dd_MatrixPtr &M, const int line, int64vec &n);
//                 void normalize();//NOTE REMOVE
                facet * enqueueNewFacets(facet *f);
                facet * enqueue2(facet *f);
//                 dd_MatrixPtr facets2Matrix(const gcone &gc);//NOTE remove
                /** Compute the lineality space Ax=0 and return it as dd_MatrixPtr dd_LinealitySpace*/
                dd_MatrixPtr computeLinealitySpace();
                inline bool iv64isStrictlyPositive(const int64vec *);
                /** Exchange 2 ordertype_a by just 1 */
                void replaceDouble_ringorder_a_ByASingleOne();
//                 static void gcone::idPrint(ideal &I);
//                 friend class facet;
};
lists lprepareResult(gcone *gc, const int n);
/* static int64 int64gcd(const int64 &a, const int64 &b); */
/* static int intgcd(const int &a, const int &b); */
/* static int dotProduct(const int64vec &iva, const int64vec &ivb); */
/* static bool isParallel(const int64vec &a, const int64vec &b); */
/* static int64vec *ivNeg(/\*const*\/ int64vec *iv); */
/* static void idDebugPrint(const ideal &I); */
/* static volatile void showSLA(facet &f); */
/* static bool isMonomial(const ideal &I); */
/* static bool ivAreEqual(const int64vec &a, const int64vec &b); */
/* static bool areEqual2(facet *f, facet *g); */
/* static bool areEqual( facet *f, facet *g); */
// bool iv64isStrictlyPositive(int64vec *);
#endif
#endif
