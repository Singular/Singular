/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file Cone.h
 *
 * This file provides the class Cone for storing cones in Gröbner fans.
 *
 * ABSTRACT: This file provides the class Cone for storing cones in Gröbner
 * fans.
 * (See the documentation of class Fan in Fan.h)
 * Each instance of class Cone holds a back link to a fan object where the
 * lineality space and all maximal rays and facet normals are stored.
 * Hence, each cone only needs to store indices to these maximal rays and/or
 * facet normals.
 *
 * @author Frank Seelisch
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef CONE_H
#define CONE_H

class Fan;

class Cone
{
  private:
    Fan*    m_pFan;      /* back pointer to the "parent" Gröbner fan */
    intvec* m_rays;      /* indices of maximal rays (stored in the fan) */
    intvec* m_facetNs;   /* indices of facet normals (stored in the fan) */
    int     m_dim;       /* dimension of the given cone */

  public:
    /**
     * Constructor for class Cone.
     *
     * Second and third argument may be NULL but not both simultaneously.
     * This constructor assumes that all data is valid. (This may be
     * checked beforehand using the static method checkConeData.)
     **/
    Cone (
       Fan* pFan,            /**< [in]  pointer to "parent" fan */
       const intvec* rays,   /**< [in]  indices of maximal rays (at fan) */
       const intvec* facetNs /**< [in]  indices of facet normals (at fan) */
         );

    /**
     * Constructor for class Cone.
     *
     **/
    Cone ();

    /**
     * Deep copy constructor for class Cone.
     *
     **/
    Cone (
       const Cone& c   /**< [in]  the cone to be deep copied */
         );

    /**
     * Destructor for class Cone.
     *
     **/
    ~Cone ();

    /**
     * Getter for the indices of the maximal rays.
     *
     * These indices correspond to the maximal rays stored in the "parent"
     * fan.
     **/
    intvec* getRays () const;

    /**
     * Getter for the indices of the facet normals.
     *
     * These indices correspond to the facet normals stored in the "parent"
     * fan.
     **/
    intvec* getFacetNs () const;

    /**
     * Method for checking the validy of cone data (prior to calling the
     * constructor with the same arguments).
     *
     * If the method returns a positive int i, then the i-1st entry of
     * rays is not a valid maximal ray index in the "parent" fan.
     * If the method returns a negative int i, then the i-1st entry of
     * facetNs is not a valid facet normal index in the "parent" fan.
     * Otherwise, 0 is returned meaning that the data is valid.
     **/
    static int checkConeData (
       const Fan* pFan,        /**< [in]  pointer to "parent" fan */
       const intvec* rays,     /**< [in]  indices of maximal rays (at fan) */
       const intvec* facetNs   /**< [in]  indices of facet normals (at fan) */
                             );

    /**
     * Method for obtaining a special cone represening a non-existing
     * intersection facet of two (non-adjacent) maximal cones.
     * This will be used in the context of adjacency information in the
     * "parent" fan.
     **/
    static Cone* noAdjacency ();

    /**
     * Method for obtaining a special cone represening an existing
     * adjacency of two (adjacent) maximal cones, but without available
     * intersection facet.
     * This will be used in the context of adjacency information in the
     * "parent" fan.
     **/
    static Cone* noFacet ();

    /**
     * Method for checking whether a given cone represents a cone similar
     * to the result of Cone::noFacet().
     **/
    bool isNoFacet ();

    /**
     * Method for checking whether a given cone represents a cone similar
     * to the result of Cone::noAdjacency().
     **/
    bool isNoAdj ();

    /**
     * For obtaining a printable representation of the given cone.
     *
     * The caller of this method is responsible for destructing
     * the obtained char*.
     **/
    char* toString () const;

    /**
     * For printing a string representation of the given fan
     * to the console.
     **/
    void print () const;

    /**
     * Getter for the "parent" fan of this cone.
     **/
    Fan* getFan () const;

    /**
     * For retrieving the dimension of the given cone.
     *
     * Returns -1 for 'unknown', otherwise a number >= 0.
     **/
    int getDim () const;

    /**
     * For setting the dimension of the given cone.
     *
     * Any negative input will be converted to -1.
     **/
    void setDim (
       const int dim   /**< [in]  the target dimension */
                );
};

#endif
/* CONE_H */

