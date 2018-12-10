#ifndef NDBM_H
#define NDBM_H
/*!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!

* ABSTRACT: DBM

*/

/*!

 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *        @(#)ndbm.h        5.1 (Berkeley) 5/30/85
 *
 * Par. 3 removed due to a license change (1999)
 * see ftp://ftp.cs.berkeley.edu/pub/4bsd/README.Impt.License.Change
 
*/

/*!

 * Hashed key data base library.
 
*/
#define PBLKSIZ 1024

#define DBLKSIZ 4096

typedef struct {
        int        dbm_dirf;                /*!
 open directory file 
*/
        int        dbm_pagf;                /*!
 open page file 
*/
        int        dbm_flags;                /*!
 flags, see below 
*/
        long        dbm_maxbno;                /*!
 last ``bit'' in dir file 
*/
        long        dbm_bitno;                /*!
 current bit number 
*/
        long        dbm_hmask;                /*!
 hash mask 
*/
        long        dbm_blkptr;                /*!
 current block for dbm_nextkey 
*/
        int        dbm_keyptr;                /*!
 current key for dbm_nextkey 
*/
        long        dbm_blkno;                /*!
 current page to read/write 
*/
        long        dbm_pagbno;                /*!
 current page in pagbuf 
*/
        char        dbm_pagbuf[PBLKSIZ];        /*!
 page file block buffer 
*/
        long        dbm_dirbno;                /*!
 current block in dirbuf 
*/
        char        dbm_dirbuf[DBLKSIZ];        /*!
 directory file block buffer 
*/
} DBM;

#define _DBM_RDONLY        0x01        /*!
 data base open read-only 
*/
#define _DBM_IOERR        0x02        /*!
 data base I/O error 
*/

#define dbm_rdonly(db)        ((db)->dbm_flags & _DBM_RDONLY)

#define dbm_error(db)        ((db)->dbm_flags & _DBM_IOERR)
        /*!
 use this one at your own risk! 
*/
#define dbm_clearerr(db)        ((db)->dbm_flags &= ~_DBM_IOERR)

/*!
 for flock(2) and fstat(2) 
*/
#define dbm_dirfno(db)        ((db)->dbm_dirf)
#define dbm_pagfno(db)        ((db)->dbm_pagf)

typedef struct {
        char        *dptr;
        int        dsize;
} datum;

/*!

 * flags to dbm_store()
 
*/
#define DBM_INSERT        0
#define DBM_REPLACE        1

DBM        *dbm_open(char *file, int flags, int mode);
void        dbm_close(DBM *db);
datum        dbm_fetch(DBM *db, datum key);
datum        dbm_firstkey(DBM *db);
datum        dbm_nextkey(DBM *db);
long        dbm_forder(DBM *db, datum key);
int        dbm_delete(DBM *db, datum key);
int        dbm_store(DBM *db, datum key, datum dat, int replace);

#endif /*!
 NDBM_H 
*/
