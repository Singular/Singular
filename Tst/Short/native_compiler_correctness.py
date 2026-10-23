#!/usr/bin/env python3
"""Build real C++ library closures; test native parity and persistent reuse.

Linux: python3 Tst/Short/native_compiler_correctness.py --prefix /path/to/install
Normal interpreter calls remain independent of the native module compiler.
"""
import argparse
import fcntl
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
TRANSLATOR = ROOT/"Singular/compiler/singular2cpp.py"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", type=Path, required=True)
    args = parser.parse_args()
    binary = args.prefix/"bin/Singular"
    checks = 0

    def check(condition, message):
        nonlocal checks
        checks += 1
        if not condition:
            raise AssertionError(message)

    python_unit = subprocess.run(
        [sys.executable, "-m", "unittest",
         "Tst.Short.native_compiler_benchmark_test",
         "Tst.Short.native_compiler_procedure_benchmark_test",
         "Tst.Short.native_compiler_index_test",
         "Tst.Short.native_compiler_compatibility_test",
         "Tst.Short.native_compiler_cache_test"],
        cwd=ROOT, capture_output=True, text=True, timeout=90)
    check(python_unit.returncode==0, python_unit.stdout+python_unit.stderr)

    with tempfile.TemporaryDirectory(prefix="singular-native-tests-") as temp:
        directory = Path(temp)
        cache = directory/"cache"

        def command(library, entries, extra=()):
            return [sys.executable, str(TRANSLATOR), str(library),
                    "--singular-prefix", str(args.prefix), "--cache-dir", str(cache), "--export-prefix", "native_"] + \
                   [arg for entry in entries for arg in ("--entry", entry)] + list(extra)

        def compile_lib(library, entries, extra=()):
            result = subprocess.run(command(library, entries, extra), capture_output=True, text=True, timeout=120)
            check(result.returncode==0, result.stderr)
            return json.loads(result.stdout)

        def singular(source, expected=None, extra_env=None):
            result = subprocess.run([str(binary), "-q", "--no-rc", "-r12345678"], input=source+"\nquit;\n",
                                    env=dict(os.environ, **(extra_env or {})),
                                    cwd=directory,
                                    capture_output=True, text=True, timeout=90)
            check(result.returncode==0 and not result.stderr, repr(result))
            if expected is not None:
                check(result.stdout==expected, result.stdout)
            return result.stdout

        # A field selection is represented as a Subexpr on a list-like
        # newstruct value at the C-procedure boundary. Materialize that field
        # without mutating the interpreter-owned object or invoking a generic
        # arithmetic dispatcher.
        field_fixture = directory/"field_boundary.lib"
        field_fixture.write_text('''proc fieldAt(intvec values,int position){return(values[position]);}
proc scalarAt(int value){return(value);}
proc matrixAt(intmat values){return(native_scalarAt(values[2,1]));}
proc nestedMatrixAt(list carrier){return(native_scalarAt(carrier[1][2,2]));}
proc polyFresh(poly value){return(jet(value,1));}
''')
        field_module = compile_lib(field_fixture, ["fieldAt", "scalarAt", "polyFresh"], ["--strict"])
        singular(f'''LIB "{field_module["module"]}";
newstruct("NativeCompilerFieldBox","intvec values");
NativeCompilerFieldBox fieldBox;
fieldBox.values=intvec(4,9);
print(native_fieldAt(fieldBox.values,2));
print(native_scalarAt(fieldBox.values[1]));
print(fieldBox.values==intvec(4,9));
intmat integerMatrix[2][2]=1,2,3,4;
print(Field_boundary::matrixAt(integerMatrix));
list matrixCarrier=list(integerMatrix);
print(Field_boundary::nestedMatrixAt(matrixCarrier));
print(integerMatrix[1,1]==1 and integerMatrix[1,2]==2 and
      integerMatrix[2,1]==3 and integerMatrix[2,2]==4);
ring fieldRing=0,(x,y),dp;
newstruct("NativeCompilerPolyBox","matrix values");
NativeCompilerPolyBox polyBox;
matrix polyMatrix[1][1]=x2+y;
polyBox.values=polyMatrix;
poly selected=polyBox.values[1,1];
print(native_polyFresh(selected)==Field_boundary::polyFresh(selected));
print(selected==x2+y);
''', "9\n4\n1\n3\n4\n1\n1\n1\n")

        # Native entries retain the exact interpreted source procinfo used by
        # help/example even after both package and top-level bindings are
        # rebound.  A same-basename cwd decoy catches accidental basename-only
        # loading, and the unrelated prefix catches name-derived lookup.
        source_directory = directory/"elsewhere"
        source_directory.mkdir()
        source_fixture = source_directory/"sourceownership.lib"
        source_fixture.write_text('''proc sourceName()
"native source ownership fixture"
{
  return(17);
}
example
{
  print("NATIVE_SOURCE_EXAMPLE_MARKER");
}
''')
        (directory/source_fixture.name).write_text('''proc sourceName()
"wrong cwd source fixture"
{
  return(99);
}
example
{
  print("WRONG_CWD_SOURCE_MARKER");
}
''')
        source_module = compile_lib(
            source_fixture, ["sourceName"],
            ["--strict", "--export-prefix", "unrelated_"])
        source_output = singular(f'''LIB "{source_module["module"]}";
proc sourceName=Sourceownership::sourceName;
Sourceownership::sourceName=unrelated_sourceName;
sourceName=unrelated_sourceName;
int sourceCallsBefore=unrelated_compilerCalls();
print("REBIND_CALLS");
print(Sourceownership::sourceName()==17);
print(sourceName()==17);
print(unrelated_compilerCalls()-sourceCallsBefore==2);
print("SOURCE_EXAMPLE_BEGIN");
example sourceName;
print("SOURCE_EXAMPLE_END");
print("COUNTER_EXAMPLE_BEGIN");
example unrelated_compilerCalls;
print("COUNTER_EXAMPLE_END");
''')
        check("REBIND_CALLS\n1\n1\n1\n" in source_output, source_output)
        check("WRONG_CWD_SOURCE_MARKER" not in source_output, source_output)
        check("SOURCE_EXAMPLE_BEGIN\n" in source_output and
              "SOURCE_EXAMPLE_END\n" in source_output and
              "COUNTER_EXAMPLE_BEGIN\n" in source_output and
              "COUNTER_EXAMPLE_END\n" in source_output, source_output)
        source_example = source_output.split("SOURCE_EXAMPLE_BEGIN\n", 1)[1].split(
            "SOURCE_EXAMPLE_END\n", 1)[0]
        counter_example = source_output.split("COUNTER_EXAMPLE_BEGIN\n", 1)[1].split(
            "COUNTER_EXAMPLE_END\n", 1)[0]
        check("// proc sourceName from lib "+str(source_fixture.resolve()) in source_example and
              "NATIVE_SOURCE_EXAMPLE_MARKER" in source_example, source_example)
        check([line.strip() for line in counter_example.splitlines() if line.strip()]==
              ["// proc unrelated_compilerCalls from lib compiled.so"], counter_example)

        library = ROOT/"Singular/LIB/absfact.lib"
        first = compile_lib(library, ["variableWithSmallestPositiveDegree"])
        check(first["stats"]["objects_compiled"]==3, first)
        manifest = json.loads(Path(first["manifest"]).read_text())
        check({n["procedure"] for n in manifest["procedures"]} ==
              {"partialDegree", "variableWithSmallestPositiveDegree"}, manifest)
        code = "\n".join(Path(n["source"]).read_text() for n in manifest["procedures"])
        check("while (true)" in code and "sn_partialDegree_" in code and
              "snative::degree" in code and "iiExprArith" not in code and "iiPStart" not in code, code)
        mtimes = {n["object"]: Path(n["object"]).stat().st_mtime_ns for n in manifest["procedures"]}
        warm = compile_lib(library, ["variableWithSmallestPositiveDegree"])
        check(warm["module"]==first["module"] and warm["stats"]["objects_compiled"]==0 and
              warm["stats"]["procedures_parsed"]==0 and warm["stats"]["libraries_indexed"]==0 and
              warm["stats"]["modules_linked"]==0, warm)
        check(all(Path(p).stat().st_mtime_ns==mtime for p, mtime in mtimes.items()), "warm build changed objects")
        helper = compile_lib(library, ["partialDegree"])
        check(helper["stats"]["objects_compiled"]==1 and helper["stats"]["objects_reused"]==1, helper)

        source = f'LIB "{library}";\nLIB "{first["module"]}";\nLIB "{helper["module"]}";\n'
        source += '''proc reference(poly p){return(AbsFact::variableWithSmallestPositiveDegree(p));}
proc referencePartial(poly p,int i){return(AbsFact::partialDegree(p,i));}
'''.replace("AbsFact::", "Absfact::")
        count = 0
        for index, (characteristic, order) in enumerate([(0,"dp"),(32003,"dp"),(0,"lp"),(0,"ds"),(0,"wp(1,2,3)")]):
            source += f"ring r{index}={characteristic},(x,y,z),{order};\npoly p;\n"
            for poly in ["0", "1", "x", "x2+y3", "x8+y5+z2", "x*y*z", "x2*y3+x*y+z7", "x5+y5+z5"]:
                source += f"p={poly};\nattrib(p,\"native_test\",23);\n"
                source += "print(reference(p)==native_variableWithSmallestPositiveDegree(p));\n"
                count += 1
                for i in range(1, 4):
                    source += f"print(referencePartial(p,{i})==native_partialDegree(p,{i}));\n"
                    count += 1
                source += f"print(p==({poly}));\nprint(attrib(p,\"native_test\")==23);\n"
                count += 2
        singular(source, "1\n"*count)
        check(count==240, count)

        # Run the real Short-suite case, substituting only our compiled roots.
        both = compile_lib(library, ["variableWithSmallestPositiveDegree", "partialDegree"])
        case = (ROOT/"Tst/Short/absfact.tst").read_text().rstrip()
        check(case.endswith("$"), "expected Short-suite termination marker")
        case = case[:-1]
        prelude = 'int tst_no_status=1; LIB "absfact.lib";\n'
        native_prelude = prelude + f'LIB "{both["module"]}";\n'
        native_prelude += 'Absfact::variableWithSmallestPositiveDegree=native_variableWithSmallestPositiveDegree;\n'
        native_prelude += 'Absfact::partialDegree=native_partialDegree;\n'
        baseline_prelude = prelude + '\n'*3  # keep diagnostic source lines identical
        reference = singular(baseline_prelude+case+'\nprint("NATIVE_COUNTER"); print(0);')
        actual = singular(native_prelude+case+'\nprint("NATIVE_COUNTER"); print(native_compilerCalls());')
        before, calls = actual.rsplit("NATIVE_COUNTER\n", 1)
        check(before==reference.rsplit("NATIVE_COUNTER\n", 1)[0], before)
        check(int(calls.strip())>0, "Short/absfact.tst did not execute native code")
        print(f"Short/absfact.tst: exact interpreter parity; {calls.strip()} native entry calls")

        deps = directory/"deps.lib"
        deps.write_text('proc leaf(poly p,int k){return(deg(p)+k+1);}\n')
        fixture = directory/"closure.lib"
        fixture.write_text('''LIB "deps.lib";
proc root(poly p,int n) {
  int total=0;
  for(int i=0;i<n;i++) { total=total+leaf(p,i); }
  return(total);
}
proc sibling(poly p){return(leaf(p,2));}
proc recursive(int n){if(n<=0){return(0);}else{return(mutual(n-1)+1);}}
proc mutual(int n){if(n<=0){return(0);}else{return(recursive(n-1)+1);}}
proc skipping(int n){int i=0;int total=0;for(i=0;i<n;i++){
  if(i==2){i=i+1;continue;}total=total+i;if(i==5){break;}}
  while(i<8){i++;total=total+1;}return(total);}
proc logic(int a,int b){return((!a)+(a and b)+(a or b)+(a<b)+(a==b));}
''')
        root = compile_lib(fixture, ["root"])
        sibling = compile_lib(fixture, ["sibling"])
        check(sibling["stats"]["objects_compiled"]==2 and sibling["stats"]["objects_reused"]==1, sibling)
        original_manifest = json.loads(Path(root["manifest"]).read_text())
        root_object = next(n["object"] for n in original_manifest["procedures"] if n["procedure"]=="root")
        deps.write_text(deps.read_text().replace("+k+1", "+k+2"))
        changed = compile_lib(fixture, ["root"])
        check(changed["module"]!=root["module"] and changed["stats"]["objects_compiled"]==1 and
              changed["stats"]["objects_reused"]==2 and changed["stats"]["modules_linked"]==1, changed)
        changed_manifest = json.loads(Path(changed["manifest"]).read_text())
        check(root_object==next(n["object"] for n in changed_manifest["procedures"] if n["procedure"]=="root"), changed_manifest)
        singular(f'LIB "{changed["module"]}";\nring r=0,(x,y),dp;\nprint(native_root(x2+y3,4));', "26\n")
        # Both images must bind their own helper, even when loaded together.
        isolated = compile_lib(fixture, ["root"], ["--export-prefix", "updated_"])
        singular(f'''LIB "{root["module"]}";
LIB "{isolated["module"]}";
ring r=0,(x,y),dp;
print(native_root(x2+y3,4));
print(updated_root(x2+y3,4));''', "22\n26\n")
        routines = compile_lib(fixture, ["recursive", "skipping", "logic"])
        source = f'LIB "{fixture}";\nLIB "{routines["module"]}";\nring r=0,(x,y),dp;\n'
        for n in range(12):
            source += f"print(recursive({n})==native_recursive({n}));\nprint(skipping({n})==native_skipping({n}));\n"
        for value in ["0", "-1", "2147483647", "2147483647+1", "(2147483647+1)*2"]:
            source += f"print(logic({value},1)==native_logic({value},1));\n"
        singular(source, "1\n"*29)

        # Polynomial ownership, typed closure inference, recursive returns and
        # direct kernel mappings. Library sources themselves are not rewritten.
        poly_deps = directory/"polydeps.lib"
        poly_deps.write_text('proc polyLeaf(poly p,int n){return(p^n+1);}\n')
        poly_fixture = directory/"polyclosure.lib"
        poly_fixture.write_text('''LIB "polydeps.lib";
proc polyRoot(poly p,int n){poly q=0;for(int i=0;i<n;i++){q=q+polyLeaf(p,i);}return(q);}
proc polyRec(poly p,int n){if(n<=0){return(poly(1));}else{return(p*polyMutual(p,n-1));}}
proc polyMutual(poly p,int n){return(polyRec(p,n));}
proc polyAlias(poly p){poly q=p;poly saved=q;q=q;q=q+q;q=q-q;return(saved);}
proc polyIdentity(poly p){return(p);}
proc polyTerms(poly p){poly q=0;for(int i=1;i<=size(p);i++){q=q+p[i];}return(q);}
proc polyTail(poly p){return(p-lead(p));}
proc polyScanIndex(poly p,int stop){int i;poly q=0;for(i=1;i<=size(p);i++){
  q=q+p[i];if(i==stop){break;}}return(i);}
proc polyScanReturn(poly p,int stop){for(int i=1;i<=size(p);i++){
  if(i==stop){return(p[i]);}}return(poly(0));}
proc polyScanUnsafe(poly p){poly q=p;poly out=0;for(int i=1;i<=size(q);i++){
  out=out+q[i];q=q;}return(out);}
proc polyHeads(poly p,int n){poly q=jet(p,n);return(q-lead(q)+leadmonom(q));}
proc polySigns(poly p){return(-p+2*p-3+poly(3));}
proc polyCompare(poly p){return((p==0)+(p!=1)+(0<>p));}
proc polyPowerSyntax(poly p){return(-p^2+p**2+(p^2)^3-p^2^3);}
proc polyIndex(poly p,int n){return(p[n]);}
proc polyFailure(poly p,int n){poly q=p+p;return(q^n);}
proc polyForwardTerm(poly p){return(polyIdentity(p)[1]);}
proc polyNames(poly p){poly call=p;poly declare=call;poly block=declare;return(block);}
proc polyAccumulate(poly p){poly q=p;q=q-lead(q);q=lead(q)+q;q=q+q;
  q=-q;q=q^2;q=jet(q,size(q));return(q);}
proc polyNestedUse(poly p){poly q=p;q=(q+1)+lead(q);return(q);}
''')
        entries = ["polyRoot", "polyRec", "polyAlias", "polyIdentity", "polyTerms", "polyTail",
                   "polyScanIndex", "polyScanReturn", "polyScanUnsafe", "polyHeads",
                   "polySigns", "polyCompare", "polyPowerSyntax", "polyIndex", "polyFailure", "polyForwardTerm", "polyNames",
                   "polyAccumulate", "polyNestedUse"]
        polys = compile_lib(poly_fixture, entries)
        poly_manifest = json.loads(Path(polys["manifest"]).read_text())
        check(len(poly_manifest["procedures"])==len(entries)+2, poly_manifest)
        poly_code = "\n".join(Path(n["source"]).read_text() for n in poly_manifest["procedures"])
        poly_sources = {n["procedure"]: Path(n["source"]).read_text()
                        for n in poly_manifest["procedures"]}
        accumulator_code = poly_sources["polyAccumulate"]
        nested_code = poly_sources["polyNestedUse"]
        terms_code = poly_sources["polyTerms"]
        return_scan_code = poly_sources["polyScanReturn"]
        unsafe_scan_code = poly_sources["polyScanUnsafe"]
        tail_code = poly_sources["polyTail"]
        check("snative::Poly sn_polyRoot" in poly_code and "sn_polyLeaf_" in poly_code and
              "poly v_p" in poly_code and "snative::Poly v_q = " in poly_code and
              "iiExprArith" not in poly_code, poly_code)
        check("snative::drop_lead(std::move(v_q))" in accumulator_code and
              "snative::add(std::move(v_q)," in accumulator_code and
              ", std::move(v_q))" in accumulator_code and
              "snative::neg(std::move(v_q))" in accumulator_code and
              "snative::power(std::move(v_q)," in accumulator_code and
              "snative::jet(std::move(v_q)," in accumulator_code and
              accumulator_code.index("pLength(v_q.get())") <
              accumulator_code.index("snative::jet(std::move(v_q)") and
              accumulator_code.count("snative::Poly::copy(v_q.get(), ctx.r)")==1,
              accumulator_code)
        check("snative::drop_lead(snative::Poly::copy(b_p.get(), ctx.r))" in tail_code and
              "pNext(" in terms_code and "pLength(v_p)" not in terms_code and
              "snative::term(v_p" not in terms_code and
              "pNext(" in return_scan_code and "snative::term(v_p" not in return_scan_code and
              "pLength(v_q.get())" in unsafe_scan_code and
              "snative::term(v_q.get()" in unsafe_scan_code,
              tail_code+terms_code+return_scan_code+unsafe_scan_code)
        # Nested consumption would invalidate the later lead(q); only the
        # outermost RHS operation may take ownership of the assignment target.
        check("snative::Poly::copy(v_q.get(), ctx.r)" in nested_code and
              "snative::add(std::move(v_q)" not in nested_code and
              "snative::sub(std::move(v_q)" not in nested_code, nested_code)
        poly_warm = compile_lib(poly_fixture, entries)
        check(poly_warm["stats"]["objects_compiled"]==0 and poly_warm["stats"]["modules_linked"]==0, poly_warm)

        real_polys = [("swalk", "initialForm"), ("goettsche", "geomS"), ("VecField", "productOfRingVars")]
        source = f'LIB "{poly_fixture}";\nLIB "{polys["module"]}";\n'
        # Call interpreted static helpers through public wrappers, comparing
        # actual unmodified library code. VecField is also a custom type name.
        for lib, entry in real_polys:
            compiled = compile_lib(ROOT/f"Singular/LIB/{lib}.lib", [entry])
            source += f'LIB "{lib}.lib";\nLIB "{compiled["module"]}";\n'
        source += '''proc refInitial(poly p){return(Swalk::initialForm(p));}
proc refGeom(poly p,int n){return(Goettsche::geomS(p,n));}
proc refProduct(){return(`"VecField"`::productOfRingVars());}
'''
        poly_count = 0
        for index, (characteristic, order) in enumerate([(0,"dp"),(32003,"dp"),(2,"dp"),(0,"lp"),(0,"ds"),(0,"wp(1,2,3)")]):
            source += f"ring pr{index}={characteristic},(x,y,z),{order};\npoly p; poly saved;\n"
            for value in ["0", "1", "x", "x2+y3", "x*y*z+2*x+y+1", "x3+y3+z2+1", "2*x2-3*y+5"]:
                source += f"p={value}; saved=p;\n"
                calls = ["polyRoot(p,5)", "polyRec(p,4)", "polyAlias(p)", "polyIdentity(p)",
                         "polyTerms(p)", "polyTail(p)", "polyScanIndex(p,2)", "polyScanIndex(p,999)",
                         "polyScanReturn(p,1)", "polyScanReturn(p,2)", "polyScanReturn(p,999)",
                         "polyScanUnsafe(p)", "polyHeads(p,2)", "polySigns(p)", "polyCompare(p)",
                         "polyPowerSyntax(p)", "polyIndex(p,-1)", "polyIndex(p,0)",
                         "polyIndex(p,1)", "polyIndex(p,999)", "polyForwardTerm(p)", "polyNames(p)",
                         "polyAccumulate(p)", "polyNestedUse(p)"]
                for call in calls:
                    source += f"print({call}==native_{call});\n"
                    poly_count += 1
                source += "print(refInitial(p)==native_initialForm(p));\n"
                source += "print(refGeom(p,5)==native_geomS(p,5));\n"
                source += "print(refProduct()==native_productOfRingVars());\nprint(p==saved);\n"
                poly_count += 4
            # Returned values must own their storage after inputs and other
            # result aliases are reassigned or destroyed.
            source += "p=x+y; saved=native_polyIdentity(p); p=0; print(saved==x+y);\n"
            source += "p=native_polyAlias(saved); saved=0; print(p==x+y);\n"
            poly_count += 2
        singular(source, "1\n"*poly_count)
        # Error cleanup: an owned local and power operand exist when this throws.
        failure_source = f'LIB "{polys["module"]}";\nring r=0,(x,y),dp; poly p=x+y;\n'
        failure_source += 'native_polyFailure(p,-1);\nprint(p==x+y);\nprint(native_polyFailure(p,2)==4*(x+y)^2);'
        failure = singular(failure_source)
        check("exponent must be non-negative" in failure and failure.endswith("1\n1\n"), failure)
        attributed = singular(f'''LIB "{polys["module"]}";
ring r=0,(x,y),dp; poly p=x+y; attrib(p,"test",23);
native_polyIdentity(p);
print(p==x+y); print(attrib(p,"test")==23);''')
        check("attributed poly arguments" in attributed and attributed.endswith("1\n1\n"), attributed)

        numbers_fixture = directory/"numbers.lib"
        numbers_fixture.write_text('''proc numIdentity(number a){return(a);}
proc numOps(number a,int n){number b=a;return((a+2)*(b-1)/(a+3)+a^n);}
proc numParts(number a){return(numerator(a)/denominator(a));}
proc numNeg(number a){number b=-a;return(-b);}
proc numCompare(number a,number b){return((a==b)+(a!=b)+(a<>b));}
proc numPoly(number a){poly p=a;return(p);}
proc polyNum(poly p){return(number(p));}
proc numDiff(number a,number b){return(diff(a,b));}
proc numDivide(number a,number b){number c=a+b;return(c/b);}
proc numList(number p,list xs){number total=p;for(int i=1;i<=size(xs);i++){
  total=total+numIdentity(xs[i]);}return(total);}
proc numBranch(number a,int n){if(n){number b=a+1;return(b);}else{number c=a-1;return(c);}}
proc numError(number a,int n){number b=a;if(n){return(b);}ERROR("number error test");}
proc numSelf(number a){number b=a;b=b;return(b);}
proc numAccumulate(number a,number b){number q=a;q=q+b;q=b+q;q=q*b;q=b*q;
  q=-q;q=q+q;q=q*q;return(q);}
proc numNested(number a){number q=a;q=(q+1)+numIdentity(q);return(q);}
proc polySub(poly p,poly x,poly q){return(subst(p,x,q));}
proc polyDiv(poly p,poly q){return(p/q);}
''')
        number_entries = ["numIdentity", "numOps", "numParts", "numNeg", "numCompare", "numPoly",
                          "polyNum", "numDiff", "numDivide", "numList", "numBranch", "numError", "numSelf",
                          "numAccumulate", "numNested",
                          "polySub", "polyDiv"]
        numbers = compile_lib(numbers_fixture, number_entries, ["--strict"])
        number_manifest = json.loads(Path(numbers["manifest"]).read_text())
        number_code = "\n".join(Path(n["source"]).read_text() for n in number_manifest["procedures"])
        number_entries_code = Path(number_manifest["entry_source"]).read_text()
        check("snative::Number v_b = snative::Number::copy" in number_code and
              "snative::NumberArgument" in number_entries_code and
              "snative::Number a0=snative::number_argument" not in number_entries_code,
              number_code+number_entries_code)
        num_self_code = next(Path(n["source"]).read_text() for n in number_manifest["procedures"]
                             if n["procedure"]=="numSelf")
        check(num_self_code.count("v_b =")==1, num_self_code)
        num_accumulator_code = next(Path(n["source"]).read_text() for n in number_manifest["procedures"]
                                    if n["procedure"]=="numAccumulate")
        num_nested_code = next(Path(n["source"]).read_text() for n in number_manifest["procedures"]
                               if n["procedure"]=="numNested")
        check("snative::number_add(std::move(v_q)," in num_accumulator_code and
              "snative::number_add(v_b, std::move(v_q)" in num_accumulator_code and
              "snative::number_mul(std::move(v_q)," in num_accumulator_code and
              "snative::number_mul(v_b, std::move(v_q)" in num_accumulator_code and
              "snative::number_neg(std::move(v_q), ctx.r)" in num_accumulator_code and
              num_accumulator_code.count("snative::Number::copy(v_q.get(), ctx.r)")==2,
              num_accumulator_code)
        check(num_nested_code.index("sn_numIdentity_") <
              num_nested_code.rindex("snative::number_add(std::move(") and
              "snative::number_add(std::move(v_q)" not in num_nested_code,
              num_nested_code)
        number_count = 0
        source = f'LIB "{numbers_fixture}";\nLIB "{numbers["module"]}";\n'
        for index, characteristic in enumerate(["0", "32003", "(0,a,b)", "(32003,a,b)"]):
            source += f"ring nr{index}={characteristic},(x,y),dp; number a0; number saved; list xs; poly q;\n"
            for value in (["1", "2/3", "-2"] if "a,b" not in characteristic else
                          ["1", "(a+b)/(a-b)", "a^2/(1+b)", "1/(a^3*(1-a))"]):
                source += f"a0={value}; saved=a0; xs=list(a0,2,a0+1);\n"
                calls = ["numIdentity(a0)", "numParts(a0)", "numNeg(a0)", "numPoly(a0)",
                         "numCompare(a0,saved)", "numCompare(a0,a0+1)", "numList(a0,xs)",
                         "numBranch(a0,0)", "numBranch(a0,1)", "numError(a0,1)", "numSelf(a0)",
                         "numAccumulate(a0,saved)", "numNested(a0)"]
                calls += [f"numOps(a0,{n})" for n in [0,1,3,-2]]
                for call in calls:
                    source += f"print({call}==native_{call});\n"
                    number_count += 1
                source += 'print(a0==saved); print(xs[1]==saved);\n'
                source += 'saved=native_numIdentity(a0); a0=0; print(saved==xs[1]);\n'
                number_count += 3
            for value in ["0", "1", "x+y", "x2+2*x*y+y2"]:
                source += f"q={value};\n"
                for call in ["polyNum(q)", "polySub(q,x,y+1)", "polySub(q,x,poly(0))", "polyDiv(q,x+y)"]:
                    source += f"print({call}==native_{call});\n"
                    number_count += 1
            source += 'print(native_numIdentity(7)==number(7)); print(native_numList(7,list())==number(7));\n'
            number_count += 2
            source += 'list nested=list(list(number(2),3));\n'
            source += 'print(native_numList(1,nested[1])==numList(1,nested[1]));\n'
            source += 'print(native_numIdentity(nested[1][1])==numIdentity(nested[1][1]));\n'
            source += 'print(native_numIdentity(nested[1][2])==numIdentity(nested[1][2]));\n'
            number_count += 3
        singular(source, "1\n"*number_count)
        number_errors = singular(f'''LIB "{numbers["module"]}";
ring r=0,(x,y),dp; number a=2;
native_numDivide(a,0);
native_numDiff(a,a);
native_numList(a,list(1,"bad"));
native_numIdentity(x);
native_numList(a,list(1)[2]);
native_numError(a,0);
print("RECOVERED"); print(a==2); print(native_numList(a,list(3,4))==9);
''')
        for message in ["div. by 0", "differentiation not defined", "number or int", "number error test"]:
            check(message in number_errors, number_errors)
        check(number_errors.endswith("RECOVERED\n1\n1\n"), number_errors)

        # Unmodified real-library closure: rational-function differentiation,
        # parameter substitution, polynomial division, and borrowed list input.
        elliptic = compile_lib(ROOT/"Singular/LIB/ellipticcovers.lib", ["evaluateIntegral"], ["--strict"])
        check(set(elliptic["native_exports"])=={"native_computeConstant", "native_evaluateIntegral"}, elliptic)
        elliptic_manifest = json.loads(Path(elliptic["manifest"]).read_text())
        elliptic_code = "\n".join(Path(n["source"]).read_text() for n in elliptic_manifest["procedures"])
        check("sn_computeConstant_" in elliptic_code and "snative::number_diff" in elliptic_code and
              not any(s in elliptic_code for s in ["iiExprArith", "iiMake_proc", "iiPStart", "execute("]), elliptic_code)
        elliptic_warm = compile_lib(ROOT/"Singular/LIB/ellipticcovers.lib", ["evaluateIntegral"], ["--strict"])
        check(elliptic_warm["stats"]["objects_compiled"]==0 and elliptic_warm["stats"]["modules_linked"]==0, elliptic_warm)
        source = f'LIB "ellipticcovers.lib";\nLIB "{elliptic["module"]}";\n'
        for index, characteristic in enumerate(["0", "32003"]):
            source += f"ring er{index}=({characteristic},a,b),(x,y),dp; number f; number saved; list xs=a,b;\n"
            for value in ["0", "1", "a", "1/a", "1/(1-a)", "1/(a^6*(1-a))",
                          "(1+a+a2)/(a3*(1-a))", "b/(a*(b-a))", "(a+b)/(a2*b3*(1-a)*(1-b))"]:
                source += f"f={value}; saved=f;\n"
                for call in ["computeConstant(f,a)", "computeConstant(f,b)", "evaluateIntegral(f,xs)"]:
                    source += f"print({call}==native_{call});\n"
                    number_count += 1
                source += 'print(f==saved); print(xs[1]==a and xs[2]==b);\n'
                number_count += 2
        singular(source, "1\n"*90)
        print("ellipticcovers.lib: native computeConstant + evaluateIntegral closure, 90 comparisons")
        elliptic = compile_lib(ROOT/"Singular/LIB/ellipticcovers.lib",
                               ["evaluateIntegral", "permute", "propagator"], ["--strict"])
        check(set(elliptic["native_exports"])=={"native_computeConstant", "native_evaluateIntegral",
                                               "native_permute", "native_propagator"} and not elliptic["aliases"], elliptic)
        native_manifest = json.loads(Path(elliptic["manifest"]).read_text())
        native_code = "\n".join(Path(n["source"]).read_text() for n in native_manifest["procedures"])
        check("sn_permute_" in native_code and "sn_propagator_" in native_code and
              "snative::parameter_number" in native_code and "snative::indexed_number" in native_code and
              "snative::type_is" in native_code and "snative::type_name" not in native_code and
              "snative::list_set_owned" in native_code and
              not any(s in native_code for s in ["iiExprArith", "iiMake_proc", "iiPStart", "execute("]), native_code)
        warmed = compile_lib(ROOT/"Singular/LIB/ellipticcovers.lib",
                             ["evaluateIntegral", "permute", "propagator"], ["--strict"])
        check(warmed["stats"]["objects_compiled"]==warmed["stats"]["modules_linked"]==0, warmed)
        list_count = 0
        source = f'LIB "ellipticcovers.lib"; LIB "{elliptic["module"]}";\n'
        # List-only entry also works before any ring exists. Check exact order,
        # nesting, arbitrary element types, duplicates and input preservation.
        source += 'list N; string savedList;\n'
        for value in ["list()", "list(5)", "list(1,2)", "list(1,2,3,4,5)",
                      'list("alpha","beta",17)', 'list(list(1,2),list(3),"end")', "list(1,1,2)"]:
            source += f'N={value}; savedList=string(N);\n'
            source += 'print(string(native_permute(N))==string(permute(N))); print(string(N)==savedList);\n'
            list_count += 2
        for index, characteristic in enumerate(["0", "32003"]):
            source += f'ring pr{index}=({characteristic},a,b,c),(x,y),dp;\n'
            source += 'list points=a,b; graph G=makeGraph(list(1,2,3),list(list(1,2),list(2,3),list(1,3)));\n'
            source += 'string savedEdges=string(G.edges);\n'
            for degree in range(13):
                source += f'print(native_propagator(points,{degree})==propagator(points,{degree}));\n'
                list_count += 1
            for degree in ["list(0,0,0)", "list(0,1,2)", "list(1,2,3)", "list(3,1,0)"]:
                source += f'print(native_propagator(G,{degree})==propagator(G,{degree}));\n'
                list_count += 1
            source += 'intvec branchType=0,1,2;\n'
            source += 'print(native_propagator(G,branchType)==propagator(G,branchType));\n'
            source += 'list branches=partitions(3,2);\n'
            source += 'for(int branch=1;branch<=size(branches);branch++){print(native_propagator(G,branches[branch])==propagator(G,branches[branch]));}\n'
            source += 'kill branchType; kill branches; kill branch;\n'
            list_count += 7
            for value in ['list(x+y,x2,number(a/b))', 'list(a,b,c)', 'list(G,list(x,a),"tail")']:
                source += f'list values={value}; string before=string(values);\n'
                source += 'print(string(native_permute(values))==string(permute(values))); print(string(values)==before);\n'
                source += 'kill values; kill before;\n'
                list_count += 2
            source += 'print(string(G.edges)==savedEdges); print(points[1]==a and points[2]==b);\n'
            source += 'kill G; kill savedEdges;\n'
            list_count += 2
        singular(source, "1\n"*list_count)
        number_count += list_count
        errors = singular(f'''LIB "ellipticcovers.lib"; LIB "{elliptic["module"]}";
ring r=(0,a,b),(x,y),dp;
native_propagator(list(a,b),-1);
native_propagator(list(a),1);
native_propagator(list(a,b),list(1));
native_propagator("bad","bad");
graph G=makeGraph(list(1,2),list(list(1,2)));
native_propagator(G,list());
native_propagator(G,1);
print("RECOVERED_LISTS");
print(native_propagator(G,list(1))==propagator(G,list(1)));
print(string(native_permute(list(1,2,3)))==string(permute(list(1,2,3))));
''')
        check("signal" not in errors and "expected non-negative degree" in errors and
              "wrong input type" in errors and errors.endswith("RECOVERED_LISTS\n1\n1\n"), errors)
        print(f"ellipticcovers.lib: native permute + both propagator overloads, {list_count} comparisons")
        list_fixture = directory/"list_kernels.lib"
        list_fixture.write_text('''proc copyList(list input){return(input);}
proc replaceList(list input){list L=input;L[1]=L[2];return(L);}
proc selfAppend(list input){list L=input;L[size(L)+1]=L;return(L);}
proc growList(list input){list L=input;L[size(L)+3]=list(8,9);return(L);}
proc joinLists(list first,list second){return(first+second);}
proc deleteItem(list input,int index){return(delete(input,index));}
proc remainderOf(int first,int second){return(first%second);}
proc selfList(list input){list L=input;L=L;return(L);}
proc setOwnedScalars(list input,poly p,number n){list L=input;L[1]=p+p;L[2]=n+n;return(L);}
proc ownedLiteral(list input,poly p,number n){return(list(p+p,n+n,list(8,9),input));}
proc appendLiteral(list input){list L=input;L=L+list(8,9);return(L);}
proc doubleList(list input){list L=input;L=L+L;return(L);}
proc nestedConcat(list input){return(list(1,2)+list(3,4)+input);}
proc growSequential(int n){list L;for(int i=1;i<=n;i++){L[i]=i;}return(L);}
proc listFailure(list input,int fail){list L=input;L[size(L)+1]=L;
if(fail){ERROR("list cleanup test");}return(L);}
''')
        list_module = compile_lib(list_fixture, [], ["--strict", "--export-prefix", "lk_"])
        list_manifest = json.loads(Path(list_module["manifest"]).read_text())
        list_code = "\n".join(Path(n["source"]).read_text() for n in list_manifest["procedures"])
        check("snative::List v_L = snative::List::copy" in list_code and
              "snative::list_set_owned" in list_code and "snative::list_set(" in list_code, list_code)
        self_list_code = next(Path(n["source"]).read_text() for n in list_manifest["procedures"]
                              if n["procedure"]=="selfList")
        check(self_list_code.count("v_L =")==1, self_list_code)
        scalar_list_code = next(Path(n["source"]).read_text() for n in list_manifest["procedures"]
                                if n["procedure"]=="setOwnedScalars")
        check(scalar_list_code.count("snative::list_set_owned")==2, scalar_list_code)
        literal_code = next(Path(n["source"]).read_text() for n in list_manifest["procedures"]
                            if n["procedure"]=="ownedLiteral")
        append_code = next(Path(n["source"]).read_text() for n in list_manifest["procedures"]
                           if n["procedure"]=="appendLiteral")
        double_code = next(Path(n["source"]).read_text() for n in list_manifest["procedures"]
                           if n["procedure"]=="doubleList")
        nested_concat_code = next(Path(n["source"]).read_text() for n in list_manifest["procedures"]
                                  if n["procedure"]=="nestedConcat")
        check(literal_code.count("snative::list_set_owned")==3 and
              "reserve(4L)" in literal_code and "snative::list_set(" in literal_code and
              "snative::list_of" not in literal_code,
              literal_code)
        check("snative::list_add(std::move(v_L)," in append_code and
              "snative::list_add(std::move(v_L)," in double_code and
              double_code.index("snative::List::copy(v_L.get(), ctx.r)") <
              double_code.index("snative::list_add(std::move(v_L)") and
              nested_concat_code.count("snative::list_add(std::move(")>=1,
              append_code+double_code+nested_concat_code)
        source = f'LIB "{list_fixture}"; LIB "{list_module["module"]}"; ring lr=(0,a),(x,y),dp;\n'
        source += 'list input=list(list(x,a),"text",4); string original=string(input);\n'
        kernel_count = 0
        for call in ['copyList(input)', 'replaceList(input)', 'selfAppend(input)', 'selfAppend(list())',
                     'growList(input)', 'growList(list())', 'joinLists(input,input)',
                     'joinLists(list(),input)', 'deleteItem(input,1)', 'deleteItem(input,3)',
                     'listFailure(input,0)', 'copyList(list(input)[1])', 'selfList(input)',
                     'setOwnedScalars(input,x+y,a)', 'ownedLiteral(input,x+y,a)',
                     'appendLiteral(input)', 'doubleList(input)', 'nestedConcat(input)',
                     'growSequential(257)']:
            source += f'print(string(lk_{call})==string({call})); print(string(input)==original);\n'
            kernel_count += 2
        source += '''list marked=list(x,y); attrib(marked[1],"tag",7);
list markedRef=appendLiteral(marked); list markedNative=lk_appendLiteral(marked);
print(typeof(attrib(markedRef[1],"tag"))==typeof(attrib(markedNative[1],"tag")));
print(attrib(marked[1],"tag")==7);
markedNative=lk_ownedLiteral(marked,x+y,a); print(attrib(markedNative[4][1],"tag")==7);
'''
        kernel_count += 3
        for a, b in [(8,3),(-8,3),(8,-3),(-8,-3),(2147483647,7),(2147483647*4,7)]:
            # Large values are built by int arithmetic, not out-of-range literals.
            lhs = str(a) if abs(a)<=2147483647 else "2147483647*4"
            source += f'print(lk_remainderOf({lhs},{b})==remainderOf({lhs},{b}));\n'
            kernel_count += 1
        singular(source, "1\n"*kernel_count)
        number_count += kernel_count
        rejected = singular(f'''LIB "{list_module["module"]}";
ring r=0,(x,y),dp; list input=list(x,y); string saved=string(input);
lk_listFailure(input,1);
lk_deleteItem(input,0);
lk_deleteItem(input,3);
lk_remainderOf(1,0);
attrib(input,"tag",7);
lk_copyList(input);
print("LIST_RECOVERY"); print(attrib(input,"tag")==7); print(string(input)==saved);
killattrib(input,"tag"); print(string(lk_copyList(input))==saved);
''')
        check("list cleanup test" in rejected and "attributed list values" in rejected and
              rejected.endswith("LIST_RECOVERY\n1\n1\n1\n") and "signal" not in rejected, rejected)

        # Milestone 2: owned intvec values, exact scalar/kernel mappings,
        # heterogeneous list #, and lazy copies for mutated typed parameters.
        typed_fixture = directory/"typed_milestone2.lib"
        typed_fixture.write_text('''proc ivSequence(intvec input,int k){intvec v=1,2,input,4..2;v[2]=k;return(v);}
proc ivConstruct(intvec input){return(intvec(1,input,3..1));}
proc ivSlice(intvec input,int first,int last){intvec selected=input[first..last];return(selected);}
proc ivEmptySlice(intvec input){intvec empty=delete(1..1,1);intvec selected=input[empty];return(selected);}
proc ivDelete(intvec input,int position){return(delete(input,position));}
proc ivCompare(intvec input,int scalar){return((input<scalar)+2*(input<=scalar)+4*(input>scalar)+8*(input>=scalar)+16*(input==scalar));}
proc intvecIdentity(intvec input){return(input);}
proc dynamicToIntvec(def value){return(intvecIdentity(value));}
proc cowIntvec(intvec v){v[1]=v[1]+10;v=v+1;return(v);}
proc cowPoly(poly p){p=p+1;return(p);}
proc cowNumber(number n){n=n+1;return(n);}
proc cowList(list L){L[1]="changed";return(L);}
proc cowListAppend(list L){L=L+list("tail");return(L);}
proc listSequence(list L){L=1,"two",L,3..1;return(L);}
proc kernelData(poly p,number n,list L,def value,int position){
  return(list(leadcoef(p),leadexp(p),int(n),insert(L,value,position)));}
proc numberOrder(number a,number b){return((a<b)+2*(a<=b)+4*(a>b)+8*(a>=b)+16*(a==b)+32*(a!=b));}
proc integerOps(int a,int b){return(100*(a div b)+(a mod b));}
proc dynamicNumberReturn(int branch,list values){if(branch){return(number(1));}return(values[1]);}
proc pack(list #){return(#);}
proc packWithLocal(list #){int args=7;return(list(args,#));}
proc packFromNative(){return(pack(1,"text",list(2,3),3..1));}
proc forwardPack(def value){return(pack(value));}
proc dynamicSelector(intvec input,list selectors){intvec out=input[selectors[1]];return(out);}
proc dynamicExpressionSelector(intvec input,list selectors){intvec out=(input+1)[selectors[1]];return(out);}
proc packNested(list carrier){return(list(carrier[1][1]));}
''')
        typed_entries = ["ivSequence", "ivConstruct", "ivSlice", "ivEmptySlice", "ivDelete", "ivCompare", "intvecIdentity", "dynamicToIntvec",
                         "cowIntvec", "cowPoly", "cowNumber", "cowList", "cowListAppend", "listSequence",
                         "kernelData", "numberOrder", "integerOps", "dynamicNumberReturn",
                         "pack", "packWithLocal", "packFromNative",
                         "forwardPack", "dynamicSelector", "dynamicExpressionSelector", "packNested"]
        typed = compile_lib(typed_fixture, typed_entries, ["--strict", "--export-prefix", "m2_"])
        typed_manifest = json.loads(Path(typed["manifest"]).read_text())
        typed_code = "\n".join(Path(n["source"]).read_text() for n in typed_manifest["procedures"])
        typed_entry_code = Path(typed_manifest["entry_source"]).read_text()
        for expected in ["snative::CowIntvec", "snative::CowPoly", "snative::CowNumber",
                         "snative::CowList", "snative::intvec_range", "snative::intvec_select",
                         "snative::intvec_delete", "snative::leadcoef",
                         "snative::leadexp", "snative::number_to_int", "n_Greater(",
                         "snative::list_insert", "snative::quotient", "snative::remainder"]:
            check(expected in typed_code, typed_code)
        check("snative::arguments_list" in typed_entry_code and "snative::IntvecArgument" in typed_entry_code and
              "snative::scalar_index_argument" in typed_entry_code and
              not any(name in typed_code for name in ["iiExprArith", "iiPStart"]),
              typed_code+typed_entry_code)
        typed_sources = {node["procedure"]: Path(node["source"]).read_text()
                         for node in typed_manifest["procedures"]}
        check("std::move(c_p)" not in typed_sources["cowPoly"] and
              "std::move(c_n)" not in typed_sources["cowNumber"] and
              "std::move(c_L)" not in typed_sources["cowListAppend"] and
              "snative::List::copy(c_L.get(), ctx.r)" in typed_sources["cowListAppend"],
              typed_sources["cowPoly"]+typed_sources["cowNumber"]+typed_sources["cowListAppend"])
        check("snative::list_set_argument" in typed_sources["forwardPack"],
              typed_sources["forwardPack"])
        check("snative::exact_number_argument" in typed_sources["dynamicNumberReturn"],
              typed_sources["dynamicNumberReturn"])

        dynamic_variadic = directory/"dynamic_variadic.lib"
        dynamic_variadic.write_text('''proc dynamicFirst(list #){return(#[1]);}
proc dynamicInt(def value){return(int(value));}
proc namedArgs(list #){return(args);}
proc nestedInsert(list L){return(insert(L,(1,2)));}
proc nestedSelector(intvec v){intvec out=v[(1,2)];return(out);}
proc defSequence(){def value=(1,2);return(1);}
proc listSlotSequence(list L){L[1]=(1,2);return(L);}
proc mixedSequenceReturn(int b){if(b){return(intvec(3));}return((1,2));}
proc sequenceIndexSource(intvec v,intvec w){intvec out=(v,w)[1];return(out);}
''')
        dynamic_result = compile_lib(dynamic_variadic,
                                     ["dynamicFirst", "dynamicInt", "namedArgs", "nestedInsert", "nestedSelector",
                                      "defSequence", "listSlotSequence", "mixedSequenceReturn", "sequenceIndexSource"],
                                     ["--export-prefix", "m2_"])
        check(dynamic_result["native_exports"]==[] and
              {entry["procedure"] for entry in dynamic_result["aliases"]}==
              {"dynamicFirst", "dynamicInt", "namedArgs", "nestedInsert", "nestedSelector", "defSequence",
               "listSlotSequence", "mixedSequenceReturn", "sequenceIndexSource"},
              dynamic_result)

        milestone_count = 0
        source = f'LIB "{typed_fixture}"; LIB "{typed["module"]}";\n'
        # Heterogeneous # and intvec-only entries do not require a basering.
        for call in ['pack()', 'pack(1,"alpha",list(2,3),3..1)',
                     'packWithLocal(1,"alpha")', 'packFromNative()']:
            source += f'print(string(m2_{call})==string({call}));\n'
            milestone_count += 1
        source += '''list packed=m2_pack(1,"alpha",list(2,3),3..1);
print(size(packed)==4 and typeof(packed[3])=="list" and typeof(packed[4])=="intvec");
packed=m2_packFromNative();
print(size(packed)==4 and typeof(packed[3])=="list" and typeof(packed[4])=="intvec");
string indexedString="abcd"; string savedIndexedString=indexedString;
print(string(m2_pack(indexedString[2]))==string(pack(indexedString[2])));
print(string(m2_forwardPack(indexedString[2]))==string(forwardPack(indexedString[2])));
intvec stringIndexes=1,5,3;
print(string(m2_pack(indexedString[stringIndexes]))==string(pack(indexedString[stringIndexes])));
list indexedStringCarrier=list(indexedString); string savedIndexedStringCarrier=string(indexedStringCarrier);
print(string(m2_pack(indexedStringCarrier[1][3]))==string(pack(indexedStringCarrier[1][3])));
print(string(m2_packNested(indexedStringCarrier))==string(packNested(indexedStringCarrier)));
print(string(m2_pack(indexedString[0]))==string(pack(indexedString[0])));
print(indexedString==savedIndexedString);
print(string(indexedStringCarrier)==savedIndexedStringCarrier);
'''
        milestone_count += 10
        source += '''print(m2_ivConstruct(5)==ivConstruct(5));
list scalarVectorArgument=list(6);print(m2_ivConstruct(scalarVectorArgument[1])==ivConstruct(scalarVectorArgument[1]));
print(m2_dynamicToIntvec(5)==dynamicToIntvec(5));
intvec dynamicVectorValue=8,9;print(m2_dynamicToIntvec(dynamicVectorValue)==dynamicToIntvec(dynamicVectorValue));
print(m2_dynamicToIntvec(scalarVectorArgument[1])==dynamicToIntvec(scalarVectorArgument[1]));
list nestedVectorArgument=list(intvec(6,7));
print(m2_ivConstruct(nestedVectorArgument[1][1])==ivConstruct(nestedVectorArgument[1][1]));
print(m2_dynamicToIntvec(nestedVectorArgument[1][1])==dynamicToIntvec(nestedVectorArgument[1][1]));
print(string(m2_pack(nestedVectorArgument[1][1]))==string(pack(nestedVectorArgument[1][1])));
intvec dynamicSelectInput=10,20;list dynamicSelectors=list(intvec(2,1));
print(m2_dynamicSelector(dynamicSelectInput,dynamicSelectors)==dynamicSelector(dynamicSelectInput,dynamicSelectors));
list dynamicScalarSelector=list(2);
print(m2_dynamicExpressionSelector(dynamicSelectInput,dynamicScalarSelector)==dynamicExpressionSelector(dynamicSelectInput,dynamicScalarSelector));
'''
        milestone_count += 10
        source += '''intvec outside=1,2,3;intvec outsideSaved=outside;
print(m2_ivSequence(outside,4)==ivSequence(outside,4));
print(m2_ivSequence(outside,scalarVectorArgument[1])==ivSequence(outside,scalarVectorArgument[1]));
print(m2_ivSequence(outside,outside[1])==ivSequence(outside,outside[1]));
print(m2_ivConstruct(outside)==ivConstruct(outside));
print(m2_ivConstruct(outside[1])==ivConstruct(outside[1]));
print(string(m2_pack(outside[1]))==string(pack(outside[1])));
print(m2_dynamicToIntvec(outside[1])==dynamicToIntvec(outside[1]));
print(m2_ivSlice(outside,1,3)==ivSlice(outside,1,3));
print(m2_ivSlice(outside,3,1)==ivSlice(outside,3,1));
print(m2_ivEmptySlice(outside)==ivEmptySlice(outside));
print(m2_ivDelete(outside,2)==ivDelete(outside,2));
print(m2_ivCompare(outside,2)==ivCompare(outside,2));
intvec equalValues=2,2,2;print(m2_ivCompare(equalValues,2)==ivCompare(equalValues,2));
print(m2_ivCompare(outside,2147483647+1)==ivCompare(outside,2147483647+1));
print(m2_cowIntvec(outside)==cowIntvec(outside));
intvec discardedCowIntvec=m2_cowIntvec(outside);print(outside==outsideSaved);
'''
        milestone_count += 16
        for index, characteristic in enumerate(["0", "7", "(0,a,b)"]):
            source += f'ring m2r{index}={characteristic},(x,y,z),dp;\n'
            source += 'poly p=x2+2*x*y+3;poly savedP=p;number n;number savedN;list L=list(x,"old",3);string savedL=string(L);\n'
            source += '''print(m2_cowNumber(outside[1])==cowNumber(outside[1]));
print(m2_cowNumber(nestedVectorArgument[1][1])==cowNumber(nestedVectorArgument[1][1]));
ideal indexedIdeal=x,y; string savedIndexedIdeal=string(indexedIdeal);
print(string(m2_pack(indexedIdeal[1]))==string(pack(indexedIdeal[1])));
list indexedIdealCarrier=list(indexedIdeal);
print(string(m2_pack(indexedIdealCarrier[1][1]))==string(pack(indexedIdealCarrier[1][1])));
print(string(m2_packNested(indexedIdealCarrier))==string(packNested(indexedIdealCarrier)));
map indexedMap=basering,x,y,z;
print(string(m2_pack(indexedMap[1]))==string(pack(indexedMap[1])));
list indexedMapCarrier=list(indexedMap);
print(string(m2_pack(indexedMapCarrier[1][1]))==string(pack(indexedMapCarrier[1][1])));
print(string(m2_packNested(indexedMapCarrier))==string(packNested(indexedMapCarrier)));
print(m2_cowPoly(indexedIdeal[1])==cowPoly(indexedIdeal[1]));
print(m2_cowPoly(indexedIdealCarrier[1][1])==cowPoly(indexedIdealCarrier[1][1]));
print(m2_cowPoly(indexedMap[1])==cowPoly(indexedMap[1]));
print(string(indexedIdeal)==savedIndexedIdeal);
list indexedPolyCarrier=list(x); string savedIndexedPolyCarrier=string(indexedPolyCarrier);
print(m2_cowPoly(indexedPolyCarrier[1])==cowPoly(indexedPolyCarrier[1]));
print(string(indexedPolyCarrier)==savedIndexedPolyCarrier);
'''
            milestone_count += 14
            values = (["3/2", "-3/2", "2147483647+1"] if characteristic=="0" else
                      ["6", "3", "0"] if characteristic=="7" else ["a/b", "a+1", "0"])
            for value in values:
                source += f'n={value};savedN=n;\n'
                for call in ["cowNumber(n)", "kernelData(p,n,L,\"new\",5)",
                             "kernelData(poly(0),n,list(),x,0)"]:
                    source += f'print(string(m2_{call})==string({call}));\n'
                    milestone_count += 1
                source += 'print(n==savedN);\n'
                milestone_count += 1
            source += 'print(m2_cowPoly(p)==cowPoly(p));print(p==savedP);\n'
            source += '''list dynamicNumberValues=list(n);
print(typeof(m2_dynamicNumberReturn(0,dynamicNumberValues))==typeof(dynamicNumberReturn(0,dynamicNumberValues)));
print(m2_dynamicNumberReturn(0,dynamicNumberValues)==dynamicNumberReturn(0,dynamicNumberValues));
print(m2_dynamicNumberReturn(1,dynamicNumberValues)==dynamicNumberReturn(1,dynamicNumberValues));
'''
            source += 'print(string(m2_cowList(L))==string(cowList(L)));print(string(L)==savedL);\n'
            source += 'print(string(m2_cowListAppend(L))==string(cowListAppend(L)));print(string(L)==savedL);\n'
            source += 'print(string(m2_listSequence(L))==string(listSequence(L)));print(string(L)==savedL);\n'
            source += '''list insertCarrier=list(x+y);string savedCarrier=string(insertCarrier);
list referenceInserted=kernelData(p,n,L,insertCarrier[1],1);
list nativeInserted=m2_kernelData(p,n,L,insertCarrier[1],1);
print(string(nativeInserted)==string(referenceInserted));print(string(insertCarrier)==savedCarrier);
'''
            milestone_count += 13
            for left, right in [(0,0),(1,2),(2,1),(-1,1)]:
                source += f'print(m2_numberOrder(number({left}),number({right}))==numberOrder(number({left}),number({right})));\n'
                milestone_count += 1
            source += ('kill p,savedP,n,savedN,L,savedL,indexedIdeal,savedIndexedIdeal,'
                       'indexedIdealCarrier,indexedMap,indexedMapCarrier,indexedPolyCarrier,'
                       'savedIndexedPolyCarrier,dynamicNumberValues,insertCarrier,savedCarrier,'
                       'referenceInserted,nativeInserted;\n')
        for left, right in [(17,5),(-17,5),(17,-5),(-17,-5)]:
            source += f'print(m2_integerOps({left},{right})==integerOps({left},{right}));\n'
            milestone_count += 1
        singular(source, "1\n"*milestone_count)
        number_count += milestone_count
        dynamic_output = singular(f'''LIB "{dynamic_variadic}"; LIB "{dynamic_result["module"]}";
print(string(m2_dynamicFirst("kept dynamic"))==string(dynamicFirst("kept dynamic")));
list nestedInput=list(9); print(string(m2_nestedInsert(nestedInput))==string(nestedInsert(nestedInput)));
intvec nestedVector=10,20; print(m2_nestedSelector(nestedVector)==nestedSelector(nestedVector));
ring dynamicRing=0,(x),dp; number dynamicNumber=3/2;
print(m2_dynamicInt(dynamicNumber)==dynamicInt(dynamicNumber));''')
        check("interpreted alias" in dynamic_output and dynamic_output.endswith("1\n1\n1\n1\n"), dynamic_output)
        undefined_variadic = singular(f'''LIB "{typed["module"]}";
def undefinedValue; m2_pack(undefinedValue);
print("VARIADIC_RECOVERY"); print(size(m2_pack(1))==1);''')
        check("undefined" in undefined_variadic and undefined_variadic.endswith("VARIADIC_RECOVERY\n1\n"),
              undefined_variadic)
        internal_undefined_variadic = singular(f'''LIB "{typed_fixture}"; LIB "{typed["module"]}";
def undefinedValue; m2_forwardPack(undefinedValue);
print("INTERNAL_VARIADIC_RECOVERY");
print(string(m2_forwardPack(7))==string(forwardPack(7)));''')
        check("undefined" in internal_undefined_variadic and
              internal_undefined_variadic.endswith("INTERNAL_VARIADIC_RECOVERY\n1\n"),
              internal_undefined_variadic)
        dynamic_number_return_error = singular(f'''LIB "{typed_fixture}"; LIB "{typed["module"]}";
ring r=0,(x),dp; list integerValue=list(1);
m2_dynamicNumberReturn(0,integerValue);
print("DYNAMIC_NUMBER_RETURN_RECOVERY");
list numberValue=list(number(1));
print(m2_dynamicNumberReturn(0,numberValue)==dynamicNumberReturn(0,numberValue));''')
        check("requires a plain number" in dynamic_number_return_error and
              dynamic_number_return_error.endswith("DYNAMIC_NUMBER_RETURN_RECOVERY\n1\n"),
              dynamic_number_return_error)
        dynamic_selector_error = singular(f'''LIB "{typed["module"]}";
intvec selectorSource=10,20; list vectorSelector=list(intvec(1,2));
m2_dynamicExpressionSelector(selectorSource,vectorSelector);
print("DYNAMIC_SELECTOR_RECOVERY");
print(m2_dynamicExpressionSelector(selectorSource,list(1))==intvec(11));''')
        check("named source" in dynamic_selector_error and
              dynamic_selector_error.endswith("DYNAMIC_SELECTOR_RECOVERY\n1\n"), dynamic_selector_error)

        # First-class strings and ideals retain Singular ownership, slot, and
        # typed-call semantics without routing arithmetic through the evaluator.
        rich_fixture = directory/"string_ideal_milestone2.lib"
        rich_fixture.write_text('''proc stringIdentity(string input){return(input);}
proc stringConcat(string input){input=input+input;return(input);}
proc stringMutate(string input,string replacement,int position){input[position]=replacement;return(input);}
proc stringIndex(string input,int position){return(input[position]);}
proc stringCompare(string left,string right){return((left<right)+2*(left<=right)+4*(left>right)+8*(left>=right)+16*(left==right)+32*(left!=right));}
proc stringToInt(string input){return(int(input));}
proc stringConvert(int value,poly p,number n){return(string("i=",value,";p=",p,";n=",n));}
proc defaultString(){string value;return(value);}
proc forwardDynamicString(list values){return(stringIdentity(values[1]));}
proc dynamicStringReturn(int branch,list values){if(branch){return("fixed");}return(values[1]);}
proc idealIdentity(ideal input){return(input);}
proc idealConstructorIdentity(ideal input){return(ideal(input));}
proc idealConstruct(poly p,number n){return(ideal(p,0,n,0));}
proc idealDeclaration(ideal input,poly p){ideal result=input,p;return(result);}
proc idealMutate(ideal input,poly p,int position){input[position]=p;return(input);}
proc idealAdd(ideal left,ideal right){return(left+right);}
proc idealAddPoly(ideal left,poly right){return(left+right);}
proc polyAddIdeal(poly left,ideal right){return(left+right);}
proc idealMultiply(ideal left,ideal right){return(left*right);}
proc idealRightMultiply(ideal left,poly right){return(left*right);}
proc idealLeftMultiply(poly left,ideal right){return(left*right);}
proc idealPower(ideal input,int exponent){return(input^exponent);}
proc idealIndex(ideal input,int position){return(input[position]);}
proc idealSize(ideal input){return(size(input));}
proc defaultIdeal(){ideal input;return(input);}
proc mutateDefaultIdeal(poly p){ideal input;input[1]=p;return(input);}
proc emptyIdeal(){return(ideal());}
proc idealSelfAdd(ideal input){input=input+input;return(input);}
proc idealSelfMultiply(ideal input){input=input*input;return(input);}
proc idealSelfAssign(ideal input){input=input;return(input);}
proc idealConstructorAssign(ideal input){input=ideal(input);return(input);}
proc idealDefaultConstructorAssign(){ideal input;input=ideal(input);return(input);}
proc idealExactCopyBox(ideal input){ideal copy=input;return(list(input,copy));}
proc idealFromPoly(poly input){return(idealIdentity(input));}
proc idealFromNumber(number input){return(idealIdentity(input));}
proc idealNumberThenIdentity(number input){ideal I=input;return(idealIdentity(I));}
proc idealNestedNumberIdentity(number input){return(idealIdentity(idealIdentity(input)));}
proc forwardDynamicIdeal(list values){return(idealIdentity(values[1]));}
proc dynamicIdealReturn(int branch,list values){if(branch){return(ideal(0));}return(values[1]);}
proc nestedValues(string text,ideal input){return(list(text,input,list(text+"!",input[1])));}
proc ownedValues(poly p){return(list(string("owned"),ideal(p,0)));}
proc wideIndexReads(poly p,list values,intvec ivalue,string text,ideal I,int position){return(list(p[position],values[position],ivalue[position],text[position],I[position]));}
proc wideIndexWrites(list values,intvec ivalue,string text,ideal I,poly p,int position){values[position]=p;ivalue[position]=77;text[position]="Z";I[position]=p;return(list(values,ivalue,text,I));}
proc dynamicWideIndex(int branch,list values,int position){if(branch){return(0);}return(values[1][position]);}
''')
        rich_entries = ["stringIdentity", "stringConcat", "stringMutate", "stringIndex", "stringCompare",
                        "stringToInt", "stringConvert", "defaultString", "forwardDynamicString", "dynamicStringReturn",
                        "idealIdentity", "idealConstructorIdentity", "idealConstruct", "idealDeclaration", "idealMutate", "idealAdd",
                        "idealAddPoly", "polyAddIdeal", "idealMultiply", "idealRightMultiply",
                        "idealLeftMultiply", "idealPower", "idealIndex", "idealSize", "defaultIdeal", "mutateDefaultIdeal", "emptyIdeal",
                        "idealSelfAdd", "idealSelfMultiply", "idealSelfAssign", "idealConstructorAssign",
                        "idealDefaultConstructorAssign", "idealExactCopyBox", "idealFromPoly",
                        "idealFromNumber", "idealNumberThenIdentity", "idealNestedNumberIdentity",
                        "forwardDynamicIdeal", "dynamicIdealReturn", "nestedValues",
                        "ownedValues", "wideIndexReads", "wideIndexWrites", "dynamicWideIndex"]
        rich = compile_lib(rich_fixture, rich_entries, ["--strict", "--export-prefix", "rich_"])
        rich_manifest = json.loads(Path(rich["manifest"]).read_text())
        rich_code = "\n".join(Path(n["source"]).read_text() for n in rich_manifest["procedures"])
        rich_sources = {n["procedure"]: Path(n["source"]).read_text()
                        for n in rich_manifest["procedures"]}
        rich_entry_code = Path(rich_manifest["entry_source"]).read_text()
        for expected in ["snative::String", "snative::Ideal", "snative::IdealView", "snative::IdealBinding",
                         "snative::PolyBinding", "snative::CowString", "snative::CowIdeal",
                         "snative::string_add", "snative::string_compare", "snative::string_index",
                         "snative::ideal_add", "snative::ideal_mul", "snative::ideal_mul_poly",
                         "snative::poly_mul_ideal", "snative::ideal_power", "snative::ideal_get",
                         "snative::IdealArgument", "snative::StringArgument", "result->flag=value.flags()"]:
            check(expected in rich_code+rich_entry_code, expected)
        check("std::string" not in rich_code+rich_entry_code, rich_code+rich_entry_code)
        check("list_set_owned" in rich_code and "std::move" in rich_code, rich_code)
        check(rich_sources["wideIndexReads"].count("(int)(v_position)")==4,
              rich_sources["wideIndexReads"])
        check(rich_sources["wideIndexWrites"].count("(int)(v_position)")==4,
              rich_sources["wideIndexWrites"])
        check(rich_sources["dynamicWideIndex"].count("(int)(v_position)")==1,
              rich_sources["dynamicWideIndex"])

        unsupported_rich = directory/"string_ideal_fallback.lib"
        unsupported_rich.write_text('''proc idealSubtract(ideal I,ideal J){return(I-J);}
proc idealCompare(ideal I,ideal J){return(I==J);}
proc idealPlusNumber(ideal I,number n){return(I+n);}
proc idealNestedConstructor(ideal I,poly p){return(ideal(I,p));}
proc idealAttribute(ideal I){return(attrib(I,"isSB"));}
proc stringVectorIndex(string s,intvec positions){return(s[positions]);}
proc stringSlice(string s){return(s[1,2]);}
''')
        unsupported_names = ["idealSubtract", "idealCompare", "idealPlusNumber", "idealNestedConstructor",
                             "idealAttribute", "stringVectorIndex", "stringSlice"]
        unsupported_result = compile_lib(unsupported_rich, unsupported_names,
                                         ["--export-prefix", "rich_"])
        check(unsupported_result["native_exports"]==[] and
              {entry["procedure"] for entry in unsupported_result["aliases"]}==set(unsupported_names),
              unsupported_result)

        rich_count = 0
        rich_source = f'LIB "{rich_fixture}"; LIB "{rich["module"]}";\n'
        rich_source += '''string original="abcd";string savedOriginal=original;
print(rich_stringIdentity(original)==stringIdentity(original));
print(rich_stringIdentity(original[2])==stringIdentity(original[2]));
print(rich_stringConcat(original)==stringConcat(original));
string ignoredString=rich_stringConcat(original);print(original==savedOriginal);
print(rich_stringMutate(original,"YZ",2)==stringMutate(original,"YZ",2));
print(rich_stringMutate(original,"",2)==stringMutate(original,"",2));
ignoredString=rich_stringMutate(original,"Z",1);print(original==savedOriginal);
print(rich_stringIndex(original,-1)==stringIndex(original,-1));
print(rich_stringIndex(original,0)==stringIndex(original,0));
print(rich_stringIndex(original,5)==stringIndex(original,5));
print(rich_stringCompare("a","b")==stringCompare("a","b"));
print(rich_stringCompare("same","same")==stringCompare("same","same"));
print(rich_stringCompare("z","b")==stringCompare("z","b"));
print(rich_stringToInt("  -17rest")==stringToInt("  -17rest"));
print(rich_defaultString()==defaultString());
list dynamicStrings=list("dynamic");
print(rich_forwardDynamicString(dynamicStrings)==forwardDynamicString(dynamicStrings));
print(rich_dynamicStringReturn(0,dynamicStrings)==dynamicStringReturn(0,dynamicStrings));
print(rich_dynamicStringReturn(1,dynamicStrings)==dynamicStringReturn(1,dynamicStrings));
list dynamicListCarrier=list(list(31,32));list dynamicVectorCarrier=list(intvec(41,42));
print(rich_dynamicWideIndex(0,dynamicListCarrier,65536*65536+1)==dynamicWideIndex(0,dynamicListCarrier,65536*65536+1));
print(rich_dynamicWideIndex(0,dynamicListCarrier,65536*65536+1)==rich_dynamicWideIndex(0,dynamicListCarrier,1));
print(rich_dynamicWideIndex(0,dynamicVectorCarrier,65536*65536+1)==dynamicWideIndex(0,dynamicVectorCarrier,65536*65536+1));
print(rich_dynamicWideIndex(0,dynamicVectorCarrier,65536*65536+1)==rich_dynamicWideIndex(0,dynamicVectorCarrier,1));
'''
        rich_count += 22
        for index, characteristic in enumerate(["0", "7", "(0,a,b)"]):
            rich_source += f'ring richr{index}={characteristic},(x,y),dp;\n'
            coefficient = "3/2" if characteristic=="0" else "3" if characteristic=="7" else "a/b"
            rich_source += f'''poly p=x2+y+1; number n={coefficient};
ideal I=p,0,y,0; ideal J=x,0; string savedI=string(I); string savedJ=string(J);
print(rich_stringConvert(-12,p,n)==stringConvert(-12,p,n));
print(string(rich_idealIdentity(I))==string(idealIdentity(I)));
print(string(rich_idealIdentity(p))==string(idealIdentity(p)));
print(string(rich_idealIdentity(n))==string(idealIdentity(n)));
print(string(rich_idealIdentity(2))==string(idealIdentity(2)));
print(string(rich_idealConstruct(p,n))==string(idealConstruct(p,n)));
ideal made=rich_idealConstruct(p,n);print(size(made)==2 and ncols(made)==4 and made[2]==0 and made[4]==0);
print(string(rich_idealDeclaration(I,x))==string(idealDeclaration(I,x)));
made=rich_idealDeclaration(I,x);print(ncols(made)==5 and made[2]==0 and made[4]==0);
print(string(rich_idealMutate(I,x+y,6))==string(idealMutate(I,x+y,6)));
made=rich_idealMutate(I,x+y,6);print(ncols(made)==6 and made[5]==0);
ideal ignoredIdeal=rich_idealMutate(I,x,1);print(string(I)==savedI);
print(string(rich_idealAdd(I,J))==string(idealAdd(I,J)));
print(string(rich_idealAddPoly(I,x))==string(idealAddPoly(I,x)));
print(string(rich_polyAddIdeal(x,I))==string(polyAddIdeal(x,I)));
print(string(rich_idealMultiply(I,J))==string(idealMultiply(I,J)));
print(string(rich_idealRightMultiply(I,x+y))==string(idealRightMultiply(I,x+y)));
print(string(rich_idealLeftMultiply(x+y,I))==string(idealLeftMultiply(x+y,I)));
print(string(rich_idealPower(I,0))==string(idealPower(I,0)));
print(string(rich_idealPower(I,2))==string(idealPower(I,2)));
print(string(rich_idealPower(I,-1))==string(idealPower(I,-1)));
ideal Z=0;print(string(rich_idealPower(Z,0))==string(idealPower(Z,0)));
print(rich_idealIndex(I,2)==idealIndex(I,2));
print(rich_idealIndex(I,4)==idealIndex(I,4));
print(rich_idealSize(I)==idealSize(I));
made=rich_defaultIdeal();print(size(made)==0 and ncols(made)==1);
made=rich_emptyIdeal();print(size(made)==0 and ncols(made)==0);
print(string(rich_idealSelfAdd(I))==string(idealSelfAdd(I)));
print(string(rich_idealSelfMultiply(I))==string(idealSelfMultiply(I)));
print(string(rich_idealFromPoly(p))==string(idealFromPoly(p)));
print(string(rich_idealFromNumber(n))==string(idealFromNumber(n)));
list dynamicIdeals=list(I);
print(string(rich_forwardDynamicIdeal(dynamicIdeals))==string(forwardDynamicIdeal(dynamicIdeals)));
print(string(rich_dynamicIdealReturn(0,dynamicIdeals))==string(dynamicIdealReturn(0,dynamicIdeals)));
print(string(rich_dynamicIdealReturn(1,dynamicIdeals))==string(dynamicIdealReturn(1,dynamicIdeals)));
print(string(rich_nestedValues("text",I))==string(nestedValues("text",I)));
print(string(rich_ownedValues(p))==string(ownedValues(p)));
ideal listInput=I; string listText="text"; string savedListInput=string(listInput);
list boxed=rich_nestedValues(listText,listInput);
listText[1]="X";listInput[1]=x;
print(boxed[1]=="text" and string(boxed[2])==savedListInput);
string boxedText=boxed[1];ideal boxedIdeal=boxed[2];
boxedText[1]="Z";boxedIdeal[1]=x;print(boxed[1]=="text" and string(boxed[2])==savedI);
print(string(I)==savedI and string(J)==savedJ);
list readValues=list(11,12);intvec readVector=21,22;string readText="ab";
print(string(rich_wideIndexReads(p,readValues,readVector,readText,I,65536*65536+1))==string(wideIndexReads(p,readValues,readVector,readText,I,65536*65536+1)));
list wideNative=rich_wideIndexReads(p,readValues,readVector,readText,I,65536*65536+1);
list oneNative=rich_wideIndexReads(p,readValues,readVector,readText,I,1);
print(wideNative[1]==0 and wideNative[2]==oneNative[2] and wideNative[3]==oneNative[3] and wideNative[4]==oneNative[4] and wideNative[5]==oneNative[5]);
list writeValues=list(11,12);intvec writeVector=21,22;string writeText="ab";ideal writeIdeal=x,y;
string savedWriteValues=string(list(writeValues,writeVector,writeText,writeIdeal));
print(string(rich_wideIndexWrites(writeValues,writeVector,writeText,writeIdeal,p,65536*65536+1))==string(wideIndexWrites(writeValues,writeVector,writeText,writeIdeal,p,65536*65536+1)));
print(string(rich_wideIndexWrites(writeValues,writeVector,writeText,writeIdeal,p,65536*65536+1))==string(rich_wideIndexWrites(writeValues,writeVector,writeText,writeIdeal,p,1)));
print(string(list(writeValues,writeVector,writeText,writeIdeal))==savedWriteValues);
ideal flagBasis=std(ideal(x,y));
print(attrib(rich_defaultIdeal(),"isSB")==attrib(defaultIdeal(),"isSB") and attrib(rich_defaultIdeal(),"isSB")==1);
print(attrib(rich_mutateDefaultIdeal(p),"isSB")==attrib(mutateDefaultIdeal(p),"isSB") and attrib(rich_mutateDefaultIdeal(p),"isSB")==0);
print(attrib(rich_idealIdentity(flagBasis),"isSB")==attrib(idealIdentity(flagBasis),"isSB") and attrib(rich_idealIdentity(flagBasis),"isSB")==1);
print(attrib(rich_idealConstructorIdentity(flagBasis),"isSB")==attrib(idealConstructorIdentity(flagBasis),"isSB") and attrib(rich_idealConstructorIdentity(flagBasis),"isSB")==1);
print(attrib(rich_idealFromPoly(p),"isSB")==attrib(idealFromPoly(p),"isSB") and attrib(rich_idealFromPoly(p),"isSB")==1);
print(attrib(rich_idealFromNumber(n),"isSB")==attrib(idealFromNumber(n),"isSB") and attrib(rich_idealFromNumber(n),"isSB")==0);
print(attrib(rich_idealNumberThenIdentity(n),"isSB")==attrib(idealNumberThenIdentity(n),"isSB") and attrib(rich_idealNumberThenIdentity(n),"isSB")==1);
print(attrib(rich_idealNestedNumberIdentity(n),"isSB")==attrib(idealNestedNumberIdentity(n),"isSB") and attrib(rich_idealNestedNumberIdentity(n),"isSB")==1);
print(attrib(rich_idealMutate(flagBasis,x,1),"isSB")==attrib(idealMutate(flagBasis,x,1),"isSB") and attrib(rich_idealMutate(flagBasis,x,1),"isSB")==0);
print(attrib(rich_idealAdd(flagBasis,flagBasis),"isSB")==attrib(idealAdd(flagBasis,flagBasis),"isSB") and attrib(rich_idealAdd(flagBasis,flagBasis),"isSB")==0);
print(attrib(rich_idealMultiply(flagBasis,flagBasis),"isSB")==attrib(idealMultiply(flagBasis,flagBasis),"isSB") and attrib(rich_idealMultiply(flagBasis,flagBasis),"isSB")==0);
print(attrib(rich_idealPower(flagBasis,2),"isSB")==attrib(idealPower(flagBasis,2),"isSB") and attrib(rich_idealPower(flagBasis,2),"isSB")==0);
print(attrib(rich_idealConstruct(p,n),"isSB")==attrib(idealConstruct(p,n),"isSB") and attrib(rich_idealConstruct(p,n),"isSB")==0);
print(attrib(rich_idealSelfAdd(flagBasis),"isSB")==attrib(idealSelfAdd(flagBasis),"isSB") and attrib(rich_idealSelfAdd(flagBasis),"isSB")==0);
print(attrib(rich_idealSelfAssign(flagBasis),"isSB")==attrib(idealSelfAssign(flagBasis),"isSB") and attrib(rich_idealSelfAssign(flagBasis),"isSB")==0);
print(attrib(rich_idealConstructorAssign(flagBasis),"isSB")==attrib(idealConstructorAssign(flagBasis),"isSB") and attrib(rich_idealConstructorAssign(flagBasis),"isSB")==1);
print(attrib(rich_idealDefaultConstructorAssign(),"isSB")==attrib(idealDefaultConstructorAssign(),"isSB") and attrib(rich_idealDefaultConstructorAssign(),"isSB")==1);
list nativeFlagBox=rich_nestedValues("flag",flagBasis);list referenceFlagBox=nestedValues("flag",flagBasis);
print(attrib(nativeFlagBox[2],"isSB")==attrib(referenceFlagBox[2],"isSB") and attrib(nativeFlagBox[2],"isSB")==1);
list dynamicFlagBox=list(flagBasis);
print(attrib(rich_forwardDynamicIdeal(dynamicFlagBox),"isSB")==attrib(forwardDynamicIdeal(dynamicFlagBox),"isSB") and attrib(rich_forwardDynamicIdeal(dynamicFlagBox),"isSB")==1);
list nativeOwnedFlag=rich_ownedValues(p);list referenceOwnedFlag=ownedValues(p);
print(attrib(nativeOwnedFlag[2],"isSB")==attrib(referenceOwnedFlag[2],"isSB") and attrib(nativeOwnedFlag[2],"isSB")==0);
kill p,n,I,J,savedI,savedJ,made,ignoredIdeal,Z,dynamicIdeals,listInput,listText,
  savedListInput,boxed,boxedText,boxedIdeal,readValues,readVector,readText,
  wideNative,oneNative,writeValues,writeVector,writeText,writeIdeal,savedWriteValues,
  flagBasis,nativeFlagBox,referenceFlagBox,dynamicFlagBox,nativeOwnedFlag,referenceOwnedFlag;
'''
            rich_count += 64
        rich_source += '''ring richnc=0,(x,y),dp;
matrix richD[2][2]; richD[1,2]=1;
def richNC=nc_algebra(1,richD); setring richNC;
ideal ncI=x+y,0; string savedNcI=string(ncI);
print(string(rich_idealRightMultiply(ncI,y))==string(idealRightMultiply(ncI,y)));
print(string(rich_idealLeftMultiply(y,ncI))==string(idealLeftMultiply(y,ncI)));
print(string(idealRightMultiply(ncI,y))!=string(idealLeftMultiply(y,ncI)));
print(string(ncI)==savedNcI);
ring richqbase=0,(x,y),dp; ideal richQ=x2+1; qring richq=std(richQ);option(qringNF);
ideal qI;qI=ideal(y,0); poly qp=x3+y;number qn=2;string savedQI=string(qI);
print(string(rich_idealMutate(qI,qp,2))==string(idealMutate(qI,qp,2)));
print(string(qI)==savedQI);
print(attrib(rich_defaultIdeal(),"isSB")==attrib(defaultIdeal(),"isSB") and attrib(rich_defaultIdeal(),"isSB")==1);
print(attrib(rich_defaultIdeal(),"qringNF")==attrib(defaultIdeal(),"qringNF") and attrib(rich_defaultIdeal(),"qringNF")==0);
print(attrib(rich_mutateDefaultIdeal(qp),"isSB")==attrib(mutateDefaultIdeal(qp),"isSB") and attrib(rich_mutateDefaultIdeal(qp),"isSB")==0 and attrib(rich_mutateDefaultIdeal(qp),"qringNF")==0);
print(attrib(rich_idealIdentity(qI),"qringNF")==attrib(idealIdentity(qI),"qringNF") and attrib(rich_idealIdentity(qI),"qringNF")==1);
print(attrib(rich_idealConstructorIdentity(qI),"qringNF")==attrib(idealConstructorIdentity(qI),"qringNF") and attrib(rich_idealConstructorIdentity(qI),"qringNF")==1);
print(attrib(rich_idealFromPoly(qp),"qringNF")==attrib(idealFromPoly(qp),"qringNF") and attrib(rich_idealFromPoly(qp),"qringNF")==0);
print(attrib(rich_idealFromNumber(qn),"qringNF")==attrib(idealFromNumber(qn),"qringNF") and attrib(rich_idealFromNumber(qn),"qringNF")==0);
print(attrib(rich_idealNumberThenIdentity(qn),"qringNF")==attrib(idealNumberThenIdentity(qn),"qringNF") and attrib(rich_idealNumberThenIdentity(qn),"qringNF")==0);
print(attrib(rich_idealNestedNumberIdentity(qn),"qringNF")==attrib(idealNestedNumberIdentity(qn),"qringNF") and attrib(rich_idealNestedNumberIdentity(qn),"qringNF")==0);
print(attrib(rich_idealMutate(qI,qp,1),"qringNF")==attrib(idealMutate(qI,qp,1),"qringNF") and attrib(rich_idealMutate(qI,qp,1),"qringNF")==0);
print(attrib(rich_idealAdd(qI,qI),"qringNF")==attrib(idealAdd(qI,qI),"qringNF") and attrib(rich_idealAdd(qI,qI),"qringNF")==0);
print(attrib(rich_idealMultiply(qI,qI),"qringNF")==attrib(idealMultiply(qI,qI),"qringNF") and attrib(rich_idealMultiply(qI,qI),"qringNF")==0);
print(attrib(rich_idealPower(qI,2),"qringNF")==attrib(idealPower(qI,2),"qringNF") and attrib(rich_idealPower(qI,2),"qringNF")==0);
print(attrib(rich_idealConstruct(qp,qn),"qringNF")==attrib(idealConstruct(qp,qn),"qringNF") and attrib(rich_idealConstruct(qp,qn),"qringNF")==0);
print(attrib(rich_idealSelfAdd(qI),"qringNF")==attrib(idealSelfAdd(qI),"qringNF") and attrib(rich_idealSelfAdd(qI),"qringNF")==0);
print(attrib(rich_idealSelfAssign(qI),"qringNF")==attrib(idealSelfAssign(qI),"qringNF") and attrib(rich_idealSelfAssign(qI),"qringNF")==0);
print(attrib(rich_idealConstructorAssign(qI),"qringNF")==attrib(idealConstructorAssign(qI),"qringNF") and attrib(rich_idealConstructorAssign(qI),"qringNF")==1);
print(attrib(rich_idealDefaultConstructorAssign(),"qringNF")==attrib(idealDefaultConstructorAssign(),"qringNF") and attrib(rich_idealDefaultConstructorAssign(),"qringNF")==0);
list nativeQBox=rich_nestedValues("q",qI);list referenceQBox=nestedValues("q",qI);
print(attrib(nativeQBox[2],"qringNF")==attrib(referenceQBox[2],"qringNF") and attrib(nativeQBox[2],"qringNF")==1);
list dynamicQBox=list(qI);
print(attrib(rich_forwardDynamicIdeal(dynamicQBox),"qringNF")==attrib(forwardDynamicIdeal(dynamicQBox),"qringNF") and attrib(rich_forwardDynamicIdeal(dynamicQBox),"qringNF")==1);
list nativeOwnedQ=rich_ownedValues(qp);list referenceOwnedQ=ownedValues(qp);
print(attrib(nativeOwnedQ[2],"qringNF")==attrib(referenceOwnedQ[2],"qringNF") and attrib(nativeOwnedQ[2],"qringNF")==0);
option(noqringNF);ideal qRawIdeal;qRawIdeal=ideal(x2,0);poly qRawPoly=x2;
string qRawIdealText=string(qRawIdeal);string qRawPolyText=string(qRawPoly);option(qringNF);
list nativeRawCopies=rich_idealExactCopyBox(qRawIdeal);list referenceRawCopies=idealExactCopyBox(qRawIdeal);
list nativeRawPoly=list(rich_idealIdentity(qRawPoly));list referenceRawPoly=list(idealIdentity(qRawPoly));
print(attrib(nativeRawCopies[1],"qringNF")==0 and attrib(nativeRawCopies[2],"qringNF")==0 and attrib(nativeRawPoly[1],"qringNF")==0);
option(noqringNF);
print(string(nativeRawCopies)==string(referenceRawCopies) and string(nativeRawCopies[1])==qRawIdealText and string(nativeRawCopies[2])==qRawIdealText);
print(string(nativeRawPoly)==string(referenceRawPoly) and string(nativeRawPoly[1])==qRawPolyText);option(qringNF);
option(noqringNF);poly qraw=x3+x2*y+y;string savedQraw=string(qraw);option(qringNF);
ideal qNative=rich_idealFromPoly(qraw);ideal qReference=idealFromPoly(qraw);
print(string(qNative)==string(qReference) and size(qNative)==size(qReference));
print(string(qraw)==savedQraw);
print(attrib(qNative,"qringNF")==attrib(qReference,"qringNF") and attrib(qNative,"qringNF")==1);
'''
        rich_count += 33
        singular(rich_source, "1\n"*rich_count)
        number_count += rich_count

        string_write_error = singular(f'''LIB "{rich_fixture}"; LIB "{rich["module"]}";
string value="abc"; rich_stringMutate(value,"x",0);
print("STRING_WRITE_RECOVERY");print(value=="abc");''')
        check("string index out of range" in string_write_error and
              string_write_error.endswith("STRING_WRITE_RECOVERY\n1\n"), string_write_error)
        ideal_index_error = singular(f'''LIB "{rich_fixture}"; LIB "{rich["module"]}";
ring r=0,(x),dp; ideal I=x,0; rich_idealIndex(I,3);
print("IDEAL_INDEX_RECOVERY");print(ncols(I)==2 and I[2]==0);''')
        check("ideal index out of range" in ideal_index_error and
              ideal_index_error.endswith("IDEAL_INDEX_RECOVERY\n1\n"), ideal_index_error)

        manual = (ROOT/"Tst/Manual/ellipticcovers.tst").read_text().rstrip()
        check(manual.endswith("$"), "expected ellipticcovers termination marker")
        prelude = 'int tst_no_status=1; LIB "ellipticcovers.lib";\n'
        native_prelude = prelude+f'LIB "{elliptic["module"]}";\n'
        native_prelude += 'Ellipticcovers::computeConstant=native_computeConstant; computeConstant=native_computeConstant;\n'
        native_prelude += 'Ellipticcovers::evaluateIntegral=native_evaluateIntegral; evaluateIntegral=native_evaluateIntegral;\n'
        native_prelude += 'Ellipticcovers::permute=native_permute; permute=native_permute;\n'
        native_prelude += 'Ellipticcovers::propagator=native_propagator; propagator=native_propagator;\n'
        reference = singular(prelude+'\n'*5+manual[:-1])
        actual = singular(native_prelude+manual[:-1]+'\nprint("NATIVE_COUNTER"); print(native_compilerCalls());')
        actual, count = actual.rsplit("NATIVE_COUNTER\n", 1)
        check(actual==reference and int(count.strip())>0, actual)
        print(f"Manual/ellipticcovers.tst: exact interpreter parity; {count.strip()} native entries")
        elliptic_all = compile_lib(ROOT/"Singular/LIB/ellipticcovers.lib", [])
        check(len(elliptic_all["native_exports"])==4 and len(elliptic_all["aliases"])==7, elliptic_all)
        descriptors = singular(f'''LIB "ellipticcovers.lib";
print("FD_BEFORE"); system("sh","ls /proc/"+string(system("pid"))+"/fd | wc -l");
LIB "{elliptic_all["module"]}";
print("FD_AFTER"); system("sh","ls /proc/"+string(system("pid"))+"/fd | wc -l");
''')
        fd_before = re.search(r"FD_BEFORE\n\s*(\d+)", descriptors)
        fd_after = re.search(r"FD_AFTER\n\s*(\d+)", descriptors)
        check(fd_before and fd_after and fd_before[1]==fd_after[1], descriptors)
        print(f"ellipticcovers.lib alias loading: {fd_before[1]} -> {fd_after[1]} open descriptors")
        mixed = compile_lib(ROOT/"Singular/LIB/customstd.lib", [])
        mixed_output = singular(f'''LIB "customstd.lib";
LIB "{mixed["module"]}";
ring r=0,(x,y),dp; ideal I=x;
print("MIXED_PACKAGE");
print(string(native_monomialabortstd(I))==string(monomialabortstd(I)));
''')
        check("package is unavailable" not in mixed_output and mixed_output.endswith("MIXED_PACKAGE\n1\n"), mixed_output)

        # A changed return ABI invalidates callers and entry wrappers, unlike a
        # body-only edit. A direct forwarding caller infers the new poly result.
        abi = directory/"abi.lib"
        abi.write_text('proc forward(poly p){return(valueOf(p));}\nproc valueOf(poly p){return(deg(p));}\n')
        before_abi = compile_lib(abi, ["forward"])
        abi.write_text(abi.read_text().replace("return(deg(p))", "return(p)"))
        after_abi = compile_lib(abi, ["forward"])
        check(after_abi["stats"]["objects_compiled"]==3 and after_abi["module"]!=before_abi["module"], after_abi)
        singular(f'LIB "{after_abi["module"]}"; ring r=0,(x,y),dp; print(native_forward(x+y)==x+y);', "1\n")

        real_cxx = shutil.which("c++")
        check(real_cxx is not None, "C++ compiler not found")
        compiler_log = directory/"compiler-commands.log"
        compiler_wrapper = directory/"record-cxx"
        compiler_wrapper.write_text('#!/bin/sh\nprintf "%s\\n" "$*" >> '+
                                    shlex.quote(str(compiler_log))+'\nexec '+
                                    shlex.quote(real_cxx)+' "$@"\n')
        compiler_wrapper.chmod(0o755)
        flags = compile_lib(fixture, ["root"], ["--cxx", str(compiler_wrapper), "--cxxflag=-O0"])
        check(flags["module"]!=changed["module"] and flags["stats"]["objects_compiled"]==3, flags)
        compiler_commands = compiler_log.read_text().splitlines()
        check(any("-shared" in line and "-O0" in line for line in compiler_commands), compiler_commands)
        flags_warm = compile_lib(fixture, ["root"], ["--cxx", str(compiler_wrapper), "--cxxflag=-O0"])
        check(flags_warm["stats"]["abi_snapshots_reused"]==1 and
              flags_warm["stats"]["objects_compiled"]==flags_warm["stats"]["modules_linked"]==0,
              flags_warm)
        check(compiler_log.read_text().splitlines()==compiler_commands,
              "warm ABI snapshot invoked the compiler")
        # Damaged cached objects are rebuilt instead of trusted on existence alone.
        helper_object = next(Path(n["object"]) for n in changed_manifest["procedures"] if n["procedure"]=="leaf")
        helper_object.write_bytes(b"not an object file")
        repaired = compile_lib(fixture, ["root"])
        check(repaired["stats"]["objects_compiled"]==1, repaired)
        # Concurrent builds serialize and share one set of compiled artifacts.
        concurrent_cache = directory/"concurrent"
        concurrent_command = command(fixture, ["root"], ["--cache-dir", str(concurrent_cache)])
        jobs = [subprocess.Popen(concurrent_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True) for _ in range(2)]
        outputs = [job.communicate(timeout=120) for job in jobs]
        check(all(job.returncode==0 for job in jobs), outputs)
        concurrent_results = [json.loads(out) for out, _ in outputs]
        check(len({result["module"] for result in concurrent_results})==1, concurrent_results)
        check(sum(result["stats"]["objects_compiled"] for result in concurrent_results)==3 and
              sum(result["stats"]["objects_reused"] for result in concurrent_results)==3 and
              sum(result["stats"]["modules_linked"] for result in concurrent_results)==1 and
              sum(result["stats"]["modules_reused"] for result in concurrent_results)==1,
              concurrent_results)

        # Failed public roots are analyzed in private closures: their partial
        # inference/emission cannot poison good roots which share helpers.
        root_isolation = directory/"root-isolation.lib"
        root_isolation.write_text('''proc shared(int n){return(n+1);}
proc aGood(int n){return(shared(n));}
static proc poison(string s){return(s[1,1]);}
proc mBad(int n){return(shared(n)+size(poison("ab")));}
proc zGood(int n){return(shared(n)+2);}
''')
        isolated_roots = compile_lib(root_isolation, [])
        isolated_manifest = json.loads(Path(isolated_roots["manifest"]).read_text())
        check({node["procedure"] for node in isolated_manifest["procedures"]}==
              {"shared", "aGood", "zGood"}, isolated_manifest)
        check({entry["procedure"] for entry in isolated_roots["aliases"]}=={"mBad"}, isolated_roots)
        isolated_output = singular(
            f'LIB "{isolated_roots["module"]}"; print(native_aGood(4)); print(native_zGood(4));')
        check("native_mBad is an interpreted alias, not native C++" in isolated_output and
              isolated_output.endswith("5\n7\n"), isolated_output)

        # Cache namespaces make all pre-v1 entries and the legacy global lock inert.
        legacy_cache = directory/"legacy-cache"
        for category in ("objects", "modules", "index", "ast"):
            (legacy_cache/category).mkdir(parents=True, exist_ok=True)
            (legacy_cache/category/"sentinel").write_text(category)
        legacy_lock = (legacy_cache/"lock").open("a")
        fcntl.flock(legacy_lock, fcntl.LOCK_EX)
        try:
            legacy = compile_lib(fixture, ["root"], ["--cache-dir", str(legacy_cache), "--jobs", "2"])
        finally:
            fcntl.flock(legacy_lock, fcntl.LOCK_UN)
            legacy_lock.close()
        check(all((legacy_cache/category/"sentinel").read_text()==category
                  for category in ("objects", "modules", "index", "ast")), legacy)
        check(str(Path(legacy["module"])).startswith(str(legacy_cache/"cache-v1")), legacy)

        # Scheduling width is not part of any content identity.
        serial = compile_lib(fixture, ["root"],
                             ["--cache-dir", str(directory/"serial-cache"), "--jobs", "1"])
        parallel = compile_lib(fixture, ["root"],
                               ["--cache-dir", str(directory/"parallel-cache"), "--jobs", "4"])
        check(Path(serial["module"]).name==Path(parallel["module"]).name, (serial, parallel))
        serial_manifest = json.loads(Path(serial["manifest"]).read_text())
        parallel_manifest = json.loads(Path(parallel["manifest"]).read_text())
        check([node["key"] for node in serial_manifest["procedures"]]==
              [node["key"] for node in parallel_manifest["procedures"]] and
              serial_manifest["exports"]==parallel_manifest["exports"],
              (serial_manifest, parallel_manifest))

        # Library-at-a-time milestone: all public procedures in template.lib,
        # including the integer specialization of its variadic msum loop.
        template = compile_lib(ROOT/"Singular/LIB/template.lib", [], ["--export-prefix", "c_", "--strict"])
        check(set(template["exports"])=={"c_mdouble","c_mtriple","c_msum"} and not template["skipped"], template)
        singular(f'''LIB "template.lib"; LIB "{template["module"]}";
print(c_mdouble(21)==mdouble(21)); print(c_mtriple(-7)==mtriple(-7));
print(c_msum()==msum()); print(c_msum(8)==msum(8));
print(c_msum(1,2,3,4)==msum(1,2,3,4));''', "1\n"*5)
        rejected = singular(f'LIB "{template["module"]}"; c_msum("text");\nprint(c_msum(2,3));')
        check("plain int" in rejected and rejected.endswith("5\n"), rejected)
        collision = singular(f'''LIB "template.lib";
int c_mdouble=77;
LIB "{template["module"]}";
print("COLLISION"); print(c_mdouble); print(mdouble(5)); print(defined(c_mtriple));''')
        check("conflicts with a non-procedure" in collision and collision.endswith("COLLISION\n77\n10\n0\n"), collision)

        partial = directory/"partial.lib"
        partial.write_text('''info="native compiler partial-library fixture";
static proc hidden(int n){return(n+1);}
proc publicHelper(int n){return(hidden(n));}
proc entry(int n){return(publicHelper(n));}
// Keep one deliberately unsupported string overload in the alias/UI tests;
// ordinary string concatenation is native now.
static proc decorate(string s){return(s[1,size(s)]+"!");}
proc unsupported(string s){return(decorate(s));}
''')
        public = compile_lib(partial, ["entry"], ["--export-prefix", "c_"])
        check(set(public["exports"])=={"c_entry","c_publicHelper"}, public)
        all_public = compile_lib(partial, [], ["--export-prefix", "c_"])
        check(set(all_public["exports"])==set(public["exports"])|{"c_unsupported"} and
              [p["procedure"] for p in all_public["skipped"]]==["unsupported"], all_public)
        check(all_public["stats"]["objects_compiled"]==1, all_public) # new alias wrapper only
        strict = subprocess.run(command(partial, [], ["--strict"]), capture_output=True, text=True, timeout=30)
        check(strict.returncode!=0 and "unsupported" in strict.stderr, strict)
        singular(f'LIB "{public["module"]}"; print(c_entry(9)); print(c_publicHelper(4)); print(defined(c_hidden));', "10\n5\n0\n")

        # Source-checkout UI, including shell metacharacters in paths. Build-only
        # must not load or rewrite anything, and failures must not load a stale
        # receipt from an earlier successful build. Unsupported public roots
        # instead load genuine interpreted aliases, with a warning.
        ui_cache = directory/"cache space ' ; literal"
        ui = f'''LIB "{ROOT}/Singular/LIB/compiler.lib";
LIB "template.lib";
proc mdouble(int i){{return(i+1000);}}
compilerSetup("{TRANSLATOR}","{ui_cache}");
string artifact=compileLib("template.lib");
print("BUILD_ONLY"); print(defined(c_mdouble)); print(mdouble(21));
loadCompiled(artifact);
print("LOADED"); print(c_mdouble(21)); print(c_msum(1,2,3)); print(mdouble(21));
print(mdouble(21));
string procModule=compileProc("{partial}","entry",1);
print("PROCEDURE"); print(c_entry(3)); print(c_publicHelper(5)); print(defined(c_hidden));
compileProc("{partial}","unsupported",1);
print("ALIASED"); print(c_unsupported("works"));
compileProc("{partial}","missing",1);
print("AFTER_FAILURE"); print(c_entry(3)); print(mdouble(21));
'''
        ui_output = singular(ui)
        for expected in ["BUILD_ONLY\n0\n1021\n", "LOADED\n42\n6\n1021\n1021\n",
                         "PROCEDURE\n4\n6\n0\n", "ALIASED\nworks!\n", "AFTER_FAILURE\n4\n1021\n"]:
            check(expected in ui_output, ui_output)
        check("no module was loaded" in ui_output, ui_output)
        check("c_unsupported is an interpreted alias, not native C++" in ui_output, ui_output)

        installed_ui = singular('''LIB "compiler.lib";
string artifact=compileLib("template.lib",1);
print("INSTALLED_UI"); print(c_mdouble(21)); print(c_msum(1,2,3,4));''',
                                extra_env={"SINGULAR_NATIVE_DRIVER":"", "SINGULAR_NATIVE_CACHE":str(directory/"installed-cache")})
        check(installed_ui.endswith("INSTALLED_UI\n42\n10\n"), installed_ui)

        # Replacing c_value must not mutate a shared interpreted procinfo.
        # Keep the original library at version one while loading native v2.
        evolving = directory/"evolving.lib"
        version_one = 'proc value(int n){print("interpreted");return(n+1);}\n'
        evolving.write_text(version_one)
        alias_image = compile_lib(evolving, [], ["--export-prefix", "c_"])
        check(alias_image["native_exports"]==[] and len(alias_image["aliases"])==1, alias_image)
        evolving.write_text('proc value(int n){return(n+10);}\n')
        native_image = compile_lib(evolving, [], ["--export-prefix", "c_"])
        evolving.write_text(version_one)
        evolution = singular(f'''LIB "{evolving}";
LIB "{alias_image["module"]}";
proc saved=c_value;
LIB "{native_image["module"]}";
print("EVOLUTION"); print(c_value(1)); print(value(1)); print(saved(1));''')
        check("interpreted alias" in evolution and evolution.endswith("EVOLUTION\n11\ninterpreted\n2\ninterpreted\n2\n"), evolution)

        bad = directory/"bad.lib"
        for unsupported in ['return(execute("1"));', 'ideal q=p;q=q-ideal(p);return(1);', 'return(unknown(p));', 'int while=2;return(1);',
                            'if(1){int k=2;}return(k);',
                            'while(1){if(1){poly q=p;break;return(q);}}return(p);',
                            'while(1){if(1){if(1){continue;}poly q=p;return(q);}}return(p);',
                            'if(1){poly q=p;return(q);}return(q);',
                            'return(diff(1,1));', 'return(diff(p,p));',
                            'execute("number xx = "+"a");kill xx;return(1);',
                            'execute("number xx = "+list("a")[1]);kill xx;return(1);',
                            'list names=ringlist(basering)[1][2];execute("number xx = "+names[1]);return(1);',
                            'list names=ringlist(basering)[1][2];names[1]=1;execute("number xx = "+names[1]);kill xx;return(1);',
                            'if(size(p)==0){return(0);}return(p);', 'return(helper(p));',
                            'return(p<poly(1));', 'return(p[1,1]);',
                            'intvec v=1,2;return(v[1..2]);',
                            'intvec v=1,2;intvec out=(v+1)[1..2];return(out);',
                            'return((1,2));',
                            'list A,B=1,2;return(B);',
                            'intvec v=1,2;return(v!=1);', 'return(p^-1+unknown(p));']:
            bad.write_text("proc bad(poly p){return(helper(p));}\nproc helper(poly p){"+unsupported+"}\n")
            result = subprocess.run(command(bad, ["bad"], ["--strict"]), capture_output=True, text=True, timeout=30)
            check(result.returncode!=0 and "bad.lib::bad -> bad.lib::helper" in result.stderr, result)
    print(f"Native compiler correctness: {checks} checks plus {274+poly_count+number_count} native/interpreted comparisons, 0 failures")
    return 0


if __name__ == "__main__":
    sys.exit(main())
