/*
Compute the Gröbner fan of an ideal
*/

#include "mod2.h"
#include "kstd1.h"
#include "intvec.h"

#ifndef gfan_DEBUG
#define gfan_DEBUG
#endif

ideal getGB(ideal inputIdeal){
  	ideal gb;
	gb=kStd(inputIdeal,NULL,testHomog,NULL); //Possible to call without testHomog/isHomog?
						// Why are 64 "results" printed?
	#ifdef gfan_DEBUG
	printf("Now in getGB\n");
	#endif
	return gb;
}

ideal gfan(ideal inputIdeal){
	#ifdef gfan_DEBUG
	printf("Now in subroutine gfan\n");
	#endif
	ideal res;
	res=getGB(inputIdeal);
	return res;
}

