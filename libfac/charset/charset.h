////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id$
////////////////////////////////////////////////////////////

#ifndef INCL_CHARSET_H
#define INCL_CHARSET_H

#include <factory.h>
#include <tmpl_inst.h>  // for typedef's

CFList     BasicSet( const CFList &PS );
CFList     CharSet( const CFList &PS );
CFList     mcharset( const CFList &PS, PremForm & Remembern );
int        checkok( const CFList & PS, CFList & FS2);
/*BEGINPUBLIC*/
CFList     MCharSetN( const CFList &PS, PremForm & Remembern );
ListCFList IrrCharSeries( const CFList &PS, int opt=0 );
/*ENDPUBLIC*/

#endif /* INCL_CHARSET_H */
