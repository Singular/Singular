#include "lib.h"
#include "map.h"

struct KeyMapper {
  Str *operator()(Int key) {
    return ToStr(key);
  }
} keymapper;
struct ValMapper {
  Int operator()(Str *val) {
    return atoi(val->c_str());
  }
} valmapper;
void Main() {
  Map<Str *, Int> *to_i = new Map<Str *, Int>();
  Map<Int, Str *> *to_s = new Map<Int, Str *>();
  const int n = 1000;
  for (int i = 0; i < n; i++) {
    char buf[sizeof(int) * 3 + 1];
    sprintf(buf, "%d", i);
    Str *s = S(buf);
    to_i->add(s, i);
    to_s->add(i, s);
  }
  Check(to_i->count() == n && to_s->count() == n, "map sizes");
  to_i->union_in_place(to_i);
  Check(to_i->count() == n, "map union");
  to_i->intersect_in_place(to_i);
  Check(to_i->count() == n, "map intersection");
  Map<Str *, Int> *aux = to_i->diff_with(to_i);
  Check(aux->count() == 0, "map difference");
  Check(to_i->keys()->len() == to_i->values()->len(), "keys & values");
  int mismatch = 0;
  for (int i = 0; i < n; i++) {
    if (!to_s->contains(i))
      mismatch++;
    Str *s = to_s->get(i, NULL);
    if (!s || to_i->get(s, -1) == -1)
      mismatch++;
  }
  Check(mismatch == 0, "map indexing");
  Map<Str *, Int> *r
      = to_s->map_pairs(new Map<Str *, Int>(), keymapper, valmapper);
  Check(r->count() == n, "map pair mapping");
  Dict *dict = to_i->map_values(new Dict(), keymapper);
  Check(dict->count() == n, "map value mapping");
}
