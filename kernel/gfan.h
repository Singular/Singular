/*
gfan.h Interface to gfan.cc

$Author: monerjan $
$Date: 2009/11/03 06:57:32 $
$Header: /usr/local/Singular/cvsroot/kernel/gfan.h,v 1.13 2009/11/03 06:57:32 monerjan Exp $
$Id$
*/
#ifdef HAVE_GFAN

#ifndef GFAN_H
#define GFAN_H

#include "intvec.h"
#include "intvec.h"

#define p800
#ifdef p800
#include "../../cddlib/include/setoper.h"
#include "../../cddlib/include/cdd.h"
#include "../../cddlib/include/cddmp.h"
#endif
extern int gfanHeuristic;
// extern dd_MatrixPtr ddLinealitySpace;
#define gfanp
// #ifdef gfanp
// extern	static float time_getConeNormals;
// extern	static float time_getCodim2Normals;
// extern	static float time_flip;
// extern	static float time_enqueue;
// extern	static float time_computeInv;
// #endif
//ideal getGB(ideal inputIdeal);
// ideal gfan(ideal inputIdeal, int heuristic);
lists gfan(ideal inputIdeal, int heuristic);
//int dotProduct(intvec a, intvec b);
//bool isParallel(intvec a, intvec b);

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
		unsigned numRays;	//Number of spanning rays of the cone			
		/** The default constructor. */
		facet();
		/** Constructor for lower dimensional faces*/
		facet(int const &n);
		/**  The copy constructor */
		facet(const facet& f);
		facet(const facet& f, bool shallow);
		/** The default destructor */
		~facet();
		/** Comparison operator*/
// 		inline bool operator==(const facet *f,const facet *g);			
		/** \brief Comparison of facets*/
		inline bool areEqual(facet *f, facet *g);
		/** Stores the facet normal \param intvec*/
		inline void setFacetNormal(intvec *iv);
		/** Hopefully returns the facet normal */
		inline intvec *getFacetNormal();
		/** Return a reference to the facet normal*/
		inline const intvec *getRef2FacetNormal();
		/** Method to print the facet normal*/
		inline void printNormal();
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
		inline void setInteriorPoint(intvec *iv);
		inline intvec *getInteriorPoint();
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
// 		ring rootRing;		//good to know this -> generic walk
		ideal inputIdeal;	//the original
		ring baseRing;		//the basering of the cone				
		intvec *ivIntPt;	//an interior point of the cone
		int UCN;		//unique number of the cone
		int pred;		//UCN of the cone this one is derived from
 		static int counter;
		
	public:	
		/** \brief Pointer to the first facet */
		facet *facetPtr;	//Will hold the adress of the first facet; set by gcone::getConeNormals
#ifdef gfanp
		static float time_getConeNormals;
		static float time_getCodim2Normals;
		static float t_getExtremalRays;
		static float time_flip;
		static float time_flip2;
		static float t_areEqual;
		static float t_ffG;
		static float t_markings;
		static float t_dd;
		static float t_kStd;
		static float time_enqueue;		
		static float time_computeInv;
		static float t_ddMC;
		static float t_mI;
		static float t_iP;
		static unsigned parallelButNotEqual;
		static unsigned numberOfFacetChecks;
#endif
		/** Matrix to contain the homogeneity/lineality space */
		static dd_MatrixPtr dd_LinealitySpace;
		static int lengthOfSearchList;
		/** Maximum size of the searchlist*/
		static int maxSize;
		/** is the ideal homogeneous? */
		static bool hasHomInput;
		/** # of variables in the ring */
		int numVars;		//#of variables in the ring
		/** The hilbert function - for the homogeneous case*/
		static intvec *hilbertFunction;
		
		/** # of facets of the cone
		 * This value is set by gcone::getConeNormals
		 */
		int numFacets;		//#of facets of the cone
		
		/**
		 * At least as a workaround we store the irredundant facets of a matrix here.
		 * This is needed to compute an interior points of a cone. Note that there 
		 * will be non-flippable facets in it!		 
		 */
		dd_MatrixPtr ddFacets;	//Matrix to store irredundant facets of the cone
		
		/** Contains the Groebner basis of the cone. Is set by gcone::getGB(ideal I)*/
		ideal gcBasis;		//GB of the cone, set by gcone::getGB();
		gcone *next;		//Pointer to next cone
		gcone *prev;
		
		gcone();
		gcone(ring r, ideal I);
		gcone(const gcone& gc, const facet &f);
		~gcone();
		inline int getCounter();
		inline ring getBaseRing();
		inline void setIntPoint(intvec *iv);
		inline intvec *getIntPoint();
		inline void showIntPoint();
		inline void setNumFacets();
		inline int getNumFacets();
		inline int getUCN();
		inline int getPredUCN();		
		volatile void showFacets(short codim=1);
		inline volatile void showSLA(facet &f);
		inline void idDebugPrint(const ideal &I);
		inline void invPrint(const ideal &I);
		inline bool isMonomial(const ideal &I);
		inline intvec *ivNeg(const intvec *iv);
// 		inline int dotProduct(intvec &iva, intvec &ivb);
		inline int dotProduct(const intvec &iva, const intvec &ivb);
		inline bool isParallel(const intvec &a, const intvec &b);
// 		inline int dotProduct(const intvec* a, const intvec *b);
// 		inline bool isParallel(const intvec* a, const intvec* b);
		inline bool areEqual(intvec &a, intvec &b);
		inline bool areEqual( facet *f,  facet *g);
		inline bool areEqual2(facet* f, facet *g);
		inline int intgcd(int a, int b);
		inline void writeConeToFile(const gcone &gc, bool usingIntPoints=FALSE);
		inline void readConeFromFile(int gcNum, gcone *gc);
		inline intvec f2M(gcone *gc, facet *f, int n=1);
		inline void sortRays(gcone *gc);
		//The real stuff
		inline void getConeNormals(const ideal &I, bool compIntPoint=FALSE);
		inline void getCodim2Normals(const gcone &gc);
		inline void getExtremalRays(const gcone &gc);
		inline void flip(ideal gb, facet *f);
		inline void flip2(const ideal gb, facet *f);
		inline void computeInv(const ideal &gb, ideal &inv, const intvec &f);// 		poly restOfDiv(poly const &f, ideal const &I); removed with r12286
		inline ideal ffG(const ideal &H, const ideal &G);
		inline void getGB(ideal const &inputIdeal);		
		inline void interiorPoint( dd_MatrixPtr &M, intvec &iv);
		inline void interiorPoint2(); //removed Feb 8th, 2010, new method Feb 19th, 2010
		inline void preprocessInequalities(dd_MatrixPtr &M);
		ring rCopyAndAddWeight(const ring &r, intvec *ivw);
		ring rCopyAndAddWeight2(const ring &, const intvec *, const intvec *);
		ring rCopyAndChangeWeight(const ring &r, intvec *ivw);		
// 		void reverseSearch(gcone *gcAct); //NOTE both removed from r12286
// 		bool isSearchFacet(gcone &gcTmp, facet *testfacet);
		void noRevS(gcone &gcRoot, bool usingIntPoint=FALSE);
		inline void makeInt(const dd_MatrixPtr &M, const int line, intvec &n);
		inline void normalize();
		facet * enqueueNewFacets(facet *f);
		facet * enqueue2(facet *f);
		dd_MatrixPtr facets2Matrix(const gcone &gc);
		/** Compute the lineality space Ax=0 and return it as dd_MatrixPtr dd_LinealitySpace*/
		inline dd_MatrixPtr computeLinealitySpace();
		inline bool iv64isStrictlyPositive(const intvec *);
		/** Exchange 2 ordertype_a by just 1 */
		inline void replaceDouble_ringorder_a_ByASingleOne();
// 		static void gcone::idPrint(ideal &I);		
		friend class facet;	
};
lists lprepareResult(gcone *gc, int n);
// bool iv64isStrictlyPositive(intvec *);
#endif
#endif
