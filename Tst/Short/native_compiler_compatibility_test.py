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
    def test_imported_newstruct_parameters_are_nominally_checked(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            base = root/"base.lib"
            base.write_text('''info="newstruct fixture";
static proc mod_init(){newstruct("Point","int x");}
''')
            consumer = root/"consumer.lib"
            consumer.write_text('''info="consumer fixture";
LIB "base.lib";
static proc pointX(Point point){int result=point.x;return(result);}
proc coordinate(Point point){return(pointX(point));}
proc indexed(Point point){int result=point[1];return(result);}
''')
            closure = compiler.Closure(root/"cache", [root])
            closure.visit(str(consumer.resolve()), "coordinate")
            closure.generate()
            coordinate = closure.nodes[(str(consumer.resolve()), "coordinate")]
            helper = closure.nodes[(str(consumer.resolve()), "pointX")]
            self.assertEqual(coordinate["nominal_params"], {"point": "Point"})
            self.assertIn('newstruct_argument(v_point, "Point")', coordinate["code"])
            self.assertIn('newstruct_argument(v_point, "Point")', helper["code"])

            indexed = compiler.Closure(root/"indexed-cache", [root])
            indexed.visit(str(consumer.resolve()), "indexed")
            with self.assertRaisesRegex(
                    compiler.CompileError, "direct newstruct indexing"):
                indexed.generate()

    def test_dynamic_equality_uses_exact_runtime_dispatch(self):
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
            closure.generate()
            source = next(iter(closure.nodes.values()))["code"]
            self.assertIn("snative::compare_values", source)
            self.assertNotIn("snative::compare_value_int", source)

    def test_dynamic_def_is_not_typed_from_procedure_result(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"dynamic-def.lib"
            library.write_text(
                'proc scan(list values){for(int i=1;i<=size(values);i++)'
                '{def value=values[i];}return(1);}\n')
            closure = compiler.Closure(root/"cache", [root])
            closure.visit(str(library.resolve()), "scan")
            with self.assertRaisesRegex(compiler.CompileError,
                                        "def initializer has an unsupported static type"):
                closure.generate()

    def test_dynamic_return_does_not_define_native_abi(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"dynamic-return.lib"
            library.write_text(
                'proc mixed(int flag,list values){if(flag){return(1);}'
                'return(values[1]+1);}\n'
                'proc explicit(list values){int result=values[1]+1;return(result);}\n'
                'proc compared(list values){return(values[1]<2);}\n')
            mixed = compiler.Closure(root/"mixed-cache", [root])
            mixed.visit(str(library.resolve()), "mixed")
            with self.assertRaisesRegex(compiler.CompileError,
                                        "cannot infer one .* return type"):
                mixed.generate()
            for name, expected in (("explicit", "snative::int_argument"),
                                   ("compared", "snative::compare_value_int")):
                closure = compiler.Closure(root/(name+"-cache"), [root])
                closure.visit(str(library.resolve()), name)
                closure.generate()
                source = next(iter(closure.nodes.values()))["code"]
                self.assertIn(expected, source)

    def test_public_sequence_return_keeps_exprlist_semantics(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"sequence-return.lib"
            library.write_text(
                'proc joined(intvec prefix,int tail){if(tail<0){return(prefix);}'
                'return(prefix,tail);}\n')
            closure = compiler.Closure(root/"cache", [root])
            closure.visit(str(library.resolve()), "joined")
            with self.assertRaisesRegex(compiler.CompileError,
                                        "comma expression return requires interpreted exprlist semantics"):
                closure.generate()

    def test_private_sequence_return_requires_typed_intvec_call_site(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"sequence-call.lib"
            library.write_text(
                'version = "sequence call compatibility";\n'
                'static proc joined(intvec prefix,int tail)'
                '{if(tail<0){return(prefix);}return(prefix,tail);}\n'
                'proc typed(intvec prefix,int tail)'
                '{intvec result=joined(prefix,tail);return(size(result));}\n'
                'proc expressionList(intvec prefix,int tail)'
                '{return(size(list(joined(prefix,tail))));}\n')

            typed = compiler.Closure(root/"typed-cache", [root])
            typed.visit(str(library.resolve()), "typed")
            typed.generate()
            self.assertIn("sn_joined_", typed.nodes[(str(library.resolve()), "typed")]["code"])

            expression_list = compiler.Closure(root/"exprlist-cache", [root])
            expression_list.visit(str(library.resolve()), "expressionList")
            with self.assertRaisesRegex(
                    compiler.CompileError,
                    "contextual comma-return procedure requires an explicit intvec destination"):
                expression_list.generate()

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
