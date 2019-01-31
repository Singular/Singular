#include "adlib/lib.h"
#include "adlib/bitset.h"

#define CHECK_MAT(mat, prop, msg) \
  { \
    Int rows = mat->len(); \
    Int cols = mat->at(0)->len(); \
    bool ok = true; \
    for (Int row = 0; row < rows; row++) { \
      for (Int col = 0; col < cols; col++) { \
        ok &= mat->at(row)->test(col) == (prop); \
      } \
    } \
    Check(ok, msg); \
  }

void Main() {
  BitSet *set = new BitSet(256);
  BitSet *set2 = new BitSet(256);
  for (Int i = 0; i < 200; i += 2) {
    set->set(i);
    set2->set(i + 1);
  }
  for (Int i = 0; i < 200; i += 4) {
    set2->set(i);
    set2->clear(i + 1);
  }
  Check(set->count() == 100, "bitset count");
  Check(set->union_with(set2)->count() == 150, "bitset union");
  Check(set->intersect_with(set2)->count() == 50, "bitset intersection");
  Check(set->diff_with(set2)->count() == 50, "bitset difference");
  Int sum = 0;
  for (BitSet::Each it(set); it; it++) {
    sum += *it;
  }
  Check(sum == 99 * 100, "bitset iteration");
  const Int n = 1000;
  BitMatrix *mat = MakeBitMatrix(n, n);
  for (Int i = 0; i < n; i++) {
    mat->at(i)->set(i);
  }
  BitMatrix *mat2 = Transpose(mat);
  CHECK_MAT(mat2, row == col, "bit matrix transposition");
  mat2 = TransitiveClosure(mat);
  CHECK_MAT(mat2, row == col, "transitive closure 1");
  mat = MakeBitMatrix(n, n);
  for (Int i = 1; i < n; i++) {
    mat->at(i - 1)->set(i);
  }
  mat->at(n - 1)->set(0);
  mat2 = TransitiveClosure(mat);
  CHECK_MAT(mat2, true, "transitive closure 2");
  mat = MakeBitMatrix(2, 2);
  mat->at(1)->set(0);
  mat->at(1)->set(0);
  mat->at(1)->set(1);
  mat2 = TransitiveClosure(mat);
  CHECK_MAT(mat2, mat->at(row)->test(col), "transitive closure 3");
}
