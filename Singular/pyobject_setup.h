// -*- c++ -*-
//*****************************************************************************
/*!
!
* @file pyobject_setup.h
 *
 * @author Alexander Dreyer
 * @date 2010-12-15
 *
 * This header file defines the @c blackbox setup interface for the pyobject
 *
 * @par Copyright:
 *   (c) 2010 by The Singular Team, see LICENSE file
**/
//*****************************************************************************


#ifndef SINGULAR_PYOBJECT_SETUP_H
#define SINGULAR_PYOBJECT_SETUP_H

/// initialize blackbox support for @c pyobject; functionilty os autoloaded on demand
void pyobject_setup();

/// force leading of pyobject functionality
BOOLEAN pyobject_ensure();

#endif
