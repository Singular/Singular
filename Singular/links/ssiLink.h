/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    ssiLink.h
 *  Purpose: declaration of sl_link routines for ssi
 ***************************************************************/
#ifndef SSILINK_H
#define SSILINK_H

#include "Singular/links/silink.h"

typedef struct
{
  s_buff f_read;
  FILE *f_write;
  ring r;
  pid_t pid; /* only valid for fork/tcp mode*/
  int fd_read,fd_write; /* only valid for fork/tcp mode*/
  char level;
  char send_quit_at_exit;
  char quit_sent;

} ssiInfo;

BOOLEAN ssiOpen(si_link l, short flag, leftv u);
BOOLEAN ssiWrite(si_link l, leftv v);
leftv ssiRead1(si_link l);
leftv ssiRead2(si_link l, leftv key);
BOOLEAN ssiClose(si_link l);
const char* slStatusSsi(si_link l, const char* request);
si_link_extension slInitSsiExtension(si_link_extension s);

si_link ssiCommandLink();

void sig_chld_hdl(int sig);
#endif
