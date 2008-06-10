////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: charset.h,v 1.4 2008-06-10 14:49:15 Singular Exp $
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

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.3  1997/09/12 07:19:41  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:42:10  michael
Version for libfac-0.2.1

*/
