// ideal maGetPreimage(ring theImageRing, map theMap,ideal id);

/*2
*returns the preimage of id under theMap,
*if id is empty or zero the kernel is computed
* (assumes) that both ring have the same coeff.field
*/
ideal maGetPreimage(ring theImageRing, map theMap, ideal id, const ring dst_r)
{
  ring sourcering = dst_r;

#ifdef HAVE_PLURAL
  if (rIsPluralRing(theImageRing))
  {
    if ((rIsPluralRing(sourcering)) && (ncRingType(sourcering)!=nc_comm)) 
    {
      Werror("Sorry, not yet implemented for noncomm. rings");
      return NULL;
    }
  }
#endif
  
  int i,j;
  poly p,pp,q;
  ideal temp1;
  ideal temp2;

  int imagepvariables = rVar(theImageRing);
  int N = rVar(dst_r)+imagepvariables;

  ring tmpR;
  if (rSumInternal(theImageRing,sourcering,tmpR,FALSE,TRUE)!=1)
  {
     WerrorS("error in rSumInternal");
     return NULL;
  }

  if (n_SetMap(theImageRing->cf,dst_r->cf) != ndCopyMap)
  {
    Werror("Coefficient fields/rings must be equal");
    return NULL;
  }

  if (id==NULL)
    j = 0;
  else
    j = IDELEMS(id);
  int j0=j;
  if (theImageRing->qideal!=NULL) j+=IDELEMS(theImageRing->qideal);
  temp1 = idInit(sourcering->N+j,1);
  for (i=0;i<sourcering->N;i++)
  {
    q = p_ISet(-1,tmpR);
    p_SetExp(q,i+1+imagepvariables,1,tmpR);
    p_Setm(q,tmpR);
    if ((i<IDELEMS(theMap)) && (theMap->m[i]!=NULL))
    {
      p = p_SortMerge(
        pChangeSizeOfPoly(theImageRing,theMap->m[i],1,imagepvariables,tmpR),
	tmpR);
      p=p_Add_q(p,q,tmpR);
    }
    else
    {
      p = q;
    }
    temp1->m[i] = p;
  }
  idTest(temp1);
  for (i=sourcering->N;i<sourcering->N+j0;i++)
  {
    temp1->m[i] = p_SortMerge(pChangeSizeOfPoly(theImageRing,
                         id->m[i-sourcering->N],1,imagepvariables,tmpR),tmpR);
  }
  for (i=sourcering->N+j0;i<sourcering->N+j;i++)
  {
    temp1->m[i] = p_SortMerge(pChangeSizeOfPoly(theImageRing,
                              theImageRing->qideal->m[i-sourcering->N-j0],
                              1,imagepvariables,tmpR),tmpR);
  }
  // we ignore here homogenity - may be changed later:
  temp2 = kStd(temp1,NULL,isNotHomog,NULL);
  id_Delete(&temp1,tmpR);
  for (i=0;i<IDELEMS(temp2);i++)
  {
    if (pLowVar(temp2->m[i])<imagepvariables) p_Delete(&(temp2->m[i]),tmpR);
  }

  // let's get back to the original ring
  //rChangeCurrRing(sourcering);
  temp1 = idInit(5,1);
  j = 0;
  for (i=0;i<IDELEMS(temp2);i++)
  {
    p = temp2->m[i];
    if (p!=NULL)
    {
      q = p_SortMerge(pChangeSizeOfPoly(tmpR, p,imagepvariables+1,N),sourcering);
      if (j>=IDELEMS(temp1))
      {
        pEnlargeSet(&(temp1->m),IDELEMS(temp1),5);
        IDELEMS(temp1)+=5;
      }
      temp1->m[j] = q;
      j++;
    }
  }
  id_Delete(&temp2, tmpR);
  idSkipZeroes(temp1);
  rKill(tmpR);
  return temp1;
}

