#!/usr/bin/env python3
"""Small source-transformation tests for native_compiler_benchmark.py."""

import importlib.util
import json
import os
from pathlib import Path
from types import SimpleNamespace
import tempfile
import unittest
from unittest import mock


HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location(
    "native_compiler_benchmark", HERE/"native_compiler_benchmark.py")
benchmark = importlib.util.module_from_spec(spec)
spec.loader.exec_module(benchmark)


class TreatmentTest(unittest.TestCase):
    def test_scan_cache_uses_versioned_namespace(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            output, prefix = root/"output", root/"prefix"
            library_dir = prefix/"share/singular/LIB"
            library_dir.mkdir(parents=True)
            output.mkdir()
            (library_dir/"tiny.lib").write_text("proc tiny(int n){return(n+1);}\n")
            result = benchmark.scan(SimpleNamespace(prefix=prefix, output=output))
            self.assertTrue(result["tiny.lib"]["procedures"]["tiny"]["native"])
            self.assertTrue((output/"scan-cache"/benchmark.compiler.CACHE_FORMAT/"index").is_dir())
            self.assertFalse((output/"scan-cache"/"index").exists())

    def test_build_uses_only_explicit_native_roots_in_strict_mode(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            (output/"coverage.json").write_text(json.dumps({
                "foo.lib": {
                    "path": "/libraries/foo.lib",
                    "procedures": {
                        "fast": {"native": True, "private": False},
                        "slow": {"native": False, "private": False},
                        "helper": {"native": True, "private": True},
                    },
                },
            }))
            profiles = {("short", "case"): {
                "calls": {"interpreted|foo.lib::fast|public": 1},
            }}
            commands = []

            def compiled(command, **kwargs):
                commands.append(command)
                return SimpleNamespace(returncode=0, stderr="", stdout=json.dumps({
                    "module": "/modules/native.so",
                    "manifest": "/modules/manifest.json",
                    "native_exports": ["native"],
                    "aliases": [],
                    "stats": {},
                }))

            args = SimpleNamespace(output=output, prefix=Path("/prefix"), library=None, jobs=7)
            with mock.patch.object(benchmark, "read_profiles", return_value=profiles), \
                    mock.patch.object(benchmark.subprocess, "run", side_effect=compiled):
                benchmark.build(args)

            self.assertEqual(len(commands), 4)  # cold/warm public and helper variants
            for command in commands:
                self.assertIn("--strict", command)
                self.assertNotIn("slow", command)
                self.assertEqual(command[command.index("--jobs")+1], "7")
            for command in commands[:2]:
                self.assertIn("fast", command)
                self.assertNotIn("helper", command)
            for command in commands[2:]:
                self.assertIn("helper", command)
                self.assertNotIn("fast", command)

    def test_only_native_exports_are_loaded_and_rebound(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            alias_manifest = output/"aliases.json"
            native_manifest = output/"native.json"
            alias_manifest.write_text(json.dumps({
                "module": "/modules/aliases-only.so",
                "exports": ["alias_slow"],
                "native_exports": [],
                "aliases": [],
            }))
            native_manifest.write_text(json.dumps({
                "module": "/modules/native.so",
                "exports": ["native_fast", "native_hidden", "native_special",
                            "native_foreign", "native_slow"],
                "native_exports": ["native_fast", "native_hidden", "native_special",
                                   "native_foreign"],
                "aliases": [],
            }))
            (output/"builds.json").write_text(json.dumps({
                "foo.lib": {"modules": [
                    {"kind": "public", "prefix": "alias_",
                     "warm": {"manifest": str(alias_manifest)}},
                    {"kind": "helpers", "prefix": "native_",
                     "warm": {"manifest": str(native_manifest)}},
                ]},
                "bar.lib": {"modules": [
                    {"kind": "public", "prefix": "alias_",
                     "warm": {"manifest": str(alias_manifest)}},
                ]},
            }))
            (output/"coverage.json").write_text(json.dumps({
                "foo.lib": {"procedures": {
                    "fast": {"private": False, "parameters": [["list", "value"]]},
                    "hidden": {"private": True, "parameters": []},
                    "special": {"private": False, "parameters": [["intargs", "args"]]},
                    "slow": {"private": False, "parameters": []},
                }},
                "bar.lib": {"procedures": {
                    "slow": {"private": False, "parameters": []},
                }},
            }))
            args = SimpleNamespace(output=output)

            source = 'LIB "foo.lib";\nfast();\n'
            treated = benchmark.treatment(args, source, {"foo.lib"})
            self.assertIn('LIB "/modules/native.so";', treated)
            self.assertIn("Foo::fast=native_fast;", treated)
            self.assertIn("if(defined(fast)){fast=native_fast;}", treated)
            self.assertIn("Foo::hidden=native_hidden;", treated)
            self.assertNotIn("if(defined(hidden))", treated)
            self.assertNotIn("aliases-only.so", treated)
            self.assertNotIn("native_special", treated)
            self.assertNotIn("native_foreign", treated)
            self.assertNotIn("native_slow", treated)

            alias_only = 'LIB "bar.lib";\nslow();\n'
            self.assertEqual(benchmark.treatment(args, alias_only, {"bar.lib"}), alias_only)

            legacy = json.loads(native_manifest.read_text())
            legacy["aliases"] = [{"procedure": "slow"}]
            native_manifest.write_text(json.dumps(legacy))
            with self.assertRaisesRegex(RuntimeError, "native-only build stage"):
                benchmark.treatment(args, source, {"foo.lib"})

    def test_synthetic_build_records_serial_parallel_and_warm_runs(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            calls = []

            def compiled(command, **kwargs):
                calls.append(command)
                cache = Path(command[command.index("--cache-dir")+1])
                prefix = command[command.index("--export-prefix")+1]
                module = cache/(prefix+"module.so")
                manifest = cache/(prefix+"manifest.json")
                module.parent.mkdir(parents=True, exist_ok=True)
                module.write_bytes(b"module")
                manifest.write_text("{}")
                return SimpleNamespace(returncode=0, stderr="", stdout=json.dumps({
                    "module": str(module), "manifest": str(manifest),
                    "native_exports": [], "aliases": [], "stats": {},
                }))

            args = SimpleNamespace(output=output, prefix=Path("/prefix"), jobs=4,
                                   timeout=5, synthetic_procedures=3)
            _, prime_source, _ = benchmark.synthetic_definition(args)
            prime = output/"prime.lib"
            prime.write_text(prime_source)
            procedures = benchmark.compiler.library_index(prime_source)["procedures"]
            self.assertEqual(list(procedures), ["syntheticPrime"])
            closure = benchmark.compiler.Closure(
                benchmark.compiler.cache_namespace(output/"analysis-cache"), [])
            benchmark.compiler.analyze_root(closure, str(prime), "syntheticPrime")
            with mock.patch.object(benchmark.subprocess, "run", side_effect=compiled):
                result = benchmark.synthetic_build(args)
            self.assertEqual(len(calls), 6)  # ABI prime + cold/warm for each width
            self.assertEqual([command[command.index("--jobs")+1] for command in calls],
                             ["1", "1", "1", "4", "4", "4"])
            self.assertEqual(result["configuration"]["procedures"], 3)
            self.assertTrue(benchmark.valid_synthetic_receipt(result, args))
            changed_prefix = SimpleNamespace(**vars(args))
            changed_prefix.prefix = Path("/different-prefix")
            self.assertFalse(benchmark.valid_synthetic_receipt(result, changed_prefix))

            original_digest = benchmark.compiler.file_digest
            runtime = benchmark.compiler.HERE/"native_runtime.h"
            with mock.patch.object(
                    benchmark.compiler, "file_digest",
                    side_effect=lambda path: ("changed" if Path(path)==runtime
                                              else original_digest(path))):
                self.assertFalse(benchmark.valid_synthetic_receipt(result, args))
            with mock.patch.dict(os.environ, {"CXX": "different-cxx"}):
                self.assertFalse(benchmark.valid_synthetic_receipt(result, args))
            with mock.patch.dict(os.environ,
                                 {"PATH": os.environ.get("PATH", "")+os.pathsep+"/changed"}):
                self.assertFalse(benchmark.valid_synthetic_receipt(result, args))
            alternate_pkg_config = output/"alternate-pkg-config"
            alternate_pkg_config.write_text("alternate")
            original_which = benchmark.shutil.which
            with mock.patch.object(
                    benchmark.shutil, "which",
                    side_effect=lambda command: (str(alternate_pkg_config)
                                                 if command=="pkg-config"
                                                 else original_which(command))):
                self.assertFalse(benchmark.valid_synthetic_receipt(result, args))
            cached_calls = []
            with mock.patch.object(benchmark.subprocess, "run", side_effect=cached_calls.append):
                self.assertEqual(benchmark.synthetic_build(args), result)
            self.assertEqual(cached_calls, [])

            broken = dict(result)
            broken["runs"] = {}
            self.assertFalse(benchmark.valid_synthetic_receipt(broken, args))


if __name__ == "__main__":
    unittest.main()
