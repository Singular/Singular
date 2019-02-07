#include "lib.h"
#include "set.h"

const char *data[] = {
  "alpha",
  "beta",
  "gamma",
  "delta",
  "epsilon",
  "zeta",
  "eta",
  "theta",
  "iota",
  "kappa",
  "lambda",
  "mu",
  "nu",
  "xi",
  "omicron",
  "pi",
  "rho",
  "sigma",
  "tau",
  "upsilon",
  "phi",
  "chi",
  "psi",
  "omega",
  NULL,
};

const int n = 100000;

void Main() {
  char buf[10];
  StrSet *set = new StrSet(Cmp, Hash);
  for (int i = 0; i < n; i++) {
    sprintf(buf, "%d", i);
    set->add(new Str(buf));
  }
  Check(set->count() == n, "set initialization");
  int miss = 0;
  for (int i = 0; i < n; i++) {
    sprintf(buf, "%d", i);
    if (!set->contains(new Str(buf))) {
      miss++;
    }
  }
  Check(miss == 0, "contains()");
  Check(set->count() == n, "count()");
  Check(set->items()->len() == n, "items()");
  set->remove(new Str("1234"));
  Check(set->count() == n - 1, "single remove()");
  for (int i = 0; i < n; i++) {
    sprintf(buf, "%d", i);
    set->remove(new Str(buf));
  }
  Check(set->count() == 0, "remove()");
  StrArr *arr1 = new StrArr(NUMOF(data) - 1, data, CStrToStr);
  StrArr *arr2 = new StrArr(data, (CStr) NULL, CStrToStr);
  Check(arr1->eq(arr2), "array/string equality");
  arr1 = arr1->subarr(10, arr1->len() - 10);
  arr2->remove(0, 10);
  Check(arr1->eq(arr2), "array ranges");
  Str *str = S("alphalpha");
  Check(str->substr(0, 5)->eq(str->substr(4, 5)), "substrings");
  Check(str->find('a', 1) == 4 && str->find("alp", 1) == 4
          && str->find("alpha", 1) == 4 && str->find("alphx") < 0,
      "string search");
  Check(str->rfind('l') == 5 && str->rfind("alp") == 4
          && str->find("alx") == NOWHERE,
      "reverse string search");
  str = S("\x80 \x81 \xff abc");
  Check(str->find(" \x81 ") == 1 && str->find("\x81 ") == 2
          && str->find("\xff abc") == 4 && str->find("\xff abd") < 0,
      "string search (signed char)");
  Check(S("here%sthere%sbe%sdragons")->replace_all(S("%s"), S(" "))
    ->eq("here there be dragons") &&
    S("axbxc")->replace_count(1, S("x"), S("y"))->eq("aybxc"),
      "string replacement");
}
