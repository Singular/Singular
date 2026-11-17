/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include "kernel/mod2.h"

#include "resources/hash_me.h"
#include "reporter/reporter.h"
#include "kernel/polys.h"
#include "Singular/ipshell.h"
#include "Singular/htable.h"

#include <string.h>

static int t_normalize_size(int s)
{
  if (s < 3) s = 3;
  int n = 3;
  while (n < s) n = n * 2 + 1;
  return n;
}

static inline uint32_t t_hash_key(const char *s)
{
  return hashlittle(s, strlen(s));
}

static void t_use_ring(ring r, ring *save)
{
  *save = currRing;
  if ((r != NULL) && (currRing != r)) rChangeCurrRing(r);
}

static void t_restore_ring(ring save)
{
  if (currRing != save) rChangeCurrRing(save);
}

static void t_delete_elem(telem p)
{
  if (p == NULL) return;
  omFree(p->key);
  if (p->val_ring != NULL)
  {
    ring save;
    t_use_ring(p->val_ring, &save);
    p->val.CleanUp(p->val_ring);
    t_restore_ring(save);
  }
  else
  {
    p->val.CleanUp();
  }
  if (p->val_ring != NULL) rKill(p->val_ring);
  omFreeSize(p, sizeof(stelem));
}

static telem t_findTableByHash(stablerec* t, const char *s, uint32_t h)
{
  if ((t == NULL) || (s == NULL) || (t->max <= 0)) return NULL;
  uint32_t b = h % t->max;
  telem p = t->t[b];
  while (p != NULL)
  {
    if ((p->hash == h) && (strcmp(s, p->key) == 0)) return p;
    p = p->next;
  }
  return NULL;
}

static BOOLEAN t_same_origin_elem(telem p, telem q)
{
  if ((p == NULL) || (q == NULL)) return FALSE;
  return ((p->origin_typ == q->origin_typ) && (p->origin_data == q->origin_data));
}

static BOOLEAN t_grow_if_needed(stablerec* t)
{
  if (t == NULL) return TRUE;
  if ((t->count + 1) * 4 <= t->max * 3) return FALSE;

  int oldmax = t->max;
  int newmax = t_normalize_size(oldmax * 2 + 1);
  telem *oldtab = t->t;
  telem *newtab = (telem*)omAlloc0(newmax * sizeof(telem));
  if (newtab == NULL) return TRUE;

  for (int i = 0; i < oldmax; i++)
  {
    telem p = oldtab[i];
    while (p != NULL)
    {
      telem n = p->next;
      uint32_t b = p->hash % newmax;
      p->next = newtab[b];
      newtab[b] = p;
      p = n;
    }
  }

  omFreeSize(oldtab, oldmax * sizeof(telem));
  t->t = newtab;
  t->max = newmax;
  return FALSE;
}

stablerec* t_createTable(int s)
{
  s = t_normalize_size(s);
  stablerec* t = (stablerec*)omAlloc(sizeof(stablerec));
  t->max = s;
  t->t = (telem*)omAlloc0(s * sizeof(telem));
  t->ref = 1;
  t->count = 0;
  return t;
}

void t_destroyTable(stablerec* t)
{
  if (t == NULL) return;
  t->ref--;
  if (t->ref <= 0)
  {
    for (int i = t->max - 1; i >= 0; i--)
    {
      telem p = t->t[i];
      while (p != NULL)
      {
        telem pp = p;
        p = p->next;
        t_delete_elem(pp);
      }
    }
    omFreeSize(t->t, t->max * sizeof(telem));
    omFreeSize(t, sizeof(stablerec));
  }
}

stablerec* copyTable(stablerec* t)
{
  if (t != NULL) t->ref++;
  return t;
}

static BOOLEAN t_insert_elem_copy(stablerec *dst, telem src)
{
  if ((dst == NULL) || (src == NULL)) return TRUE;
  telem dup = t_findTableByHash(dst, src->key, src->hash);
  if (dup != NULL)
  {
    if (t_same_origin_elem(dup, src)) return FALSE;
    Werror("duplicate key `%s' in htable", src->key);
    return TRUE;
  }

  if (t_grow_if_needed(dst)) return TRUE;

  telem p = (telem)omAlloc0(sizeof(stelem));
  p->key = omStrDup(src->key);
  p->hash = src->hash;
  p->origin_typ = src->origin_typ;
  p->origin_data = src->origin_data;
  if (src->val_ring != NULL) p->val_ring = rIncRefCnt(src->val_ring);
  p->val.Init();

  ring save;
  t_use_ring(src->val_ring, &save);
  p->val.Copy(&(src->val));
  t_restore_ring(save);

  if (errorreported)
  {
    t_delete_elem(p);
    return TRUE;
  }

  uint32_t b = p->hash % dst->max;
  p->next = dst->t[b];
  dst->t[b] = p;
  dst->count++;
  return FALSE;
}

stablerec* t_cloneTable(stablerec* t)
{
  if (t == NULL) return NULL;
  stablerec *n = t_createTable(t->count * 2 + 3);
  if (t_mergeTable(n, t))
  {
    t_destroyTable(n);
    return NULL;
  }
  return n;
}

char* stringTable(stablerec* t)
{
  if (t == NULL) return omStrDup("htable()\n");
  StringSetS("htable:\n");
  char *s;
  for (int i = t->max - 1; i >= 0; i--)
  {
    telem p = t->t[i];
    while (p != NULL)
    {
      StringAppendS(p->key);
      StringAppendS(" -> ");
      ring save;
      t_use_ring(p->val_ring, &save);
      s = p->val.String();
      t_restore_ring(save);
      StringAppendS(s);
      omFree(s);
      StringAppendS("\n");
      p = p->next;
    }
  }
  return StringEndS();
}

int t_countTable(stablerec *t)
{
  return (t == NULL ? 0 : t->count);
}

BOOLEAN t_RingDependend(stablerec *t)
{
  if (t == NULL) return FALSE;
  for (int i = t->max - 1; i >= 0; i--)
  {
    telem p = t->t[i];
    while (p != NULL)
    {
      if (p->val_ring != NULL) return TRUE;
      if (p->val.RingDependend()) return TRUE;
      p = p->next;
    }
  }
  return FALSE;
}

/// find the entry to key s
telem t_findTable(stablerec* t, const char *s)
{
  if ((t == NULL) || (s == NULL)) return NULL;
  return t_findTableByHash(t, s, t_hash_key(s));
}

/// find the data to key s
leftv t_findTabelVal(stablerec* t, const char *s)
{
  telem p = t_findTable(t, s);
  if (p == NULL) return NULL;
  return &(p->val);
}

BOOLEAN t_copyTableVal(leftv res, stablerec* t, const char *s)
{
  telem p = t_findTable(t, s);
  if (p == NULL)
  {
    Werror("no such key `%s' in htable", s == NULL ? "" : s);
    return TRUE;
  }
  if ((p->val_ring != NULL) && (p->val_ring != currRing))
  {
    Werror("object `%s' in htable is from another ring", s);
    return TRUE;
  }
  ring save;
  t_use_ring(p->val_ring, &save);
  res->Copy(&(p->val));
  t_restore_ring(save);
  return errorreported;
}

/// add a new entry (key s, data v) to table t, eats s, copies v
BOOLEAN t_addTable(stablerec* t, char *s, leftv v)
{
  if (v == NULL)
  {
    if (s != NULL) omFree(s);
    return TRUE;
  }
  return t_addTableOrigin(t, s, v, v->Typ(), v->Data());
}

BOOLEAN t_addTableOrigin(stablerec* t, char *s, leftv v, int origin_typ, void *origin_data)
{
  if ((t == NULL) || (s == NULL) || (v == NULL))
  {
    if (s != NULL) omFree(s);
    return TRUE;
  }
  if ((v->Typ() == UNKNOWN) || (origin_typ == UNKNOWN))
  {
    Werror("undefined value `%s' in htable entry `%s'", v->Fullname(), s);
    omFree(s);
    return TRUE;
  }
  if (errorreported)
  {
    omFree(s);
    return TRUE;
  }

  uint32_t h = t_hash_key(s);
  telem dup = t_findTableByHash(t, s, h);
  if (dup != NULL)
  {
    if ((dup->origin_typ == origin_typ) && (dup->origin_data == origin_data))
    {
      omFree(s);
      return FALSE;
    }
    Werror("duplicate key `%s' in htable", s);
    omFree(s);
    return TRUE;
  }

  if (t_grow_if_needed(t))
  {
    omFree(s);
    return TRUE;
  }

  telem p = (telem)omAlloc0(sizeof(stelem));
  p->next = NULL;
  p->key = s;
  p->hash = h;
  p->origin_typ = origin_typ;
  p->origin_data = origin_data;
  if (v->RingDependend() && (currRing != NULL)) p->val_ring = rIncRefCnt(currRing);
  p->val.Init();
  p->val.Copy(v);
  if (errorreported)
  {
    t_delete_elem(p);
    return TRUE;
  }

  uint32_t b = h % t->max;
  p->next = t->t[b];
  t->t[b] = p;
  t->count++;
  return FALSE;
}

BOOLEAN t_mergeTable(stablerec* dst, stablerec* src)
{
  if ((dst == NULL) || (src == NULL)) return TRUE;
  for (int i = src->max - 1; i >= 0; i--)
  {
    telem p = src->t[i];
    while (p != NULL)
    {
      if (t_insert_elem_copy(dst, p)) return TRUE;
      p = p->next;
    }
  }
  return FALSE;
}

//-------------------------------------------------------
void htable_Print(stablerec *d)
{
  stablerec* lt = (stablerec*)d;
  char* s = stringTable(lt);
  PrintS(s);
  omFree(s);
  int cnt = 0;
  int cnt2 = 0;
  if (lt != NULL)
  {
    for (int i = 0; i < lt->max; i++)
    {
      if (lt->t[i] != NULL)
      {
        cnt++;
        telem p = lt->t[i];
        while (p != NULL) { cnt2++; p = p->next; }
      }
    }
    Print("%d columns, %d entries, size:%d", cnt, cnt2, lt->max);
  }
}
