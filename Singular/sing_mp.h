/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Log: not supported by cvs2svn $
*/

#include "subexpr.h"
#include "silink.h"


void FreeCmdArgs(int argc, char** argv);
#ifdef HAVE_MPSR
void slInitBatchLink(si_link l, int argc, char** argv);
si_link_extension slInitMPFile();
si_link_extension slInitMPTcp();
extern BOOLEAN mpsr_IsMPLink(si_link l);
typedef struct MP_Link_t;
typedef MP_Link_t * MP_Link_pt;
extern MP_Link_pt OpenMPFile(char *fn, short mode);
#endif

int Batch_do(int argc, char **argv);
