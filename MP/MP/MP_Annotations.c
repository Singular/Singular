/***************************************************************************
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
 *  IMPORTANT ADDITION: as of September 2006, MP is also licenced under GPL
 *
 ***************************************************************************/


#ifndef lint
static char vcid[] = "@(#) $Id: MP_Annotations.c,v 1.2 2008-07-09 07:42:23 Singular Exp $";
#endif /* lint */

/* MP_MpDict.h - the Multi Protocol dictionary for protocol-specific 
                 definitions */

char *MpAnnotations[]={
    "",
    "Label_AP",
    "Reference_AP",
    "Store_AP",
    "Stored_AP",
    "Retrieve_AP",
    "Source_AP",
    "Comment_AP",
    "Timing_AP",
    "Type_AP",
    "Units_AP"
};


/* MP_ProtoDict.h - the Prototype dictionary */

char *ProtoAnnotations[]={
    "",
    "Prototype_AP"
};
