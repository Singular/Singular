/*
gfan.h Interface to gfan.cc

$Author: monerjan $
$Date: 2009-04-03 13:49:16 $
$Header: /exports/cvsroot-2/cvsroot/kernel/gfan.h,v 1.6 2009-04-03 13:49:16 monerjan Exp $
$Id: gfan.h,v 1.6 2009-04-03 13:49:16 monerjan Exp $
*/
#ifdef HAVE_GFAN
#include "intvec.h"

//ideal getGB(ideal inputIdeal);
ideal gfan(ideal inputIdeal);
//int dotProduct(intvec a, intvec b);
//bool isParallel(intvec a, intvec b);
#endif
