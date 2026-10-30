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
    def test_exact_kernels_and_fingerprinted_general_specializations(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            kernels = root/"typed-kernels.lib"
            kernels.write_text('''info="typed kernel fixture";
proc checked(poly p,number n,ideal I,string text,string needle,int start)
{
  return(list(univariate(p),pardeg(n),find(text,needle),
              find(text,"ana",start),ord(p),npars(basering),par(1),
              prime(start),charstr(basering),simplify(p,65),simplify(I,127)));
}
''')
            closure = compiler.Closure(root/"kernel-cache", [root])
            closure.visit(str(kernels.resolve()), "checked")
            closure.generate()
            code = closure.nodes[(str(kernels.resolve()), "checked")]["code"]
            for helper in ("snative::univariate", "snative::parameter_degree",
                           "snative::find_string", "snative::order",
                           "snative::parameter_count", "snative::parameter(",
                           "snative::previous_prime",
                           "snative::characteristic_string",
                           "snative::simplify_poly",
                           "snative::simplify_ideal"):
                self.assertIn(helper, code)

            imported = root/"typed-general.lib"
            imported.write_text('''info="typed general fixture";
LIB "general.lib";
proc sumSelected(intvec values,intvec selected){return(sum(values,selected));}
proc productSelected(intvec values,int selected){return(product(values,selected));}
proc sortVector(intvec values){return(sort(values));}
proc sortLiteral(int a,int b,int c){return(sort(list(a,b,c)));}
''')
            general = ROOT/"Singular/LIB"
            specialized = compiler.Closure(root/"general-cache", [root, general])
            for name in ("sumSelected", "productSelected", "sortVector", "sortLiteral"):
                specialized.visit(str(imported.resolve()), name)
            specialized.generate()
            emitted = lambda name: specialized.nodes[(str(imported.resolve()), name)]["code"]
            self.assertIn("snative::sum_intvec", emitted("sumSelected"))
            self.assertIn("snative::product_intvec", emitted("productSelected"))
            self.assertIn("snative::sort_intvec", emitted("sortVector"))
            self.assertIn("snative::sort_int_list", emitted("sortLiteral"))
            for name in ("sumSelected", "productSelected", "sortVector", "sortLiteral"):
                self.assertEqual(specialized.nodes[(str(imported.resolve()), name)]["dependencies"], set())

            # sum's large-input behavior lives in the private lsum helper.
            # Keeping the public sum source unchanged while altering that
            # helper must disable specialization of the whole call site.
            modified_dir = root/"modified-general"
            modified_dir.mkdir()
            modified_general = modified_dir/"general.lib"
            general_source = (general/"general.lib").read_text()
            self.assertIn("Summe=Summe+l[i]", general_source)
            modified_general.write_text(
                general_source.replace("Summe=Summe+l[i]", "Summe=Summe-l[i]", 1))
            modified_caller = modified_dir/"caller.lib"
            modified_caller.write_text(
                'LIB "general.lib";\n'
                'proc caller(intvec values){return(sum(values));}\n')
            changed = compiler.Closure(root/"changed-helper-cache", [modified_dir])
            with self.assertRaises(compiler.CompileError):
                changed.visit(str(modified_caller.resolve()), "caller")
            changed_caller = changed.nodes[(str(modified_caller.resolve()), "caller")]
            self.assertEqual(changed_caller["special_calls"], {})
            self.assertNotIn("snative::sum_intvec", changed_caller.get("code", ""))

            # Even a token-identical user copy is not the bundled general.lib
            # artifact. It must retain ordinary transitive-closure semantics.
            copied_dir = root/"copied-general"
            copied_dir.mkdir()
            (copied_dir/"general.lib").write_text(general_source)
            copied_caller = copied_dir/"caller.lib"
            copied_caller.write_text(
                'LIB "general.lib";\n'
                'proc caller(intvec values){return(sum(values));}\n')
            copied = compiler.Closure(root/"copied-helper-cache", [copied_dir])
            with self.assertRaises(compiler.CompileError):
                copied.visit(str(copied_caller.resolve()), "caller")
            copied_node = copied.nodes[(str(copied_caller.resolve()), "caller")]
            self.assertEqual(copied_node["special_calls"], {})

            # A same-named user procedure is an ordinary transitive call.  Its
            # different source fingerprint must never select the general.lib
            # fast path.
            replacement = root/"replacement.lib"
            replacement.write_text('''proc sum(def id,list #){return(41);}
proc caller(intvec values){return(sum(values));}
''')
            ordinary = compiler.Closure(root/"replacement-cache", [root])
            ordinary.visit(str(replacement.resolve()), "caller")
            ordinary.generate()
            caller = ordinary.nodes[(str(replacement.resolve()), "caller")]
            self.assertEqual(caller["special_calls"], {})
            self.assertIn((str(replacement.resolve()), "sum"), caller["dependencies"])
            self.assertNotIn("snative::sum_intvec", caller["code"])

            invalid = root/"invalid-kernel.lib"
            invalid.write_text('proc invalid(int value){return(univariate(value));}\n')
            rejected = compiler.Closure(root/"invalid-kernel-cache", [root])
            rejected.visit(str(invalid.resolve()), "invalid")
            with self.assertRaisesRegex(compiler.CompileError,
                                        "unsupported kernel overload univariate\\(int\\)"):
                rejected.generate()

            undefined_find = root/"unsafe-find.lib"
            undefined_find.write_text(
                'proc unsafe(string text,string needle,int start)'
                '{return(find(text,needle,start));}\n')
            rejected_find = compiler.Closure(root/"unsafe-find-cache", [root])
            rejected_find.visit(str(undefined_find.resolve()), "unsafe")
            with self.assertRaisesRegex(
                    compiler.CompileError,
                    "three-argument find requires a nonempty literal needle"):
                rejected_find.generate()

    def test_matrix_and_intmat_have_distinct_owned_abi_paths(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"matrix-owned.lib"
            library.write_text('''info="native matrix owner fixture";
static proc mod_init(){newstruct("MatrixCarrier","matrix values, intmat indexes");}
proc selected(matrix source,intvec rows,intvec columns)
{
  matrix result[size(rows)][size(columns)]=source[rows,columns];
  return(result);
}
proc updated(matrix source,int row,int column,poly value)
{
  source[row,column]=value;
  return(source);
}
proc intUpdated(intmat source,int row,int column,int value)
{
  source[row,column]=value;
  return(source);
}
proc dimensions(matrix source,intmat indexes)
{
  return(nrows(source)+ncols(source)+nrows(indexes)+ncols(indexes));
}
proc boxed(matrix source,intmat indexes)
{
  MatrixCarrier carrier; carrier.values=source; carrier.indexes=indexes;
  return(carrier);
}
proc matrixAssignIdeal(matrix target,ideal source)
{target=source;return(target);}
proc matrixAssignInt(matrix target,int source)
{target=source;return(target);}
proc matrixAssignMatrix(matrix target,matrix source)
{target=source;return(target);}
proc matrixAssignIntmat(matrix target,intmat source)
{target=source;return(target);}
proc matrixDeclareIdeal(ideal source)
{matrix target[2][3]=source;return(target);}
proc intmatAssignInt(intmat target,int source)
{target=source;return(target);}
proc intmatDeclareInt(int source)
{intmat target[2][3]=source;return(target);}
proc intmatAssignIntvec(intmat target,intvec source)
{target=source;return(target);}
proc matrixCastIdeal(ideal source){return(matrix(source));}
proc matrixCastIntmat(intmat source){return(matrix(source));}
proc intmatCastIntvec(intvec source){return(intmat(source));}
proc matrixDynamic(list source)
{matrix target[2][3]=source[1];return(target);}
proc matrixDynamicCast(list source){return(matrix(source[1]));}
proc intmatDynamicAssign(intmat target,list source)
{target=source[1];return(target);}
proc intmatDynamicCast(list source){return(intmat(source[1]));}
''')
            closure = compiler.Closure(root/"cache", [root])
            for name in ("selected", "updated", "intUpdated", "dimensions", "boxed",
                         "matrixAssignIdeal", "matrixAssignInt",
                         "matrixAssignMatrix", "matrixAssignIntmat",
                         "matrixDeclareIdeal", "intmatAssignInt", "intmatDeclareInt",
                         "intmatAssignIntvec", "matrixCastIdeal",
                         "matrixCastIntmat", "intmatCastIntvec",
                         "matrixDynamic", "matrixDynamicCast",
                         "intmatDynamicAssign", "intmatDynamicCast"):
                closure.visit(str(library.resolve()), name)
            closure.generate()

            selected = closure.nodes[(str(library.resolve()), "selected")]
            updated = closure.nodes[(str(library.resolve()), "updated")]
            int_updated = closure.nodes[(str(library.resolve()), "intUpdated")]
            boxed = closure.nodes[(str(library.resolve()), "boxed")]
            self.assertEqual(selected["result"], "matrix")
            self.assertEqual(selected["params"][0], ["matrix", "source"])
            self.assertIn("snative::matrix_select", selected["code"])
            self.assertIn("snative::CowMatrix", updated["code"])
            self.assertIn("snative::matrix_set", updated["code"])
            self.assertIn("snative::CowIntmat", int_updated["code"])
            self.assertIn("snative::intmat_set", int_updated["code"])
            self.assertIn("MATRIX_CMD", boxed["code"])
            self.assertIn("INTMAT_CMD", boxed["code"])

            entry = compiler.module_source(
                [selected, updated, int_updated], "native_", [])
            self.assertIn("snative::MatrixArgument", entry)
            self.assertIn("snative::IntmatArgument", entry)
            self.assertIn("result->rtyp=MATRIX_CMD", entry)
            self.assertIn("result->rtyp=INTMAT_CMD", entry)
            self.assertIn("result->attribute=value.release_attributes()", entry)

            node = lambda name: closure.nodes[(str(library.resolve()), name)]["code"]
            self.assertIn("matrix_from_ideal_shaped", node("matrixAssignIdeal"))
            self.assertIn("matrix_rows(c_target.get())", node("matrixAssignIdeal"))
            self.assertIn("matrix_from_int_shaped", node("matrixAssignInt"))
            self.assertNotIn("_shaped", node("matrixAssignMatrix"))
            self.assertIn("matrix_from_intvec", node("matrixAssignIntmat"))
            self.assertNotIn("matrix_from_intvec_shaped", node("matrixAssignIntmat"))
            self.assertIn("matrix_from_ideal_shaped(", node("matrixDeclareIdeal"))
            self.assertIn(", 2L, 3L, ctx.r)", node("matrixDeclareIdeal"))
            self.assertIn("intmat_assign_int(c_target.get()",
                          node("intmatAssignInt"))
            self.assertIn("intmat_from_int_shaped", node("intmatDeclareInt"))
            self.assertIn("snative::Intmat(new intvec(v_source)",
                          node("intmatAssignIntvec"))
            self.assertIn("matrix_from_ideal(", node("matrixCastIdeal"))
            self.assertIn(", ctx.r)", node("matrixCastIdeal"))
            self.assertNotIn("matrix_from_ideal_shaped", node("matrixCastIdeal"))
            self.assertIn("matrix_from_intvec(v_source.data(), ctx.r)",
                          node("matrixCastIntmat"))
            self.assertIn("snative::Intmat(new intvec(v_source), ctx.r)",
                          node("intmatCastIntvec"))
            self.assertRegex(node("matrixDynamic"),
                             r"MatrixArgument t\d+\([^;]+, 2L, 3L, false\)")
            self.assertRegex(node("matrixDynamicCast"),
                             r"MatrixArgument t\d+\([^;]+, 1L, 1L, true\)")
            self.assertRegex(node("intmatDynamicAssign"),
                             r"IntmatArgument t\d+\([^;]+, false, c_target\.get\(\)\)")
            self.assertRegex(node("intmatDynamicCast"),
                             r"IntmatArgument t\d+\([^;]+, 1L, 1L, true\)")

            invalid = root/"invalid-intmat-cast.lib"
            invalid.write_text('proc invalid(int value){return(intmat(value));}\n')
            rejected = compiler.Closure(root/"invalid-cache", [root])
            rejected.visit(str(invalid.resolve()), "invalid")
            with self.assertRaisesRegex(
                    compiler.CompileError, "explicit intmat\\(int\\)"):
                rejected.generate()

    def test_matrix_conversion_runtime_preserves_exact_values_only(self):
        self.assertEqual(compiler.VERSION, "singular-native-v19")
        runtime = (ROOT/"Singular/compiler/native_runtime.h").read_text()
        matrix_argument = runtime[
            runtime.index("class MatrixArgument {"):
            runtime.index("inline Matrix exact_matrix_argument")]
        intmat_argument = runtime[
            runtime.index("class IntmatArgument {"):
            runtime.index("inline Intmat exact_intmat_argument")]
        poly_argument = runtime[
            runtime.index("class PolyArgument {"):
            runtime.index("inline long int_argument")]

        # Ring-bound values must be rejected before their payload is borrowed
        # and interpreted in the current ring.
        ring_guard = matrix_argument.index("other_ring_value(argument)")
        self.assertLess(ring_guard, matrix_argument.index("borrow_poly(argument)"))
        self.assertLess(ring_guard, matrix_argument.index("(number)argument->Data()"))
        self.assertIn("type==IDEAL_CMD || type==MODUL_CMD || type==SMATRIX_CMD ||",
                      matrix_argument)
        self.assertLess(poly_argument.index("other_ring_value(argument)"),
                        poly_argument.index("borrow_poly(argument)"))
        self.assertIn(
            "materialized.flag=value_flags(current) & Sy_bit(FLAG_OTHER_RING);",
            runtime)

        # Indexed scalar arguments need materializing before Typ()/Data(), in
        # particular for the permissive int -> typed-intmat formal conversion.
        self.assertLess(intmat_argument.index("scalar_index_argument"),
                        intmat_argument.index("const int type=argument->Typ()"))
        self.assertIn("Poly selected;", matrix_argument)
        self.assertIn("&selected,r", matrix_argument)
        self.assertIn("type==SMATRIX_CMD && index->next!=nullptr", runtime)
        self.assertIn("type==VECTOR_CMD && selected_owner!=nullptr", runtime)
        self.assertIn("type==MATRIX_CMD && selected_owner!=nullptr", runtime)
        smatrix_selection = runtime.index(
            "type==SMATRIX_CMD && index->next!=nullptr")
        vector_selection = runtime.index(
            "type==VECTOR_CMD && selected_owner!=nullptr")
        self.assertLess(runtime.index("other_ring_value(current)",
                                      smatrix_selection),
                        runtime.index("selected_vector_component(",
                                      smatrix_selection))
        self.assertLess(runtime.index("other_ring_value(current)",
                                      vector_selection),
                        runtime.index("selected_vector_component(",
                                      vector_selection))
        matrix_column = runtime.index(
            "type==MATRIX_CMD && selected_owner!=nullptr")
        self.assertIn("position>MATCOLS(source)",
                      runtime[matrix_column:vector_selection])

        # Exact types retain their flag/attribute view. Cross conversions are
        # all constructed without a metadata argument.
        self.assertIn("IntmatView(data,value_flags_without_ring_marker(argument),",
                      intmat_argument)
        self.assertIn("converted=matrix_from_intvec(data,r);", matrix_argument)
        self.assertIn("converted=matrix_from_module(data,r);", matrix_argument)
        self.assertIn("type==VECTOR_CMD", matrix_argument)
        self.assertIn("type==SMATRIX_CMD", matrix_argument)
        self.assertIn("matrix_from_vector(data,r)", matrix_argument)
        self.assertIn("matrix_from_matrix_shaped(natural.get(),target_rows,",
                      matrix_argument)
        self.assertIn("if (!explicit_cast) id_Normalize((ideal)converted.data(),r);",
                      matrix_argument)
        self.assertNotIn("FLAG_QRING", runtime[
            runtime.index("inline Matrix matrix_from_module"):
            runtime.index("inline Matrix matrix_from_poly_shaped")])
        self.assertNotIn("value_flags_without_ring_marker(argument));\n    } else",
                         matrix_argument)

    def test_matrix_slice_write_and_reassignment_stay_interpreted(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"unsafe-matrix-slices.lib"
            library.write_text('''proc sliceWrite(matrix source,intvec rows)
{source[rows,1]=1;return(source);}
proc sliceReassign(matrix source,intvec rows)
{source=source[rows,1];return(source);}
''')
            for name in ("sliceWrite", "sliceReassign"):
                closure = compiler.Closure(root/(name+"-cache"), [root])
                closure.visit(str(library.resolve()), name)
                with self.assertRaises(compiler.CompileError):
                    closure.generate()

    def test_bigint_is_a_distinct_owned_type_through_the_native_abi(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            library = root/"bigint.lib"
            library.write_text('''proc floorLike(number a)
{
  bigint m=bigint(numerator(a));
  bigint n=bigint(denominator(a));
  return(number(m div n));
}
proc arithmetic(bigint a,bigint b,int exponent)
{
  bigint value=(a+b)*(a-b);
  return(list(value,a mod b,a^exponent,-a,a<b));
}
proc identity(bigint value){value=value+1;return(value);}
proc literal(){return(123456789012345678901234567890);}
''')
            closure = compiler.Closure(root/"cache", [root])
            for name in ("floorLike", "arithmetic", "identity", "literal"):
                closure.visit(str(library.resolve()), name)
            closure.generate()

            floor = closure.nodes[(str(library.resolve()), "floorLike")]
            arithmetic = closure.nodes[(str(library.resolve()), "arithmetic")]
            identity = closure.nodes[(str(library.resolve()), "identity")]
            literal = closure.nodes[(str(library.resolve()), "literal")]
            self.assertEqual(floor["result"], "number")
            self.assertIn("snative::bigint_div", floor["code"])
            self.assertIn("snative::number_from_bigint", floor["code"])
            self.assertEqual(arithmetic["params"][:2],
                             [["bigint", "a"], ["bigint", "b"]])
            for helper in ("snative::bigint_add", "snative::bigint_sub",
                           "snative::bigint_mul", "snative::bigint_mod",
                           "snative::bigint_power", "snative::bigint_neg",
                           "snative::bigint_compare"):
                self.assertIn(helper, arithmetic["code"])
            self.assertIn("snative::CowBigint", identity["code"])
            self.assertEqual(literal["result"], "bigint")
            self.assertIn("snative::bigint_from_string", literal["code"])

            entry = compiler.module_source(
                [arithmetic, identity, literal], "native_", [])
            self.assertIn("snative::BigintArgument", entry)
            self.assertIn("result->rtyp=BIGINT_CMD", entry)

    def test_enumnf_floor_compiles_with_direct_bigint_arithmetic(self):
        library = ROOT/"Singular/LIB/enumnf.lib"
        with tempfile.TemporaryDirectory() as directory:
            closure = compiler.Closure(Path(directory)/"cache", [library.parent])
            closure.visit(str(library.resolve()), "NNBfloor")
            closure.generate()
            node = closure.nodes[(str(library.resolve()), "NNBfloor")]
            self.assertEqual(node["result"], "number")
            self.assertIn("snative::bigint_from_number", node["code"])
            self.assertIn("snative::bigint_div", node["code"])
            self.assertIn("snative::number_from_bigint", node["code"])

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
