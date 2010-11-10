/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
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

#ifndef TEMPLATE_FOR_C_H
#define TEMPLATE_FOR_C_H

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
 * @return information about return value
 * @sa strcpy
 *
 * The end!
 */
int F1(
	int a1,  /**< [in] some int. 
		  * aasas.
		  * bbbb.
		  */
  int & r1 /**< [in,out] integer input and some resulting value */        
       );



#endif
/* TEMPLATE_FOR_C_H */

/* Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab 
*/
