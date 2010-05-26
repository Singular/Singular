/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file template.h
 * 
 * This file defines the class Template. 
 *
 * ABSTRACT: We need this for something...
 *
 * @author Oleksandr Motsak
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef TEMPLATE_H
#define TEMPLATE_H

// include basic definitions
#include "singularxx_defs.h"

BEGIN_NAMESPACE_SINGULARXX

/** @class Template template.h "someRootDirectory/template.h"
 * 
 * Brief description of class Template.
 *
 * Detailed description of class Template. 
 *
 * @sa some other classes
 */
class Template: public TemplateBaseClass
{
public:
  /// Name type of *this.  
  typedef Template Self;

  /// My enum. Or yours, if you want!
  enum MyEnum 
  {
    int EVal1, ///< Enum value 1
    int EVal2  ///< Enum value 2
  };

  /// Default constructor. 
  /// And now goes the detailed description....
  Template();

  /// Default destructor. 
  ~Template();
  

  /// Brief method description.
  ///
  /// Detailed method description.
  ///
  /// @return information about return value
  /// @sa strcpy2
  ///
  int someMethod( 
	     int a, ///< [in] some int
	     int &o ///< [out] integer result
	    );

  /// Example of Copy a string method.
  ///
  /// The strcpy2 function copies @a strSource, including
  /// the terminating null character, to the location
  /// specified by @a strDestination. No overflow checking
  /// is performed when strings are copied or appended.
  /// The behavior of @a strcpy2 is undefined if the source
  /// and destination strings overlap.
  ///
  /// @return @a strcpy2 returns the destination string.
  /// No return value is reserved to indicate an error.
  /// @sa someMethod(), wcscpy(), _mbscpy(), strncpy()
  char* strcpy2(
            char* strDestination,  ///< [out] pointer to source null terminated.
                                   ///< continue ...
            const char* strSource  ///< [in] pointer to destination memory.
               );

protected:
  /// Short description of some flag till first dot. Everything after that will 
  /// be considered as a detaiiled description.
  bool m_bSomeFlag;
};

END_NAMESPACE_SINGULARXX

#endif 
/* #ifndef TEMPLATE_H */
