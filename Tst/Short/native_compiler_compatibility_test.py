#!/usr/bin/env python3
"""Regression tests for opt-in native compiler compatibility boundaries."""

import importlib.util
from pathlib import Path
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location(
    "singular2cpp_compatibility_test", ROOT/"Singular/compiler/singular2cpp.py")
compiler = importlib.util.module_from_spec(spec)
spec.loader.exec_module(compiler)


class CompatibilityTest(unittest.TestCase):
    def test_dynamic_equality_is_not_inferred_from_integer_return(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"dynamic.lib"
            library.write_text(
                'proc compare(list a,list b){return(a[1]==b[1]);}\n')
            cache = root/"cache"
            cache.mkdir()
            # Milestone 3 adds a versioned namespace; this regression also
            # remains runnable against the preceding compiler snapshot.
            if hasattr(compiler, "cache_namespace"):
                cache = compiler.cache_namespace(cache)
            closure = compiler.Closure(cache, [root])
            closure.visit(str(library.resolve()), "compare")
            with self.assertRaisesRegex(compiler.CompileError,
                                        r"unsupported operator ==\(value, value\)"):
                closure.generate()

    def test_generated_entry_retains_source_library_identity(self):
        node = {
            "name": "monomials",
            "path": "/installed/share/singular/LIB/arnold.lib",
            "symbol": "sn_monomials_test",
            "params": [["poly", "f"]],
            "result": "list",
            "needs_ring": True,
        }
        source = compiler.module_source([node], "c_", [])
        capture = ('snative::SourceProc source_0=snative::native_source('
                   '"/installed/share/singular/LIB/arnold.lib", "monomials");')
        counter = ('if (!api->iiAddCproc("compiled.so", "c_compilerCalls", '
                   'FALSE, compiled_calls))')
        registration = ('if (!api->iiAddCproc("arnold.lib", "c_monomials", '
                        'FALSE, entry_0))')
        attach = ('snative::attach_native_source("c_monomials", '
                  'entry_0, source_0);')
        self.assertIn(capture, source)
        self.assertIn(counter, source)
        self.assertIn(registration, source)
        self.assertIn(attach, source)
        self.assertNotIn('iiAddCproc("compiled.so", "c_monomials"', source)
        self.assertNotIn('attach_native_source("c_compilerCalls"', source)
        try_start = source.index("  try {", source.index("int mod_init"))
        catch_start = source.index("  } catch", try_start)
        prepare = source.index('prepare_native("c_monomials")')
        counter_position = source.index(counter)
        registration_position = source.index(registration)
        attach_position = source.index(attach)
        self.assertLess(try_start, source.index(capture))
        self.assertLess(source.index(capture), prepare)
        self.assertLess(try_start, counter_position)
        self.assertLess(counter_position, catch_start)
        self.assertLess(prepare, registration_position)
        self.assertLess(registration_position, attach_position)
        self.assertLess(attach_position, catch_start)


if __name__ == "__main__":
    unittest.main()
