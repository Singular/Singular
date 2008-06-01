/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.6 2008-06-01 15:14:37 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/

#include "mod2.h"
#include "kutil.h"
#include "structs.h"
#include "omalloc.h"
#include "polys.h"
#include "p_polys.h"
#include "ideals.h"
#include "febase.h"
#include "kstd1.h"
#include "khstd.h"
#include "kbuckets.h"
#include "weight.h"
#include "intvec.h"
#include "pInline1.h"
#include "f5gb.h"
#ifdef HAVE_F5



void lpoly::setPoly(poly* p){
        p_ptr = p;
}

void lpoly::setTerm(poly* t){
        t_ptr = t;
}

void lpoly::setIndex(long* i){
        i_ptr = i;
}

poly* lpoly::getPoly(){
        return p_ptr;
}

poly* lpoly::getTerm(){
        return t_ptr;
}

long* lpoly::getIndex(){
        return i_ptr;
}




/*2
* sorting ideals by decreasing total degree 
* "left" and "right" are the pointer of the first
* and last polynomial in the considered ideal
*/
void qsort_degree(poly* left, poly* right)
{
        poly* ptr1 = left;
        poly* ptr2 = right;
        poly p1,p2;

        p2 = *(left + (right - left >> 1));
        do{
                while(pTotaldegree(*ptr1, currRing) < pTotaldegree(p2, currRing)){
                        ptr1++;
                }
                while(pTotaldegree(*ptr2, currRing) > pTotaldegree(p2,currRing)){
                        ptr2--;
                }
                if(ptr1 > ptr2){
                        break;
                }
                p1    = *ptr1;
                *ptr1 = *ptr2;
                *ptr2 = p1;
        } while(++ptr1 <= --ptr2);

        if(left < ptr2){
                qsort_degree(left,ptr2);
        }
        if(ptr1 < right){
                qsort_degree(ptr1,right);
        }
}


/*2
* computes a gb of the ideal i in the ring r with our f5 
* implementation 
*/
ideal F5main(ideal i, ring r)
{
      ideal iTmp, g;
      int j;      
      lpoly* lp;
      intvec* sort;
      iTmp = idInit(IDELEMS(i),i->rank);
  
      for(j=0; j<IDELEMS(i); j++)
      {
              if(NULL != i->m[j])
              {
                      iTmp->m[j] = i->m[j];
              }
      }

      iTmp = kInterRed(i,0);  
      qsort_degree(&iTmp->m[0],&iTmp->m[IDELEMS(iTmp)-1]);
      idShow(iTmp);
      
      lp = new lpoly[IDELEMS(iTmp)];
      for(j=0; j <IDELEMS(iTmp); j++){
                lp[j].setPoly(&iTmp->m[j]);
                Print("Labeled Polynomial %d: ",j+1);
                pWrite(*(lp[j].getPoly()));
      }
                

            


        return iTmp;
}


#endif
