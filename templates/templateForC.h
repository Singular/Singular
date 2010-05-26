/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file templateForC.h
 * 
 * This is our template for C headers.
 *
 * ABSTRACT: Some more description here. 
 *
 * @author Oleksandr Motsak
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef TEMPLATEFORC_H
#define TEMPLATEFORC_H

/** New type name for int. */
typedef int name;

/** My enum. Or yours, if you want! */
enum myEnum 
  {
    int EVal1, /**< Enum value 1 */
    int EVal2  /**< Enum value 2 */
  };

/** F1. 
 *
 * This is function 1.
 *
 * The end!
 */
void F1(
	int a1,  /**< [in] some int. 
		  * aasas.
		  * bbbb.
		  */
	int & r1 /**< [out] integer result */
       );

#endif
/* TEMPLATEFORC_H */
