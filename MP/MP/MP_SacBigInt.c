/*******************************************************************
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
 *   IMPLEMENTATION FILE:  MP_SacBigInt.c
 *
 *      The default BigInt package is the Gnu Multiple Precision 
 *      package.  Here we have the routines to put/get/print a SAC bigint
 *      and to convert between the SAC and GMP bigints.
 *
 *   NOTE: This is a short explanation of the the way MP deals with
 *         arbtrary precision numbers.  For now this really only applies
 *         to Big Ints. When the sender and receiver are using the same
 *         format, we don't want to force them to have to convert to a 
 *         common format to do the exchange.  So we provide conversion
 *         routines for some common formats (SAC <-> GMP, PARI <-> GMP).
 *         The BigNum format to be used is characterized by routines to
 *         Put(), Get(), and Print() these formats.  The routines are
 *         kept in a BigInt ops struct (see MP_Env.h).  The user may 
 *         replace the default GMP ops in an MP env with those of some 
 *         other format by calling MP_SetEnvBigIntFormat() and sending 
 *         in the address of the struct containing the proper routines
 *         and the "name" of the format to be used.  It must be one of
 *         the formats supported (as defined in MP_Types.h).
 *         In any event, we want the sender to always have at least the 
 *         illusion that it is sending in its "native" format, so all
 *         necessary conversions between representations are hidden in
 *         supplied ops structs.  See below for examples!!!  Observe
 *         that if SAC supported a straightforward conversion to string
 *         routine, we wouldn't have to do all this messy convert to GMP
 *         format first.  For TCP connections, the two endpoints negotiate
 *         which format to use.  If both use the same format (GMP or other-
 *         wise) then transmission should be "optimal".  But if they use
 *         different formats, the non-GMP side (may be both sides), must
 *         do conversions.  For other transport devices (e.g., files,
 *         PVM, ToolBus), there is no negotiation.  There is no miracle
 *         here, the programmer must be aware of the needs of the other
 *         applications involved and know if they all support the same
 *         format, setting the environment accordingly.
 *         --> BTW: This approach is consistent with and similar to how
 *             we deal with new transport devices.
 *
 *  Change Log:
 *      4/22/96 sgray Life begins for this file.
 *      4/23/96 sgray Completed and tested.  Seems okay. :-)
 *
 ************************************************************************/

#ifndef lint
static char vcid[] = "@(#) $Id: MP_SacBigInt.c,v 1.2 2008-07-09 07:42:23 Singular Exp $";
#endif /* lint */

#include "MP.h"

#ifdef MP_HAVE_SAC

/*#define MP_DEBUG */

MP_BigIntOps_t imp_sac_bigint_ops = {
  IMP_PutSacBigInt,
  IMP_GetSacBigInt,
  IMP_SacBigIntToStr,
  IMP_SacBigIntAsciiSize
};

#if MP_DEFAULT_APINT_FORMAT == MP_SAC
MP_BigIntOps_t imp_default_bigint_ops = {
  IMP_PutSacBigInt,
  IMP_GetSacBigInt,
  IMP_SacBigIntToStr,
  IMP_SacBigIntAsciiSize
};
MP_BigNumFormat_t imp_default_bigint_format = MP_SAC;
#endif


/*
 * macro to determine if the GMP bignum is negative 
 */
#define mpz_is_neg(mp_int) ((mp_int->_mp_size < 0))


/*
 * sac_to_gmp() assumes that the incoming integer really is a bignum and is
 * not representable as a machine precision integer.  If the argument sac is
 * machine precision, the while loop becomes infinite (this is bad).
 *
 * On entry I also assume that the caller has initialized gmp_int.
 */
static void sac_to_gmp(mpz_ptr gmp_int, Word sac)
{
    short set_neg_flag = -1; /* when set, this will be 0 or 1 */
    MP_INT pwr, temp;
    Word lsac, d;
#ifdef MP_DEBUG   
    fprintf(stderr, "sac_to_gmp: entering, the bignum here is ");
    IWRITE(sac);printf("\n"); fflush(stderr);
#endif

    if (ISATOM(sac)) {
      if (sac < 0) {
	 set_neg_flag = 1;
	 sac = -sac;
	 }
      else set_neg_flag = 0;
      mpz_set_si(gmp_int, (int) sac);
      if (set_neg_flag) mpz_neg(gmp_int, gmp_int);
      return;
     }
    ADV(sac, &d, &lsac);
    mpz_init(&pwr); mpz_set_si(&pwr, 1);
    
    if (d != 0) {
      set_neg_flag = (d < 0) ? 1 : 0;
      if (set_neg_flag) d = -d;
      }

    mpz_set_si(gmp_int, d);
    mpz_init(&temp);
    while (!ISNIL(lsac)) {
        mpz_mul_ui(&pwr, &pwr, BETA);
        ADV(lsac, &d, &lsac);
        if (d != 0) {
            mpz_clear(&temp); 
	    /* Argh!!  We need to keep checking because we may have
	       had all leading zeroes to this point! */
	    if (set_neg_flag == -1) set_neg_flag = (d < 0) ? 1 : 0;
            if (set_neg_flag) d = -d;
            mpz_mul_ui(&temp, &pwr, d);
            mpz_add(gmp_int, gmp_int, &temp);
        }
    }
    if (set_neg_flag) mpz_neg(gmp_int, gmp_int);

#ifdef MP_DEBUG
    fprintf(stderr,"sac_to_gmp: exiting\n");fflush(stderr);
#endif
   return;
}



static void gmp_to_sac(Word *sac_int, mpz_ptr gmp_int)
{
    MP_INT  n, q, r;
    Word    i = 0;
    long    d;
    short   set_neg_flag = mpz_is_neg(gmp_int);

    if (abs(gmp_int->_mp_size) == 1) {
      d = mpz_get_si(gmp_int);
      if (set_neg_flag && (d > 0)) d = -d;
      if (abs(d) < BETA) {
	*sac_int = d;
	return;
	}
    }
    mpz_init(&n); 
    mpz_set(&n, gmp_int);
    mpz_init(&q); mpz_init(&r);
    if (set_neg_flag) n._mp_size = -n._mp_size;
    *sac_int = NIL;
    while (1) {
	d = mpz_fdiv_qr_ui(&q, &r, &n, BETA);
	if (set_neg_flag && (d > 0)) d = -d;
        *sac_int =  LEINST(*sac_int, i, d); i++;
	d = mpz_cmp_si(&q, BETA);
	if (mpz_cmp_si(&q, BETA) < 0) 
	    break; 
        mpz_clear(&n);
        mpz_set(&n, &q);
    }
    /*
     * now get the last value from q 
     */
    d = mpz_get_si(&q);
    if (set_neg_flag && (d > 0)) d = -d;
    *sac_int = LEINST(*sac_int, i, d); 
}



/* Generic routine to recursively put a SAC list */
#ifdef __STDC__
int IMP_PutSacList(MP_Link_pt link,
                   Word L)
#else
int IMP_PutSacList(link, L)
    MP_Link_pt link;
    Word L;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "IMP_PutSacList: entering\n"); fflush(stderr);
#endif

    if ( L == NIL || ISATOM(L) ) { /* Packing atom*/
        if (MP_PutSint32Packet(link, L, 0) != MP_Success)
            return(-1);
    }
    else {                       /* Packing s-exp*/
        if (MP_PutCommonOperatorPacket(link, MP_BasicDict, 
                   MP_CopBasicList, 0, 1)
            != MP_Success)
            return(-1);
        if (IMP_PutSacList(link, FIRST(L)) < 0 ) 
            return(-1);
        if (IMP_PutSacList(link, RED(L)) < 0 )
            return(-1);
    }

#ifdef MP_DEBUG
   fprintf(stderr, "IMP_PutSacList: exiting\n");  fflush(stderr);
#endif

    return(0);
}



#ifdef __STDC__
Word IMP_GetSacList(MP_Link_pt link)
#else
Word IMP_GetSacList(link)
    MP_Link_pt link;
#endif
{
    Word a, b;
    MP_NodeType_t ntype;
    MP_NumAnnot_t na;
    MP_NumChild_t nc;
    MP_DictTag_t  dtag;
    MP_Common_t   cv;
#ifdef MP_DEBUG
     fprintf(stderr, "IMP_GetSacList: entering\n");fflush(stderr); 
#endif

    if (IMP_GetNodeHeader(link, &ntype, 
                          &dtag, &cv, &na, &nc) != MP_Success){
        MP_PrintError(link);
        return(-1); 
    }
    if (ntype == MP_Sint32Type) {  /* Unpack atom */
        if (IMP_GetSint32(link, (MP_Sint32_t *) &a) != MP_Success) {
	  MP_PrintError(link);
	  return -1;
	  }
        return(a);
	}
    else if (ntype == MP_CommonOperatorType && dtag == MP_BasicDict &&
                   cv == MP_CopBasicList) { 
        /* Unpack s-exp */
        a = IMP_GetSacList(link);
        b = IMP_GetSacList(link);
        return(COMP(a,b));
    } else { /* error */
        fprintf(stderr, "IMP_GetSacList: invalid s-expression\n");
        fflush(stderr);
        return -1;
      }
}



#ifdef __STDC__
MP_Status_t IMP_PutSacBigInt(MP_Link_pt link, MP_ApInt_t sac_int)
#else
MP_Status_t IMP_PutSacBigInt(link, sac_int)
    MP_Link_pt link;
    MP_ApInt_t sac_int;
#endif
{
    Word n = *((Word *)sac_int);
    MP_INT  gmp_int;
#ifdef MP_DEBUG
    fprintf(stderr, "IMP_PutSacBigInt: entering, n is \n");fflush(stderr);
    IWRITE (n); printf("\n"); fflush(stdout);
#endif 

    if (link->link_bigint_format == MP_SAC) 
        return(IMP_PutSacList(link, n));
    else {
        mpz_init(&gmp_int);
        sac_to_gmp(&gmp_int, n);
        ERR_CHK(IMP_PutGmpInt(link, (MP_ApInt_t)&gmp_int));
        mpz_clear(&gmp_int);  /* sloppy, we don't cleanup on failure */
    }
    RETURN_OK(link);
}




#ifdef __STDC__
MP_Status_t IMP_GetSacBigInt(MP_Link_pt link, MP_ApInt_pt sac_int)
#else
MP_Status_t IMP_GetSacBigInt(link, sac_int)
    MP_Link_pt link;
    MP_ApInt_pt sac_int;
#endif
{
    mpz_ptr gmp_int = NULL;
    Word    sac_tmp = 0, *sacptr;

#ifdef MP_DEBUG
    fprintf(stderr,"IMP_GetSacBigInt: entering\n");
    fprintf(stderr, "\tbigint_format is %d\n", link->link_bigint_format);
    fflush(stderr);
#endif


    if (link->link_bigint_format == MP_SAC) {
	sacptr = (Word *)*sac_int; 
        *sacptr = IMP_GetSacList(link);
        if (*sacptr == -1) 
            return MP_Failure; 
        else 
	  RETURN_OK(link); 
    } else {
        ERR_CHK(IMP_GetGmpInt(link, (MP_ApInt_t *)&gmp_int));
/*          gmp_to_sac((Word *)sac_int, gmp_int);  */
         gmp_to_sac((Word)*((Word *)sac_int), gmp_int); 
        mpz_clear(gmp_int);  /* sloppy, we don't cleanup on failure */
    }

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetSacBigInt: returning\n");fflush(stderr);
#endif

    RETURN_OK(link);
}



char * 
#ifdef __STDC__
IMP_SacBigIntToStr(MP_ApInt_t big_int, char *buffer)
#else
IMP_SacBigIntToStr(big_int, buffer)
MP_ApInt_t  big_int;
char       *buffer;
#endif
{
    MP_INT gmp_int;
#ifdef MP_DEBUG
    fprintf(stderr,"IMP_SacBigIntToStr: entering.\n");
    fflush(stderr);  fprintf(stderr, "\tthe bignum is ");
    IWRITE(*(Word*)big_int);fprintf(stderr, "\n");fflush(stdout);
#endif

    mpz_init(&gmp_int);
    sac_to_gmp(&gmp_int, *(Word*)big_int); 
    mpz_get_str(buffer, 10, &gmp_int);
    mpz_clear(&gmp_int);

#ifdef MP_DEBUG
    fprintf(stderr,"IMP_SacBigIntToStr: exiting\n");fflush(stderr);
#endif

    return buffer;
}


#ifdef __STDC__
long IMP_SacBigIntAsciiSize(MP_ApInt_t mp_apint)
#else
long IMP_SacBigIntAsciiSize(mp_apint)
    MP_ApInt_t mp_apint;
#endif
{
  int len = 0;
  Word tmp = *((Word *)mp_apint);

  if (ISATOM(tmp)) return 9;

  len = LENGTH(tmp);
  return len * 9;
}



char * OLD_IMP_SacBigIntToStrOriginal(MP_Link_pt link, MP_ApInt_t big_int,
 int direction)
{
    char *str = NULL;
    mpz_ptr gmp_int = NULL;


    fprintf(stderr,"IMP_SacBigIntToStr: entering\n");
    if (direction == MP_SEND || 
        link->link_bigint_format == MP_SAC) {
        fprintf(stderr,"\thave to convert - SAC format\n");
        mpz_init(gmp_int);
        sac_to_gmp(gmp_int, *((Word *)big_int));
        printf("\tdone conversion\n");
        str = mpz_get_str(NULL, 10, gmp_int);
        printf("\thave the string, cleaning up\n");
        mpz_clear(gmp_int);
    } else {  /* must be receiving in GMP format */
        fprintf(stderr,"\tdont' have to convert - GMP format\n");
        str = mpz_get_str(NULL, 10, (mpz_ptr)big_int);
    }
    fprintf(stderr,"IMP_SacBigIntToStr: exiting\n");fflush(stderr);
  
    return str;
}


#endif /* MP_HAVE_SAC */
