/*
gfan.h Interface to gfan.cc

$Author: Singular $
$Date: 2009-02-19 15:39:08 $
$Header: /exports/cvsroot-2/cvsroot/kernel/gfan.h,v 1.5 2009-02-19 15:39:08 Singular Exp $
$Id: gfan.h,v 1.5 2009-02-19 15:39:08 Singular Exp $
*/
#ifdef HAVE_GFAN
#include "intvec.h"

ideal getGB(ideal inputIdeal);
ideal gfan(ideal inputIdeal);
#endif
