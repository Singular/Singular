#!/usr/bin/env python3
"""Small interpreter correctness checks; no timings or performance thresholds.

Run with: python3 Tst/Short/interpreter_correctness.py --singular /path/to/Singular
Optionally pass --reference /path/to/spielwiese for differential checks of
standard syntax and legacy lambdas. New syntax is checked against explicit
expected results, including equivalent ordinary procedures, not Spielwiese.

Unlike regress.cmd, this checks unfiltered stdout/stderr and also exercises
seekable files, execute buffers, and library loading. Each child has a timeout.
All generated inputs live in an automatically removed temporary directory.
"""

import argparse
from dataclasses import dataclass
import difflib
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


MODES = ("file", "stdin", "execute", "library")


@dataclass(frozen=True)
class Case:
    name: str
    source: str
    expected: str
    reference: bool = True
    modes: tuple = MODES


def checks(name, source, expressions, reference=True, modes=MODES, prefix=""):
    """Every expression must evaluate to 1, without additional output/errors."""
    source += "\n" + "\n".join("print(" + expr + ");" for expr in expressions)
    return Case(name, source + "\n", prefix + "1\n" * len(expressions), reference, modes)


def cases():
    result = []
    # Match all eight benchmark call shapes, but call each only a few times.
    for arity in (1, 2):
        for kind in ("int", "def"):
            for used in (False, True):
                names = ("x", "y")[:arity]
                parameters = ",".join(kind + " " + name for name in names)
                expression = "+".join(names) if used else "17"
                arguments = "effect(a)" if arity == 1 else "effect(a),effect(b)"
                expected = (3 if arity == 1 else 10) if used else 17
                suffix = "used" if used else "unused"
                result.append(checks(
                    f"ordinary_{arity}_{kind}_{suffix}",
                    f"proc f({parameters}){{return({expression});}};\n"
                    "proc effect(int v){print(v);return(v);};\n"
                    f"int a=3;int b=7;int value=f({arguments});\n"
                    "proc copy=f;kill f;\n",
                    [f"value=={expected}", "a==3", "b==7",
                     f"copy({','.join(['5'] * arity)})=={5 * arity if used else 17}"],
                    prefix="3\n" if arity == 1 else "3\n7\n",
                ))

    result.append(checks("ordinary_def_types_and_aliases", """
ring r=0,(x,y),dp;
proc combine(def a,def b){return(a+b);};
proc aliases(alias int a,alias int b){return(a+b);};
int a=2;int b=3;
int first=aliases(a,b);
int second=aliases(a,a);
""", ['combine(2,3)==5', 'combine("ab","cd")=="abcd"',
       'combine(x,y)==x+y', 'first==5', 'second==4', 'a==2', 'b==3']))

    result.append(checks("ordinary_lifecycle", """
proc original(int x){return(x+1);};
proc saved=original;
list holders=original,saved;
proc replacement(int x){return(2*x);};
original=replacement;
int replaced=original(4);
kill original,replacement;
proc fromList=holders[1];
kill holders;
int total=0;
for(int i=0;i<4;i++){proc temporary(int x){return(x+3);};total=total+temporary(i);kill temporary;}
""", ['replaced==8', 'saved(4)==5', 'fromList(7)==8', 'total==18']))

    result.append(checks("legacy_lifecycle", """
proc f=x->{int next=x+1;next*next};
proc saved=f;
list holders=f,saved;
f=x->{x+2};
int replaced=f(4);
kill f;
proc fromList=holders[1];
kill holders;
int total=0;
for(int i=0;i<4;i++){proc temporary=x->{x+3};total=total+temporary(i);kill temporary;}
proc identity=x->{x};
list mixed=apply(list(2,"ab"),identity);
""", ['replaced==6', 'saved(4)==25', 'fromList(2)==9', 'total==18',
       'mixed[1]==2', 'mixed[2]=="ab"']))

    for arity in (1, 2):
        for kind in ("int", "def"):
            for used in (False, True):
                names = ("x", "y")[:arity]
                parameters = ",".join(kind + " " + name for name in names)
                expression = "+".join(names) if used else "17"
                arguments = "effect(a)" if arity == 1 else "effect(a),effect(b)"
                expected = (3 if arity == 1 else 10) if used else 17
                suffix = "used" if used else "unused"
                result.append(checks(
                    f"new_{arity}_{kind}_{suffix}",
                    f"proc f=({parameters})->{{return({expression});}};\n"
                    f"proc ordinary({parameters}){{return({expression});}};\n"
                    "proc effect(int v){print(v);return(v);};\n"
                    f"int a=3;int b=7;int value=f({arguments});\n"
                    "proc copy=f;kill f;\n",
                    [f"value=={expected}", "a==3", "b==7",
                     f"copy({','.join(['5'] * arity)})==ordinary({','.join(['5'] * arity)})",
                     f"copy({','.join(['5'] * arity)})=={5 * arity if used else 17}"],
                    reference=False,
                    prefix="3\n" if arity == 1 else "3\n7\n",
                ))

    result.append(checks("new_lifecycle_and_side_effects", """
proc f=(int x)->{return(x+1);};
proc saved=f;
list holders=f,saved;
f=(int x)->{return(2*x);};
int replaced=f(4);
kill f;
proc fromList=holders[1];
kill holders;
int total=0;
for(int i=0;i<4;i++){proc temporary=(int x)->{return(x+3);};total=total+temporary(i);kill temporary;}
proc aliases=(alias int x,alias int y)->{return(x+y);};
int a=2;int b=3;int aliasResult=aliases(a,b);
proc combine=(def x,def y)->{return(x+y);};
proc make=()->{return((int x)->{return(x+5);});};
proc returned=make();kill make;
""", ['replaced==8', 'saved(4)==5', 'fromList(7)==8', 'total==18',
       'a==2', 'b==3', 'aliasResult==5', 'combine("ab","cd")=="abcd"', 'returned(4)==9'],
       reference=False))

    # This fixes an old allocator limitation; do not execute on plain Spielwiese.
    padding = "a" * 8192
    for spelling, definition in (
        ("legacy", "x->{/*" + padding + "*/x+1}"),
        ("new", "(int x)->{/*" + padding + "*/return(x+1);}"),
    ):
        result.append(checks(spelling + "_long_body_lifecycle",
            f"proc f={definition};proc saved=f;kill f;\n"
            "int value=saved(4);kill saved;\n"
            "int total=0;for(int i=0;i<3;i++){"
            f"proc temporary={definition};"
            "total=total+temporary(i);kill temporary;}\n",
            ['value==5', 'total==6'], reference=False))

    result.append(checks("standard_multiline_and_comments", """
int a=1;int b=2;list pair;
pair=(
a,
b
);
int grouped=(a /* ) -> { */ + b);
int commented=(a // ) -> {
+ b);
string key="actual";
htable h=htable((key)->3);
proc old=x->{int y=x+1;y*y};
""", ['size(pair)==2', 'pair[1]==1', 'pair[2]==2', 'grouped==3',
       'commented==3', 'h["actual"]==3', 'old(3)==16'],
       # libparse has its own bracket scanner; unmatched lambda-looking text
       # in comments is not accepted there by Spielwiese either.
       modes=("file", "stdin", "execute")))

    result.append(checks("standard_library_multiline",
        "int a=1;int b=2;list pair=(\na,\nb\n);\n"
        "int grouped=(a /* ordinary comment */ + b);\n",
        ['size(pair)==2', 'pair[1]==1', 'pair[2]==2', 'grouped==3']))
    result.append(checks("standard_continuation", "int value=1+\\\n2;\n",
        ['value==3'], modes=("file", "stdin", "execute")))

    result.append(checks("new_multiline_header", """
proc f=(
int x,
int y
)
->
{return(x+y);};
""", ['f(3,4)==7'], reference=False,
       # Ordinary non-seekable input is line-oriented. Its supported spelling
       # is tested separately with explicit backslash continuations.
       modes=("file", "execute", "library")))

    result.append(checks("new_continued_header",
        "proc f=(int x)\\\n-\\\n>\\\n{return(x+2);};\n",
        ['f(3)==5'], reference=False, modes=("file", "stdin", "execute")))

    # Exercise both sides of Flex's 16 KiB refill boundary with no long loops.
    for width in (16382, 16384, 16386):
        padding = "a" * width
        result.append(checks(f"standard_refill_{width}",
            "int value=(1 /*" + padding + "*/ + 2);\n",
            ['value==3']))
        result.append(checks(f"new_header_refill_{width}",
            "proc f=(int /*" + padding + "*/ x)->{return(x+2);};\n",
            ['f(3)==5'], reference=False))

    # EOF and CRLF must be exercised as bytes, not translated text streams.
    result.append(Case("standard_no_final_newline",
        "proc f(int x){return(x+1);};print(f(3));", "4\n",
        modes=("file", "stdin")))
    result.append(Case("standard_crlf",
        "proc f(int x)\r\n{\r\nreturn(x+1);\r\n}\r\nprint(f(3));\r\n",
        "4\n", modes=("file", "stdin")))
    result.extend(diagnostic_cases())
    return result


def diagnostic_cases():
    """Fixed Spielwiese expectations, including line numbers and recovery.

    Deliberately do not use regress.cmd's diagnostic-location filters. The
    relative library filename also makes these expectations path-independent.
    """
    result = []
    for name, source, context, outer_line in (
        ("undefined_location",
         "proc bad(int x)\n{\n\n  missingDiagnostic;\n}\nbad(3);\nprint(17);\n",
         "  missingDiagnostic;", 10),
        ("long_line_location",
         "proc bad()\n{\n" + " " * 96 + "missingDiagnostic;\n}\nbad();\nprint(17);\n",
         " " * 60 + "missingDiagnostic;", 9),
    ):
        output = ("   ? `missingDiagnostic` is undefined\n"
                  "   ? error occurred in or before ::bad line 3: `" + context + "`\n"
                  "   ? leaving ::bad (0)\n"
                  "   skipping text from `;` error at token `)`\n")
        for mode in MODES:
            suffix = "17\n" if mode in ("file", "stdin") else ""
            if mode == "library":
                suffix = f"   ? leaving ./case.lib::run_input_case ({outer_line})\n"
            result.append(Case(name, source, output + suffix, modes=(mode,)))

    source = "proc required(int x){return(x);};\nrequired();\nprint(19);\n"
    output = ("   ? not enough arguments for proc ::required\n"
              "   ? error occurred in or before ::required line 1: `parameter int x; `\n"
              "   ? wrong type declaration. type 'help int;'\n"
              "   ? leaving ::required (0)\n"
              "   skipping text from `;` error at token `)`\n")
    for mode in MODES:
        suffix = "19\n" if mode in ("file", "stdin") else ""
        if mode == "library":
            suffix = "   ? leaving ./case.lib::run_input_case (6)\n"
        result.append(Case("missing_argument", source, output + suffix, modes=(mode,)))
    return result


def singular_string(source):
    return '"' + source.replace('\\', '\\\\').replace('"', '\\"') + '"'


def invoke(binary, case, mode, directory, timeout, env):
    command = [binary, "-q", "--no-rc"]
    if mode == "file":
        (directory / "case.sing").write_bytes(case.source.encode())
        command.append("case.sing")
        source = b""
    elif mode == "stdin":
        source = case.source.encode()
    elif mode == "execute":
        source = ("execute(" + singular_string(case.source) + ");\nquit;\n").encode()
    elif mode == "library":
        (directory / "case.lib").write_bytes(
            ("version=\"1.0\";\nproc run_input_case()\n{\n" + case.source + "\n}\n").encode())
        source = b'LIB "./case.lib";\nrun_input_case();\nquit;\n'
    else:
        raise ValueError(mode)
    completed = subprocess.run(command, input=source, cwd=directory, env=env,
                               capture_output=True, timeout=timeout)
    return completed.returncode, completed.stdout, completed.stderr


def difference(expected, actual):
    return "".join(difflib.unified_diff(
        expected.decode(errors="backslashreplace").splitlines(keepends=True),
        actual.decode(errors="backslashreplace").splitlines(keepends=True),
        fromfile="expected", tofile="actual"))


def executable(value):
    path = shutil.which(value)
    if path is None:
        raise argparse.ArgumentTypeError("executable not found: " + value)
    return str(Path(path).resolve())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--singular", required=True, type=executable)
    parser.add_argument("--reference", type=executable,
                        help="optional Spielwiese executable for compatible cases")
    parser.add_argument("--timeout", type=float, default=15,
                        help="maximum seconds per child (a hang guard, not a speed test)")
    args = parser.parse_args()
    if args.timeout <= 0:
        parser.error("--timeout must be positive")
    env = dict(os.environ, LC_ALL="C", LANG="C")
    reference_env = dict(env)
    # A candidate library override must never silently affect the reference.
    reference_env.pop("LD_LIBRARY_PATH", None)
    reference_env.pop("DYLD_LIBRARY_PATH", None)
    failed = 0
    count = 0
    compared = 0
    with tempfile.TemporaryDirectory(prefix="singular-correctness-") as temporary:
        directory = Path(temporary)
        for case in cases():
            for mode in case.modes:
                count += 1
                label = case.name + ":" + mode
                expected = (0, case.expected.encode(), b"")
                try:
                    actual = invoke(args.singular, case, mode, directory, args.timeout, env)
                    if actual != expected:
                        failed += 1
                        print("FAIL " + label, file=sys.stderr)
                        print("exit:", actual[0], "expected:", expected[0], file=sys.stderr)
                        print(difference(expected[1], actual[1]), file=sys.stderr)
                        print("stderr:", repr(actual[2]), file=sys.stderr)
                    if args.reference and case.reference:
                        compared += 1
                        reference = invoke(args.reference, case, mode, directory,
                                           args.timeout, reference_env)
                        if reference != expected or reference != actual:
                            failed += 1
                            print("FAIL reference " + label, file=sys.stderr)
                            print("reference exit/stderr:", reference[0], repr(reference[2]), file=sys.stderr)
                            print(difference(expected[1], reference[1]), file=sys.stderr)
                except (OSError, subprocess.TimeoutExpired) as error:
                    failed += 1
                    print("FAIL " + label + ": " + str(error), file=sys.stderr)
    print(f"Interpreter correctness: {count} checks, {compared} reference comparisons, {failed} failures")
    return int(failed != 0)


if __name__ == "__main__":
    sys.exit(main())
