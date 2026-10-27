#!/usr/bin/env python3
"""Focused tests for the native compiler's top-level library indexer."""

import importlib.util
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location(
    "singular2cpp", ROOT/"Singular/compiler/singular2cpp.py")
compiler = importlib.util.module_from_spec(spec)
spec.loader.exec_module(compiler)


class LibraryIndexTest(unittest.TestCase):
    def test_legacy_preamble_keywords_quotes_and_braces_are_not_declarations(self):
        source = '''Legacy prose mentions proc fake() { return(0); } and LIB "fake.lib".
More prose contains "proc quoted() { LIB \\"quoted.lib\\"; }" and {literal braces}.
An unmatched documentation brace { and a stray " quote are ordinary prose.
LIB is documentation, not an import.
proc(fake) is documentation, not a declaration.
static proc static_fake() is documentation too.
  proc indented_fake() is also legacy prose.
proc\ttabbed_fake() remains legacy prose too.
/* a leading comment */
proc real()
{
  return(1);
}
'''
        index = compiler.library_index(source)
        self.assertEqual(set(index["procedures"]), {"real"})
        self.assertEqual(index["imports"], [])

    def test_new_style_header_and_top_level_example_hide_declarations(self):
        source = '''info="
proc documented_only() { return(0); }
LIB "documented.lib";
";
proc first() { return(1); }
example
{
  proc example_only() { return(0); }
  LIB "example.lib";
}
proc second() { return(2); }
'''
        index = compiler.library_index(source)
        self.assertEqual(set(index["procedures"]), {"first", "second"})
        self.assertEqual(index["imports"], [])

    def test_unanchored_new_style_header_hides_multiline_declarations(self):
        source = '''Legacy preface: info="
proc documented_only() { return(0); }
LIB "documented.lib";
";
proc real() { return(1); }
'''
        index = compiler.library_index(source)
        self.assertEqual(set(index["procedures"]), {"real"})
        self.assertEqual(index["imports"], [])

    def test_help_comment_or_shebang_enters_initial_but_header_comment_does_not(self):
        header = '''// still a header comment
static proc hidden() { return(0); }
proc real() { return(1); }
'''
        self.assertEqual(set(compiler.library_index(header)["procedures"]),
                         {"real"})
        for delimiter in ("// end legacy help", "#! end legacy help"):
            with self.subTest(delimiter=delimiter):
                help_source = f'''Legacy documentation
{delimiter}
static proc helper() {{ return(1); }}
proc real() {{ return(2); }}
'''
                procedures = compiler.library_index(help_source)["procedures"]
                self.assertEqual(set(procedures), {"helper", "real"})
                self.assertTrue(procedures["helper"]["private"])

    def test_space_or_block_comment_enters_help_before_comment_delimiter(self):
        for prefix in (" ", "/* legacy preamble */"):
            with self.subTest(prefix=prefix):
                source = f'''{prefix}
// end legacy help
static proc helper() {{ return(1); }}
proc real() {{ return(2); }}
'''
                procedures = compiler.library_index(source)["procedures"]
                self.assertEqual(set(procedures), {"helper", "real"})

    def test_initial_mode_accepts_same_line_top_level_declarations(self):
        source = '''proc first() { return(1); } proc second() { return(2); }
'''
        self.assertEqual(set(compiler.library_index(source)["procedures"]),
                         {"first", "second"})

    def test_initial_mode_ignores_shebang_declaration_text(self):
        source = '''info="new style";
#! proc fake() { return(0); }
#! LIB "fake.lib";
proc real() { return(1); }
'''
        index = compiler.library_index(source)
        self.assertEqual(set(index["procedures"]), {"real"})
        self.assertEqual(index["imports"], [])

    def test_initial_static_survives_shebang_trivia(self):
        source = '''info="new style";
static
#! generated-file comment
proc helper() { return(1); }
'''
        procedure = compiler.library_index(source)["procedures"]["helper"]
        self.assertTrue(procedure["private"])

    def test_help_accepts_embedded_quotes(self):
        source = '''proc quoted(int value)
"RETURN: the string "quoted" is documentation
"
{
  return(value);
}
'''
        index = compiler.library_index(source)
        self.assertEqual(set(index["procedures"]), {"quoted"})
        self.assertEqual(index["procedures"]["quoted"]["body"][0], ["return", 5])

    def test_well_formed_help_accepts_literal_braces_and_escaped_quotes(self):
        source = r'''proc braced(int value)
"RETURN: the set { x | x > 0 } and the word \"quoted\""
{
  return(value);
}
'''
        index = compiler.library_index(source)
        self.assertEqual(set(index["procedures"]), {"braced"})
        self.assertEqual(index["procedures"]["braced"]["body"][0], ["return", 4])

    def test_optional_signature_semicolon_with_help_or_bare_body(self):
        source = '''proc documented(int value);
"RETURN: value"
{
  return(value);
}
static proc bare();
{
  return(1);
}
'''
        procedures = compiler.library_index(source)["procedures"]
        self.assertEqual(set(procedures), {"documented", "bare"})
        self.assertFalse(procedures["documented"]["private"])
        self.assertTrue(procedures["bare"]["private"])

    def test_no_parentheses_are_variadic_but_empty_parentheses_are_not(self):
        source = '''
proc variadic
{
  return(size(#));
}
proc documented;
"help"
{
  return(size(#));
}
proc empty()
{
  return(1);
}
'''
        procedures = compiler.library_index(source)["procedures"]
        self.assertEqual(procedures["variadic"]["params"],
                         [["list", 2], ["#", 2]])
        self.assertEqual(compiler.signature(procedures["variadic"]),
                         [["varargs", "__snative_varargs"]])
        self.assertEqual(procedures["documented"]["params"],
                         [["list", 6], ["#", 6]])
        self.assertEqual(procedures["empty"]["params"], [])

    def test_newline_before_parentheses_remains_implicit_variadic(self):
        source = '''proc newline
(int value)
{
  return(size(#));
}
proc commented /* a block comment
continues here */ (int value)
{
  return(value);
}
'''
        procedures = compiler.library_index(source)["procedures"]
        self.assertEqual(compiler.signature(procedures["newline"]),
                         [["varargs", "__snative_varargs"]])
        self.assertEqual(procedures["commented"]["params"],
                         [["int", 7], ["value", 7]])

    def test_parameter_and_body_tokens_keep_source_lines(self):
        source = '''
// heading
proc shifted(
  int value
);
"help"
{
  return(value);
}
'''
        procedure = compiler.library_index(source)["procedures"]["shifted"]
        self.assertEqual(procedure["params"], [["int", 4], ["value", 4]])
        self.assertEqual(procedure["body"][:2], [["return", 8], ["(", 8]])

    def test_only_top_level_literal_imports_are_indexed(self):
        source = '''LIB "first.lib";
info = "LIB \\"text.lib\\";";
proc contains_import()
{
  LIB "body.lib";
}
example
{
  LIB "example.lib";
}
LIB "second.lib";
'''
        self.assertEqual(compiler.library_index(source)["imports"],
                         ["first.lib", "second.lib"])

    def test_only_unconditional_literal_mod_init_newstructs_are_indexed(self):
        source = '''info="new style";
static proc mod_init()
{
  newstruct("Point","int x, string label");
  if (1) { newstruct("Conditional","int x"); }
  newstruct(dynamicName,"int x");
  newstruct("BeforeReturn","int x");
  return();
  newstruct("Unreachable","int x");
}
proc coordinate(Point point) { return(point.x); }
'''
        index = compiler.library_index(source)
        self.assertEqual(index["newstructs"], ["Point", "BeforeReturn"])
        params, nominal = compiler.signature(
            index["procedures"]["coordinate"], index["newstructs"],
            include_nominal=True)
        self.assertEqual(params, [["value", "point"]])
        self.assertEqual(nominal, {"point": "Point"})

    def test_unsupported_import_forms_remain_errors(self):
        cases = {
            'LIB "missing-semicolon.lib"\n': "expected ';' after LIB declaration",
            'LIB("parenthesized.lib");\n': "dynamic LIB declaration is unsupported",
            'LIB library_name;\n': "dynamic LIB declaration is unsupported",
        }
        for source, message in cases.items():
            with self.subTest(source=source):
                with self.assertRaisesRegex(compiler.CompileError, message):
                    compiler.library_index(source)

    def test_duplicate_procedure_names_remain_errors(self):
        source = '''proc overloaded() { return(1); }
static proc overloaded(int value) { return(value); }
'''
        with self.assertRaisesRegex(compiler.CompileError,
                                    "duplicate procedure overloaded"):
            compiler.library_index(source)

    def test_library_corpus_sentinels(self):
        library_dir = ROOT/"Singular/LIB"
        paths = sorted(library_dir.rglob("*.lib"))
        failures = {}
        counts = {}
        total = 0
        for path in paths:
            relative = str(path.relative_to(library_dir))
            try:
                index = compiler.library_index(path.read_text())
            except compiler.CompileError as exc:
                failures[relative] = str(exc)
                continue
            counts[path.stem] = len(index["procedures"])
            total += len(index["procedures"])

        self.assertEqual(len(paths), 227)
        self.assertEqual(set(failures),
                         {"central.lib", "KVequiv.lib", "ncfrac.lib", "ncloc.lib"})
        self.assertIn("expected ';' after LIB declaration", failures["central.lib"])
        self.assertIn("dynamic LIB declaration is unsupported", failures["KVequiv.lib"])
        self.assertIn("duplicate procedure testNcfrac", failures["ncfrac.lib"])
        self.assertIn("duplicate procedure testNcloc", failures["ncloc.lib"])
        self.assertEqual(len(paths) - len(failures), 223)
        self.assertEqual(total, 5947)
        self.assertEqual({name: counts[name] for name in (
            "general", "arnold", "arr", "matrix", "primdec", "ring",
            "freegb", "integralbasis")}, {
                "general": 22,
                "arnold": 75,
                "arr": 100,
                "matrix": 34,
                "primdec": 108,
                "ring": 34,
                "freegb": 72,
                "integralbasis": 108,
            })

        general = compiler.library_index((library_dir/"general.lib").read_text())
        self.assertEqual(general["procedures"]["killall"]["params"],
                         [["list", 340], ["#", 340]])
        self.assertEqual(compiler.signature(general["procedures"]["killall"]),
                         [["varargs", "__snative_varargs"]])


if __name__ == "__main__":
    unittest.main()
