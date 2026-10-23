#!/usr/bin/env python3
"""Measure individual compiled procedures on inputs exercised by a CI suite.

The ordinary native compiler benchmark deliberately measures whole tests.  This
companion benchmark instead loads exactly one compiled root and times only its
outermost invocations, using the real test which exercised that root most often.
Compilation, module loading, unrelated test work and recursive double-counting
are outside the reported procedure time.
"""

import argparse
from collections import Counter
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import signal
import statistics
import subprocess
import tempfile
import threading
import time


ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location(
    "native_compiler_benchmark", HERE/"native_compiler_benchmark.py")
benchmark = importlib.util.module_from_spec(spec)
spec.loader.exec_module(benchmark)
compiler = benchmark.compiler
FORMAT = "singular-native-procedure-benchmark-v1"


def read_profiles(evidence):
    """Return the newest record for every suite/test pair."""
    records = {}
    for path in sorted(evidence.glob("profiles-*.json"), key=lambda p: p.stat().st_mtime_ns):
        for record in json.loads(path.read_text()):
            records[record["suite"], record["key"]] = record
    return records


def discover(evidence, suite):
    """Find compiler-eligible procedures actually entered by the selected suite."""
    coverage = json.loads((evidence/"coverage.json").read_text())
    profiles = read_profiles(evidence)
    called, cases_by_name = Counter(), {}
    exercised = set()
    unsafe_specializations = set()
    for (record_suite, key), record in profiles.items():
        if record_suite != suite or record["timeout"]:
            continue
        per_case = Counter()
        for entry, count in record["calls"].items():
            kind, qualified, _ = entry.split("|", 2)
            if kind != "interpreted" or "::" not in qualified:
                continue
            library, procedure = qualified.split("::", 1)
            if library in ("", "tst.lib") or procedure == "mod_init":
                continue
            exercised.add(qualified)
            info = coverage.get(library, {}).get("procedures", {}).get(procedure)
            if not info or not info.get("native"):
                continue
            if info.get("parameters") == [["intargs", "args"]]:
                unsafe_specializations.add(qualified)
                continue
            per_case[qualified] += count
        for qualified, count in per_case.items():
            called[qualified] += count
            cases_by_name.setdefault(qualified, []).append({
                "key": key,
                "test": record["test"],
                "calls": count,
                "profile_elapsed": record["elapsed"],
            })
    candidates = []
    for qualified in sorted(called):
        library, procedure = qualified.split("::", 1)
        info = coverage[library]["procedures"][procedure]
        cases = sorted(cases_by_name[qualified], key=lambda value: (-value["calls"], value["key"]))
        candidates.append({
            "name": qualified,
            "library": library,
            "procedure": procedure,
            "private": info["private"],
            "parameters": info.get("parameters"),
            "result": info.get("result"),
            "library_path": coverage[library]["path"],
            "profile_calls": called[qualified],
            "cases": cases,
            "selected_case": cases[0],
        })
    denominator = {
        "exercised_library_procedures": len(exercised),
        "compiler_eligible_procedures": len(candidates),
        "unsafe_integer_specializations": sorted(unsafe_specializations),
        "compile_fallback_procedures": len(exercised)-len(candidates)-len(unsafe_specializations),
    }
    return candidates, denominator


def package_name(library):
    package = Path(library).stem
    package = package[0].upper()+package[1:]
    return '`"VecField"`' if package == "VecField" else package


def inject_target(source, candidate, compiled):
    """Load one strict module and replace only its requested source root."""
    prefix = compiled["prefix"]
    export = prefix+candidate["procedure"]
    package = package_name(candidate["library"])
    bindings = package+"::"+candidate["procedure"]+"="+export+";"
    if not candidate["private"]:
        bindings += ("if(defined("+candidate["procedure"]+")){"+
                     candidate["procedure"]+"="+export+";}")
    hook = ("if(defined("+package+")){if(!defined("+prefix+"compilerCalls)){LIB "+
            json.dumps(compiled["module"])+";}"+bindings+"}")
    tokens = [match for match in benchmark.LEX.finditer(source)
              if not match.group().startswith(("//", "/*"))]
    positions = []
    for index, match in enumerate(tokens):
        if match.group() != "LIB":
            continue
        for following in tokens[index+1:index+6]:
            if following.group() == ";":
                positions.append(following.end())
                break
    for position in reversed(positions):
        source = source[:position]+hook+source[position:]
    return source


def compile_target(args, candidate):
    prefix = "nb_"+hashlib.sha256(candidate["name"].encode()).hexdigest()[:12]+"_"
    command = [
        "python3", str(ROOT/"Singular/compiler/singular2cpp.py"), candidate["library_path"],
        "--singular-prefix", str(args.prefix), "--cache-dir", str(args.output/"native-cache"),
        "--export-prefix", prefix, "--strict", "--jobs", str(args.jobs),
        "--entry", candidate["procedure"],
    ]
    started = time.perf_counter()
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            encoding="utf8", timeout=max(300, args.timeout))
    if result.returncode:
        raise RuntimeError(result.stderr.strip() or "native compiler failed")
    compiled = json.loads(result.stdout)
    export = prefix+candidate["procedure"]
    if export not in compiled.get("native_exports", []):
        raise RuntimeError("requested procedure is absent from native exports")
    if compiled.get("aliases"):
        raise RuntimeError("strict target module unexpectedly contains aliases")
    return {"prefix": prefix, "module": compiled["module"], "manifest": compiled["manifest"],
            "stats": compiled["stats"], "seconds": time.perf_counter()-started}


def build_timer(args):
    target = args.output/"procedure-timer.so"
    environment = dict(os.environ, PKG_CONFIG_PATH=str(args.prefix/"lib/pkgconfig"))
    flags = shlex.split(compiler.run(["pkg-config", "--cflags", "Singular"], env=environment))
    compiler.run(["c++", "-std=c++11", "-O2", "-shared", "-fPIC"]+flags+
                 [str(HERE/"native_compiler_procedure_timer.cc"), "-ldl", "-o", str(target)],
                 env=environment)
    return target


def timer_result(path):
    try:
        lines = path.read_text().splitlines()
        if len(lines) != 1:
            return None
        calls, nanoseconds, kind, library, procedure = lines[0].split("\t")
        return {"calls": int(calls), "nanoseconds": int(nanoseconds), "kind": kind,
                "library": library, "procedure": procedure}
    except (OSError, ValueError):
        return None


def run_case(args, timer, candidate, path, directory, source):
    directory.mkdir(parents=True, exist_ok=True)
    output_path, timing_path = directory/"stdout.txt", directory/"timing.tsv"
    compiler.atomic(timing_path, "")
    environment = dict(os.environ)
    environment["SINGULARPATH"] = ":".join(map(str, [args.prefix/"share/singular/LIB", path.parent]))
    environment["LD_PRELOAD"] = str(timer)
    environment["SINGULAR_BENCH_TIMING_PROFILE"] = str(timing_path)
    environment["SINGULAR_BENCH_TIMED_LIBRARY"] = candidate["library"]
    environment["SINGULAR_BENCH_TIMED_PROCEDURE"] = candidate["procedure"]
    command = [str(args.prefix/"bin/Singular"), "--ticks-per-sec=100", "-tqsr12345678",
               "--no-rc", "-c", "int tst_no_status=1;", "run.tst"]
    work_root = args.output/"work"
    work_root.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="procedure-", dir=work_root) as temporary:
        work = Path(temporary)/path.parent.name
        shutil.copytree(path.parent, work)
        compiler.atomic(work/"run.tst", source)
        started = time.perf_counter()
        with output_path.open("wb") as output:
            process = subprocess.Popen(command, cwd=work, env=environment,
                                       stdin=subprocess.DEVNULL, stdout=output,
                                       stderr=subprocess.STDOUT, start_new_session=True)
            expired = threading.Event()

            def stop():
                expired.set()
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass

            watchdog = threading.Timer(args.timeout, stop)
            watchdog.start()
            process.wait()
            watchdog.cancel()
            watchdog.join()
    return {"elapsed": time.perf_counter()-started, "returncode": process.returncode,
            "timeout": expired.is_set(), "output": str(output_path),
            "target": timer_result(timing_path)}


def compare_pair(baseline, native, expected=None):
    reasons = []
    if baseline["timeout"] or native["timeout"]:
        reasons.append("timeout")
    if baseline["returncode"] != native["returncode"] or baseline["returncode"]:
        reasons.append("return code")
    if benchmark.normalize(baseline["output"]) != benchmark.normalize(native["output"]):
        reasons.append("output mismatch")
    before, after = baseline.get("target"), native.get("target")
    if before is None or after is None:
        reasons.append("missing target timing")
    else:
        if before["kind"] != "interpreted":
            reasons.append("baseline target is not interpreted")
        if after["kind"] != "native":
            reasons.append("compiled target is not native")
        observed = (before["library"], before["procedure"])
        if observed != (after["library"], after["procedure"]):
            reasons.append("target identity mismatch")
        if expected is not None and expected != observed:
            reasons.append("unexpected target identity")
        if before["calls"] != after["calls"] or before["calls"] == 0:
            reasons.append("outer call count mismatch")
        if after["nanoseconds"] <= 0:
            reasons.append("zero compiled duration")
    ratio = (before["nanoseconds"]/after["nanoseconds"]
             if not reasons and after["nanoseconds"] else None)
    return {"valid": not reasons, "reasons": reasons, "speedup": ratio}


def is_near_identical(speedup, tolerance):
    return speedup is not None and 1.0-tolerance <= speedup <= 1.0+tolerance


def summarize(receipt, minimum_nanoseconds):
    qualified, invalid, near, measured_twice = [], [], 0, 0
    for name, record in receipt["results"].items():
        pairs = record.get("pairs", [])
        comparisons = [pair["comparison"] for pair in pairs]
        if not comparisons or not all(item["valid"] for item in comparisons):
            invalid.append(name)
            continue
        if record.get("near_identical"):
            near += 1
        if len(pairs) == 2:
            measured_twice += 1
        before = sum(pair["baseline"]["target"]["nanoseconds"] for pair in pairs)
        after = sum(pair["native"]["target"]["nanoseconds"] for pair in pairs)
        speedup = before/after if after else None
        record["combined_speedup"] = speedup
        record["timed_interpreted_nanoseconds"] = before
        record["timed_native_nanoseconds"] = after
        if speedup is not None and speedup >= 2.0 and before >= minimum_nanoseconds:
            qualified.append({"name": name, "speedup": speedup,
                              "pair_speedups": [item["speedup"] for item in comparisons],
                              "calls": [pair["baseline"]["target"]["calls"] for pair in pairs],
                              "test": record["candidate"]["selected_case"]["key"],
                              "private": record["candidate"]["private"],
                              "interpreted_nanoseconds": before, "native_nanoseconds": after})
    qualified.sort(key=lambda value: (-value["speedup"], value["name"]))
    return {"candidate_procedures": len(receipt["candidates"]),
            "completed_procedures": len(receipt["results"]),
            "near_identical_after_one_pair": near, "measured_twice": measured_twice,
            "invalid": invalid, "at_least_2x": qualified}


def configuration(args, candidates):
    inputs = [args.evidence/"coverage.json"]+sorted(args.evidence.glob("profiles-*.json"))
    return {"format": FORMAT, "suite": args.suite, "prefix": str(args.prefix),
            "procedure_filter": args.procedure,
            "candidates": [candidate["name"] for candidate in candidates],
            "near_identical_tolerance": args.near_identical,
            "minimum_target_nanoseconds": int(args.minimum_target_ms*1_000_000),
            "timeout": args.timeout, "jobs": args.jobs,
            "implementation": {
                "benchmark": compiler.file_digest(Path(__file__)),
                "timer": compiler.file_digest(HERE/"native_compiler_procedure_timer.cc"),
                "compiler": compiler.file_digest(Path(compiler.__file__)),
                "singular": compiler.file_digest(args.prefix/"bin/Singular"),
            },
            "evidence": {str(path): compiler.file_digest(path) for path in inputs}}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", type=Path, required=True)
    parser.add_argument("--evidence", type=Path, required=True,
                        help="coverage/profile directory from native_compiler_benchmark.py")
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--suite", choices=("short", "new"), default="short")
    parser.add_argument("--procedure", help="optional regular expression selecting qualified names")
    parser.add_argument("--near-identical", type=float, default=0.10,
                        help="skip the second pair when the first ratio is within this fraction of 1")
    parser.add_argument("--minimum-target-ms", type=float, default=0.0,
                        help="minimum aggregate interpreted target time for the >=2x list")
    parser.add_argument("--timeout", type=int, default=600)
    parser.add_argument("--jobs", type=int, default=compiler.DEFAULT_JOBS)
    parser.add_argument("--retry-invalid", action="store_true",
                        help="rerun prior errors or invalid comparisons")
    args = parser.parse_args()
    args.prefix, args.evidence, args.output = (args.prefix.resolve(), args.evidence.resolve(),
                                               args.output.resolve())
    if not 0 <= args.near_identical < 1 or args.minimum_target_ms < 0 or args.jobs < 1:
        parser.error("invalid tolerance, timing floor, or job count")
    args.output.mkdir(parents=True, exist_ok=True)
    candidates, denominator = discover(args.evidence, args.suite)
    if args.procedure:
        candidates = [candidate for candidate in candidates
                      if re.search(args.procedure, candidate["name"])]
    config = configuration(args, candidates)
    receipt_path = args.output/("procedure-benchmark-"+args.suite+".json")
    if receipt_path.exists():
        receipt = json.loads(receipt_path.read_text())
        if receipt.get("configuration") != config:
            raise RuntimeError("existing procedure benchmark has a different configuration")
    else:
        receipt = {"format": FORMAT, "configuration": config, "denominator": denominator,
                   "candidates": candidates, "results": {}, "summary": {}}
        benchmark.save(receipt_path, receipt)
    timer = build_timer(args)
    for index, candidate in enumerate(candidates, 1):
        name = candidate["name"]
        if name in receipt["results"]:
            old = receipt["results"][name]
            comparisons = [pair.get("comparison", {}) for pair in old.get("pairs", [])]
            invalid = (old.get("error") or not comparisons or
                       not all(item.get("valid") for item in comparisons))
            if not invalid or not args.retry_invalid:
                continue
            del receipt["results"][name]
        slug = hashlib.sha256(name.encode()).hexdigest()[:12]
        record = {"candidate": candidate, "pairs": []}
        try:
            compiled = compile_target(args, candidate)
            record["compiled"] = compiled
            path = Path(candidate["selected_case"]["test"])
            source = path.read_text()
            native_source = inject_target(source, candidate, compiled)
            fixture = args.output/"fixtures"/slug
            compiler.atomic(fixture/"baseline.tst", source)
            compiler.atomic(fixture/"native.tst", native_source)
            first = {
                "baseline": run_case(args, timer, candidate, path,
                                     args.output/"runs"/slug/"first"/"baseline", source),
                "native": run_case(args, timer, candidate, path,
                                   args.output/"runs"/slug/"first"/"native", native_source),
            }
            expected = (candidate["library"], candidate["procedure"])
            first["comparison"] = compare_pair(first["baseline"], first["native"], expected)
            record["pairs"].append(first)
            if first["comparison"]["valid"] and is_near_identical(
                    first["comparison"]["speedup"], args.near_identical):
                record["near_identical"] = True
            else:
                second = {
                    "native": run_case(args, timer, candidate, path,
                                       args.output/"runs"/slug/"second"/"native", native_source),
                    "baseline": run_case(args, timer, candidate, path,
                                         args.output/"runs"/slug/"second"/"baseline", source),
                }
                second["comparison"] = compare_pair(
                    second["baseline"], second["native"], expected)
                record["pairs"].append(second)
        except (OSError, RuntimeError, subprocess.TimeoutExpired) as exc:
            record["error"] = str(exc)
        receipt["results"][name] = record
        receipt["summary"] = summarize(receipt, config["minimum_target_nanoseconds"])
        benchmark.save(receipt_path, receipt)
        ratios = [pair["comparison"]["speedup"] for pair in record["pairs"]
                  if pair["comparison"]["speedup"] is not None]
        print("PROCEDURE", index, "/", len(candidates), name,
              "ratios", [round(value, 3) for value in ratios],
              "pairs", len(record["pairs"]), "error", record.get("error", "none"), flush=True)
    receipt["summary"] = summarize(receipt, config["minimum_target_nanoseconds"])
    benchmark.save(receipt_path, receipt)
    print(json.dumps(receipt["summary"], indent=2), flush=True)


if __name__ == "__main__":
    main()
