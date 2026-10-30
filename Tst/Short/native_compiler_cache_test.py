#!/usr/bin/env python3
"""Focused unit tests for native compiler cache/build coordination."""

import importlib.util
import json
import os
from pathlib import Path
import tempfile
import threading
import time
import unittest
from unittest import mock


ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location(
    "singular2cpp_cache_test", ROOT/"Singular/compiler/singular2cpp.py")
compiler = importlib.util.module_from_spec(spec)
spec.loader.exec_module(compiler)


class CacheTest(unittest.TestCase):
    def test_subprocesses_get_deterministic_source_date_epoch(self):
        completed = mock.Mock(returncode=0, stdout=b"", stderr=b"")
        with mock.patch.dict(os.environ, {}, clear=True), \
                mock.patch.object(compiler.subprocess, "run", return_value=completed) as run:
            compiler.run(["tool"])
            self.assertEqual(run.call_args.kwargs["env"]["SOURCE_DATE_EPOCH"], "0")
        with mock.patch.dict(os.environ, {"SOURCE_DATE_EPOCH": "1700000000"}, clear=True), \
                mock.patch.object(compiler.subprocess, "run", return_value=completed) as run:
            compiler.run(["tool"])
            self.assertEqual(run.call_args.kwargs["env"]["SOURCE_DATE_EPOCH"], "1700000000")

    def test_ldd_paths_and_loader_search_candidates_are_lossless(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            target = root/"tool"
            wrapper = root/"wrapper"
            script = root/"script"
            selected = root/"late directory/libTool.so"
            missing = root/"early directory/libTool.so"
            target.write_text("tool")
            wrapper.write_bytes(b"\x7fELF\x02\x01"+b"\0"*10+b"\x02\x00")
            script.write_text("#!/bin/sh\nexit 0\n")
            self.assertEqual(compiler._elf_dynamic_inputs([script, wrapper]), [wrapper])
            selected.parent.mkdir()
            selected.write_text("selected")
            output = f"libTool.so => {selected} (0x1234)\n"
            self.assertEqual(compiler._ldd_inputs(output), [str(selected)])
            relative = root/"relative/libRelative.so"
            relative.parent.mkdir()
            relative.write_text("relative selected")
            self.assertEqual(
                compiler._ldd_inputs(
                    "libRelative.so => ./relative/libRelative.so (0x1234)\n", root),
                [str(relative)])
            with self.assertRaisesRegex(compiler.CompileError, "unresolved"):
                compiler._ldd_inputs("libMissing.so => not found\n")

            trace = (f"  1: trying file={missing}\n"
                     f"  1: trying file={selected}\n")
            with mock.patch.dict(os.environ, {"LD_DEBUG_OUTPUT": str(root/"redirect")}), \
                    mock.patch.object(compiler, "run", return_value=trace) as run:
                candidates = compiler._loader_search_candidates("ldd", [target])
            self.assertEqual(candidates, [str(missing), str(selected)])
            self.assertTrue(run.call_args.kwargs["capture_stderr"])
            self.assertEqual(run.call_args.kwargs["env"]["LD_DEBUG"], "libs")
            self.assertNotIn("LD_DEBUG_OUTPUT", run.call_args.kwargs["env"])
            with mock.patch.object(
                    compiler, "run", return_value="  1: trying file=./relative/libRelative.so\n"):
                self.assertEqual(
                    compiler._loader_search_candidates("ldd", [target], root),
                    [str(relative)])
            records = [compiler.candidate_file_record(path) for path in candidates]
            self.assertTrue(all(compiler.validate_candidate_file_record(record)
                                for record in records))
            request = {"format": compiler.ABI_SNAPSHOT_FORMAT,
                       "request": "loader-candidate"}
            snapshot = compiler.seal_abi_snapshot({
                "format": compiler.ABI_SNAPSHOT_FORMAT, "request": request,
                "files": [], "directories": [], "include_directories": [],
                "include_candidates": [], "program_directories": [],
                "program_candidates": [], "spec_candidates": [],
                "link_candidates": [],
                "loader_candidates": {"compile": [records[1]],
                                      "link": [], "runtime": []},
                "module_runtime_directories": [],
                "link_directories": [], "runtime_directories": [],
                "file_sets": {"compile": [], "link": [], "runtime": []},
                "compiler": "test", "compile_options": [], "link_options": [],
                "libraries": [], "compiler_version": "test",
                "identities": {"compile": "c", "link": "l", "runtime": "r"},
            })
            self.assertTrue(compiler.validate_abi_snapshot(snapshot, request))
            selected.write_text("accepted")  # same kind and length; may become loadable
            # Some Linux filesystems coalesce immediate same-size writes into
            # one timestamp tick.  The warm validator is intentionally
            # stat-only, so make the state transition explicit instead of
            # relying on host timestamp granularity.
            state = selected.stat()
            os.utime(selected, ns=(state.st_atime_ns,
                                   records[1]["lstat"]["st_mtime_ns"]+1_000_000_000))
            self.assertFalse(compiler.validate_abi_snapshot(snapshot, request))
            self.assertIsNone(compiler.refresh_abi_snapshot(snapshot, request))
            missing.parent.mkdir()
            missing.write_text("new earlier dependency")
            self.assertFalse(all(compiler.validate_candidate_file_record(record)
                                 for record in records))

    def test_response_files_are_recursive_and_cwd_relative(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            outer, inner, at_literal = root/"outer.rsp", root/"inner.rsp", root/"@literal"
            outer.write_text("@inner.rsp -O3 @@literal\n")
            inner.write_text("-B 'tool directory' -fuse-ld=gold\n")
            at_literal.write_text("-flto\n")
            expanded, files = compiler._expand_response_files(
                ["@"+str(outer), "-Wall"], root)
            self.assertEqual(expanded,
                             ["-B", "tool directory", "-fuse-ld=gold",
                              "-O3", "-flto", "-Wall"])
            self.assertEqual(files, [str(outer), str(inner), str(at_literal)])

            inner.write_text("@outer.rsp\n")
            with self.assertRaisesRegex(compiler.CompileError, "recursive"):
                compiler._expand_response_files(["@"+str(outer)], root)
            inner.write_text("'unterminated\n")
            with self.assertRaisesRegex(compiler.CompileError, "cannot parse"):
                compiler._expand_response_files(["@"+str(inner)], root)
            inner.write_text("-O2\n")
            with self.assertRaisesRegex(compiler.CompileError, "nesting"):
                compiler._expand_response_files(["@"+str(outer)], root, max_depth=0)

            assembler_inner = root/"assembler-inner.rsp"
            assembler_outer = root/"assembler-outer.rsp"
            assembler_inner.write_text("--compress-debug-sections\n")
            assembler_outer.write_text("@assembler-inner.rsp\n")
            trace = f' "as" "@{assembler_outer}"\n'
            self.assertEqual(
                compiler._compiler_trace_input_files(trace, root),
                [str(assembler_outer), str(assembler_inner)])

    def test_link_trace_uses_only_linker_commands(self):
        trace = ("COLLECT_GCC_OPTIONS='-shared'\n"
                 ' "/tools/cc1plus" "-quiet" "/dev/null"\n'
                 ' "/tools/as" "-o" "/tmp/output.o"\n'
                 ' "/tool directory/collect2" "-plugin" "/plugins/lto plugin.so" '
                 '"/startup/crtbeginS.o" "-l" "Choice"\n')
        self.assertEqual(
            compiler._compiler_link_trace_tokens(trace, ["collect2", "ld.gold"]),
            ["/tool directory/collect2", "-plugin", "/plugins/lto plugin.so",
             "/startup/crtbeginS.o", "-l", "Choice"])
        with self.assertRaisesRegex(compiler.CompileError, "link command"):
            compiler._compiler_link_trace_tokens(
                ' "/tools/cc1plus" "-quiet" "/dev/null"\n', ["collect2"])

    def test_dynamic_or_quoted_gcc_spec_includes_are_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            search = root/"startfiles"
            search.mkdir()
            explicit = root/"explicit.spec"
            for directive in ("%:include(%b.spec)\n",
                              '%:include("quoted.spec")\n'):
                with self.subTest(directive=directive):
                    explicit.write_text("*link:\n"+directive)
                    with mock.patch.dict(os.environ,
                                         {"GCC_SPECS": "", "GCC_EXEC_PREFIX": ""}):
                        with self.assertRaisesRegex(
                                compiler.CompileError, "GCC %:include"):
                            compiler._compiler_spec_files(
                                "compiler", [], set(),
                                ["-specs="+str(explicit)], [search], None,
                                "target", "13", root, [])
            for directive in ("%:if-exists(toggle.file)",
                              "%:if-exists-else(a.file b.file)",
                              "%:find-file(libChoice.a)",
                              "%:getenv(HOME suffix)"):
                with self.subTest(directive=directive):
                    explicit.write_text("*link:\n"+directive+"\n")
                    with mock.patch.dict(os.environ,
                                         {"GCC_SPECS": "", "GCC_EXEC_PREFIX": ""}):
                        with self.assertRaisesRegex(
                                compiler.CompileError, "spec functions"):
                            compiler._compiler_spec_files(
                                "compiler", [], set(),
                                ["-specs="+str(explicit)], [search], None,
                                "target", "13", root, [])

    def test_default_gcc_specs_includes_are_scanned(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            search = root/"startfiles"
            search.mkdir()
            install = root/"toolchain/host/13"
            install.mkdir(parents=True)
            child = search/"child.spec"
            child.write_text("*link:\nchild\n")

            primary = search/"specs"
            primary.write_text("*link:\n%:include(child.spec)\n")
            with mock.patch.dict(os.environ,
                                 {"GCC_SPECS": "", "GCC_EXEC_PREFIX": ""}):
                files = compiler._compiler_spec_files(
                    "compiler", [], set(), [], [search], install,
                    "target", "13", root, [str(primary)])
                self.assertTrue({str(primary.absolute()), str(child.absolute())}
                                <= set(files))
                primary.write_text("*link:\n%:include(%b.spec)\n")
                with self.assertRaisesRegex(compiler.CompileError,
                                            "GCC %:include"):
                    compiler._compiler_spec_files(
                        "compiler", [], set(), [], [search], install,
                        "target", "13", root, [str(primary)])

                primary.write_text("*link:\nprimary\n")
                secondary = install/"target/specs"
                secondary.parent.mkdir(parents=True)
                secondary.write_text("*link:\n%:include(child.spec)\n")
                files = compiler._compiler_spec_files(
                    "compiler", [], set(), [], [search], install,
                    "target", "13", root, [str(primary), str(secondary)])
                self.assertTrue({str(secondary.absolute()), str(child.absolute())}
                                <= set(files))
                secondary.write_text('*link:\n%:include("child.spec")\n')
                with self.assertRaisesRegex(compiler.CompileError,
                                            "GCC %:include"):
                    compiler._compiler_spec_files(
                        "compiler", [], set(), [], [search], install,
                        "target", "13", root, [str(primary), str(secondary)])

    def test_double_dash_specs_records_earlier_shadow_candidates(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            first, second = root/"first", root/"second"
            first.mkdir()
            second.mkdir()
            selected = second/"extra.spec"
            selected.write_text("*link:\nselected\n")
            with mock.patch.dict(os.environ,
                                 {"GCC_SPECS": "", "GCC_EXEC_PREFIX": ""}):
                for options in (["--specs", "extra.spec"],
                                ["--specs=extra.spec"],
                                ["-specs", "extra.spec"],
                                ["-specs=extra.spec"]):
                    with self.subTest(options=options):
                        candidates = set()
                        files = compiler._compiler_spec_files(
                            "compiler", [], candidates, options, [first, second],
                            None, "target", "13", root, [str(selected)])
                        self.assertIn(str(selected.absolute()), files)
                        self.assertIn(first/"extra.spec", candidates)
            record = compiler.candidate_file_record(first/"extra.spec")
            self.assertTrue(compiler.validate_candidate_file_record(record))
            (first/"extra.spec").write_text("*link:\nshadow\n")
            self.assertFalse(compiler.validate_candidate_file_record(record))

    def test_missing_search_directories_validate_unchanged(self):
        with tempfile.TemporaryDirectory() as directory:
            missing = Path(directory)/"missing"
            self.assertTrue(compiler.validate_directory_record(
                compiler.directory_record(missing)))
            self.assertTrue(compiler.validate_link_directory_record(
                compiler.link_directory_record(missing)))
            self.assertTrue(compiler.validate_namespace_directory_record(
                compiler.namespace_directory_record(missing)))

    def test_file_and_artifact_records_detect_links_and_corruption(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            first, second, link = root/"first", root/"second", root/"link"
            first.write_bytes(b"first")
            second.write_bytes(b"second")
            link.symlink_to(first)
            record = compiler.file_record(link)
            with mock.patch.object(compiler, "file_digest", side_effect=AssertionError("unexpected hash")):
                self.assertTrue(compiler.validate_file_record(record))
            link.unlink()
            link.symlink_to(second)
            self.assertFalse(compiler.validate_file_record(record))

            artifact = root/"object.o"
            artifact.write_bytes(b"abcd")
            compiler.publish_artifact(artifact)
            self.assertTrue(compiler.valid_artifact(artifact))
            receipt = compiler._artifact_receipt(artifact)
            receipt.write_text("[]\n")
            self.assertFalse(compiler.valid_artifact(artifact))
            compiler.publish_artifact(artifact)
            receipt.write_text(json.dumps({"format": compiler.ARTIFACT_RECEIPT_FORMAT,
                                           "artifact": []}))
            self.assertFalse(compiler.valid_artifact(artifact))
            compiler.publish_artifact(artifact)
            os.utime(artifact, None)
            self.assertTrue(compiler.valid_artifact(artifact))  # refresh identical bytes
            artifact.write_bytes(b"wxyz")  # same length, different content
            self.assertFalse(compiler.valid_artifact(artifact))
            artifact.unlink()
            artifact.symlink_to(first)
            self.assertFalse(compiler.valid_artifact(artifact))

    def test_module_receipt_tracks_runtime_dependencies_and_loader_candidates(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            module = root/"module.so"
            dependency = root/"default/libChoice.so.1"
            earlier = root/"early/libChoice.so.1"
            module.write_bytes(b"module")
            dependency.parent.mkdir()
            dependency.write_bytes(b"dependency-A")
            compiler.publish_artifact(
                module, "module", [dependency], [earlier, dependency])
            self.assertTrue(compiler.valid_artifact(module, "module"))

            os.utime(dependency, None)
            self.assertTrue(compiler.valid_artifact(module, "module"))
            dependency.write_bytes(b"dependency-B")
            self.assertFalse(compiler.valid_artifact(module, "module"))

            dependency.write_bytes(b"dependency-A")
            compiler.publish_artifact(
                module, "module", [dependency], [earlier, dependency])
            earlier.parent.mkdir()
            earlier.write_bytes(b"new earlier choice")
            self.assertFalse(compiler.valid_artifact(module, "module"))
            self.assertFalse(compiler.valid_artifact(module))

    def test_link_module_probes_before_publish_and_reuses_without_subprocesses(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            cache = root/"cache"
            target = root/"modules/native.so"
            dependency = root/"system/libChoice.so.1"
            missing = root/"early/libChoice.so.1"
            dependency.parent.mkdir()
            dependency.write_bytes(b"dependency")

            def fake_link(command, **kwargs):
                Path(command[command.index("-o")+1]).write_bytes(b"linked module")
                return ""

            with mock.patch.object(compiler, "run", side_effect=fake_link), \
                    mock.patch.object(compiler, "_dynamic_dependencies",
                                      return_value=[str(dependency)]) as dependencies, \
                    mock.patch.object(compiler, "_loader_search_candidates",
                                      return_value=[str(missing), str(dependency)]) as candidates:
                outcome = compiler.link_module(
                    cache, "key", target, "c++", [], [], [], "ldd", root)
            self.assertEqual(outcome, "linked")
            self.assertEqual(target.read_bytes(), b"linked module")
            dependencies.assert_called_once()
            candidates.assert_called_once()

            with mock.patch.object(compiler, "run", side_effect=AssertionError("warm subprocess")), \
                    mock.patch.object(compiler, "_dynamic_dependencies",
                                      side_effect=AssertionError("warm dependency probe")), \
                    mock.patch.object(compiler, "_loader_search_candidates",
                                      side_effect=AssertionError("warm loader probe")):
                self.assertEqual(
                    compiler.link_module(
                        cache, "key", target, "c++", [], [], [], "ldd", root),
                    "reused")

            failed_target = root/"modules/failing.so"
            failed_target.write_bytes(b"previous good module")
            with mock.patch.object(compiler, "run", side_effect=fake_link), \
                    mock.patch.object(compiler, "_dynamic_dependencies",
                                      side_effect=compiler.CompileError("probe failed")):
                with self.assertRaisesRegex(compiler.CompileError, "probe failed"):
                    compiler.link_module(
                        cache, "failed", failed_target, "c++", [], [], [], "ldd", root)
            self.assertEqual(failed_target.read_bytes(), b"previous good module")
            self.assertEqual(list(failed_target.parent.glob(".*.tmp")), [])

    def test_warm_abi_snapshot_has_no_probe_and_refreshes_metadata(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            tracked = root/"tool"
            tracked.write_bytes(b"tool")
            request = {"format": compiler.ABI_SNAPSHOT_FORMAT, "request": "test"}
            snapshot = compiler.seal_abi_snapshot({
                "format": compiler.ABI_SNAPSHOT_FORMAT,
                "request": request,
                "files": [compiler.file_record(tracked)],
                "directories": [], "include_directories": [], "include_candidates": [],
                "program_directories": [],
                "program_candidates": [], "spec_candidates": [], "link_candidates": [],
                "loader_candidates": {"compile": [], "link": [], "runtime": []},
                "module_runtime_directories": [],
                "link_directories": [], "runtime_directories": [],
                "file_sets": {"compile": [], "link": [], "runtime": []},
                "compiler": str(tracked), "compile_options": [], "link_options": [],
                "libraries": [], "compiler_version": "test",
                "identities": {"compile": "c", "link": "l", "runtime": "r"},
            })
            stats = compiler.empty_stats()
            cache = compiler.cache_namespace(root/"cache")
            with mock.patch.object(compiler, "configuration_request", return_value=request), \
                    mock.patch.object(compiler, "probe_build_configuration", return_value=snapshot) as probe:
                compiler.load_build_configuration(cache, root, "c++", [], stats)
                compiler.load_build_configuration(cache, root, "c++", [], stats)
                self.assertEqual(probe.call_count, 1)
                os.utime(tracked, None)
                compiler.load_build_configuration(cache, root, "c++", [], stats)
                self.assertEqual(probe.call_count, 1)
            self.assertEqual(stats["abi_snapshots_created"], 1)
            self.assertEqual(stats["abi_snapshots_reused"], 2)
            tampered = dict(snapshot)
            tampered["compile_options"] = ["-funsafe-change"]
            self.assertFalse(compiler.validate_abi_snapshot(tampered, request))

    def test_compile_link_and_runtime_identities_are_independent(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            prefix = root/"prefix"
            (prefix/"bin").mkdir(parents=True)
            (prefix/"lib").mkdir()
            paths = {name: root/name for name in ("compiler", "pkg-config", "ldd", "header", "runtime")}
            for name, path in paths.items():
                path.write_text(name)
            singular = prefix/"bin/Singular"
            singular.write_text("singular")
            link_library = prefix/"lib/libSingular.so"
            link_library.write_text("link")
            loader_cache = root/"ld.so.cache"
            loader_preload = root/"ld.so.preload"
            loader_cache.write_text("cache\n")
            loader_preload.write_text("A.so\nB.so\n")
            request = {"format": compiler.ABI_SNAPSHOT_FORMAT, "prefix": str(prefix),
                       "compiler": str(paths["compiler"]), "pkg_config": str(paths["pkg-config"]),
                       "ldd": str(paths["ldd"]), "flags": [],
                       "environment": {"GLIBC_TUNABLES": "example.a=1:example.b=2"}}

            def output(command, **kwargs):
                if ("-E" in command or "-M" in command or
                        "-###" in command and "-shared" not in command):
                    include = command.index("-I")
                    self.assertEqual(command[include+1], str(compiler.HERE))
                if "--cflags" in command:
                    return ""
                if "--libs" in command:
                    return f"-L{prefix/'lib'} -lSingular\n"
                if "--variable=pc_path" in command:
                    return ""
                if "--variable=pcfiledir" in command:
                    return str(root)+"\n"
                if "--print-requires" in command:
                    return ""
                if "-E" in command:
                    self.assertEqual(kwargs["env"]["LC_ALL"], "C")
                    self.assertEqual(kwargs["env"]["LANG"], "C")
                    return ("#include <...> search starts here:\n "+str(root)+
                            "\nEnd of search list.\n")
                if "-M" in command:
                    return f"runtime.o: {paths['header']}\n"
                if "--version" in command:
                    return "fake compiler\n"
                if "-print-search-dirs" in command:
                    self.assertEqual(kwargs["env"]["LC_ALL"], "C")
                    self.assertEqual(kwargs["env"]["LANG"], "C")
                    return (f"install: {root/'toolchain/target/1'}\n"
                            f"programs: ={root}\nlibraries: ={prefix/'lib'}\n")
                if "-dumpmachine" in command:
                    return "target\n"
                if "-dumpversion" in command:
                    return "1\n"
                if "-###" in command:
                    return ' "collect2"\n' if "-shared" in command else ""
                if any(value.startswith("-print-prog-name=") for value in command):
                    return command[-1].split("=", 1)[1]+"\n"
                if any(value.startswith("-print-file-name=") for value in command):
                    return command[-1].split("=", 1)[1]+"\n"
                if command[0] == str(paths["ldd"]):
                    if kwargs.get("capture_stderr"):
                        return (f"  1: trying file={paths['runtime']}\n"
                                if command[1] == str(singular) else "")
                    return (f"libRuntime.so => {paths['runtime']} (0x1)\n"
                            if command[1] == str(singular) else "")
                raise AssertionError(command)

            with mock.patch.object(compiler.sys, "platform", "linux"), \
                    mock.patch.object(compiler, "run", side_effect=output), \
                    mock.patch.object(compiler, "LOADER_CONFIGURATION_PATHS",
                                      (loader_cache, loader_preload)):
                base = compiler.probe_build_configuration(request)
                self.assertEqual(base["compile_options"][-2:],
                                 ["-I", str(compiler.HERE)])
                paths["runtime"].write_text("runtime changed")
                runtime = compiler.probe_build_configuration(request)
                link_library.write_text("link changed")
                linked = compiler.probe_build_configuration(request)
                paths["header"].write_text("header changed")
                compiled = compiler.probe_build_configuration(request)
                reordered_request = dict(request)
                reordered_request["environment"] = {
                    "GLIBC_TUNABLES": "example.b=2:example.a=1"}
                reordered = compiler.probe_build_configuration(reordered_request)
                loader_preload.write_text("B.so\nA.so\n")
                reordered_preload = compiler.probe_build_configuration(request)
            self.assertEqual(base["identities"]["compile"], runtime["identities"]["compile"])
            self.assertEqual(base["identities"]["link"], runtime["identities"]["link"])
            self.assertNotEqual(base["identities"]["runtime"], runtime["identities"]["runtime"])
            self.assertNotEqual(runtime["identities"]["link"], linked["identities"]["link"])
            self.assertEqual(runtime["identities"]["compile"], linked["identities"]["compile"])
            self.assertNotEqual(linked["identities"]["compile"], compiled["identities"]["compile"])
            self.assertNotEqual(compiled["identities"]["compile"],
                                reordered["identities"]["compile"])
            self.assertNotEqual(compiled["identities"]["link"],
                                reordered["identities"]["link"])
            self.assertNotEqual(compiled["identities"]["runtime"],
                                reordered["identities"]["runtime"])
            for phase in ("compile", "link", "runtime"):
                self.assertNotEqual(compiled["identities"][phase],
                                    reordered_preload["identities"][phase])

    def test_flag_selected_subprograms_lto_and_specs_are_tracked(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            prefix, preferred, fallback = root/"prefix", root/"preferred", root/"fallback"
            program_only = root/"program-only"
            path_preferred = root/"path-preferred"
            # Model an automatically relocated driver whose internal
            # gcc_exec_prefix is the reported install path, not an environment
            # GCC_EXEC_PREFIX and not a target/version-shaped directory.
            install = root/"relocated/lib/gcc"
            target_override = install/"target/specs"
            (prefix/"bin").mkdir(parents=True)
            (prefix/"lib").mkdir()
            preferred.mkdir()
            fallback.mkdir()
            program_only.mkdir()
            path_preferred.mkdir()
            install.mkdir(parents=True)
            target_override.parent.mkdir(parents=True)
            files = {name: root/name for name in
                     ("compiler", "pkg-config", "ldd", "header", "runtime")}
            for name, path in files.items():
                path.write_text(name)
            (prefix/"bin/Singular").write_text("singular")
            (prefix/"lib/libSingular.so").write_text("library")
            for name in ("cc1plus", "as", "collect2", "ld.gold", "ld.bfd", "lto-wrapper", "lto1",
                         "specs"):
                (fallback/name).write_text("fallback "+name)
                (fallback/name).chmod(0o755)
            plugin = program_only/"liblto_plugin.so"
            plugin.write_text("program-prefix-only plugin")
            compiler_plugin = root/"compiler-plugin.so"
            auto_profile = root/"profile.afdo"
            clang_config = root/"clang.cfg"
            compile_tool_dso = root/"libCompileTool.so"
            link_tool_dso = root/"libLinkTool.so"
            compiler_plugin.write_text("compiler plugin")
            auto_profile.write_text("auto profile")
            clang_config.write_text("-Wall\n")
            compile_tool_dso.write_text("compile tool dependency")
            link_tool_dso.write_text("link tool dependency")
            implicit_crt = root/"crtbeginS.o"
            implicit_crt.write_text("implicit crt")
            version_script = root/"exports.map"
            secondary_version_script = root/"secondary.map"
            linker_inner_response = root/"linker-inner.rsp"
            linker_outer_response = root/"linker-outer.rsp"
            version_script.write_text("VERSION { global: compiled_*; local: *; };\n")
            secondary_version_script.write_text(
                "VERSION { global: secondary_*; local: *; };\n")
            linker_inner_response.write_text(
                f'--version-script "{secondary_version_script}"\n')
            linker_outer_response.write_text(
                f'--version-script="{version_script}" @linker-inner.rsp\n')
            # These earlier -B candidates exist in the initial directory
            # namespace but are not selected until they become executable.
            # chmod does not update the parent directory timestamp.
            (preferred/"cc1plus").write_text("preferred frontend")
            (preferred/"as").write_text("preferred assembler")
            (preferred/"ld.gold").write_text("preferred linker")
            explicit_spec = root/"explicit.spec"
            included_spec = fallback/"included.spec"
            including_file_neighbor = root/"included.spec"
            preferred_include_target = preferred/"included-target.spec"
            (preferred/"included.spec").symlink_to(preferred_include_target)
            function_spec = fallback/"function.spec"
            cwd_include = root/"cwd-only.spec"
            cwd_noerr = root/"noerr-only.spec"
            cwd_default = root/"specs"
            environment_spec = root/"environment.spec"
            explicit_spec.write_text(
                "%include <included.spec>\n"
                "%include <cwd-only.spec>\n"
                "%include_noerr <noerr-only.spec>\n"
                "*link:\n%:include(function.spec)\n")
            included_spec.write_text("*link:\n")
            including_file_neighbor.write_text("*link:\nwrong including-file-relative lookup\n")
            function_spec.write_text("*link:\n")
            cwd_include.write_text("*cc1plus:\n")
            cwd_noerr.write_text("*cc1plus:\nwrong noerr cwd fallback\n")
            cwd_default.write_text("*cc1plus:\nwrong default cwd fallback\n")
            environment_spec.write_text("*cc1plus:\n")
            b_option = "-B"+str(preferred)
            inner_response = root/"inner.rsp"
            outer_response = root/"outer.rsp"
            inner_response.write_text(" ".join(
                (b_option, "-fuse-ld=gold", "-specs="+str(explicit_spec)))+"\n")
            outer_response.write_text("@inner.rsp\n")
            response_option = "@"+str(outer_response)
            request = {"format": compiler.ABI_SNAPSHOT_FORMAT, "prefix": str(prefix),
                       "compiler": str(files["compiler"]),
                       "pkg_config": str(files["pkg-config"]), "ldd": str(files["ldd"]),
                       "flags": [response_option], "working_directory": str(root),
                       "environment": {"PATH": os.pathsep.join(
                           (str(path_preferred), str(fallback)))}}

            def output(command, **kwargs):
                if "--cflags" in command:
                    return ""
                if "--libs" in command:
                    return f"-L{prefix/'lib'} -lSingular\n"
                if "--variable=pc_path" in command:
                    return ""
                if "--variable=pcfiledir" in command:
                    return str(root)+"\n"
                if "--print-requires" in command:
                    return ""
                if "-E" in command:
                    self.assertEqual(kwargs["env"]["LC_ALL"], "C")
                    self.assertEqual(kwargs["env"]["LANG"], "C")
                    return f"#include <...> search starts here:\n {root}\nEnd of search list.\n"
                if "-M" in command:
                    return f"runtime.o: {files['header']}\n"
                if "-print-search-dirs" in command:
                    self.assertIn(response_option, command)
                    self.assertEqual(kwargs["env"]["LC_ALL"], "C")
                    self.assertEqual(kwargs["env"]["LANG"], "C")
                    return (f"install: {install}\n"
                            f"programs: ={preferred}{os.pathsep}{program_only}"
                            f"{os.pathsep}{fallback}\n"
                            f"libraries: ={preferred}{os.pathsep}{fallback}"
                            f"{os.pathsep}{prefix/'lib'}\n")
                if "-dumpmachine" in command:
                    return "target\n"
                if "-dumpversion" in command:
                    return "13\n"
                if "-###" in command:
                    selected_include = (preferred/"included.spec"
                                        if (preferred/"included.spec").is_file()
                                        else included_spec)
                    active_specs = [fallback/"specs", explicit_spec, selected_include,
                                    cwd_include, function_spec, environment_spec]
                    if target_override.is_file():
                        active_specs.append(target_override)
                    trace = "".join("Reading specs from "+str(path)+"\n"
                                    for path in active_specs)
                    if "-shared" in command:
                        trace += (f' "{fallback/"collect2"}" "-plugin" "{plugin}"'
                                  f' "{implicit_crt}" "@{linker_outer_response}"\n')
                    else:
                        trace += (f"Configuration file: {clang_config}\n"
                                  f' "{fallback/"cc1plus"}"'
                                  f' "-fplugin={compiler_plugin}"'
                                  f' "-fauto-profile={auto_profile}"\n')
                    return trace
                if any(value.startswith("-print-prog-name=") for value in command):
                    self.assertIn(response_option, command)
                    name = command[-1].split("=", 1)[1]
                    if name == "collect2":
                        return name+"\n"
                    if name == "liblto_plugin.so":
                        return str(plugin)+"\n"
                    selected = preferred/name
                    return str(selected if selected.is_file() and os.access(selected, os.X_OK)
                               else fallback/name)+"\n"
                if any(value.startswith("-print-file-name=") for value in command):
                    self.assertIn(response_option, command)
                    name = command[-1].split("=", 1)[1]
                    selected = preferred/name
                    return str(selected if selected.is_file() else fallback/name)+"\n"
                if "--version" in command:
                    return "fake compiler\n"
                if command[0] == str(files["ldd"]):
                    target = Path(command[1]).name
                    if kwargs.get("capture_stderr"):
                        if command[1] == str(prefix/"bin/Singular"):
                            return f"  1: trying file={files['runtime']}\n"
                        if target == "cc1plus":
                            return f"  1: trying file={compile_tool_dso}\n"
                        if target in ("ld.gold", "ld.bfd"):
                            return f"  1: trying file={link_tool_dso}\n"
                        return ""
                    if command[1] == str(prefix/"bin/Singular"):
                        return f"libRuntime.so => {files['runtime']} (0x1)\n"
                    if target == "cc1plus":
                        return f"libCompileTool.so => {compile_tool_dso} (0x1)\n"
                    if target in ("ld.gold", "ld.bfd"):
                        return f"libLinkTool.so => {link_tool_dso} (0x1)\n"
                    return ""
                raise AssertionError(command)

            def probe():
                return compiler.probe_build_configuration(request)

            with mock.patch.object(compiler.sys, "platform", "linux"), \
                    mock.patch.object(compiler, "run", side_effect=output), \
                    mock.patch.dict(os.environ, {"GCC_SPECS": str(environment_spec),
                                                 "GCC_EXEC_PREFIX": ""}):
                base = probe()
                compile_set = set(base["file_sets"]["compile"])
                link_set = set(base["file_sets"]["link"])
                self.assertTrue({str((fallback/name).absolute()) for name in ("cc1plus", "as")} <=
                                compile_set)
                self.assertTrue({str(compiler_plugin.absolute()),
                                 str(auto_profile.absolute()),
                                 str(clang_config.absolute()),
                                 str(compile_tool_dso.absolute())} <= compile_set)
                self.assertTrue({str((fallback/name).absolute()) for name in
                                 ("collect2", "ld.gold", "lto-wrapper", "lto1")} <=
                                link_set)
                self.assertTrue({str(plugin.absolute()), str(implicit_crt.absolute())} <=
                                link_set)
                self.assertIn(str(link_tool_dso.absolute()), link_set)
                self.assertTrue({str(path.absolute()) for path in
                                 (version_script, secondary_version_script,
                                  linker_inner_response, linker_outer_response)} <=
                                link_set)
                self.assertTrue({str(path.absolute()) for path in
                                 (fallback/"specs", explicit_spec, included_spec,
                                  function_spec, cwd_include, environment_spec)} <=
                                compile_set & link_set)
                self.assertNotIn(str(cwd_default.absolute()), compile_set | link_set)
                self.assertNotIn(str(cwd_noerr.absolute()), compile_set | link_set)
                self.assertNotIn(str(including_file_neighbor.absolute()),
                                 compile_set | link_set)
                spec_candidates = {entry["logical"] for entry in base["spec_candidates"]}
                self.assertIn(str((preferred/"included.spec").absolute()), spec_candidates)
                self.assertIn(str(target_override.absolute()), spec_candidates)
                self.assertIn(str((install/"specs").absolute()), spec_candidates)
                self.assertTrue({str(path.absolute()) for path in
                                 (outer_response, inner_response)} <= compile_set & link_set)
                base = json.loads(json.dumps(base))
                self.assertTrue(compiler.validate_abi_snapshot(base, request))

                compiler_plugin.write_text("changed compiler plugin")
                self.assertFalse(compiler.validate_abi_snapshot(base, request))
                changed_compiler_input = probe()
                self.assertNotEqual(base["identities"]["compile"],
                                    changed_compiler_input["identities"]["compile"])
                self.assertEqual(base["identities"]["link"],
                                 changed_compiler_input["identities"]["link"])
                base = changed_compiler_input

                clang_config.write_text("-Wall -Wextra\n")
                self.assertFalse(compiler.validate_abi_snapshot(base, request))
                changed_clang_config = probe()
                self.assertNotEqual(base["identities"]["compile"],
                                    changed_clang_config["identities"]["compile"])
                self.assertEqual(base["identities"]["link"],
                                 changed_clang_config["identities"]["link"])
                base = changed_clang_config

                compile_tool_dso.write_text("changed compile tool dependency")
                self.assertFalse(compiler.validate_abi_snapshot(base, request))
                changed_compile_tool_dso = probe()
                self.assertNotEqual(base["identities"]["compile"],
                                    changed_compile_tool_dso["identities"]["compile"])
                self.assertEqual(base["identities"]["link"],
                                 changed_compile_tool_dso["identities"]["link"])
                base = changed_compile_tool_dso

                version_script.write_text(
                    "VERSION { global: compiled_*; local: helper_*; };\n")
                self.assertFalse(compiler.validate_abi_snapshot(base, request))
                changed_version_script = probe()
                self.assertEqual(base["identities"]["compile"],
                                 changed_version_script["identities"]["compile"])
                self.assertNotEqual(base["identities"]["link"],
                                    changed_version_script["identities"]["link"])

                linker_inner_response.write_text(
                    f'--version-script "{secondary_version_script}"  \n')
                self.assertFalse(compiler.validate_abi_snapshot(
                    changed_version_script, request))
                changed_linker_response = probe()
                self.assertEqual(changed_version_script["identities"]["compile"],
                                 changed_linker_response["identities"]["compile"])
                self.assertNotEqual(changed_version_script["identities"]["link"],
                                    changed_linker_response["identities"]["link"])
                base = changed_linker_response

                link_tool_dso.write_text("changed link tool dependency")
                self.assertFalse(compiler.validate_abi_snapshot(base, request))
                changed_link_tool_dso = probe()
                self.assertEqual(base["identities"]["compile"],
                                 changed_link_tool_dso["identities"]["compile"])
                self.assertNotEqual(base["identities"]["link"],
                                    changed_link_tool_dso["identities"]["link"])
                base = changed_link_tool_dso

                implicit_crt.write_text("changed implicit crt")
                self.assertFalse(compiler.validate_abi_snapshot(base, request))
                changed_implicit = probe()
                self.assertEqual(base["identities"]["compile"],
                                 changed_implicit["identities"]["compile"])
                self.assertNotEqual(base["identities"]["link"],
                                    changed_implicit["identities"]["link"])

                plugin.write_text("changed program-prefix-only plugin")
                self.assertFalse(compiler.validate_abi_snapshot(changed_implicit, request))
                changed_plugin = probe()
                self.assertEqual(changed_implicit["identities"]["compile"],
                                 changed_plugin["identities"]["compile"])
                self.assertNotEqual(changed_implicit["identities"]["link"],
                                    changed_plugin["identities"]["link"])
                base = changed_plugin

                (preferred/"cc1plus").chmod(0o755)
                self.assertFalse(compiler.validate_abi_snapshot(base, request))
                selected_frontend = probe()
                self.assertNotEqual(base["identities"]["compile"],
                                    selected_frontend["identities"]["compile"])
                self.assertEqual(base["identities"]["link"],
                                 selected_frontend["identities"]["link"])

                (preferred/"as").chmod(0o755)
                self.assertFalse(compiler.validate_abi_snapshot(selected_frontend, request))
                selected_assembler = probe()
                self.assertNotEqual(selected_frontend["identities"]["compile"],
                                    selected_assembler["identities"]["compile"])
                self.assertEqual(selected_frontend["identities"]["link"],
                                 selected_assembler["identities"]["link"])

                (preferred/"as").write_text("changed preferred assembler")
                changed_assembler = probe()
                self.assertNotEqual(selected_assembler["identities"]["compile"],
                                    changed_assembler["identities"]["compile"])

                (preferred/"ld.gold").chmod(0o755)
                selected_linker = probe()
                self.assertNotEqual(changed_assembler["identities"]["link"],
                                    selected_linker["identities"]["link"])

                (fallback/"lto1").write_text("changed lto")
                self.assertFalse(compiler.validate_abi_snapshot(selected_linker, request))
                changed_lto = probe()
                self.assertNotEqual(selected_linker["identities"]["link"],
                                    changed_lto["identities"]["link"])

                # The earlier startfile-prefix candidate is a broken symlink.
                # Making only its target appear must invalidate stat-only warm
                # validation and select it; the including file's directory is
                # never a specs search root.
                preferred_include_target.write_text("*link:\npreferred shadow\n")
                self.assertFalse(compiler.validate_abi_snapshot(changed_lto, request))
                shadowed_spec = probe()
                self.assertNotEqual(changed_lto["identities"]["compile"],
                                    shadowed_spec["identities"]["compile"])
                self.assertNotEqual(changed_lto["identities"]["link"],
                                    shadowed_spec["identities"]["link"])
                shadow_compile_set = set(shadowed_spec["file_sets"]["compile"])
                self.assertIn(str((preferred/"included.spec").absolute()),
                              shadow_compile_set)
                self.assertNotIn(str(included_spec.absolute()), shadow_compile_set)

                # GCC reads a separate exec-prefix/target/specs override after
                # the primary default. Its previously-missing exact candidate
                # must invalidate, then its content must affect both phases.
                target_override.write_text("*link:\nsecond override\n")
                self.assertFalse(compiler.validate_abi_snapshot(shadowed_spec, request))
                second_override = probe()
                self.assertNotEqual(shadowed_spec["identities"]["compile"],
                                    second_override["identities"]["compile"])
                self.assertNotEqual(shadowed_spec["identities"]["link"],
                                    second_override["identities"]["link"])
                self.assertIn(str(target_override.absolute()),
                              second_override["file_sets"]["compile"])
                target_override.write_text("*link:\nchanged second override\n")
                changed_override = probe()
                self.assertNotEqual(second_override["identities"]["compile"],
                                    changed_override["identities"]["compile"])
                self.assertNotEqual(second_override["identities"]["link"],
                                    changed_override["identities"]["link"])

                (path_preferred/"collect2").write_text("preferred collect2")
                (path_preferred/"collect2").chmod(0o755)
                self.assertFalse(compiler.validate_abi_snapshot(changed_override, request))
                path_selected = probe()
                self.assertNotEqual(changed_override["identities"]["link"],
                                    path_selected["identities"]["link"])

                explicit_spec.write_text("*link:\nchanged\n")
                changed_spec = probe()
                self.assertNotEqual(path_selected["identities"]["compile"],
                                    changed_spec["identities"]["compile"])
                self.assertNotEqual(path_selected["identities"]["link"],
                                    changed_spec["identities"]["link"])

                environment_spec.write_text("*cc1plus:\nchanged\n")
                changed_environment_spec = probe()
                self.assertNotEqual(changed_spec["identities"]["compile"],
                                    changed_environment_spec["identities"]["compile"])
                self.assertNotEqual(changed_spec["identities"]["link"],
                                    changed_environment_spec["identities"]["link"])

                inner_response.write_text(" ".join(
                    (b_option, "-fuse-ld=bfd", "-fno-lto",
                     "-specs="+str(explicit_spec)))+"\n")
                self.assertFalse(compiler.validate_abi_snapshot(
                    changed_environment_spec, request))
                changed_response = probe()
                self.assertNotEqual(changed_environment_spec["identities"]["compile"],
                                    changed_response["identities"]["compile"])
                self.assertNotEqual(changed_environment_spec["identities"]["link"],
                                    changed_response["identities"]["link"])
                changed_link_set = set(changed_response["file_sets"]["link"])
                self.assertIn(str((fallback/"ld.bfd").absolute()), changed_link_set)
                self.assertIn(str((fallback/"lto1").absolute()), changed_link_set)

    def test_link_input_resolution_honors_explicit_search_order(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            explicit, default = root/"explicit", root/"default"
            explicit.mkdir()
            default.mkdir()
            (explicit/"libChoice.so").write_bytes(b"explicit")
            (explicit/"libChoice.a").write_bytes(b"explicit archive")
            (default/"libChoice.so").write_bytes(b"default")
            (default/"libChoice.a").write_bytes(b"default archive")
            candidates = set()
            inputs, directories = compiler._link_inputs(
                ["-L", str(explicit), "-l", "Choice", "-Wl,-Bstatic"],
                [default], root, candidates)
            self.assertEqual(inputs, [str(explicit/"libChoice.so"),
                                      str(explicit/"libChoice.a"),
                                      str(default/"libChoice.so"),
                                      str(default/"libChoice.a")])
            self.assertEqual(directories[0], explicit)
            self.assertEqual(candidates, {
                explicit/"libChoice.so", explicit/"libChoice.a",
                default/"libChoice.so", default/"libChoice.a"})
            records = compiler._records(inputs)
            (explicit/"libChoice.a").write_bytes(b"changed selected archive")
            self.assertFalse(all(compiler.validate_file_record(record)
                                 for record in records))

    def test_link_input_sysroot_rpath_link_and_script_closure(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            sysroot_library = root/"sysroot/usr/lib"
            rpath_link = root/"rpath-link"
            module_rpath = root/"module-rpath"
            sysroot_library.mkdir(parents=True)
            rpath_link.mkdir()
            module_rpath.mkdir()
            library = sysroot_library/"libSysroot.so"
            dependency = rpath_link/"libDependency.so.1"
            symbols = root/"symbols.o"
            library.write_text("sysroot library")
            dependency.write_text("rpath-link dependency")
            symbols.write_text("symbols")
            for spelling in ("=/usr/lib", "$SYSROOT/usr/lib"):
                with self.subTest(spelling=spelling):
                    runtime_directories = set()
                    inputs, directories = compiler._link_inputs(
                        ["--sysroot", str(root/"sysroot"), "-L"+spelling,
                         "-l", "Sysroot", "--rpath-link="+str(rpath_link),
                         "--rpath="+str(module_rpath), "-R"+str(symbols)],
                        [], root, set(), runtime_directories)
                    self.assertTrue({str(library), str(dependency), str(symbols)} <=
                                    set(inputs))
                    self.assertIn(sysroot_library, directories)
                    self.assertIn(rpath_link, directories)
                    self.assertEqual(runtime_directories, {module_rpath})
            script_directory = root/"scripts"
            script_directory.mkdir()
            implementation = script_directory/"libimpl.a"
            transitive = script_directory/"libtransitive.so"
            implementation.write_bytes(b"!<arch>\nimplementation")
            transitive.write_bytes(b"\x7fELFtransitive")
            nested = script_directory/"nested.ld"
            member = script_directory/"member.o"
            member.write_text("member")
            nested.write_text("INPUT(libimpl.a); LIB(member.o);\n")
            script = script_directory/"libWrapper.so"
            script.write_text(
                "SEARCH_DIR("+str(script_directory)+")\n"
                "GROUP(AS_NEEDED(libtransitive.so))\n"
                "INCLUDE nested.ld\n")
            for tokens in (("--script="+str(script),),
                           ("-T"+str(script),),
                           ("-L"+str(script_directory), "-lWrapper")):
                with self.subTest(tokens=tokens):
                    candidates = set()
                    inputs, directories = compiler._link_inputs(
                        list(tokens), [], root, candidates)
                    self.assertTrue({str(script), str(nested), str(implementation), str(member),
                                     str(transitive)} <= set(inputs))
                    self.assertIn(script_directory, directories)
                    self.assertIn(script_directory/"libimpl.a", candidates)
            records = compiler._records(inputs)
            implementation.write_bytes(b"!<arch>\nchanged implementation")
            self.assertFalse(all(compiler.validate_file_record(record)
                                 for record in records))
            for option in ("--mri-script="+str(script), "-c"):
                with self.subTest(option=option):
                    tokens = [option] if option != "-c" else [option, str(script)]
                    with self.assertRaisesRegex(compiler.CompileError, "MRI"):
                        compiler._link_inputs(tokens, [], root, set())
            thin = root/"libThin.a"
            thin.write_bytes(b"!<thin>\n")
            with self.assertRaisesRegex(compiler.CompileError, "thin archive"):
                compiler._link_inputs([str(thin)], [], root, set())
            for option in ("--audit=audit.so", "--depaudit", "--filter=libFilter.so",
                           "--auxiliary=libAux.so", "-P"):
                with self.subTest(option=option):
                    with self.assertRaisesRegex(compiler.CompileError,
                                                "loader-injecting"):
                        compiler._link_inputs([option], [], root, set())

    def test_link_candidate_kind_and_exact_name_changes_reject_refresh(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            libraries = root/"libraries"
            libraries.mkdir()
            early, late = root/"early", root/"late"
            early.mkdir()
            late.mkdir()
            selected_crt = late/"crtbeginS.o"
            selected_crt.write_text("selected startup object")
            ordinary = libraries/"libChoice.so"
            ordinary.mkdir()
            exact = libraries/"arbitrary-name"
            candidates = set()
            inputs, link_directories = compiler._link_inputs(
                ["-L", str(libraries), "-lChoice", "-l:arbitrary-name",
                 str(selected_crt)],
                [early, late], root, candidates)
            self.assertEqual(inputs, [str(selected_crt)])
            self.assertIn(ordinary, candidates)
            self.assertIn(exact, candidates)
            self.assertIn(early/"crtbeginS.o", candidates)
            request = {"format": compiler.ABI_SNAPSHOT_FORMAT, "request": "link-candidate"}
            snapshot = compiler.seal_abi_snapshot({
                "format": compiler.ABI_SNAPSHOT_FORMAT,
                "request": request,
                "files": [], "directories": [], "include_directories": [],
                "include_candidates": [],
                "program_directories": [], "program_candidates": [],
                "spec_candidates": [],
                "link_candidates": [compiler.candidate_file_record(path) for path in
                                    sorted(candidates, key=lambda path: str(path))],
                "loader_candidates": {"compile": [], "link": [], "runtime": []},
                "module_runtime_directories": [],
                "link_directories": [compiler.link_directory_record(path)
                                     for path in link_directories],
                "runtime_directories": [],
                "file_sets": {"compile": [], "link": [], "runtime": []},
                "compiler": "test", "compile_options": [], "link_options": [],
                "libraries": [], "compiler_version": "test",
                "identities": {"compile": "c", "link": "l", "runtime": "r"},
            })
            self.assertTrue(compiler.validate_abi_snapshot(snapshot, request))
            ordinary.rmdir()
            ordinary.write_text("now a regular shared library")
            exact.write_text("now an exact-name library")
            (early/"crtbeginS.o").write_text("now shadows the selected startup object")
            self.assertFalse(compiler.validate_abi_snapshot(snapshot, request))
            self.assertIsNone(compiler.refresh_abi_snapshot(snapshot, request))

    def test_pkg_config_file_discovery_uses_portable_pcfiledir(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for filename in ("Singular.pc", "Singular-uninstalled.pc", "provider.pc"):
                (root/filename).write_text("Name: "+filename+"\n")

            commands = []
            def output(command, **kwargs):
                commands.append(command)
                if "--path" in command:
                    raise AssertionError("pkgconf-only --path must not be used")
                if "--variable=pcfiledir" in command:
                    return str(root)+"\n"
                if "--print-requires" in command:
                    return "provided-alias >= 1\n" if command[-1] == "Singular" else ""
                raise AssertionError(command)

            with mock.patch.object(compiler, "run", side_effect=output):
                files, directories = compiler._pkg_config_files("pkg-config", {})
            self.assertEqual(files, sorted(str(root/name) for name in
                                           ("Singular.pc", "Singular-uninstalled.pc",
                                            "provider.pc")))
            self.assertEqual(directories, [str(root)])
            self.assertFalse(any("--path" in command for command in commands))
            records = compiler._records(files)
            (root/"Singular-uninstalled.pc").write_text("changed\n")
            self.assertFalse(all(compiler.validate_file_record(record) for record in records))

    def test_pkg_config_output_environment_is_part_of_request_identity(self):
        values = {
            "PKG_CONFIG_DISABLE_UNINSTALLED": "1",
            "PKG_CONFIG_ALLOW_SYSTEM_CFLAGS": "1",
            "PKG_CONFIG_ALLOW_SYSTEM_LIBS": "1",
            "PKG_CONFIG_SYSTEM_INCLUDE_PATH": "/system/include",
            "PKG_CONFIG_SYSTEM_LIBRARY_PATH": "/system/lib",
            "PKG_CONFIG_TOP_BUILD_DIR": "/build",
            "LD_PRELOAD": "/loader/preload.so",
            "LD_AUDIT": "/loader/audit.so",
            "GLIBC_TUNABLES": "glibc.cpu.hwcaps=-AVX2",
            "LD_HWCAP_MASK": "0",
        }
        with mock.patch.dict(os.environ, values), \
                mock.patch.object(compiler, "_which", side_effect=lambda value: "/tools/"+value):
            request = compiler.configuration_request(Path("/prefix"), "c++", [])
        self.assertEqual({name: request["environment"][name] for name in values}, values)
        changed = dict(values)
        changed["PKG_CONFIG_ALLOW_SYSTEM_CFLAGS"] = "0"
        with mock.patch.dict(os.environ, changed), \
                mock.patch.object(compiler, "_which", side_effect=lambda value: "/tools/"+value):
            changed_request = compiler.configuration_request(Path("/prefix"), "c++", [])
        self.assertNotEqual(request, changed_request)

    def test_configured_pkg_config_directory_tracks_all_pc_contents(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            provider = root/"provider.pc"
            provider.write_text("Name: provider\nProvides: unrelated\n")
            external = root.parent/(root.name+"-linked.pc")
            external.write_text("Name: linked\n")
            linked = root/"linked.pc"
            linked.symlink_to(external)
            try:
                record = compiler.directory_record(root)
                self.assertEqual({Path(entry["logical"]).name for entry in record["pc_files"]},
                                 {"provider.pc", "linked.pc"})
                self.assertTrue(compiler.validate_directory_record(record))
                provider.write_text("Name: provider\nProvides: Singular\n")
                self.assertFalse(compiler.validate_directory_record(record))

                record = compiler.directory_record(root)
                external.write_text("Name: linked changed\n")
                self.assertFalse(compiler.validate_directory_record(record))
            finally:
                external.unlink(missing_ok=True)

    def test_unlistable_pkg_config_directory_fails_closed(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)/"execute-only"
            root.mkdir()
            (root/"Singular.pc").write_text("Name: Singular\n")
            root.chmod(0o300)
            try:
                with self.assertRaisesRegex(compiler.CompileError,
                                            "pkg-config directory"):
                    compiler.directory_record(root)

                def output(command, **kwargs):
                    if "--variable=pcfiledir" in command:
                        return str(root)+"\n"
                    if "--print-requires" in command:
                        return ""
                    raise AssertionError(command)

                with mock.patch.object(compiler, "run", side_effect=output):
                    with self.assertRaisesRegex(compiler.CompileError,
                                                "pkg-config directory"):
                        compiler._pkg_config_files("pkg-config", {})
            finally:
                root.chmod(0o700)

    def test_include_shadow_directory_appearance_invalidates_record(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            early, late = root/"early", root/"late"
            early.mkdir()
            (late/"nested").mkdir(parents=True)
            header = late/"nested/value.h"
            header.write_text("value")
            records = compiler._include_shadow_directories([header], [early, late])
            early_nested = next(record for record in records
                                if record["logical"] == str((early/"nested").absolute()))
            self.assertEqual(early_nested["kind"], "missing")
            (early/"nested").mkdir()
            (early/"nested/value.h").write_text("shadow")
            self.assertFalse(compiler.validate_namespace_directory_record(early_nested))

    def test_literal_has_include_candidates_change_compile_selection(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            include = root/"include"
            optional = include/"optional"
            optional.mkdir(parents=True)
            header = include/"runtime.h"
            header.write_text(
                "#ifdef __has_include\n"
                "#if __has_include(<optional/feature.h>)\n#endif\n"
                "#if __has_include(\"peer.h\")\n#endif\n"
                "#endif\n"
                "// __has_include(<ignored-comment.h>)\n"
                "const char *s = \"__has_include(<ignored-string.h>)\";\n")
            candidates = set()
            records = compiler._include_shadow_directories(
                [header], [include], candidates)
            feature = optional/"feature.h"
            peer = include/"peer.h"
            self.assertTrue({feature, peer, header} <= candidates)
            self.assertIn(str(optional.absolute()),
                          {record["logical"] for record in records})
            candidate_records = [compiler.candidate_file_record(path) for path in
                                 sorted(candidates, key=lambda path: str(path))]
            old_identity = compiler._candidate_selection_identity(candidate_records)
            self.assertTrue(all(compiler.validate_candidate_file_record(record)
                                for record in candidate_records))
            feature.write_text("now available\n")
            self.assertFalse(all(compiler.validate_candidate_file_record(record)
                                 for record in candidate_records))
            refreshed = [compiler.candidate_file_record(record["logical"])
                         for record in candidate_records]
            self.assertNotEqual(old_identity,
                                compiler._candidate_selection_identity(refreshed))

            header.write_text("#if __has_include(HEADER_NAME)\n#endif\n")
            with self.assertRaisesRegex(compiler.CompileError, "non-literal"):
                compiler._include_shadow_directories([header], [include], set())
            header.write_text(
                "#define HAS_HEADER __has_include\n"
                "#if HAS_HEADER(\"optional/feature.h\")\n#endif\n")
            with self.assertRaisesRegex(compiler.CompileError, "standalone"):
                compiler._include_shadow_directories([header], [include], set())

    def test_unlistable_include_shadow_fails_closed(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            early, late = root/"early", root/"late"
            early_nested, late_nested = early/"nested", late/"nested"
            early_nested.mkdir(parents=True)
            late_nested.mkdir(parents=True)
            header = late_nested/"value.h"
            header.write_text("late\n")
            early_nested.chmod(0o300)
            try:
                with self.assertRaisesRegex(compiler.CompileError, "include search"):
                    compiler._include_shadow_directories(
                        [header], [early, late], set())
            finally:
                early_nested.chmod(0o700)

    def test_has_include_splicing_and_include_next_are_tracked(self):
        with tempfile.TemporaryDirectory() as directory:
            include = Path(directory)/"include"
            include.mkdir()
            header = include/"runtime.h"
            candidate = include/"later.h"
            spellings = (
                "__has_include\\\n(<later.h>)",
                "__has_\\\ninclude(<later.h>)",
                "__has_??/\ninclude(<later.h>)",
                "__has_include_next(<later.h>)",
            )
            for spelling in spellings:
                with self.subTest(spelling=spelling):
                    header.write_text(f"#if {spelling}\n#endif\n")
                    candidates = set()
                    compiler._include_shadow_directories(
                        [header], [include], candidates)
                    self.assertIn(candidate, candidates)

    def test_token_pasted_has_include_fails_closed(self):
        with tempfile.TemporaryDirectory() as directory:
            include = Path(directory)/"include"
            optional = include/"optional"
            optional.mkdir(parents=True)
            header = include/"runtime.h"
            header.write_text(
                "#define P2(a,b) a ## b\n"
                "#define P(a,b) P2(a,b)\n"
                "#if P(__has_,include)(\"optional/feature.h\")\n#endif\n")
            with self.assertRaisesRegex(compiler.CompileError,
                                        "token-pasted __has_include"):
                compiler._include_shadow_directories([header], [include], set())
            for target in ("__has_include", "__has_include_next"):
                for split in range(1, len(target)):
                    with self.subTest(target=target, split=split):
                        header.write_text(
                            "#define P2(a,b) a ## b\n"
                            "#define P(a,b) P2(a,b)\n"
                            f"#if P({target[:split]},{target[split:]})"
                            "(\"/tmp/external/feature.h\")\n#endif\n")
                        with self.assertRaisesRegex(
                                compiler.CompileError,
                                "unsupported"):
                            compiler._include_shadow_directories(
                                [header], [include], set())

    def test_unrelated_token_paste_in_default_style_header_is_accepted(self):
        with tempfile.TemporaryDirectory() as directory:
            header = Path(directory)/"gthr-default.h"
            header.write_text(
                "#define CONCAT(a,b) a ## b\n"
                "#if __has_attribute(weak)\n"
                "# include <stddef.h>\n"
                "#endif\n"
                "int CONCAT(gthr_,once);\n"
                "#define _GLIBCXX_USE_TBB_PAR_BACKEND __has_include(<tbb/tbb.h>)\n"
                "#if _GLIBCXX_USE_TBB_PAR_BACKEND\n#endif\n")
            self.assertEqual(compiler._literal_has_include_operands(header),
                             [("tbb/tbb.h", False)])

        candidates = []
        for root in (Path("/usr/include/c++"), Path("/usr/local/include/c++")):
            if root.is_dir():
                candidates += list(root.glob("**/bits/gthr-default.h"))
                candidates += list(root.glob("**/bits/c++config.h"))
        for header in candidates[:4]:
            with self.subTest(system_header=str(header)):
                compiler._literal_has_include_operands(header)

    def test_include_scan_ignores_irrelevant_unreadable_subtrees_and_tracks_pch(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            include = root/"include"
            include.mkdir()
            header = include/"runtime.h"
            header.write_text("#pragma once\n")
            irrelevant = include/"large-package-tree"
            irrelevant.mkdir()
            irrelevant.chmod(0)
            try:
                records = compiler._include_shadow_directories(
                    [header], [include], set())
                self.assertEqual([entry["logical"] for entry in records],
                                 [str(include.absolute())])
            finally:
                irrelevant.chmod(0o700)

            candidates = set()
            self.assertEqual(
                compiler._implicit_pch_files([header], root, candidates), [])
            pch = Path(str(header)+".gch")
            self.assertIn(pch, candidates)
            pch.write_bytes(b"precompiled header")
            self.assertEqual(
                compiler._implicit_pch_files([header], root, set()), [str(pch)])
            record = compiler.file_record(pch)
            pch.write_bytes(b"changed pch bytes")
            self.assertFalse(compiler.validate_file_record(record))

    def test_loader_injection_environment_fails_closed(self):
        for name in ("LD_PRELOAD", "LD_AUDIT"):
            with self.subTest(name=name):
                with self.assertRaisesRegex(compiler.CompileError, name):
                    compiler._validate_loader_environment({name: "/tmp/injected.so"})

    def test_directory_profile_inputs_are_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            profiles = root/"profiles"
            profiles.mkdir()
            trace = f' "cc1plus" "-fprofile-use={profiles}"\n'
            with self.assertRaisesRegex(compiler.CompileError,
                                        "directory-valued profile"):
                compiler._compiler_trace_input_files(trace, root)
            for options in (["-fprofile-use"], ["-fbranch-probabilities"],
                            ["-fauto-profile"], ["-fprofile-dir="+str(profiles)],
                            ["-fprofile-use="+str(profiles)]):
                with self.subTest(options=options):
                    with self.assertRaisesRegex(compiler.CompileError, "profile"):
                        compiler._validate_profile_inputs(options, root)
            profile = root/"profile.afdo"
            profile.write_text("profile")
            compiler._validate_profile_inputs(["-fauto-profile="+str(profile)], root)

    def test_entry_kind_changes_reject_namespace_refresh(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            include_directory = root/"include"
            runtime_directory = root/"runtime"
            include_directory.mkdir()
            runtime_directory.mkdir()
            (include_directory/"value.h").mkdir()
            (runtime_directory/"libChoice.so").mkdir()
            request = {"format": compiler.ABI_SNAPSHOT_FORMAT,
                       "request": "entry-kind"}
            snapshot = compiler.seal_abi_snapshot({
                "format": compiler.ABI_SNAPSHOT_FORMAT,
                "request": request,
                "files": [], "directories": [],
                "include_directories": [
                    compiler.namespace_directory_record(include_directory)],
                "include_candidates": [],
                "program_directories": [], "program_candidates": [],
                "spec_candidates": [], "link_candidates": [],
                "loader_candidates": {"compile": [], "link": [], "runtime": []},
                "module_runtime_directories": [],
                "link_directories": [],
                "runtime_directories": [
                    compiler.link_directory_record(runtime_directory)],
                "file_sets": {"compile": [], "link": [], "runtime": []},
                "compiler": "test", "compile_options": [], "link_options": [],
                "libraries": [], "compiler_version": "test",
                "identities": {"compile": "c", "link": "l", "runtime": "r"},
            })
            self.assertTrue(compiler.validate_abi_snapshot(snapshot, request))
            (include_directory/"value.h").rmdir()
            (include_directory/"value.h").write_text("now a header")
            (runtime_directory/"libChoice.so").rmdir()
            (runtime_directory/"libChoice.so").write_text("now a runtime library")
            self.assertFalse(compiler.validate_abi_snapshot(snapshot, request))
            self.assertIsNone(compiler.refresh_abi_snapshot(snapshot, request))

    def test_broken_shadow_symlink_target_appearance_invalidates_record(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            early, late, external = root/"early", root/"late", root/"external"
            (early/"nested").mkdir(parents=True)
            (late/"nested").mkdir(parents=True)
            external.mkdir()
            header = late/"nested/value.h"
            header.write_text("late")
            target = external/"value.h"
            (early/"nested/value.h").symlink_to(target)
            records = compiler._include_shadow_directories([header], [early, late])
            early_nested = next(record for record in records
                                if record["logical"] == str((early/"nested").absolute()))
            self.assertTrue(compiler.validate_namespace_directory_record(early_nested))
            target.write_text("early")
            self.assertFalse(compiler.validate_namespace_directory_record(early_nested))

            early_lib, late_lib = root/"early-lib", root/"late-lib"
            early_lib.mkdir()
            late_lib.mkdir()
            library_target = external/"libChoice.so"
            (early_lib/"libChoice.so").symlink_to(library_target)
            (late_lib/"libChoice.so").write_text("late")
            inputs, _ = compiler._link_inputs(
                ["-L"+str(early_lib), "-L"+str(late_lib), "-lChoice"], [])
            self.assertEqual(inputs, [str(late_lib/"libChoice.so")])
            link_record = compiler.link_directory_record(early_lib)
            self.assertTrue(compiler.validate_link_directory_record(link_record))
            library_target.write_text("early")
            self.assertFalse(compiler.validate_link_directory_record(link_record))

    def test_failed_root_does_not_poison_later_shared_root(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"isolation.lib"
            library.write_text('''
proc shared(int n) { return(n+1); }
proc aGood(int n) { return(shared(n)); }
static proc poison(string s) { return(s[1,1]); }
proc mBad(int n) { return(shared(n)+size(poison("ab"))); }
proc zGood(int n) { return(shared(n)+2); }
''')
            cache = compiler.cache_namespace(root/"cache")
            discovery = compiler.Closure(cache, [root])
            path = str(library.resolve())
            discovery.library(path)
            merged, failures = {}, []
            for name in ("aGood", "mBad", "zGood"):
                closure = compiler.Closure(cache, [root])
                closure.libraries = discovery.libraries
                try:
                    compiler.analyze_root(closure, path, name)
                except compiler.CompileError:
                    failures.append(name)
                    continue
                for key, node in closure.nodes.items():
                    if key in merged:
                        self.assertEqual(compiler._node_identity(merged[key]),
                                         compiler._node_identity(node))
                    merged.setdefault(key, node)
            self.assertEqual(failures, ["mBad"])
            self.assertEqual({key[1] for key in merged}, {"shared", "aGood", "zGood"})

    def test_old_cache_layout_is_inert(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            legacy = root/"objects"
            legacy.mkdir()
            sentinel = legacy/"sentinel"
            sentinel.write_bytes(b"legacy")
            namespace = compiler.cache_namespace(root)
            self.assertEqual(namespace, root/compiler.CACHE_FORMAT)
            self.assertEqual(sentinel.read_bytes(), b"legacy")
            self.assertFalse(str(namespace).startswith(str(legacy)))

    def test_per_key_object_lock_deduplicates_and_other_keys_overlap(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            cache = compiler.cache_namespace(root/"cache")
            calls = []

            def fake_compile(command, **kwargs):
                calls.append(command)
                time.sleep(0.05)
                Path(command[command.index("-o")+1]).write_bytes(b"object")
                return ""

            common = dict(cache=cache, key="same", source=root/"same.cc",
                          target=root/"same.o", code="same", compiler="c++", options=[])
            with mock.patch.object(compiler, "run", side_effect=fake_compile):
                with compiler.ThreadPoolExecutor(max_workers=2) as executor:
                    outcomes = list(executor.map(lambda _: compiler.compile_object(**common), range(2)))
            self.assertEqual(sorted(outcomes), ["compiled", "reused"])
            self.assertEqual(len(calls), 1)

            barrier = threading.Barrier(2)
            active = []
            def overlapping(command, **kwargs):
                active.append(command)
                barrier.wait(timeout=2)
                Path(command[command.index("-o")+1]).write_bytes(b"object")
                return ""
            tasks = [dict(cache=cache, key=key, source=root/(key+".cc"),
                          target=root/(key+".o"), code=key, compiler="c++", options=[])
                     for key in ("first", "second")]
            with mock.patch.object(compiler, "run", side_effect=overlapping):
                with compiler.ThreadPoolExecutor(max_workers=2) as executor:
                    self.assertEqual(list(executor.map(compiler._compile_task, tasks)),
                                     ["compiled", "compiled"])
            self.assertEqual(len(active), 2)

    def test_failed_compile_removes_unique_temporary(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            cache = compiler.cache_namespace(root/"cache")
            target = root/"failed.o"
            def failure(command, **kwargs):
                Path(command[command.index("-o")+1]).write_bytes(b"partial")
                raise compiler.CompileError("expected failure")
            with mock.patch.object(compiler, "run", side_effect=failure), \
                    self.assertRaisesRegex(compiler.CompileError, "expected failure"):
                compiler.compile_object(cache, "failed", root/"failed.cc", target,
                                        "source", "c++", [])
            self.assertFalse(target.exists())
            self.assertEqual(list(root.glob(".failed.o.*.tmp")), [])


if __name__ == "__main__":
    unittest.main()
