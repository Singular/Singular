#!/usr/bin/env python3
"""Focused tests for the native compiler's top-level library indexer."""

import importlib.util
from pathlib import Path
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location(
    "singular2cpp", ROOT/"Singular/compiler/singular2cpp.py")
compiler = importlib.util.module_from_spec(spec)
spec.loader.exec_module(compiler)


class LibraryIndexTest(unittest.TestCase):
    def test_matrix_declarations_retain_runtime_dimensions_and_two_indexes(self):
        parsed = compiler.Parser(compiler.tokens('''matrix values[rows][columns];
intmat indexes[2][3];
values[1,2]=7;
return(values[1,2]);
''')).parse()
        matrix_declaration, intmat_declaration, assignment, returned = parsed[1]
        self.assertEqual(matrix_declaration[1], "matrix")
        self.assertEqual(matrix_declaration[2][0][2],
                         [["var", "rows"], ["var", "columns"]])
        self.assertEqual(intmat_declaration[2][0][2],
                         [["int", 2], ["int", 3]])
        self.assertEqual(assignment[1][0], "index2")
        self.assertEqual(returned[1][0], "index2")

    def test_integer_literal_classification_matches_singular_length_threshold(self):
        parsed = compiler.Parser(compiler.tokens('''int shortValue=00000001;
bigint longValue=000000001;
return(2147483648);
''')).parse()
        short_declaration, long_declaration, returned = parsed[1]
        self.assertEqual(short_declaration[2][0][1], ["int", 1])
        self.assertEqual(long_declaration[2][0][1], ["bigint", "000000001"])
        self.assertEqual(returned[1], ["bigint", "2147483648"])

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

    def test_fixed_prefix_may_end_in_variadic_arguments(self):
        source = '''proc optional(int count, string label, list #)
{
  return(size(#)+count+size(label));
}
proc misplaced(int count, list #, string label)
{
  return(count);
}
'''
        procedures = compiler.library_index(source)["procedures"]
        self.assertEqual(
            compiler.signature(procedures["optional"]),
            [["int", "count"], ["string", "label"],
             ["varargs", "__snative_varargs"]])
        with self.assertRaisesRegex(compiler.CompileError,
                                    "unsupported parameter list"):
            compiler.signature(procedures["misplaced"])

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
  newstruct("x","int invalid");
  newstruct("int","int invalid");
  newstruct("ReservedField","int r_value");
  newstruct("Duplicate","int first");
  newstruct("Duplicate","int second");
  newstruct("Point","int x, string label");
  if (1) { newstruct("Conditional","int x"); }
  if (0) newstruct("UnbracedConditional","int x");
  for (int i=0; i<1; i++) newstruct("LoopConditional","int x");
  newstruct(dynamicName,"int x");
  newstruct("BeforeReturn","int x");
  return();
  newstruct("Unreachable","int x");
}
proc coordinate(Point point) { return(point.x); }
'''
        index = compiler.library_index(source)
        self.assertEqual(index["newstructs"], ["Point", "BeforeReturn"])
        self.assertEqual(index["newstruct_schemas"], {
            "Point": [["int", "x"], ["string", "label"]],
            "BeforeReturn": [["int", "x"]]})
        params, nominal = compiler.signature(
            index["procedures"]["coordinate"], index["newstructs"],
            include_nominal=True)
        self.assertEqual(params, [["newstruct:Point", "point"]])
        self.assertEqual(nominal, {"point": "Point"})

    def test_transitive_schema_requires_supported_member_owners(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"types.lib"
            library.write_text('''info="schema closure";
static proc mod_init()
{
  newstruct("Plain","int x, string label");
  newstruct("Opaque","map value");
  newstruct("Parent","Opaque value");
}
proc entry(Plain value) { return(value.x); }
''')
            cache = root/"cache"
            cache.mkdir()
            closure = compiler.Closure(cache, [root])
            schemas = closure.available_newstruct_schemas(str(library))
            self.assertEqual(schemas, {
                "Plain": [["int", "x"], ["string", "label"]]})
            self.assertFalse(compiler.nominal_needs_ring(
                compiler.nominal_kind("Plain"), schemas))
            self.assertTrue(compiler.nominal_needs_ring("map", schemas))

    def test_guarded_terminator_invalidates_later_schema_declarations(self):
        for guarded in ('if (condition) return();',
                        'if (condition) { ERROR("stop"); }'):
            with self.subTest(guarded=guarded):
                source = f'''info="conditional terminator";
static proc mod_init()
{{
  newstruct("Before","int x");
  {guarded}
  newstruct("MaybeInstalled","int x");
}}
'''
                index = compiler.library_index(source)
                self.assertEqual(index["newstructs"], ["Before"])
                self.assertEqual(set(index["newstruct_schemas"]), {"Before"})

    def test_guarded_duplicate_poisons_global_schema_in_both_orders(self):
        bodies = (
            'if (condition) newstruct("Point","string x");\n'
            '  newstruct("Point","int x");',
            'newstruct("Point","int x");\n'
            '  if (condition) newstruct("Point","string x");')
        for body in bodies:
            with self.subTest(body=body):
                source = f'''info="conditional duplicate";
static proc mod_init()
{{
  {body}
}}
'''
                index = compiler.library_index(source)
                self.assertEqual(index["newstructs"], [])
                self.assertEqual(index["newstruct_schemas"], {})

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

    def test_general_specializations_require_complete_source_fingerprints(self):
        general = compiler.library_index(
            (ROOT/"Singular/LIB/general.lib").read_text())
        for name in ("sum", "product", "sort"):
            with self.subTest(name=name):
                procedure = general["procedures"][name]
                self.assertEqual(compiler.canonical_generic(procedure), name)

                # Physical line changes are immaterial; the complete token
                # stream, signature, and visibility are the identity.
                relined = dict(procedure)
                relined["body"] = [[token, line+1000]
                                   for token, line in procedure["body"]]
                self.assertEqual(compiler.canonical_generic(relined), name)

                changed = dict(procedure)
                changed["body"] = [list(token) for token in procedure["body"]]
                changed["body"][-1][0] = ";" if changed["body"][-1][0] != ";" else "0"
                self.assertIsNone(compiler.canonical_generic(changed))

                private = dict(procedure, private=True)
                self.assertIsNone(compiler.canonical_generic(private))

        self.assertTrue(compiler.canonical_generic_dependencies(
            "sum", general["procedures"]))
        changed_procedures = dict(general["procedures"])
        changed_lsum = dict(changed_procedures["lsum"])
        changed_lsum["body"] = [list(token) for token in changed_lsum["body"]]
        changed_lsum["body"][-1][0] = (";" if changed_lsum["body"][-1][0] != ";"
                                         else "0")
        changed_procedures["lsum"] = changed_lsum
        self.assertFalse(compiler.canonical_generic_dependencies(
            "sum", changed_procedures))
        self.assertTrue(compiler.canonical_generic_dependencies(
            "product", changed_procedures))

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
