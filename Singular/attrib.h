#ifndef ATTRIB_H
#define ATTRIB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: attrib.h,v 1.7 2004-06-16 12:18:12 Singular Exp $ */
/*
* ABSTRACT: attributes to leftv and idhdl
*/
#include <string.h>
#include "structs.h"

class sattr;
typedef sattr * attr;
class sattr
{
  public:
    void Init() { memset(this,0,sizeof(*this)); }
    char *  name;
    void *  data;
    attr    next;
    int     atyp; /* the type of the attribut, describes the data field
                  */

    void Print();
    attr Copy(); /* copy all arguments */
    void * CopyA(); /* copy the data of this attribute */
    attr set(char * s, void * data, int t);
    attr get(char * s);
    void kill();
    void killAll();
};

void * atGet(idhdl root,char * name);
void * atGet(leftv root,char * name);
void * atGet(idhdl root,char * name, int t);
void * atGet(leftv root,char * name, int t);
void atSet(idhdl root,char * name,void * data,int typ);
void atSet(leftv root,char * name,void * data,int typ);
void atKillAll(idhdl root);
void atKill(idhdl root,char * name);

BOOLEAN atATTRIB1(leftv res,leftv a);
BOOLEAN atATTRIB2(leftv res,leftv a,leftv b);
BOOLEAN atATTRIB3(leftv res,leftv a,leftv b,leftv c);
BOOLEAN atKILLATTR1(leftv res,leftv a);
BOOLEAN atKILLATTR2(leftv res,leftv a,leftv b);
#endif
