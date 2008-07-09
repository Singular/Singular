/****************************************************************
 *                                                                  
 *                                                                  
 *                    MP version 1.1.2:  Multi Protocol
 *                    Kent State University, Kent, OH
 *                 Authors:  S. Gray, N. Kajler, P. Wang 
 *          (C) 1993, 1994, 1995, 1996, 1997 All Rights Reserved
 * 
 *                                 NOTICE
 * 
 *  Permission to use, copy, modify, and distribute this software and
 *  its documentation for non-commercial purposes and without fee is 
 *  hereby granted provided that the above copyright notice appear in all
 *  copies and that both the copyright notice and this permission notice 
 *  appear in supporting documentation.
 *  
 *  Neither Kent State University nor the Authors make any representations 
 *  about the suitability of this software for any purpose.  The MP Library
 *  is distributed in the hope that it will be useful, but is provided  "as
 *  is" and without any warranty of any kind and without even the implied  
 *  warranty of merchantability or fitness for a particular purpose. 
 *
 *    IMPORTANT ADDITION: as of September 2006, MP is also licenced under GPL
 *
 *   IMPLEMENTATION FILE:  MP_Error.c
 *                                                                
 *          The error handling routines and a definition 
 *        for the global variable MP_Status which is set after
 *        almost every routine.
 *                                                                
 *  The MP Library is distributed in the hope that it will be useful, 
 *  but without any warranty of any kind and without even the implied 
 *  warranty of merchantability or fitness for a particular purpose.
 *
 *  Change Log:
 *       July 7, 1995  SG - updated MP_errlist[] to reflect newtypes
 *                          in MP-1.0 and coelesced a few error types.
 *                          Also made the necessary changes to 
 *                          MP_Error.c, MP_Put.c, MP_Get.c
 *       July 21, 1995 SG - more additions/corrections.  Checked against
 *                          MP_Error.h for accuracy (enums with corresponding
 *                          error messages)
 *       September 11, 1995 SG - Added MP_LogEvent().
 *       November 20, 1995 SG - added MP_Failure and MP_Success to be 
 *                              consistent with the enums in MP_Error.h.
 *
 ***************************************************************************/

#ifndef lint
static char vcid[] = "@(#) $Id: MP_Error.c,v 1.4 2008-07-09 07:42:23 Singular Exp $";
#endif /* lint */

#include "MP.h"

char *MP_errlist[]={
        "MP: MP_Failure",
        "MP: MP_Success",
        "MP: Unknown type",
        "MP: Unknown annotation",
        "MP: Unknown required annotation",
        "MP: Null link",   
        "MP: Null transport device",
        "MP: Can't open file link",
        "MP: Can't make connection",
        "MP: Can't allocate memory", 
        "MP: Can't convert", 
        "MP: Value is out of the legal range for this type",
        "MP: Can't put node header",  
        "MP: Can't put the requested node packet",
        "MP: Can't put data packet",
        "MP: Can't put annotation ", 

        "MP: Can't get node header",  
        "MP: Can't get node packet",
        "MP: Can't get data packet",
        "MP: Can't get annotation ", 

        "MP: Packet type not of the type requested",
        "MP: Can't convert, bad string",
        "MP: Can't write link",
        "MP: Can't read link",
        "MP: Identifier is not common (longer than one character)",
        "MP: Illegal element type for a Basic Vector",
        "MP: Can't put/get dummy bignum",
        "MP: Can't skip message (end of stream)",
        "MP: Can't init message (end of stream)",
        "MP: Error of external application",
        };


