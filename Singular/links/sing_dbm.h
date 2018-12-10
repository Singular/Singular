#ifndef SING_DBM_H
#define SING_DBM_H
/*!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!

* ABSTRACT: interface to DBM links

*/

struct s_si_link_extension;
typedef struct s_si_link_extension *si_link_extension;

si_link_extension slInitDBMExtension(si_link_extension s);
#endif
