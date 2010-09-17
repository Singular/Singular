/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file Fan.h
 * 
 * This file provides the class Fan for storing Gröbner fans.
 *
 * ABSTRACT: This file provides the class Fan for storing Gröbner fans.
 * Fans are given by a set of maximal cones, each of which can be given
 * by a collection of maximal rays and/or facet normals. Moreover, a fan
 * knows its lineality space L given by a matrix the columns of which span
 * L.
 * In this implementation, the user can store all maximal rays and facet
 * normals in a Fan so that any cone can be defined by providing indices
 * to these maximal rows and/or facet normals. Thereby, cones only need to
 * store integer indices instead of column vectors (which are all stored
 * at Fan).
 * Besides some properties which can be set to 'yes', 'no' or 'do not know'
 * (encoded by the three int values 1, 0, -1), there is a member m_adjacency
 * which stores an adjacency matrix for the neighbourhood relations among
 * the maximal cones. Each entry of this matrix is some c of type Cone*:
 * Normally, c stores the intersection facet of two adjacent maximal
 * cones. If they are not adjacent, then c->isNoAdj() evaluates to true.
 * If they are adjacent but the user did not store a concrete intersection
 * facet then c->isNoFacet() evaluates to true.
 *
 * @author Frank Seelisch
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAN_H
#define FAN_H

class Cone;

class Fan
{
  private:
    intvec* m_maxRays;      /* matrix with maximal rays as columns */
    intvec* m_facetNs;      /* matrix with facet normals as columns */
    intvec* m_linSpace;     /* matrix with columns spanning the lin. sp. */
    lists   m_maxCones;     /* list of max. cones; i.e. objcts of type Cone */
    int     m_dim;          /* dim >= 0 or -1 if unknown */
    int     m_complete;     /* = 0 if 'no', = 1 if 'yes', = -1 if unknown */
    int     m_simplicial;   /* = 0 if 'no', = 1 if 'yes', = -1 if unknown */
    int     m_pure;         /* = 0 if 'no', = 1 if 'yes', = -1 if unknown */
    lists   m_adjacency;    /* a list of objects of type Cone; this list is
                               treated as a matrix of size
                               (number of max. cones)^2 */
    /**
     * For checking whether an intvec contains a specific int.
     *
     * Returns true iff i is contained in iv.
     **/
    static bool contains (
       const intvec* iv,  /**< [in]  the intvec to be searched for i, */
       const int i        /**< [in]  the int to be found in iv,       */
                         );

    /**
     * Getter for the member m_adjacency.
     *
     **/
    lists getRawAdj () const;
    
    /**
     * For inserting s new rows and columns in m_adjacency.
     *
     * This method will be called prior to the insertion of
     * s new maximal cones in the given fan.
     * s rows will be appended at the bottom of m_adjacency and
     * s column at the right. All previous entries will remain
     * untouched.
     **/
    void insertCrosses (
       int s   /**< [in]  the number of crosses to be inserted, */
                       );
                       
    /**
     * For deleting rows and columns in m_adjacency with the
     * indices given in the argument intvec.
     *
     * This method will be called prior to the deletion of
     * maximal cones from the given fan.
     * Entries of m_adjacency which are on none of the specified
     * rows and columns will remain untouched.
     **/
    void deleteCrosses (
       const intvec* indices
                       );

  public:
    /**
     * Constructor for class Fan.
     *
     * Each of the arguments may be NULL but not both maxRays and
     * facetNs simultaneously.
     **/
    Fan (
       const intvec* maxRays, /**< [in]  maximal rays given by columns */
       const intvec* facetNs, /**< [in]  facet normals given by columns */
       const intvec* linSpace /**< [in]  lineality space spannced by col.'s */
        );

    /**
     * Plain constructor for class Fan.
     *
     **/
    Fan ();

    /**
     * Deep copy constructor for class Fan.
     *
     **/
    Fan (
       const Fan& f   /**< [in]  the fan to be deep copied */
        );

    /**
     * Destructor for class Fan.
     *
     **/
    ~Fan ();

    /**
     * For obtaining a printable representation of the given fan.
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
     * For retrieving the ambient dimension of the given fan.
     *
     * At runtime this will be read off the number of rows of
     * the lineality space.
     **/
    int getAmbientDim () const;

    /**
     * For retrieving the dimension of the given fan.
     *
     * Returns -1 for 'unknown', otherwise a number >= 0.
     **/
    int getDim () const;

    /**
     * For retrieving whether the given fan is complete.
     *
     * Returns -1 for 'unknown', 1 for 'yes' and '0' for 'no'.
     * Completeness means that the fan covers the entire ambient
     * space.
     **/
    int getComplete () const;
    
    /**
     * For retrieving whether the given fan is simplicial.
     *
     * Returns -1 for 'unknown', 1 for 'yes' and '0' for 'no'.
     * Simpliciality means that each maximal cone has dimension
     * ambient dim minus 1.
     **/
    int getSimplicial () const;
    
    /**
     * For retrieving whether the given fan is pure.
     *
     * Returns -1 for 'unknown', 1 for 'yes' and '0' for 'no'.
     * Purity means that each maximal cone has the same dimension.
     **/
    int getPure () const;

    /**
     * For setting the dimension of the given fan.
     *
     * Any negative input will be converted to -1.
     **/
    void setDim (
       const int dim /**< [in]  target dim of the given fan */
                );

    /**
     * For setting the completeness of the given fan.
     *
     * Any input other than 0 and 1 will be converted to -1.
     **/
    void setComplete (
       const int complete /**< [in]  target completeness */
                     );

    /**
     * For setting the simpliciality of the given fan.
     *
     * Any input other than 0 and 1 will be converted to -1.
     **/
    void setSimplicial (
       const int simplicial /**< [in]  target simpliciality */
                       );

    /**
     * For setting the purity of the given fan.
     *
     * Any input other than 0 and 1 will be converted to -1.
     **/
    void setPure (
       const int pure /**< [in]  target purity */
                 );

    /**
     * For retrieving the maximal rays of the given fan.
     *
     * Returns a matrix the columns of which are the maximal rays.
     **/
    intvec* getMaxRays () const;
    
    /**
     * For retrieving the facet normals of the given fan.
     *
     * Returns a matrix the columns of which are the facet normals.
     **/
    intvec* getFacetNs () const;

    /**
     * For retrieving the lineality space of the given fan.
     *
     * Returns a matrix the columns of which span the lineality space.
     * If the lineality space is trivial, the matrix consists of a
     * single zero column.
     **/
    intvec* getLinSpace () const;

    /**
     * For adding a new maximal cone to the given fan.
     *
     * If the given cone refers (in its member m_pFan) to a fan which
     * differs from this fan, then no maximal cone is added. In this
     * case the method returns false; otherwise true.
     **/
    bool addMaxCone (
       const Cone* pCone /**< [in]  pointer to the new maximal cone */
                    );

    /**
     * For adding new maximal cones to the given fan.
     *
     * If any of the given cones refers (in its member m_pFan) to a fan
     * which differs from this fan, then no maximal cone is added. In this
     * case the method returns the list index (1st entry -> index = 1) of
     * the invalid cone; otherwise 0.
     **/
    int addMaxCones (
       const lists L /**< [in]  the list of new maximal cones */
                    );

    /**
     * For deleting a maximal cone from the given fan.
     *
     * If the index is not in [1..number of maximal cones], then
     * no cone is deleted and false is returned; otherwise deletion
     * takes place and true is returned.
     **/
    bool deleteMaxCone (
       const int index /**< [in]  the cone index to be deleted */
                       );

    /**
     * For deleting maximal cones from the given fan.
     *
     * If any of the indices is not in [1..number of maximal cones],
     * then no cone is deleted and the position of the invalid index
     * (1st index -> position 1) is returned; otherwise deletion
     * takes place and 0 is returned.
     **/
    int deleteMaxCones (
       const intvec* indices /**< [in]  the cone indices to be deleted */
                       );

    /**
     * For retrieving the maximal cones of the given fan.
     *
     * Returns a list with entries of type Cone. These
     * represent the maximal cones of the given fan.
     **/
    lists getMaxCones () const;

    /**
     * For retrieving adjacency information concerning two maximal
     * cones in the given fan.
     *
     * The method assumes that both arguments lie in
     * [1..number of max. cones]
     * Returns Cone::noAdjacency() if the two cones are not adjacent;
     * Cone::noFacet() if they are but no intersection facet is available.
     * Otherwise, the intersection facet will be returned (as an object of
     * type Cone).
     **/
    Cone* getAdjacencyFacet (
       int maxCone1, /**< [in]  the 1st max. cone index */
       int maxCone2  /**< [in]  the 2nd max. cone index */
                            ) const;
    
    /**
     * For retrieving the adjacency matrix of the given fan.
     *
     * The method returns a matrix of size (number of max. cones)^2.
     * Entries are either 1 (adjacent and intersection facet available),
     * -1 (adjacent but no intersection facet available), or
     * 0 (not adjacent).
     **/
    intvec* getAdjacency () const;
    
    /**
     * For adding adjacency information concerning two maximal
     * cones in the given fan.
     *
     * The method assumes that both int arguments are in the set
     * [1..number of max. cones].
     * The 3rd argument may be NULL signaling that an intersection
     * facet is not available.
     **/
    void addAdjacency (
       const int i,  /**< [in]  the 1st max. cone index */
       const int j,  /**< [in]  the 2nd max. cone index */
       const Cone* c /**< [in]  the intersection facet or NULL */
                      );
    
    /**
     * For adding adjacency information concerning numerous pairs of
     * maximal cones of the given fan.
     *
     * The method assumes that all indices are in the valid set, i.e., in
     * [1..number of max. cones].
     * The 3rd argument is a list of Cone* or int 0, where 0 signals
     * that an intersection facet is not available.
     **/
    void addAdjacencies (
       const intvec* iv, /**< [in]  the 1st max. cone index */
       const intvec* jv, /**< [in]  the 2nd max. cone index */
       const lists L     /**< [in]  a list of Cone* or NULL */
                        );
};

#endif
/* FAN_H */
