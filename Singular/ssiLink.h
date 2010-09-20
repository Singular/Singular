/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    ssiLink.h
 *  Purpose: declaration of sl_link routines for ssi
 *  Version: $Id$
 ***************************************************************/
 #ifndef SSILINK_H
 #define SSILINK_H
BOOLEAN ssiOpen(si_link l, short flag, leftv u);
BOOLEAN ssiWrite(si_link l, leftv v);
leftv ssiRead1(si_link l);
leftv ssiRead2(si_link l, leftv key);
BOOLEAN ssiClose(si_link l);
const char* slStatusSsi(si_link l, const char* request);
si_link_extension slInitSsiExtension(si_link_extension s);
int ssiBatch(const char *host, const char * port);
/* return 0 on success, >0 else*/
#endif

