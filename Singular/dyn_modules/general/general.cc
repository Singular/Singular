#include <Singular/libsingular.h>

#include <vector>

void subset(std::vector<int> &arr, int size, int left, int index, std::vector<int> &l, std::vector<std::vector<int> > &L)
{
  if(left==0)
  {
    L.push_back(l);
    return;
  }

  for(int i=index; i<size;i++)
  {
    l.push_back(arr[i]);
    subset(arr,size,left-1,i+1,l,L);
    l.pop_back();
  }
}

BOOLEAN subsets(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==INT_CMD))
  {
    leftv v = u->next;
    if ((v!=NULL) && (v->Typ()==INT_CMD) && (v->next==NULL))
    {
      int n = (int)(long) u->Data();
      int k = (int)(long) v->Data();
      std::vector<int> array(n);
      for (int i=0; i<n; i++)
        array[i]=i+1;
      std::vector<int> ltemp;
      std::vector<std::vector<int> > lt;
      subset(array,n,k,0,ltemp,lt);

      lists Lt = (lists) omAllocBin(slists_bin);
      Lt->Init(lt.size());
      for (int i=0; i<lt.size(); i++)
      {
        std::vector<int> lti = lt[i];
        lists Lti = (lists) omAllocBin(slists_bin);
        Lti->Init(k);
        for(int j=0; j<lti.size(); j++)
        {
          Lti->m[j].rtyp = INT_CMD;
          Lti->m[j].data = (void*)(long)lti[j];
        }
        Lt->m[i].rtyp = LIST_CMD;
        Lt->m[i].data = (void*) Lti;
      }

      res->rtyp = LIST_CMD;
      res->data = (void*) Lt;
      return FALSE;
    }
  }
  WerrorS("subsets: unexpected parameter");
  return TRUE;
}

//------------------------------------------------------------------------
// initialisation of the module
extern "C" int SI_MOD_INIT(customstd)(SModulFunctions* p)
{
  p->iiAddCproc("general.lib","subsets",FALSE,subsets);
  return (MAX_TOK);
}
