/*
 * This is "mp_pvm.c". It contains all MP's utility routines associated with
 * PVM. It is to be included with all applications that employ PVM to
 * transport MP trees.
 *
 * Note:
 *       mp_pvm_unpack() anticipates PvmNoData error from pvm_recv().
 *       PVM defaults to printing an error message when encounter such
 *       error. To disable printing of error message, use the following:
 *
 *              pvm_setopt(PvmAutoErr, 0);
 *
 *       See "PVM 3 User'sGuide and Reference Manual" for detail.
 */

/*********************************************************
 * mp_pvm_pack(link, where, len)
 * return: 0 if success, -1 if failure.
 * arguments: link - can be anything, even NULL.
 *            where - pointer to data to be packed.
 *            len - number of bytes to be packed.
 *********************************************************/
int
#ifdef _ANSIC_
mp_pvm_pack(MP_Link_pt link, char *where, unsigned long len)
#else
mp_pvm_pack(link, where, len)
MP_Link_pt link;
char *where;
unsigned long len;
#endif
{
  int status;
  status = pvm_pkbyte(where, len, 1);
  if (status < 0) return(-1);
  else return(len);
}

/*********************************************************
 * mp_pvm_unpack(link, where, len) 
 * return: number of bytes actually unpacked. -1 if failure.
 * arguments: link - can be anything, even NULL.
 *            where - pointer to memory location to store unpacked data.
 *            len - number of bytes to be packed.
 *********************************************************/
int
#ifdef _ANSIC_
mp_pvm_unpack(MP_Link_pt link, char *where, unsigned long len)
#else
mp_pvm_unpack(link, where, len)
MP_Link_pt link;
char *where;
unsigned long len;
#endif
{
  int status, bufid, actual_len, lenrecv, tid, msgtag;
  actual_len = len;
  status = pvm_upkbyte(where, actual_len, 1);
  if (status == PvmNoData) {
    bufid = pvm_recv(-1, -1);
    if (bufid < 0) return(-1);
    status = pvm_bufinfo(bufid, &lenrecv, &msgtag, &tid);
    actual_len = (len < lenrecv) ? len : lenrecv;
    status = pvm_upkbyte(where, actual_len, 1);
  }
 
  if (status < 0) return(-1);
  return(actual_len);
}
