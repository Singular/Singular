#!/usr/bin/env python3
"""Write the labeled Singular inputs used for the PR #1404 timing table.

Each generated file is timed in a fresh Singular process.  Keeping the cases
separate avoids parser state from an earlier case affecting a later result.
"""

from argparse import ArgumentParser
from pathlib import Path


LONG_LIST = ",".join(["a"] * 64)
BLOCK_COMMENT = "a" * 240
LONG_BODY_COMMENT = "a" * 8192

CASES = {
    # Existing spielwiese syntax.  These are the primary non-regression cases.
    "plain_loop": (
        "existing syntax: assignment in a tight loop",
        "int s;for(int i=0;i<1500000;i++){s=i;}s;quit;\n",
    ),
    "group_loop": (
        "existing syntax: parenthesized identifier in a tight loop",
        "int s;for(int i=0;i<1500000;i++){s=(i);}s;quit;\n",
    ),
    "group_ws_var_loop": (
        "existing syntax: parenthesized identifier with leading whitespace",
        "int s;for(int i=0;i<1500000;i++){s=( i);}s;quit;\n",
    ),
    "group_identifier_block_comment_100k": (
        "existing syntax: block comment after a grouped identifier",
        "int s;for(int i=0;i<600000;i++){s=(i/*"
        + BLOCK_COMMENT
        + "*/+1);}s;quit;\n",
    ),
    "nested8_loop": (
        "existing syntax: eight nested parentheses",
        "int s;for(int i=0;i<1200000;i++){s=((((((((i))))))));}s;quit;\n",
    ),
    "ambiguous_long_list_loop": (
        "existing syntax: 64-item parenthesized expression list",
        f"int a=1;list l;for(int i=0;i<150000;i++){{l=({LONG_LIST});}}"
        "size(l);quit;\n",
    ),
    "multiline_exprlist_flat": (
        "existing syntax: 200,000 multiline expression lists",
        "int a=1;int b=2;list l;\n"
        + ("l=(\na,\nb\n);\n" * 200000)
        + "size(l);quit;\n",
    ),
    "line_comments_flat": (
        "existing syntax: 300,000 lambda-looking line comments",
        "int s=7;\n"
        + ("// ignored ) -> { ordinary comment\n" * 300000)
        + "s;quit;\n",
    ),
    "ordinary_whitespace_flat": (
        "existing syntax: 600,000 whitespace-only line pairs",
        "int s=7;\n" + ("        \n\t \r\n" * 600000) + "s;quit;\n",
    ),
    "multiline_long_parens": (
        "existing syntax: 60 parenthesized sums spanning 5,000 lines",
        "int s;\n"
        + (("s=(\n" + "+\n".join(["1"] * 5000) + "\n);\n") * 60)
        + "s;quit;\n",
    ),
    "proc_calls": (
        "existing syntax: typed procedure call",
        "proc id(int x){return(x);};int s;for(int i=0;i<600000;i++){s=id(i);}"
        "s;quit;\n",
    ),
    "proc_def_unused": (
        "existing syntax: def procedure argument",
        "proc id(def x){return(1);};int s;for(int i=0;i<600000;i++){s=id(i);}"
        "s;quit;\n",
    ),
    "ordinary_empty_calls": (
        "existing syntax: zero-argument procedure call",
        "proc f(){return(1);};int s;for(int i=0;i<750000;i++){s=f();}s;quit;\n",
    ),
    "ordinary_empty_ws_calls": (
        "existing syntax: zero-argument call containing whitespace",
        "proc f(){return(1);};int s;for(int i=0;i<750000;i++){s=f( );}s;quit;\n",
    ),
    "ordinary_two_identifier_arg_calls": (
        "existing syntax: two identifier arguments",
        "proc f(int x,int y){return(x+y);};int one=1;int s;"
        "for(int i=0;i<500000;i++){s=f(i,one);}s;quit;\n",
    ),
    "legacy_lambda": (
        "existing syntax: legacy lambda calls",
        "proc f=x->{x+1};int s;for(int i=0;i<500000;i++){s=f(i);}s;quit;\n",
    ),
    "ordinary_proc_create_kill": (
        "existing syntax: procedure construction and destruction",
        "for(int i=0;i<750000;i++){proc f(){return(1);};kill f;}quit;\n",
    ),
    "ordinary_int_create_kill": (
        "existing syntax: integer construction and destruction",
        "for(int i=0;i<1000000;i++){int f=1;kill f;}quit;\n",
    ),
    "ordinary_map_create_kill": (
        "existing syntax: map construction and destruction",
        "ring r=0,x,dp;for(int i=0;i<800000;i++){map f;kill f;}quit;\n",
    ),
    "ordinary_map_reassign": (
        "existing syntax: map assignment",
        "ring r=0,x,dp;map f,g;for(int i=0;i<800000;i++){f=g;}"
        "kill f,g;quit;\n",
    ),
    "legacy_lambda_create_kill": (
        "existing syntax: legacy lambda construction and destruction",
        "for(int i=0;i<700000;i++){proc f=x->{x+1};kill f;}quit;\n",
    ),
    "cyclic6_x20": (
        "existing syntax: repeated cyclic(6) Groebner basis computation",
        "LIB \"polylib.lib\";\nring r=0,(x1,x2,x3,x4,x5,x6),dp;\n"
        "ideal I=cyclic(6);\noption(redSB);\nideal G;\n"
        "for(int j=0;j<20;j++){G=std(I);}\nsize(G);\nquit;\n",
    ),
    # New parameter-list lambda syntax.  spielwiese has no reference value.
    "new_lambda_empty_calls": (
        "new syntax: zero-argument lambda calls",
        "proc f=()->{return(1);};int s;for(int i=0;i<250000;i++){s=f();}"
        "s;quit;\n",
    ),
    "new_lambda": (
        "new syntax: typed lambda calls",
        "proc f=(int x)->{return(x+1);};int s;for(int i=0;i<100000;i++){s=f(i);}"
        "s;quit;\n",
    ),
    "new_lambda_create_kill": (
        "new syntax: typed lambda construction and destruction",
        "for(int i=0;i<600000;i++){proc f=(int x)->{return(x+1);};kill f;}quit;\n",
    ),
    "new_lambda_reassign": (
        "new syntax: typed lambda reassignment",
        "proc f=(int x)->{return(x);};for(int i=0;i<250000;i++){"
        "f=(int x)->{return(x+1);};}kill f;quit;\n",
    ),
    "new_lambda_longbody_create_kill": (
        "new syntax: lambda construction with an 8 KiB body comment",
        "for(int i=0;i<10000;i++){proc f=(int x)->{/*"
        + LONG_BODY_COMMENT
        + "*/return(x+1);};kill f;}quit;\n",
    ),
}


def main() -> None:
    parser = ArgumentParser()
    parser.add_argument(
        "output_dir",
        nargs="?",
        type=Path,
        default=Path("pr1404-performance-inputs"),
    )
    parser.add_argument("--list", action="store_true", dest="list_cases")
    args = parser.parse_args()

    if args.list_cases:
        for label, (description, _) in CASES.items():
            print(f"{label}: {description}")
        return

    args.output_dir.mkdir(parents=True, exist_ok=True)
    for label, (description, source) in CASES.items():
        path = args.output_dir / f"{label}.sing"
        path.write_text(source, encoding="utf-8")
        print(f"{label}: {description} -> {path}")


if __name__ == "__main__":
    main()
