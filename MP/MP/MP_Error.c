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
static char vcid[] = "@(#) $Id: MP_Error.c,v 1.1.1.1 1997-05-25 20:31:46 obachman Exp $";
#endif /* lint */

#include "MP.h"

char *MP_errlist[]={
        "MP_Failure",
        "MP_Success",
        "Unknown type",
        "Unknown annotation",
        "Unknown required annotation",
        "Null link",   
        "Null transport device",
        "Can't open file link",
        "Can't make connection",
        "Can't allocate memory", 
        "Can't convert", 
        "Value is out of the legal range for this type",
        "Can't put node header",  
        "Can't put the requested node packet",
        "Can't put data packet",
        "Can't put annotation ", 

        "Can't get node header",  
        "Can't get node packet",
        "Can't get data packet",
        "Can't get annotation ", 

        "Packet type not of the type requested",
        "Can't convert, bad string",
        "Can't write link",
        "Can't read link",
        "Identifier is not common (longer than one character)",
        "Illegal element type for a Basic Vector",
        "Can't put/get dummy bignum"
        };


