/* This is a workaround around <stdint.h> not being available
 * everywhere.
 */
#include <stdio.h>
#include <stdlib.h>

void gen(const char *tname, const char *prefix, size_t size) {
  const char *ty;
  if (size == sizeof(long long)) ty = "long long";
  else if (size == sizeof(long)) ty = "long";
  else if (size == sizeof(int)) ty = "int";
  else if (size == sizeof(short)) ty = "short";
  else if (size == sizeof(char)) ty = "char";
  else exit(1);
  printf("typedef %s %s %s;\n", prefix, ty, tname);
}

int main() {
  printf("#pragma once\n\n");
  gen("Word", "unsigned", sizeof(void *));
  if (sizeof(long long) >= 8)
    gen("Word64", "unsigned", 8);
  gen("Word32", "unsigned", 4);
  gen("Word16", "unsigned", 2);
  printf("\n");
  gen("Int", "signed", sizeof(void *));
  if (sizeof(long long) >= 8)
    gen("Int64", "signed", 8);
  gen("Int32", "signed", 4);
  gen("Int16", "signed", 2);
  return 0;
}
