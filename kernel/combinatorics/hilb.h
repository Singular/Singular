#ifndef HILB_H
#define HILB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/

struct spolyrec  ; typedef struct spolyrec polyrec; typedef polyrec * poly;
struct ip_sring  ; typedef struct ip_sring                          * ring;
struct sip_sideal; typedef struct sip_sideal                        * ideal;

class intvec;

extern ring currRing;

intvec * hHstdSeries(ideal S, intvec *modulweight, intvec *wdegree, ideal Q=NULL, ring tailRing = currRing);
intvec * hFirstSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL, ring tailRing = currRing);

intvec * hSecondSeries(intvec *hseries1);

void hLookSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL, ring tailRing = currRing);

#endif


