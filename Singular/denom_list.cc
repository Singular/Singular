#include <Singular/mod2.h>
#include <kernel/kutil.h>
#include <Singular/lists.h>

lists get_denom_list()
{
  int size;
  denominator_list d = DENOMINATOR_LIST;
  size=0;
  while(d!=0)
    {
      size++;
      d=d->next;
    }
  lists l=(lists)omAlloc(sizeof(*l));
  l->Init(size);
  d=DENOMINATOR_LIST;
  for (int i=0;i<size;i++)
    {
      l->m[i].rtyp=NUMBER_CMD;
      l->m[i].data=d->n;
      d=d->next;
      omFree(DENOMINATOR_LIST);
      DENOMINATOR_LIST=d;
    }
  return l;
}
