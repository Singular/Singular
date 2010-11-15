/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file DegreePattern.h
 *
 * This file provides a class to handle degree patterns.
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/
#ifndef DEGREE_PATTERN_H
#define DEGREE_PATTERN_H

#include <config.h>

#include "assert.h"

#include "canonicalform.h"
#include "cf_iter.h"
#include "ftmpl_functions.h"
#include "gfops.h"

/** @class DegreePattern DegreePattern.h "factory/DegreePattern.h"
 *
 * DegreePattern provides a functionality to create, intersect and refine
 * degree patterns.
 *
 *
 */
class DegreePattern
{
private:
  struct Pattern
  {
    int  m_refCounter; ///< reference counter
    int  m_length;     ///< length of m_pattern
    int* m_pattern;    ///< some array containing the degree pattern

    /// construct a Pattern from an int
    Pattern(int n): m_refCounter(1), m_length(n), m_pattern( new int[n]) {};
    /// default contructor
    Pattern(): m_refCounter(1), m_length(0), m_pattern(NULL) {};

  }* m_data;

  /// clear m_data
  void release()
  {
    ASSERT ( m_data != NULL, "non-null pointer expected");
    ASSERT ( m_data->m_refCounter == 0, "ref count of 0 expected");
    if( m_data->m_pattern != NULL )
      delete[] m_data->m_pattern;
    m_data->m_pattern = NULL;

    delete m_data;
    m_data = NULL;
  }
  /// initialise a DegreePattern
  void init( int n )
  {
    ASSERT ( m_data != NULL, "non-null pointer expected" );
    ASSERT( m_data->m_refCounter > 0, "ref count > 0 expected" );

    if( (--m_data->m_refCounter) < 1 )
      release();

    m_data = new Pattern(n);
  }

  /// getter
  ///
  /// @return @a getPattern returns a degree pattern
  inline int* getPattern() const
  {
    ASSERT( m_data != NULL, "non-null pointer expected" );
    ASSERT( m_data->m_pattern != NULL, "non-null pointer expected" );
    return m_data->m_pattern;
  }


public:
  /// getter
  ///
  /// @return @a getLength returns the length of the degree pattern
  inline int getLength() const
  {
    ASSERT( m_data != NULL, "non-null pointer expected" );
    return m_data->m_length;
  }

  /// operator []
  ///
  /// @return @a operator[] returns the element at @a index
  inline int operator[] (const int index ///< [in] some int >= 0, < getLength()
                        ) const
  {
    ASSERT( m_data != NULL, "non-null pointer expected" );
    ASSERT( index >= 0 && index < getLength(), "bad index" );
    ASSERT( getPattern() != NULL, "non-null pointer expected" );
    return getPattern()[index];
  }

  /// operator []
  ///
  /// @return @a operator[] sets the element at @a index
  inline int& operator[] (const int index ///< [in] some int >= 0, < getLength()
                         )
  {
    ASSERT( m_data != NULL, "non-null pointer expected" );
    ASSERT( index >= 0 && index < getLength(), "bad index" );
    ASSERT( getPattern() != NULL, "non-null pointer expected" );
    return getPattern()[index];
  }

  /// default constructor
  DegreePattern(): m_data( new Pattern() ){}

  /// copy constructor
  DegreePattern (const DegreePattern& degPat ///< [in] some degree pattern
                ): m_data( degPat.m_data )
  {
    ASSERT( degPat.m_data != NULL, "non-null pointer expected"  );
    m_data->m_refCounter++;
  };

  /// construct a degree pattern from a list of (univariate) polys
  DegreePattern (const CFList& l ///< [in] some list of (univariate) polys
                );

  /// assignment
  DegreePattern& operator= (const DegreePattern& degPat ///< [in] some degree
                                                        ///< pattern
                           )
  {
    ASSERT( m_data != NULL, "non-null pointer expected"  );
    ASSERT( degPat.m_data != NULL, "non-null pointer expected" );
    if( m_data != degPat.m_data )
    {
      m_data = degPat.m_data;
      m_data->m_refCounter++;
    }

    return *this;
  }

  /// destructor
  ~DegreePattern ()
  {
    ASSERT( m_data !=  NULL, "non-null pointer expected"  );
    if( (--m_data->m_refCounter) < 1 )
      release();
  }

  /// find an element @a x
  ///
  /// @return @a find returns the index + 1 of @a x, if @a x is an element of
  ///         the degree pattern, 0 otherwise
  int find (const int x ///< [in] some int
           ) const
  {
    if (getLength() == 0) return 0;
    for (int i= 0; i < getLength(); i++)
      if ((*this)[i] == x) return i + 1;
    return 0;
  };

  /// intersect two degree patterns
  void intersect (const DegreePattern& degPat ///< [in] some degree pattern
                 );
  /// Refine a degree pattern. Assumes that (*this)[0]:= @a d is the degree
  /// of the poly to be factored. Now for every other entry @a a there should be
  /// some entry @a b such that @a a+b= d. Elements which do not satisfy this
  /// relation are removed.
  void refine ();
};

#endif
/* DEGREE_PATTERN_H */

