#ifndef MMBT_H
#define MMBT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmbt.h,v 1.5 1999-10-22 09:07:04 obachman Exp $ */
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

int mmTrackInit (); /* init. of global variables */
void mmTrack (unsigned long *bt_stack); /* store the current stack into bt_stack */

void mmP2cNameInit(); /* init. of PC -> Name resolution */
char * mmP2cName(unsigned long p); /* PC -> Name resolution */
void mmPrintStack(FILE *fd, unsigned long *stack, int mm);
void mmDBPrintStack(FILE *fd, void* memblock, int mm);
void mmDBPrintStackFrames(FILE *fd, void* memblock, int start, int end);
void mmPrintStackFrames(FILE *fd, unsigned long *bt_stack, int start, int end, int mm); 
void mmDBPrintThisStack(FILE *fd, void* memblock, int all, int free);
#endif /* linux, i386 */
#endif /* not optimize */
#endif /* MTRACK */

#endif /* MMBT_H */
