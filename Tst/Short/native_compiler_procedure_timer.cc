// Benchmark-only LD_PRELOAD timer for one library procedure.
//
// This observer is deliberately separate from native_compiler_profile.cc.  It
// does no map or string allocation on the call path and measures only the
// configured procedure.  Recursive invocations are included in the outermost
// duration, but are not counted or timed a second time.
#include "Singular/libsingular.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>

namespace {

using Clock = std::chrono::steady_clock;
using Entry = BOOLEAN (*)(idhdl, package, leftv);

const char *target_library = "";
const char *target_procedure = "";
const char *profile_path = "";
std::atomic<unsigned long long> outer_calls(0);
std::atomic<unsigned long long> elapsed_nanoseconds(0);
std::atomic<unsigned int> observed_kinds(0);
thread_local unsigned int target_depth = 0;

enum : unsigned int {
  KIND_INTERPRETED = 1,
  KIND_NATIVE = 2,
  KIND_OTHER = 4
};

const char *base_name(const char *path)
{
  if (path == nullptr) return "";
  const char *name = path;
  for (const char *cursor = path; *cursor != '\0'; ++cursor)
    if (*cursor == '/' || *cursor == '\\') name = cursor + 1;
  return name;
}

const char *copy_string(const char *value)
{
  if (value == nullptr) return "";
  const size_t size = std::strlen(value) + 1;
  char *copy = static_cast<char *>(std::malloc(size));
  if (copy == nullptr) std::abort();
  std::memcpy(copy, value, size);
  // Keep configuration alive for the atexit reporter.
  return copy;
}

procinfov interpreted_source(procinfov info)
{
  procinfov seen[32];
  unsigned int seen_count = 0;
  while (info != nullptr && info->language == LANG_C)
  {
    if (seen_count == sizeof(seen) / sizeof(seen[0])) return nullptr;
    for (unsigned int i = 0; i < seen_count; ++i)
      if (seen[i] == info) return nullptr;
    seen[seen_count++] = info;
    info = info->data.o.singular_source;
  }
  return (info != nullptr && info->language == LANG_SINGULAR) ? info : nullptr;
}

bool is_target(procinfov actual)
{
  procinfov source = interpreted_source(actual);
  return source != nullptr && source->libname != nullptr &&
         source->procname != nullptr &&
         std::strcmp(base_name(source->libname), target_library) == 0 &&
         std::strcmp(source->procname, target_procedure) == 0;
}

unsigned int kind(procinfov actual)
{
  if (actual == nullptr) return KIND_OTHER;
  if (actual->language == LANG_SINGULAR) return KIND_INTERPRETED;
  if (actual->language == LANG_C) return KIND_NATIVE;
  return KIND_OTHER;
}

const char *kind_name(unsigned int value)
{
  if (value == KIND_INTERPRETED) return "interpreted";
  if (value == KIND_NATIVE) return "native";
  if (value == KIND_OTHER || value == 0) return "other";
  return "mixed";
}

void report()
{
  if (*profile_path == '\0') return;
  FILE *file = std::fopen(profile_path, "w");
  if (file == nullptr) return;
  const unsigned long long calls = outer_calls.load(std::memory_order_relaxed);
  const unsigned long long nanoseconds =
      elapsed_nanoseconds.load(std::memory_order_relaxed);
  const unsigned int kinds = observed_kinds.load(std::memory_order_relaxed);
  std::fprintf(file, "%llu\t%llu\t%s\t%s\t%s\n", calls, nanoseconds,
               kind_name(kinds), target_library, target_procedure);
  std::fclose(file);
}

struct Configuration {
  Configuration()
  {
    const char *library = std::getenv("SINGULAR_BENCH_TIMED_LIBRARY");
    const char *procedure = std::getenv("SINGULAR_BENCH_TIMED_PROCEDURE");
    const char *profile = std::getenv("SINGULAR_BENCH_TIMING_PROFILE");
    if (library != nullptr) target_library = copy_string(base_name(library));
    if (procedure != nullptr) target_procedure = copy_string(procedure);
    if (profile != nullptr) profile_path = copy_string(profile);
    std::atexit(report);
  }
};

Configuration configuration;

} // namespace

BOOLEAN iiMake_proc(idhdl procedure, package scope, leftv arguments)
{
  static Entry original = reinterpret_cast<Entry>(
      dlsym(RTLD_NEXT, "_Z11iiMake_procP5idrecP11sip_packageP6sleftv"));
  if (original == nullptr)
  {
    std::fprintf(stderr, "procedure timer cannot resolve iiMake_proc\n");
    std::abort();
  }

  // The outer timer already includes recursive and nested calls.  Avoid doing
  // name/source-chain work in the interpreted body that native direct calls do
  // not perform.
  if (target_depth != 0) return original(procedure, scope, arguments);

  procinfov actual = IDPROC(procedure);
  if (*target_library == '\0' || *target_procedure == '\0' || !is_target(actual))
    return original(procedure, scope, arguments);

  observed_kinds.fetch_or(kind(actual), std::memory_order_relaxed);
  const bool outermost = target_depth++ == 0;
  Clock::time_point started;
  if (outermost) started = Clock::now();
  const BOOLEAN result = original(procedure, scope, arguments);
  --target_depth;
  if (outermost)
  {
    const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
        Clock::now() - started).count();
    outer_calls.fetch_add(1, std::memory_order_relaxed);
    elapsed_nanoseconds.fetch_add(
        static_cast<unsigned long long>(elapsed), std::memory_order_relaxed);
  }
  return result;
}
