/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.7 2008-06-26 16:05:07 ederc Exp $ */
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

/*2
* all functions working on the class lpoly for labeled polynomials
*/

void lpoly::setPoly(poly p){
        polynomial = p;
}

void lpoly::setTerm(poly t){
        term = t;
}

void lpoly::setIndex(long i){
        index = i;
}

poly lpoly::getPoly(){
        return polynomial;
}

poly lpoly::getTerm(){
        return term;
}

long lpoly::getIndex(){
        return index;
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
* computes incrementally gbs of subsets of the input 
* gb{f_m} -> gb{f_m,f_(m-1)} -> gb{f_m,...,f_1}
*/  
lpoly* f5_inc(lpoly* lp, lpoly* g_prev)
{
        
        
        return lp;
}    


/*2
* computes a gb of the ideal i in the ring r with our f5 
* implementation 
*/
ideal F5main(ideal i, ring r)
{
      ideal iTmp, g_basis;
      long j, k;
      poly one = pInit();
      pSetCoeff(one, nInit(1));
      pWrite(one);
      lpoly *lp, *gb;
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
                lp[j].setPoly(iTmp->m[j]);
                
                if(pComparePolys(lp[j].getPoly(), one)){
                                Print("1 in GB");
                                return(NULL);
                }
                
                lp[j].setIndex(j+1);
                lp[j].setTerm(one);
                Print("Labeled Polynomial %d: ",j+1);
                Print("Signature Term: ");
                pWrite(lp[j].getTerm());
                Print("Signature Index: %d\n", lp[j].getIndex());
                pWrite(lp[j].getPoly());
                Print("\n\n");
                         
      }
      
      // PROBLEM: muss signatur mitliefern, daher datentyp
      //          ideal nicht zu gebrauchen? 
      gb = new lpoly;
      gb = &lp[IDELEMS(iTmp)-1];
      pWrite((*gb).getPoly());

      for(j=IDELEMS(iTmp)-2; j>0; j--){
             //PROBLEM: muss dynamisch Speicher allozieren
             gb = f5_inc(&lp[j], gb);
             for(k=0; k< IDELEMS(iTmp); k++){
                    if(gb[k].getPoly()){
                    }
             }



               
      }
                
                        


        return iTmp;
}


#endif
