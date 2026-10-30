#!/usr/bin/env python3
"""Unit tests for the per-procedure native compiler benchmark."""

import importlib.util
import json
from pathlib import Path
import tempfile
import unittest


HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location(
    "native_compiler_procedure_benchmark", HERE/"native_compiler_procedure_benchmark.py")
procedure_benchmark = importlib.util.module_from_spec(spec)
spec.loader.exec_module(procedure_benchmark)


class ProcedureBenchmarkTest(unittest.TestCase):
    def test_discover_reports_eligible_fallback_and_specialized_procedures(self):
        with tempfile.TemporaryDirectory() as directory:
            evidence = Path(directory)
            (evidence/"coverage.json").write_text(json.dumps({
                "foo.lib": {"path": "/prefix/foo.lib", "procedures": {
                    "fast": {"native": True, "private": False, "parameters": [["int", "n"]]},
                    "helper": {"native": True, "private": True, "parameters": []},
                    "special": {"native": True, "private": False,
                                "parameters": [["intargs", "args"]]},
                    "slow": {"native": False, "private": False},
                }},
            }))
            (evidence/"profiles-short.json").write_text(json.dumps([
                {"suite": "short", "key": "000_first", "test": "/tests/first.tst",
                 "elapsed": 2.0, "timeout": False, "calls": {
                     "interpreted|foo.lib::fast|public": 4,
                     "interpreted|foo.lib::helper|private": 2,
                     "interpreted|foo.lib::special|public": 3,
                     "interpreted|foo.lib::slow|public": 7,
                     "interpreted|tst.lib::local|public": 99,
                 }},
                {"suite": "short", "key": "001_second", "test": "/tests/second.tst",
                 "elapsed": 1.0, "timeout": False, "calls": {
                     "interpreted|foo.lib::fast|public": 8,
                 }},
            ]))
            candidates, denominator = procedure_benchmark.discover(evidence, "short")
            self.assertEqual([candidate["name"] for candidate in candidates],
                             ["foo.lib::fast", "foo.lib::helper"])
            self.assertEqual(candidates[0]["selected_case"]["key"], "001_second")
            self.assertEqual(candidates[0]["profile_calls"], 12)
            self.assertEqual(denominator["exercised_library_procedures"], 4)
            self.assertEqual(denominator["compiler_eligible_procedures"], 2)
            self.assertEqual(denominator["compile_fallback_procedures"], 1)
            self.assertEqual(denominator["unsafe_integer_specializations"],
                             ["foo.lib::special"])

    def test_inject_target_loads_and_rebinds_only_requested_root(self):
        source = '// LIB "ignored.lib";\nstring note="LIB fake;";\nLIB "foo.lib";\nfast(2);\n'
        candidate = {"library": "foo.lib", "procedure": "fast", "private": False}
        compiled = {"prefix": "nb_123abc_", "module": "/modules/fast.so"}
        treated = procedure_benchmark.inject_target(source, candidate, compiled)
        self.assertEqual(treated.count('LIB "/modules/fast.so"'), 1)
        self.assertIn("Foo::fast=nb_123abc_fast", treated)
        self.assertIn("if(defined(fast)){fast=nb_123abc_fast;}", treated)
        self.assertNotIn("ignored.lib\";if(defined(Foo)", treated)

        private = dict(candidate, procedure="helper", private=True)
        hidden = procedure_benchmark.inject_target(source, private, compiled)
        self.assertIn("Foo::helper=nb_123abc_helper", hidden)
        self.assertNotIn("defined(helper)", hidden)

    def test_pair_validation_near_band_and_summary(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            baseline_output, native_output = root/"baseline.txt", root/"native.txt"
            baseline_output.write_text("answer\n")
            native_output.write_text("answer\n")

            def run(kind, duration, output):
                return {"timeout": False, "returncode": 0, "output": str(output),
                        "target": {"calls": 5, "nanoseconds": duration, "kind": kind,
                                   "library": "foo.lib", "procedure": "fast"}}

            baseline = run("interpreted", 4_000_000, baseline_output)
            native = run("native", 1_000_000, native_output)
            comparison = procedure_benchmark.compare_pair(
                baseline, native, ("foo.lib", "fast"))
            self.assertTrue(comparison["valid"])
            self.assertEqual(comparison["speedup"], 4.0)
            mismatch = procedure_benchmark.compare_pair(
                baseline, native, ("foo.lib", "other"))
            self.assertFalse(mismatch["valid"])
            self.assertIn("unexpected target identity", mismatch["reasons"])
            self.assertTrue(procedure_benchmark.is_near_identical(1.05, 0.10))
            self.assertFalse(procedure_benchmark.is_near_identical(1.20, 0.10))

            receipt = {"candidates": [{"name": "foo.lib::fast"}], "results": {
                "foo.lib::fast": {"candidate": {
                    "selected_case": {"key": "000_case"}, "private": False},
                    "pairs": [{"baseline": baseline, "native": native,
                               "comparison": comparison}]},
            }}
            summary = procedure_benchmark.summarize(receipt, 1_000_000)
            self.assertEqual(summary["at_least_2x"][0]["name"], "foo.lib::fast")
            self.assertEqual(summary["at_least_2x"][0]["speedup"], 4.0)


if __name__ == "__main__":
    unittest.main()
