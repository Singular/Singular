#!/usr/bin/env python3
"""Opt-in compiler coverage and paired benchmarks; never changes installed LIBs.

All generated files, profiles and compiler caches live under --output.
Profiling is a separate, untimed run using an optional LD_PRELOAD observer.
No installed interpreter/library files or normal dispatch hooks are modified.
"""

import argparse
from collections import Counter
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import shutil
import signal
import stat
import statistics
import subprocess
import threading
import time

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location("singular2cpp", ROOT/"Singular/compiler/singular2cpp.py")
compiler = importlib.util.module_from_spec(spec)
spec.loader.exec_module(compiler)
LEX = re.compile(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\[\s\S]|[^"\\])*"|[A-Za-z_][A-Za-z_0-9]*|[^\s]', re.S)


def save(path, value):
    compiler.save_json(path, value)


def scan(args):
    library_dir = args.prefix/"share/singular/LIB"
    closure = compiler.Closure(compiler.cache_namespace(args.output/"scan-cache"), [library_dir])
    libraries = {}
    for path in sorted(library_dir.glob("*.lib")):
        entry = {"path": str(path), "procedures": {}, "imports": []}
        libraries[path.name] = entry
        try:
            index = closure.library(str(path))
            entry["imports"] = index["imports"]
        except (compiler.CompileError, UnicodeError) as exc:
            entry["index_error"] = str(exc)
            continue
        for name, proc in index["procedures"].items():
            closure.nodes = {}
            result = {"private": proc["private"]}
            entry["procedures"][name] = result
            try:
                node = closure.visit(str(path), name)
                closure.generate()
                result.update(native=True, parameters=node["params"], result=node["result"])
            except (compiler.CompileError, RecursionError) as exc:
                result.update(native=False, reason=str(exc))
        print("SCAN", path.name, sum(p["native"] for p in entry["procedures"].values()),
              "/", len(entry["procedures"]), flush=True)
    save(args.output/"coverage.json", libraries)
    return libraries


def cases(args):
    result = []
    for suite in args.suite:
        if suite == "absfact-loop":
            fixture = args.output/"micro/absfact_loop.tst"
            compiler.atomic(fixture, '''LIB "absfact.lib";
proc benchProbe(poly p) { return(Absfact::variableWithSmallestPositiveDegree(p)); }
ring r=0,(x,y,z,t),dp;
poly p=(x+y+z+t)^8;
int total=0;
for (int k=0;k<20000;k++) { total=total+benchProbe(p); }
print(total);
$
''')
            paths = [fixture]
        elif suite == "ellipticcovers":
            paths = [ROOT/"Tst/Manual/ellipticcovers.tst"]
        else:
            listing = ROOT/"Tst"/("Short/ok_s.lst" if suite == "short" else "New.lst")
            paths = []
            for line in listing.read_text().splitlines():
                item = line.split(";", 1)[0].strip()
                if item:
                    paths.append(listing.parent/(item if item.endswith(".tst") else item+".tst"))
        for i, path in enumerate(paths):
            result.append((suite, f"{i:03d}_{path.stem}", path))
    return [r for r in result if not args.case or re.search(args.case, r[1])]


def run_case(args, path, directory, profile=False, prelude="", source=None):
    directory.mkdir(parents=True, exist_ok=True)
    # Copy test data into a private working directory. Some tests write fixed
    # filenames; no generated files should land in a source checkout.
    work = args.output/"work"/("profile" if profile else "timing")/path.parent.name
    if not work.exists():
        shutil.copytree(path.parent, work)
    if source is None:
        source = path.read_text()
    compiler.atomic(work/"run.tst", source)
    env = dict(os.environ)
    dirs = [args.prefix/"share/singular/LIB", path.parent]
    env["SINGULARPATH"] = ":".join(map(str, dirs))
    if profile:
        env["LD_PRELOAD"] = str(args.output/"profile.so")
        env["SINGULAR_BENCH_PROFILE"] = str(directory/"calls.tsv")
        compiler.atomic(directory/"calls.tsv", "")
    else:
        env.pop("SINGULAR_BENCH_PROFILE", None)
    command = [str(args.prefix/"bin/Singular"), "--ticks-per-sec=100", "-tqsr12345678", "--no-rc",
               "-c", "int tst_no_status=1;"+prelude, "run.tst"]
    start = time.perf_counter()
    with (directory/"stdout.txt").open("wb") as output:
        proc = subprocess.Popen(command, cwd=work, env=env, stdin=subprocess.DEVNULL,
                                stdout=output, stderr=subprocess.STDOUT, start_new_session=True)
        expired = threading.Event()
        def stop():
            expired.set()
            try:
                os.killpg(proc.pid, signal.SIGKILL)
            except ProcessLookupError:
                pass
        watchdog = threading.Timer(args.timeout, stop)
        watchdog.start()
        proc.wait()
        watchdog.cancel()
        timeout = expired.is_set()
    elapsed = time.perf_counter()-start
    counts = Counter()
    if profile and (directory/"calls.tsv").exists():
        # Anonymous procedure names can contain their multiline source header.
        pattern = r"^(\d+)\t(interpreted|native|other)\t([^\t]*)\t(.*?)\t(public|private)$"
        for count, kind, lib, name, visibility in re.findall(
                pattern, (directory/"calls.tsv").read_text(), re.M | re.S):
            counts[kind+"|"+Path(lib).name+"::"+name+"|"+visibility] += int(count)
    return {"elapsed": elapsed, "returncode": proc.returncode, "timeout": timeout,
            "calls": dict(counts), "output": str(directory/"stdout.txt")}


def profile(args):
    receipt = args.output/("profiles-"+"-".join(args.suite)+".json")
    results = json.loads(receipt.read_text()) if receipt.exists() else []
    completed = {(r["suite"], r["key"]) for r in results}
    for suite, key, path in cases(args):
        if (suite, key) in completed:
            continue
        result = run_case(args, path, args.output/"profiles"/suite/key, profile=True)
        result.update(suite=suite, key=key, test=str(path))
        results.append(result)
        save(receipt, results)
        print("PROFILE", suite, key, round(result["elapsed"], 3),
              "calls", sum(result["calls"].values()), "timeout", result["timeout"], flush=True)


def read_profiles(args):
    profiles = []
    for path in sorted(args.output.glob("profiles-*.json"), key=lambda p: p.stat().st_mtime_ns):
        profiles += json.loads(path.read_text())
    # Multiple invocations may select overlapping suites; latest file wins.
    return {(r["suite"], r["key"]): r for r in profiles}


def build(args):
    coverage = json.loads((args.output/"coverage.json").read_text())
    libraries = {key.split("|", 2)[1].split("::")[0]
                 for result in read_profiles(args).values() for key in result["calls"]
                 if key.startswith("interpreted|")}
    libraries -= {"", "tst.lib", "standard.lib"}
    receipt = args.output/"builds.json"
    results = json.loads(receipt.read_text()) if args.library and receipt.exists() else {}
    for library in sorted(libraries):
        if args.library and library not in args.library:
            continue
        info = coverage.get(library)
        if info is None or "index_error" in info:
            results[library] = {"error": "library indexing failed" if info else "not an installed library"}
            continue
        # Benchmark native code only. User-facing compileLib deliberately emits
        # aliases for unsupported public procedures, but merely loading such a
        # mixed module installs/copies those aliases and contaminates timings.
        # Select every supported public root explicitly and use strict mode.
        public = [name for name, p in info["procedures"].items()
                  if p["native"] and not p["private"]]
        private = [name for name, p in info["procedures"].items() if p["native"] and p["private"]]
        variants = []
        if public:
            variants.append(("public", public))
        if private:
            variants.append(("helpers", private))
        records = []
        for kind, roots in variants:
            prefix = "nb_"+hashlib.sha256((library+kind).encode()).hexdigest()[:10]+"_"
            command = ["python3", str(ROOT/"Singular/compiler/singular2cpp.py"), info["path"],
                       "--singular-prefix", str(args.prefix), "--cache-dir", str(args.output/"native-cache"),
                       "--export-prefix", prefix, "--strict", "--jobs", str(args.jobs)]
            for root in roots:
                command += ["--entry", root]
            record = {"kind": kind, "prefix": prefix}
            for phase in ("cold", "warm"):
                start = time.perf_counter()
                proc = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding="utf8")
                record[phase+"_seconds"] = time.perf_counter()-start
                if proc.returncode:
                    record["error"] = proc.stderr
                    break
                record[phase] = json.loads(proc.stdout)
            records.append(record)
        results[library] = {"modules": records}
        save(args.output/"builds.json", results)
        print("BUILD", library, [(r["kind"], round(r["cold_seconds"], 3), r.get("error", "ok"))
                                 for r in records], flush=True)
    save(args.output/"builds.json", results)


SYNTHETIC_FORMAT = "singular-native-synthetic-build-v1"


def synthetic_path_identity(path):
    """Stat-only executable identity: receipt checks must not run build probes."""
    logical = Path(os.path.abspath(os.fspath(path)))
    try:
        link_stat = os.lstat(logical)
    except OSError:
        return {"logical": str(logical), "kind": "missing"}
    resolved = Path(os.path.realpath(logical))
    result = {"logical": str(logical), "resolved": str(resolved),
              "link_target": os.readlink(logical) if stat.S_ISLNK(link_stat.st_mode) else None,
              "lstat": compiler._stat_fields(link_stat)}
    try:
        result["stat"] = compiler._stat_fields(os.stat(resolved))
        result["kind"] = "available"
    except OSError:
        result["kind"] = "broken"
    return result


def synthetic_tool_identity(command):
    found = shutil.which(command)
    return {"command": command,
            "file": synthetic_path_identity(found) if found is not None else None}


def synthetic_build_identity(args):
    cxx = os.environ.get("CXX", "c++")
    environment_names = ("CXX",)+compiler.BUILD_ENVIRONMENT_NAMES
    return {
        "implementation": {
            "singular2cpp.py": compiler.file_digest(Path(compiler.__file__)),
            "native_runtime.h": compiler.file_digest(compiler.HERE/"native_runtime.h"),
            "table.h": compiler.file_digest(compiler.TABLE),
        },
        "tools": {
            "python3": synthetic_tool_identity("python3"),
            "cxx": synthetic_tool_identity(cxx),
            "pkg-config": synthetic_tool_identity("pkg-config"),
            "ldd": synthetic_tool_identity("ldd"),
            "Singular": synthetic_path_identity(args.prefix/"bin/Singular"),
        },
        "environment": {name: os.environ.get(name) for name in environment_names},
    }


def synthetic_definition(args):
    bodies = ['info="parallel native compiler benchmark";']
    for index in range(args.synthetic_procedures):
        bodies.append(f'''proc independent{index:03d}(int n)
{{
  int total={index};
  for (int i=0;i<n;i++) {{ total=total+i+{index}; }}
  return(total);
}}''')
    source = "\n".join(bodies)+"\n"
    prime = 'proc syntheticPrime(int n) { return(n+1); }\n'
    configuration = {"prefix": str(args.prefix.resolve()), "translator": compiler.VERSION,
                     "cache_format": compiler.CACHE_FORMAT,
                     "procedures": args.synthetic_procedures, "parallel_jobs": args.jobs,
                     "source_digest": compiler.digest([source, prime]),
                     "build_identity": synthetic_build_identity(args)}
    return source, prime, configuration


def valid_synthetic_receipt(value, args):
    try:
        _, _, configuration = synthetic_definition(args)
        if value["format"] != SYNTHETIC_FORMAT or value["configuration"] != configuration:
            return False
        for mode in ("serial", "parallel"):
            run = value["runs"][mode]
            if (run["jobs"] != (1 if mode=="serial" else args.jobs) or
                    not isinstance(run["cold_seconds"], (int, float)) or
                    not isinstance(run["warm_seconds"], (int, float)) or
                    not isinstance(run["cache"], str)):
                return False
            for phase in ("cold", "warm"):
                result = run[phase]
                if (not isinstance(result["stats"], dict) or
                        not Path(result["manifest"]).is_file() or not Path(result["module"]).is_file()):
                    return False
        return True
    except (KeyError, TypeError):
        return False


def synthetic_build(args):
    """Time independent object builds at one worker and at the requested width."""
    source, prime_source, configuration = synthetic_definition(args)
    receipt = args.output/"synthetic-build.json"
    if receipt.exists():
        try:
            existing = json.loads(receipt.read_text())
        except (OSError, ValueError, UnicodeError):
            existing = None
        if existing is not None and valid_synthetic_receipt(existing, args):
            return existing
    fixture = args.output/"synthetic"/"independent.lib"
    prime = args.output/"synthetic"/"prime.lib"
    compiler.atomic(fixture, source)
    compiler.atomic(prime, prime_source)
    run_root = args.output/"synthetic"/("run-"+str(time.time_ns()))

    def invoke(library, cache, jobs, prefix):
        command = ["python3", str(ROOT/"Singular/compiler/singular2cpp.py"), str(library),
                   "--singular-prefix", str(args.prefix), "--cache-dir", str(cache),
                   "--export-prefix", prefix, "--strict", "--jobs", str(jobs)]
        started = time.perf_counter()
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                encoding="utf8", timeout=max(300, args.timeout))
        elapsed = time.perf_counter()-started
        if result.returncode:
            raise RuntimeError("synthetic native build failed:\n"+result.stderr)
        return elapsed, json.loads(result.stdout)

    runs = {}
    for mode, jobs in (("serial", 1), ("parallel", args.jobs)):
        cache = run_root/mode
        # Prime ABI discovery outside the timed region so this measures object
        # scheduling rather than pkg-config/compiler/ldd startup.
        invoke(prime, cache, jobs, "prime_")
        cold_seconds, cold = invoke(fixture, cache, jobs, "synthetic_")
        warm_seconds, warm = invoke(fixture, cache, jobs, "synthetic_")
        runs[mode] = {"jobs": jobs, "cache": str(cache),
                      "cold_seconds": cold_seconds, "warm_seconds": warm_seconds,
                      "cold": cold, "warm": warm}
    parallel_seconds = runs["parallel"]["cold_seconds"]
    value = {"format": SYNTHETIC_FORMAT, "configuration": configuration, "runs": runs,
             "cold_speedup": (runs["serial"]["cold_seconds"]/parallel_seconds
                               if parallel_seconds else None)}
    save(receipt, value)
    print("SYNTHETIC BUILD", args.synthetic_procedures, "procedures",
          "serial", round(runs["serial"]["cold_seconds"], 3),
          "parallel", round(parallel_seconds, 3), "jobs", args.jobs, flush=True)
    return value


def treatment(args, source, called):
    builds = json.loads((args.output/"builds.json").read_text())
    coverage = json.loads((args.output/"coverage.json").read_text())
    hooks = []
    for library in sorted(called):
        for record in builds.get(library, {}).get("modules", []):
            if "error" in record:
                continue
            manifest = json.loads(Path(record["warm"]["manifest"]).read_text())
            if manifest["aliases"]:
                raise RuntimeError("benchmark module contains interpreted aliases; rerun the native-only build stage")
            prefix = record["prefix"]
            package = Path(library).stem
            package = package[0].upper()+package[1:]
            # VecField is also a registered blackbox type. Its package must be
            # referenced by a quoted identifier, as in the correctness tests.
            if package == "VecField":
                package = '`"VecField"`'
            bindings = []
            # "exports" also contains warned interpreter aliases for unsupported
            # public procedures.  They are useful to the user-facing compiler
            # interface, but loading and rebinding them here would make the
            # performance treatment measure fallback alias overhead.  Benchmark
            # only genuine C++ entries; aliases remain reported separately.
            for exported in manifest["native_exports"]:
                name = exported[len(prefix):]
                proc = coverage[library]["procedures"].get(name)
                if proc is None:
                    continue  # a public helper from a different imported library
                if proc.get("parameters") == [["intargs", "args"]]:
                    continue  # narrowed integer specialization is not a generic substitute
                bindings.append(package+"::"+name+"="+exported+";")
                if not proc["private"]:
                    bindings.append("if(defined("+name+")){"+name+"="+exported+";}")
            # In particular, do not load modules which contain aliases only.
            # Explicitly selected static helpers still have native_exports and
            # are rebound in their package, but not into the public namespace.
            if not bindings:
                continue
            body = 'LIB '+json.dumps(manifest["module"])+";"+"".join(bindings)
            hooks.append("if(defined("+package+") && !defined("+prefix+"compilerCalls)){"+body+"}")
    if not hooks:
        return source
    hook = "".join(hooks)
    # Only actual LIB tokens, never text in examples/comments/strings. Keep the
    # number of source lines unchanged so interpreter diagnostic positions match.
    matches = [m for m in LEX.finditer(source) if not m.group().startswith(("//", "/*"))]
    positions = []
    for i, match in enumerate(matches):
        if match.group() != "LIB":
            continue
        for j in range(i+1, min(i+6, len(matches))):
            if matches[j].group() == ";":
                positions.append(matches[j].end())
                break
    for position in reversed(positions):
        source = source[:position]+hook+source[position:]
    return source


def normalize(path):
    lines = []
    for line in Path(path).read_text(errors="replace").splitlines():
        if re.search(r"used time:|tst_ignore|tst_status|\$Id:|error occurred in or before|init>>", line):
            continue
        if re.match(r".*\s+\d+[>.] ", line):
            continue  # explicit echo commands in individual tests
        if re.search(r"// \*\* nb_[a-f0-9]+_.* is an interpreted alias, not native C\+\+", line):
            continue  # expected load-time fallback warnings; retained in raw output
        if re.match(r"// \*\* redefining nb_[a-f0-9]+_", line):
            continue  # loader autoexports aliases already installed in basePack
        # In-process opt-in appends setup to the LIB line. Retain ordinary
        # warning identity/multiplicity but not its changed source-line excerpt.
        line = re.sub(r"^(// \*\* redefining \S+) \(.*\) run\.tst:\d+$", r"\1", line)
        lines.append(line)
    return "\n".join(lines)


def timings(args):
    profiles = read_profiles(args)
    receipt = args.output/("timings-"+"-".join(args.suite)+".json")
    results = json.loads(receipt.read_text()) if receipt.exists() else []
    if args.retry_failures:
        selected = {(s, k) for s, k, _ in cases(args)}
        retained = []
        for record in results:
            valid = (not record["profile"]["timeout"] and all(
                normalize(r["baseline"]["output"]) == normalize(r["native"]["output"]) and
                r["baseline"]["returncode"] == r["native"]["returncode"] and
                not r["baseline"]["timeout"] and not r["native"]["timeout"] for r in record["runs"]))
            if valid or (record["suite"], record["key"]) not in selected:
                retained.append(record)
                continue
            backup = args.output/"superseded"/(record["suite"]+"-"+record["key"]+"-"+str(time.time_ns()))
            save(backup/"record.json", record)
            for category in ("timings", "native-profiles"):
                source = args.output/category/record["suite"]/record["key"]
                if source.exists():
                    shutil.copytree(source, backup/category)
        results = retained
    completed = {(r["suite"], r["key"]) for r in results}
    for suite, key, path in cases(args):
        if (suite, key) in completed:
            continue
        prior = profiles[(suite, key)]
        if prior["timeout"]:
            print("SKIP profile timeout", suite, key, flush=True)
            continue
        called = {k.split("|", 2)[1].split("::")[0] for k in prior["calls"]}
        source = path.read_text()
        native = treatment(args, source, called)
        record = {"suite": suite, "key": key, "test": str(path), "changed": native != source, "runs": []}
        # Count actual treatment entries separately. This observes true aliases
        # and compiled C entrypoints, including privately compiled helpers.
        record["profile"] = run_case(args, path, args.output/"native-profiles"/suite/key,
                                     profile=True, source=native)
        for repetition in range(args.repetitions):
            pair = {}
            order = ("baseline", "native") if repetition % 2 == 0 else ("native", "baseline")
            for mode in order:
                pair[mode] = run_case(args, path, args.output/"timings"/suite/key/str(repetition)/mode,
                                      source=source if mode == "baseline" else native)
            pair["equal"] = (normalize(pair["baseline"]["output"]) == normalize(pair["native"]["output"]) and
                             pair["baseline"]["returncode"] == pair["native"]["returncode"] and
                             not pair["baseline"]["timeout"] and not pair["native"]["timeout"])
            record["runs"].append(pair)
        results.append(record)
        save(receipt, results)
        print("TIMING", suite, key, "changed", record["changed"], "equal", all(r["equal"] for r in record["runs"]),
              "baseline", round(statistics.median(r["baseline"]["elapsed"] for r in record["runs"]), 4),
              "native", round(statistics.median(r["native"]["elapsed"] for r in record["runs"]), 4), flush=True)


def report(args):
    coverage = json.loads((args.output/"coverage.json").read_text())
    builds = json.loads((args.output/"builds.json").read_text())
    records = {}
    for path in sorted(args.output.glob("timings-*.json")):
        for record in json.loads(path.read_text()):
            for pair in record["runs"]:
                pair["equal"] = (normalize(pair["baseline"]["output"]) == normalize(pair["native"]["output"]) and
                                 pair["baseline"]["returncode"] == pair["native"]["returncode"] and
                                 not pair["baseline"]["timeout"] and not pair["native"]["timeout"])
            records[record["suite"], record["key"]] = record
    result = {"definitions": {
        "fallback_calls": "Interpreted library entries left unchanged by the timed native treatment, including standard.lib, private callees and indexing-failed libraries; excludes tst.lib, mod_init and test-local procedures.",
        "fallback_aliases": "The benchmark builds explicit supported roots in strict mode, so timed modules contain no interpreted aliases. User-facing non-strict compilation still emits warned aliases.",
        "native_calls": "Generated C++ entries from compiled.so; internal native-to-native calls do not cross this boundary.",
        "timing": "Sum of per-test process wall times, with native module load/rebinding but no fallback-alias setup, compile/cache checks or profiler. Alternating paired order; fixed seed 12345678.",
        "scope": "CI Short/ok_s.lst (197), full New.lst (93), Manual/ellipticcovers.tst; static helpers are explicitly opted in as experimental replacements.",
    }, "suites": {}, "builds": {}}
    profiles = read_profiles(args)
    native_sources = {record["prefix"]: library for library, value in builds.items()
                      for record in value.get("modules", []) if "error" not in record}
    for suite in args.suite:
        selected = [r for (s, _), r in records.items() if s == suite]
        pairs = [r for r in selected if all(x["equal"] for x in r["runs"])]
        native_calls, fallback_calls, existing_c_calls = Counter(), Counter(), Counter()
        baseline = Counter()
        for (s, _), r in profiles.items():
            if s == suite:
                baseline.update(r["calls"])
        for r in pairs:
            for key, count in r["profile"]["calls"].items():
                kind, name, private = key.split("|", 2)
                library, proc = name.split("::", 1)
                if kind == "native" and library == "compiled.so":
                    source = next((lib+"::"+proc[len(prefix):] for prefix, lib in native_sources.items()
                                   if proc.startswith(prefix)), name)
                    native_calls[source] += count
                elif kind == "native":
                    existing_c_calls[name] += count
                elif kind == "interpreted" and library not in ("", "tst.lib") and proc != "mod_init":
                    fallback_calls[name] += count
        called = {k.split("|", 2)[1] for k in baseline if k.startswith("interpreted|")
                  and not k.startswith(("interpreted|::", "interpreted|tst.lib::")) and "::mod_init|" not in k}
        eligible = [name for name in called
                    if coverage.get(name.split("::")[0], {}).get("procedures", {}).get(name.split("::")[1], {}).get("native")]
        totals = {mode: [sum(r["runs"][i][mode]["elapsed"] for r in pairs)
                         for i in range(min([len(r["runs"]) for r in pairs], default=0))]
                  for mode in ("baseline", "native")}
        summary = {"measured": len(selected), "matching": len(pairs),
                   "mismatches": [r["key"] for r in selected if r not in pairs],
                   "profile_timeouts": [r["key"] for (s, _), r in profiles.items() if s == suite and r["timeout"]],
                   "treatment_profile_timeouts": [r["key"] for r in selected if r["profile"]["timeout"]],
                   "baseline_unique_library_procedures": len(called), "eligible_called_procedures": sorted(eligible),
                   "native_entry_calls": sum(native_calls.values()), "fallback_entry_calls": sum(fallback_calls.values()),
                   "existing_C_entry_calls": sum(existing_c_calls.values()),
                   "native_by_procedure": dict(native_calls), "fallback_by_procedure": dict(fallback_calls.most_common()),
                   "times": totals}
        if pairs:
            summary["baseline_median_seconds"] = statistics.median(totals["baseline"])
            summary["native_median_seconds"] = statistics.median(totals["native"])
            summary["speedup"] = summary["baseline_median_seconds"]/summary["native_median_seconds"]
        result["suites"][suite] = summary
    stats = {phase: Counter() for phase in ("cold", "warm")}
    elapsed = Counter()
    exports = Counter()
    errors = {}
    for library, lib in builds.items():
        if "error" in lib:
            errors[library] = coverage.get(library, {}).get("index_error", lib["error"])
        for record in lib.get("modules", []):
            if "error" in record:
                errors[library+":"+record["kind"]] = record["error"]
                continue
            for phase in ("cold", "warm"):
                stats[phase].update(record[phase]["stats"])
                elapsed[phase] += record[phase+"_seconds"]
            data = record["cold"]
            exports[record["kind"]+"_native"] += len(data["native_exports"])
            exports[record["kind"]+"_aliases"] += len(data["aliases"])
            exports["modules"] += 1
    result["builds"] = {"stats": stats, "seconds": elapsed, "exports": exports, "failures": errors}
    synthetic_path = args.output/"synthetic-build.json"
    if synthetic_path.exists():
        try:
            synthetic = json.loads(synthetic_path.read_text())
        except (OSError, ValueError, UnicodeError):
            synthetic = None
        if synthetic is not None:
            result["synthetic_build"] = synthetic
    load_path = args.output/"loading.json"
    if load_path.exists():
        result["loading"] = json.loads(load_path.read_text())
    save(args.output/"summary.json", result)
    print(json.dumps(result, indent=2))


def loading(args):
    """Measure load/setup separately, without running the mathematical workload."""
    profiles = read_profiles(args)
    results = {}
    for suite, library in (("ellipticcovers", "ellipticcovers.lib"), ("absfact-loop", "absfact.lib")):
        prior = next(r for (s, _), r in profiles.items() if s == suite)
        called = {k.split("|", 2)[1].split("::")[0] for k in prior["calls"]}
        source = 'LIB '+json.dumps(library)+';\n$\n'
        native = treatment(args, source, called)
        fixture = args.output/"load-fixtures"/suite/"load.tst"
        compiler.atomic(fixture, source)
        values = {"baseline": [], "native": []}
        for i in range(args.repetitions):
            order = ("baseline", "native") if i % 2 == 0 else ("native", "baseline")
            for mode in order:
                result = run_case(args, fixture, args.output/"load-runs"/suite/str(i)/mode,
                                  source=source if mode == "baseline" else native)
                if result["timeout"] or result["returncode"] or normalize(result["output"]):
                    raise RuntimeError("load-only check failed: "+result["output"])
                values[mode].append(result["elapsed"])
        results[suite] = {"times": values,
                          "baseline_median": statistics.median(values["baseline"]),
                          "native_median": statistics.median(values["native"])}
        results[suite]["incremental_load_setup_seconds"] = (
            results[suite]["native_median"]-results[suite]["baseline_median"])
        save(args.output/"loading.json", results)
        print("LOAD", suite, results[suite], flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--stage", choices=("scan", "observer", "profile", "build", "timings", "loading", "report"), required=True)
    parser.add_argument("--suite", action="append", choices=("short", "new", "ellipticcovers", "absfact-loop"))
    parser.add_argument("--case", help="optional regex selecting test keys")
    parser.add_argument("--library", action="append", help="rebuild only this library, preserving other build records")
    parser.add_argument("--timeout", type=int, default=180)
    parser.add_argument("--repetitions", type=int, default=3)
    parser.add_argument("--jobs", type=int, default=compiler.DEFAULT_JOBS,
                        help="parallel jobs used by native compiler builds")
    parser.add_argument("--synthetic-procedures", type=int, default=48,
                        help="number of independent procedures in the build benchmark")
    parser.add_argument("--retry-failures", action="store_true", help="retry selected failed pairs, preserving their previous evidence")
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    args.prefix = args.prefix.resolve()
    args.output = args.output.resolve()
    args.suite = args.suite or ["short", "new", "ellipticcovers"]
    if args.jobs < 1 or args.synthetic_procedures < 1:
        parser.error("--jobs and --synthetic-procedures must be at least 1")
    if args.stage == "scan":
        scan(args)
    elif args.stage == "observer":
        env = dict(os.environ, PKG_CONFIG_PATH=str(args.prefix/"lib/pkgconfig"))
        import shlex
        flags = shlex.split(compiler.run(["pkg-config", "--cflags", "Singular"], env=env))
        compiler.run(["c++", "-std=c++11", "-O2", "-shared", "-fPIC"]+flags+
                     [str(Path(__file__).with_name("native_compiler_profile.cc")),
                      "-ldl", "-o", str(args.output/"profile.so")])
    elif args.stage == "profile":
        profile(args)
    elif args.stage == "build":
        build(args)
        synthetic_build(args)
    elif args.stage == "timings":
        timings(args)
    elif args.stage == "loading":
        loading(args)
    else:
        report(args)


if __name__ == "__main__":
    main()
