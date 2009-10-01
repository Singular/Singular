/*
gfan.h Interface to gfan.cc

$Author: monerjan $
$Date: 2009-10-01 07:15:49 $
$Header: /exports/cvsroot-2/cvsroot/kernel/gfan.h,v 1.7 2009-10-01 07:15:49 monerjan Exp $
$Id: gfan.h,v 1.7 2009-10-01 07:15:49 monerjan Exp $
*/
#ifdef HAVE_GFAN
#include "intvec.h"

//ideal getGB(ideal inputIdeal);
ideal gfan(ideal inputIdeal);
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
					
		/** The default constructor. Do I need a constructor of type facet(intvec)? */
		facet();
		facet(int const &n);
		/** \brief The copy constructor
		 */
		facet(const facet& f);
		
		/** The default destructor */
		~facet();
				
		/** \brief Comparison of facets*/
		bool areEqual(facet &f, facet &g);
		/** Stores the facet normal \param intvec*/
		void setFacetNormal(intvec *iv);
		/** Hopefully returns the facet normal */
		intvec *getFacetNormal();
		/** Method to print the facet normal*/
		void printNormal();
		/** Store the flipped GB*/
		void setFlipGB(ideal I);
		/** Return the flipped GB*/
		ideal getFlipGB();
		/** Print the flipped GB*/
		void printFlipGB();
		/** Set the UCN */
		void setUCN(int n);
		/** \brief Get the UCN 
		 * Returns the UCN iff this != NULL, else -1
		 */
		int getUCN();
		/** Store an interior point of the facet */
		void setInteriorPoint(intvec *iv);
		intvec *getInteriorPoint();
		/** \brief Debugging function
		 * prints the facet normal an all (codim-2)-facets that belong to it
		 */
		void fDebugPrint();
		friend class gcone;
		
};

#endif
