/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.11 2008-11-20 17:55:04 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#include "mod2.h"

#ifdef HAVE_F5
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
#include "lpolynom.h"


// global variables
const poly one = pInit();


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
lpoly *f5_inc(lpoly* lp, lpoly* g_prev)
{
        long length = 1;
        
        while(g_prev->getNext() != NULL) {
                length++;
                g_prev++; 
        }
        Print("Laenge der Liste: %d\n",length);
        /*critpair *critpairs = new critpair[length];
        * int i = 1;
        *
        * while(g_prev->getNext()->getNext() != NULL) {
        *        critpairs[i] = {lp, g_prev, critpairs[i+1]};
        *        i++;
        *        g_prev++;
        * } 
        * *critpairs[length] = {lp, g_prev+length-1, NULL};
        *
        * return lp;
        */
}    


// generating the list lp of ideal generators and test if 1 is in lp
bool generate_input_list(lpoly* lp, ideal id)
{
        long j;
        for(j=0; j<IDELEMS(id)-1; j++){

                lp[j].setPoly(id->m[j]);
                
                if(pComparePolys(lp[j].getPoly(), one)){
                        Print("1 in GB\n");
                        return(1);
                }
                
                lp[j].setIndex(j+1);
                lp[j].setTerm(one);
                lp[j].setDel(false);
                lp[j].setNext(&lp[j+1]);
                Print("Labeled Polynomial %d: ",j+1);
                Print("Label Term: ");
                pWrite(lp[j].getTerm());
                Print("Index: %d\n", lp[j].getIndex());
                Print("Delete? %d\n", lp[j].getDel());
                pWrite(lp[j].getPoly());
                Print("NEUNEUNEU\n\n");
        }
        return(0);
}


/*2
* computes a gb of the ideal i in the ring r with our f5 
* implementation 
*/
ideal F5main(ideal i, ring r)
{
        ideal iTmp, g_basis;
        bool one_id;
        long j,k;
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
        ;}
        
        iTmp = kInterRed(i,0);  
        qsort_degree(&iTmp->m[0],&iTmp->m[IDELEMS(iTmp)-1]);
        idShow(iTmp);
        
        // generating the list lp of ideal generators 
        lp = new lpoly[IDELEMS(iTmp)];
        
        one_id = generate_input_list(lp,iTmp);        
         
        
        // PROBLEM: muss signatur mitliefern, daher datentyp
        //          ideal nicht zu gebrauchen? 
        

        // gb: Grobner Bases, pointer on the first element in the list, 
        // end of the list is marked by next = NULL
        
        gb = new lpoly;
        gb = &lp[IDELEMS(iTmp)-1];
        (*gb).setNext(NULL);
        Print("last element in sequence = first element in gb:\n");
        pWrite((*gb).getPoly());
        Print("Next: %p\n", lp[IDELEMS(iTmp)-2].getNext());
        Print("Next: %p\n", lp[IDELEMS(iTmp)-1].getNext());
        
        for(j=IDELEMS(iTmp)-2; j>0; j--){
                //PROBLEM: muss dynamisch Speicher allozieren
                gb = f5_inc(&lp[j], gb);
                
                for(k=0; k< IDELEMS(iTmp); k++){
                        
                        if(pComparePolys(gb[k].getPoly(),one)){
                                Print("1 in GB\n");
                                return(iTmp);
                        }

                }
        
         
        }

        Print("Es klappt!\n");
        return(iTmp); 
                        


}


#endif
