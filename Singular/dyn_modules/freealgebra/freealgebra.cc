#include "Singular/libsingular.h"
#include <vector>

#ifdef HAVE_SHIFTBBA
static BOOLEAN freeAlgebra(leftv res, leftv args)
{
  const short t1[]={2,RING_CMD,INT_CMD};
  if (iiCheckTypes(args,t1,1))
  {
    ring r=(ring)args->Data();
    int d=(int)(long)args->next->Data();
    if (d<2)
    {
      WerrorS("degree must be >=2");
      return TRUE;
    }
    int i=0;
    while(r->order[i]!=0)
    {
      if ((r->order[i]==ringorder_c) ||(r->order[i]==ringorder_C)) i++;
      else if ((r->block0[i]==1)&&(r->block1[i]==r->N)) i++;
      else
      {
        WerrorS("only for rings with a global ordering of one block");
        return TRUE;
      }
    }
    if ((r->order[i]!=0)
    || (rHasLocalOrMixedOrdering(r)))
    {
      WerrorS("only for rings with a global ordering of one block");
      //Werror("only for rings with a global ordering of one block,i=%d, o=%d",i,r->order[i]);
      return TRUE;
    }
    ring R=freeAlgebra(r,d);
    res->rtyp=RING_CMD;
    res->data=R;
    return R==NULL;
  }
  return TRUE;
}

static BOOLEAN stest(leftv res, leftv args)
{
  const short t[]={2,POLY_CMD,INT_CMD};
  if (iiCheckTypes(args,t,1))
  {
    poly p=(poly)args->CopyD();
    args=args->next;
    int sh=(int)((long)(args->Data()));
    if (sh<0)
    {
      WerrorS("negative shift for pLPshift");
      return TRUE;
    }
    int L = pLastVblock(p);
    if (L+sh > currRing->N/currRing->isLPring)
    {
      WerrorS("pLPshift: too big shift requested\n");
      return TRUE;
    }
    p_LPshift(p,sh,currRing);
    res->data = p;
    res->rtyp = POLY_CMD;
    return FALSE;
  }
  else return TRUE;
}

static BOOLEAN btest(leftv res, leftv h)
{
  const short t[]={1,POLY_CMD};
  if (iiCheckTypes(h,t,1))
  {
    poly p=(poly)h->Data();
    res->rtyp = INT_CMD;
    res->data = (void*)(long)pLastVblock(p);
    return FALSE;
  }
  else return TRUE;
}

static BOOLEAN lpLmDivides(leftv res, leftv h)
{
  const short t1[]={2,POLY_CMD,POLY_CMD};
  const short t2[]={2,IDEAL_CMD,POLY_CMD};
  if (iiCheckTypes(h,t1,0))
  {
    poly p=(poly)h->Data();
    poly q=(poly)h->next->Data();
    res->rtyp = INT_CMD;
    res->data = (void*)(long)p_LPDivisibleBy(p, q, currRing);
    return FALSE;
  }
  else if (iiCheckTypes(h,t2,1))
  {
    ideal I=(ideal)h->Data();
    poly q=(poly)h->next->Data();
    res->rtyp = INT_CMD;
    for(int i=0;i<IDELEMS(I);i++)
    {
      if (p_LPDivisibleBy(I->m[i],q, currRing))
      {
        res->data=(void*)(long)1;
        return FALSE;
      }
    }
    res->data=(void*)(long)0;
    return FALSE;
  }
  else return TRUE;
}

static BOOLEAN lpVarAt(leftv res, leftv h)
{
  const short t[]={2,POLY_CMD,INT_CMD};
  if (iiCheckTypes(h,t,1))
  {
    poly p=(poly)h->Data();
    int pos=(int)((long)(h->next->Data()));
    res->rtyp = POLY_CMD;
    res->data = p_LPVarAt(p, pos, currRing);
    return FALSE;
  }
  else return TRUE;
}

// NULL if graph is undefined
static intvec* ufnarovskiGraph(ideal G)
{
  long l = 0;
  for (int i = 0; i < IDELEMS(G); i++)
    l = si_max(pTotaldegree(G->m[i]), l);
  l--;
  if (l <= 0)
  {
    WerrorS("Ufnarovski graph not implemented for l <= 0");
    return NULL;
  }
  int lV = currRing->isLPring;

  ideal words = idMaxIdeal(l);
  ideal standardWords = kNF(G, currRing->qideal, words);
  idSkipZeroes(standardWords);

  int n = IDELEMS(standardWords);
  intvec* UG = new intvec(n, n, 0);
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      poly v = standardWords->m[i];
      poly w = standardWords->m[j];

      // check whether v*x1 = x2*w (overlap)
      bool overlap = true;
      for (int k = 1; k <= (l - 1) * lV; k++)
      {
        if (pGetExp(v, k + lV) != pGetExp(w, k)) {
          overlap = false;
          break;
        }
      }

      if (overlap)
      {
        // create the overlap
        poly p = pMult(pCopy(v), p_LPVarAt(w, l, currRing));

        // check whether the overlap is normal
        bool normal = true;
        for (int k = 0; k < IDELEMS(G); k++)
        {
          if (p_LPDivisibleBy(G->m[k], p, currRing))
          {
            normal = false;
            break;
          }
        }

        if (normal)
        {
          IMATELEM(*UG, i + 1, j + 1) = 1;
        }
      }
    }
  }
  return UG;
}

static std::vector<int> countCycles(const intvec* _G, int v, std::vector<int> path, std::vector<BOOLEAN> visited, std::vector<BOOLEAN> cyclic, std::vector<int> cache)
{
  intvec* G = ivCopy(_G); // modifications must be local

  if (cache[v] != -2) return cache; // value is already cached

  visited[v] = TRUE;
  path.push_back(v);

  int cycles = 0;
  for (int w = 0; w < G->cols(); w++)
  {
    if (IMATELEM(*G, v + 1, w + 1)) // edge v -> w exists in G
    {
      if (!visited[w])
      { // continue with w
        cache = countCycles(G, w, path, visited, cyclic, cache);
        if (cache[w] == -1)
        {
          cache[v] = -1;
          return cache;
        }
        cycles = si_max(cycles, cache[w]);
      }
      else
      { // found new cycle
        int pathIndexOfW = -1;
        for (int i = path.size() - 1; i >= 0; i--) {
          if (cyclic[path[i]] == 1) { // found an already cyclic vertex
            cache[v] = -1;
            return cache;
          }
          cyclic[path[i]] = TRUE;

          if (path[i] == w) { // end of the cycle
            assume(IMATELEM(*G, v + 1, w + 1) != 0);
            IMATELEM(*G, v + 1, w + 1) = 0; // remove edge v -> w
            pathIndexOfW = i;
            break;
          } else {
            assume(IMATELEM(*G, path[i - 1] + 1, path[i] + 1) != 0);
            IMATELEM(*G, path[i - 1] + 1, path[i] + 1) = 0; // remove edge vi-1 -> vi
          }
        }
        assume(pathIndexOfW != -1); // should never happen
        for (int i = path.size() - 1; i >= pathIndexOfW; i--) {
          cache = countCycles(G, path[i], path, visited, cyclic, cache);
          if (cache[path[i]] == -1)
          {
            cache[v] = -1;
            return cache;
          }
          cycles = si_max(cycles, cache[path[i]] + 1);
        }
      }
    }
  }
  cache[v] = cycles;

  delete G;
  return cache;
}

// -1 is infinity
static int graphGrowth(const intvec* G)
{
  // init
  int n = G->cols();
  std::vector<int> path;
  std::vector<BOOLEAN> visited;
  std::vector<BOOLEAN> cyclic;
  std::vector<int> cache;
  visited.resize(n, FALSE);
  cyclic.resize(n, FALSE);
  cache.resize(n, -2);

  // get max number of cycles
  int cycles = 0;
  for (int v = 0; v < n; v++)
  {
    cache = countCycles(G, v, path, visited, cyclic, cache);
    if (cache[v] == -1)
      return -1;
    cycles = si_max(cycles, cache[v]);
  }
  return cycles;
}

// -1 is infinity, -2 is error
static int id_LPGkDim(const ideal _G)
{
  if (rField_is_Ring(currRing)) {
      WerrorS("GK-Dim not implemented for rings");
      return -2;
  }

  for (int i=IDELEMS(_G)-1;i>=0; i--)
  {
    if (pGetComp(_G->m[i]) != 0)
    {
      WerrorS("GK-Dim not implemented for modules");
      return -2;
    }
  }

  ideal G = id_Head(_G, currRing); // G = LM(G) (and copy)
  idSkipZeroes(G); // remove zeros
  id_DelLmEquals(G, currRing); // remove duplicates

  // get the max deg
  long maxDeg = 0;
  for (int i = 0; i < IDELEMS(G); i++)
  {
    maxDeg = si_max(maxDeg, pTotaldegree(G->m[i]));

    // also check whether G = <1>
    if (pIsConstantComp(G->m[i]))
    {
      WerrorS("GK-Dim not defined for 0-ring");
      return -2;
    }
  }

  // early termination if G \subset X
  if (maxDeg <= 1)
  {
    int lV = currRing->isLPring;
    if (IDELEMS(G) == lV) // V = {1} no edges
      return 0;
    if (IDELEMS(G) == lV - 1) // V = {1} with loop
      return 1;
    if (IDELEMS(G) <= lV - 2) // V = {1} with more than one loop
      return -1;
  }

  intvec* UG = ufnarovskiGraph(G);
  if (errorreported || UG == NULL) return -2;
  return graphGrowth(UG);
}


static BOOLEAN lpGkDim(leftv res, leftv h)
{
  const short t[]={1,IDEAL_CMD};
  if (iiCheckTypes(h,t,1))
  {
    assumeStdFlag(h);
    ideal G = (ideal) h->Data();
    res->rtyp = INT_CMD;
    res->data = (void*)(long) id_LPGkDim(G);
    if (errorreported) return TRUE;
    return FALSE;
  }
  else return TRUE;
}
#endif

//------------------------------------------------------------------------
// initialisation of the module
extern "C" int SI_MOD_INIT(freealgebra)(SModulFunctions* p)
{
#ifdef HAVE_SHIFTBBA
  p->iiAddCproc("freealgebra.so","freeAlgebra",FALSE,freeAlgebra);
  p->iiAddCproc("freealgebra.so","lpLmDivides",FALSE,lpLmDivides);
  p->iiAddCproc("freealgebra.so","lpVarAt",FALSE,lpVarAt);
  p->iiAddCproc("freealgebra.so","lpGkDim",FALSE,lpGkDim);

  p->iiAddCproc("freealgebra.so","stest",TRUE,stest);
  p->iiAddCproc("freealgebra.so","btest",TRUE,btest);
#endif
  return (MAX_TOK);
}
