#include "lib.h"

#include "set.h"
#include <ctype.h>

int IntCmp(Int a, Int b) {
  if (a < b)
    return -1;
  if (a > b)
    return 1;
  return 0;
}

Str *capitalize(Str *arg) {
  Str *result = arg->clone();
  result->ch(0) = toupper(result->ch(0));
  return result;
}

Int add(Int acc, Int value) {
  return acc + value;
}

bool even(Int value) {
  return value % 2 == 0;
}

Int succ(Int value) {
  return value + 1;
}

Str *join(Str *acc, Str *value) {
  acc->add(value);
  return acc;
}

bool ends_with(Str *a, Str *b) {
  return a->ends_with(b);
}

void Main() {
  StrArr *arr = new StrArr();
  Str *collect = new Str();

  arr->add(S("x"))->add(S("y"))->add(S("abc"));
  arr = arr->clone();
  for (StrArr::Each iter(arr); iter; ++iter) {
    collect->add(*iter)->add(iter->c_str());
  }
  for (Str::Each iter(arr->last()->clone()); iter; ++iter) {
    collect->add(*iter);
  }
  Check(collect->eq("xxyyabcabcabc"), "array/string iteration");
  collect = new Str();
  arr = arr->map<Str *>(capitalize);
  for (StrArr::Each iter(arr); iter; ++iter) {
    collect->add(*iter);
    collect->add(iter->c_str());
  }
  for (Str::Each iter(arr->last()->clone()); iter; ++iter) {
    collect->add(*iter);
  }
  Check(collect->eq("XXYYAbcAbcAbc"), "array map");
  Arr<Int> *intarr = new Arr<Int>();
  for (int i = 1; i <= 10; i++) {
    intarr->add(i);
  }
  Check(intarr->fold(0, add) == 10 * 11 / 2, "array folding");
  Check(intarr->filter(even)->len() == 5, "array filtering");
  intarr = new Arr<Int>();
  for (Word i = 0, v = 1; i < 100000; i++) {
    intarr->add((Int) v);
    v = (5 * v + 1) & 0xffffff;
  }
  intarr = intarr->sort(IntCmp);
  int mismatch = 0;
  for (Int i = 1; i < intarr->len(); i++) {
    if (intarr->at(i - 1) >= intarr->at(i))
      mismatch++;
  }
  Check(mismatch == 0, "sorting arrays");
  StrSet *set = new StrSet();
  set->add(S("x"));
  set->add(S("y"));
  set->add(S("z"));
  set->add(S("x"));
  StrSet *set2 = new StrSet();
  set2->add(S("u"));
  set2->add(S("y"));
  set2->add(S("z"));

  StrSet *uset = set->union_with(set2);
  StrSet *iset = set->intersect_with(set2);
  StrSet *dset = set->diff_with(set2);
  Check(uset->count() == 4, "set union");
  Check(iset->count() == 2, "set intersection");
  Check(dset->count() == 1, "set difference");
  uset = set->clone();
  uset->union_in_place(set2);
  iset = set->clone();
  iset->intersect_in_place(set2);
  dset = set->clone();
  dset->diff_in_place(set2);
  Check(uset->count() == 4, "set union in place");
  Check(iset->count() == 2, "set intersection in place");
  Check(dset->count() == 1, "set difference in place");
  Str *folded = uset->fold(S(""), join);
  StrSet *mapped = dset->map(new StrSet(), capitalize);
  Set<Int> *intset = new Set<Int>();
  for (Int i = 0; i < 100; i++)
    intset->add(i);
  intset = intset->filter(even);
  Check(folded->len() == 4, "set folding");
  Check(mapped->contains(S("X")), "set mapping");
  Check(intset->count() == 50, "set filtering");
  Int sum = intset->fold(0, add);
  intset = intset->map(new Set<Int>(), succ);
  Check(intset->fold(-50, add) == sum, "set mapping/folding");
  StrSet *uniq = new StrSet();
  Str *a = uniq->get_or_add(S("alpha"));
  Str *b = uniq->get_or_add(S("alpha"));
  Check(a == b, "string interning");
  StrArr *files = A("alpha.c", "alpha.h", "beta.c", "beta.h");
  StrArr *headers = files->filter(F(ends_with, S(".h")));
  Check(headers->len() == 2, "lambda filters");
}
