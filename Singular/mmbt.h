#ifndef MMBT_H
#define MMBT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmbt.h,v 1.1 1999-03-18 16:30:52 Singular Exp $ */
/*
* ABSTRACT: backtrace: part of memory subsystem (for linux/elf)
* needed programs: - mprpc to set the variable MPRPC
*                  - mprdem: must be in the current directory
*                  - mprnm: must be in thje current directory
* files: - Singularg: the name of the executable
*        - nm.log: temp. file for the map address -> name
*/

#ifdef MTRACK
#ifndef __OPTIMIZE__
/* does only work in debug mode: 
* requires that -fomit-frame-pointer is not given
*/
#if defined(linux) && defined(__i386__)

#undef ulong
#define ulong unsigned long

int mmTrackInit (); /* init. of global variables */
void mmTrack (ulong *bt_stack); /* store the current stack into bt_stack */

void mmP2cNameInit(); /* init. of PC -> Name resolution */
char * mmP2cName(ulong p); /* PC -> Name resolution */
void mmPrintStack(ulong *bt_stack); /* print stack */

#endif /* linux, i386 */
#endif /* not optimize */
#endif /* MTRACK */

#endif /* MMBT_H */
