// Benchmark-only LD_PRELOAD observer. Not linked into Singular or its compiler.
// Timed runs must NOT load this file: map/string bookkeeping changes timings.
#include "Singular/libsingular.h"
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>

namespace {
typedef std::map<std::string, unsigned long long> Counts;
Counts *counts = nullptr;
void report() {
  const char *path = std::getenv("SINGULAR_BENCH_PROFILE");
  if (path == nullptr || counts == nullptr) return;
  FILE *file = std::fopen(path, "w");
  if (file == nullptr) return;
  for (const auto &entry : *counts)
    std::fprintf(file, "%llu\t%s\n", entry.second, entry.first.c_str());
  std::fclose(file);
}
}

BOOLEAN iiMake_proc(idhdl procedure, package scope, leftv arguments) {
  typedef BOOLEAN (*Entry)(idhdl, package, leftv);
  static Entry original = reinterpret_cast<Entry>(
    dlsym(RTLD_NEXT, "_Z11iiMake_procP5idrecP11sip_packageP6sleftv"));
  if (original == nullptr) {
    std::fprintf(stderr, "benchmark observer cannot resolve iiMake_proc\n");
    std::abort();
  }
  if (counts == nullptr) {
    counts = new Counts;
    std::atexit(report);
  }
  const procinfov info = IDPROC(procedure);
  const char *kind = info->language == LANG_SINGULAR ? "interpreted" :
                     info->language == LANG_C ? "native" : "other";
  const std::string key = std::string(kind) + "\t" +
    (info->libname == nullptr ? "" : info->libname) + "\t" +
    (info->procname == nullptr ? "" : info->procname) + "\t" +
    (info->is_static ? "private" : "public");
  ++(*counts)[key];
  return original(procedure, scope, arguments);
}
