#!/usr/bin/env python3
"""Import concrete surface data into nonGeneralTypeSurfacesP4.lib.

This is a maintainer-only, standard-library Python 3 converter.  Singular uses
the generated data directly; OSCAR, Julia and Python are not runtime dependencies.
The source is the 48 MPolyIdeal files loaded by SurfacesP4.jl, not a collection
of random constructors.  Input files must agree with the recorded Git snapshot.
The snapshot records the data files, not the mathematical origin of the catalogue.

Usage (from the Singular source root):
  python3 misc/import_non_general_type_surfaces_p4.py --oscar-root /path/to/Oscar.jl \
    --output /tmp/stg_fixed_catalogue_data.inc
  python3 misc/import_non_general_type_surfaces_p4.py --oscar-root /path/to/Oscar.jl \
    --update-library Singular/LIB/nonGeneralTypeSurfacesP4.lib
  python3 misc/import_non_general_type_surfaces_p4.py --oscar-root /path/to/Oscar.jl \
    --check-library Singular/LIB/nonGeneralTypeSurfacesP4.lib

Only the marked generated block is replaced by --update-library.  Neither this
program nor the generated Singular code executes any string read from OSCAR:
only schema-checked identifiers, decimal coefficients and exponents are emitted.
"""

import argparse
from collections import Counter
from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys


BEGIN = "// BEGIN FIXED SURFACE DATA"
END = "// END FIXED SURFACE DATA"
SOURCE = "src/AlgebraicGeometry/Surfaces/SurfacesP4.jl"
DIRECTORY = "data/Surfaces"
IDENTIFIER = re.compile(r"[A-Za-z][A-Za-z0-9_]*\Z")
DECIMAL = re.compile(r"(?:0|[1-9][0-9]*)\Z")


@dataclass
class Surface:
    name: str
    characteristic: int
    variables: list
    generators: list
    sha256: str


def require(condition, message):
    if not condition:
        raise ValueError(message)


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        require(key not in result, "duplicate JSON key: " + key)
        result[key] = value
    return result


def keys(value, expected, context):
    require(isinstance(value, dict) and set(value) == set(expected),
            context + ": unexpected object schema")


def decimal(value, context):
    require(isinstance(value, str) and DECIMAL.fullmatch(value),
            context + ": expected a nonnegative decimal string")
    return int(value)


def prime(value):
    if value < 2:
        return False
    return all(value % divisor for divisor in range(2, int(value ** 0.5) + 1))


def git(root, *arguments):
    return subprocess.run(["git", "-C", str(root), *arguments], check=True,
                          capture_output=True, text=True).stdout.strip()


def read_surface(path):
    raw = path.read_bytes()
    obj = json.loads(raw, object_pairs_hook=unique_object)
    keys(obj, ["_ns", "_type", "_refs", "data"], path.name)
    require(obj["_ns"] == {
        "Oscar": ["https://github.com/oscar-system/Oscar.jl", "1.4.0"]
    }, path.name + ": unsupported serialization namespace/version")
    keys(obj["_type"], ["name", "params"], path.name + " ideal type")
    require(obj["_type"]["name"] == "MPolyIdeal", path.name + ": not an ideal")
    reference = obj["_type"]["params"]
    require(isinstance(reference, str), path.name + ": invalid ring reference")
    keys(obj["_refs"], [reference], path.name + " ring references")
    ring = obj["_refs"][reference]
    keys(ring, ["_type", "data"], path.name + " ring")
    keys(ring["_type"], ["name", "params"], path.name + " ring type")
    require(ring["_type"]["name"] == "MPolyRing", path.name + ": not MPolyRing")
    field = ring["_type"]["params"]
    keys(field, ["_type", "data"], path.name + " coefficient field")
    require(field["_type"] == {"name": "FiniteField", "_instance": "fpField"},
            path.name + ": coefficient field is not a prime field")
    characteristic = decimal(field["data"], path.name + " characteristic")
    require(characteristic < 2 ** 31 and prime(characteristic),
            path.name + ": unsupported/nonprime characteristic")
    keys(ring["data"], ["symbols"], path.name + " ring data")
    variables = ring["data"]["symbols"]
    require(isinstance(variables, list) and len(variables) == 5,
            path.name + ": expected five variables")
    require(all(isinstance(var, str) and IDENTIFIER.fullmatch(var)
                for var in variables) and len(set(variables)) == len(variables),
            path.name + ": invalid/duplicate variable names")
    data = obj["data"]
    require(isinstance(data, list) and len(data) > 0,
            path.name + ": empty or invalid generator list")
    generators = []
    for index, poly in enumerate(data, 1):
        context = path.name + " generator " + str(index)
        require(isinstance(poly, list) and len(poly) > 0,
                context + ": empty or invalid polynomial")
        terms = []
        seen = set()
        degrees = set()
        for term in poly:
            require(isinstance(term, list) and len(term) == 2,
                    context + ": invalid term")
            exponents, coefficient = term
            require(isinstance(exponents, list) and len(exponents) == 5,
                    context + ": invalid exponent vector")
            exponents = tuple(decimal(entry, context + " exponent")
                              for entry in exponents)
            coefficient = decimal(coefficient, context + " coefficient")
            require(0 < coefficient < characteristic,
                    context + ": coefficient is not a nonzero canonical residue")
            require(exponents not in seen, context + ": duplicate monomial")
            require(all(exponent < 2 ** 31 for exponent in exponents),
                    context + ": exponent exceeds Singular integer range")
            seen.add(exponents)
            degrees.add(sum(exponents))
            terms.append((exponents, coefficient))
        require(len(degrees) == 1 and next(iter(degrees)) > 0,
                context + ": expected a positive-degree homogeneous polynomial")
        generators.append(terms)
    return Surface(path.stem, characteristic, variables, generators,
                   hashlib.sha256(raw).hexdigest())


def polynomial(terms, variables):
    rendered = []
    for exponents, coefficient in terms:
        factors = [str(coefficient)]
        for variable, exponent in zip(variables, exponents):
            if exponent:
                factors.append(variable if exponent == 1
                               else variable + "^" + str(exponent))
        rendered.append("*".join(factors))
    expression = "+".join(rendered)

    # Independently parse the emitted restricted syntax, preserving term order.
    # This catches missing coefficients/exponents or reordered source terms.
    decoded = []
    for term in expression.split("+"):
        factors = term.split("*")
        exponents = [0] * len(variables)
        for factor in factors[1:]:
            parts = factor.split("^")
            index = variables.index(parts[0])
            require(exponents[index] == 0, "duplicate emitted variable")
            exponents[index] = 1 if len(parts) == 1 else int(parts[1])
        decoded.append((tuple(exponents), int(factors[0])))
    require(decoded == terms, "polynomial round-trip failed")
    return expression


def chunks(expression, width=84):
    result = []
    current = ""
    for index, term in enumerate(expression.split("+")):
        piece = ("+" if index else "") + term
        require(len(piece) <= width, "monomial exceeds generated line width")
        if len(current) + len(piece) > width:
            result.append(current)
            current = ""
        current += piece
    result.append(current)
    require("".join(result) == expression, "string chunking changed polynomial")
    return result


def load_catalogue(root):
    source_text = (root / SOURCE).read_text()
    names = re.findall(r"^export ([A-Za-z][A-Za-z0-9_]*)$", source_text, re.M)
    require(len(names) == len(set(names)) == 48,
            "SurfacesP4.jl must export exactly 48 distinct catalogue names")
    loaders = re.findall(
        r'^([A-Za-z][A-Za-z0-9_]*)\(\) = surface\("([A-Za-z][A-Za-z0-9_]*)"\)$',
        source_text, re.M)
    require(len(loaders) == 48 and all(a == b for a, b in loaders)
            and {a for a, b in loaders} == set(names),
            "exported names and fixed file loaders do not agree")
    files = list((root / DIRECTORY).glob("*.mrdi"))
    require({path.stem for path in files} == set(names),
            "surface data files and exported catalogue names do not agree")
    git(root, "diff", "--quiet", "HEAD", "--", SOURCE, DIRECTORY)
    provenance = {
        "commit": git(root, "rev-parse", "HEAD"),
        "data_tree": git(root, "rev-parse", "HEAD:" + DIRECTORY),
        "source_blob": git(root, "rev-parse", "HEAD:" + SOURCE),
    }
    require(all(re.fullmatch(r"[0-9a-f]{40,64}", value)
                for value in provenance.values()), "invalid Git provenance")
    surfaces = [read_surface(root / DIRECTORY / (name + ".mrdi")) for name in names]
    return surfaces, provenance


def render(surfaces, provenance):
    lines = [
        BEGIN,
        "// Generated by misc/import_non_general_type_surfaces_p4.py; do not edit by hand.",
        "// Source: https://github.com/oscar-system/Oscar.jl",
        "// OSCAR snapshot commit: " + provenance["commit"],
        "// data/Surfaces tree: " + provenance["data_tree"],
        "// SurfacesP4.jl blob: " + provenance["source_blob"],
        "// Copyright (c) 2019-2026 The OSCAR Development Team",
        "// SPDX-License-Identifier: GPL-3.0-or-later (imported catalogue data)",
        "// Contributors: https://www.oscar-system.org/contributors/",
        "// This program is free software: you can redistribute it and/or modify",
        "// it under the terms of the GNU General Public License as published by",
        "// the Free Software Foundation, either version 3 of the License, or",
        "// (at your option) any later version.",
        "// This program is distributed in the hope that it will be useful,",
        "// but WITHOUT ANY WARRANTY; without even the implied warranty of",
        "// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the",
        "// GNU General Public License for more details.",
        "// Full license: GPL3 at the Singular source root; OSCAR LICENSE.md.",
        "// Exact stored equations over their original prime fields.",
        "// Generator order, term order, variable order and residues are retained.",
        "// Each returned SHA-256 hashes the unmodified source .mrdi file.",
        "// No runtime OSCAR, Julia, Python, filesystem or network dependency.",
        "static proc nonGeneralTypeSurfaceP4Data(string name)",
        "{",
        "  list storedVariables;",
        "  list equations;",
    ]
    for surface in surfaces:
        lines.extend([
            '  if(name=="' + surface.name + '")',
            "  {",
            "    storedVariables=list(" + ",".join(json.dumps(var)
                                           for var in surface.variables) + ");",
        ])
        for index, terms in enumerate(surface.generators, 1):
            pieces = chunks(polynomial(terms, surface.variables))
            lines.append("    equations[" + str(index) + "]=")
            for piece_index, piece in enumerate(pieces):
                suffix = ";" if piece_index + 1 == len(pieces) else " +"
                lines.append("      " + json.dumps(piece) + suffix)
        lines.extend([
            "    return(list(" + str(surface.characteristic)
            + ",storedVariables,equations,",
            '      "' + surface.sha256 + '"));',
            "  }",
        ])
    lines.extend([
        '  ERROR("nonGeneralTypeSurfaceP4: unknown fixed surface \'"+name+"\'");',
        "}",
        END,
    ])
    require(max(map(len, lines)) < 100, "generated line exceeds 99 columns")
    return "\n".join(lines) + "\n"


def generated_block(library):
    text = library.read_text()
    require(text.count(BEGIN) == text.count(END) == 1,
            "library must contain exactly one pair of generated-data markers")
    start = text.index(BEGIN)
    end = text.index(END) + len(END)
    require(start < end, "generated-data markers are out of order")
    if end < len(text) and text[end] == "\n":
        end += 1
    return text, start, end


def check_public_names(text, surfaces):
    declaration = re.search(
        r'\bproc nonGeneralTypeSurfaceP4Names\(\)\s*'
        r'(?:"(?:\\.|[^"\\])*"\s*)?\{([^}]*)\}', text, re.S)
    require(declaration is not None, "missing public catalogue-name procedure")
    labels = re.search(r'\blist catalogueLabels\s*=([^;]*);',
                       declaration.group(1), re.S)
    require(labels is not None, "public catalogue names must be a literal list")
    names = json.loads("[" + labels.group(1) + "]")
    require(all(isinstance(name, str) for name in names)
            and len(names) == len(set(names)) == len(surfaces)
            and set(names) == {surface.name for surface in surfaces},
            "public catalogue names do not match the imported data")


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--oscar-root", required=True, type=Path)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--output", type=Path, help="write generated fragment")
    mode.add_argument("--update-library", type=Path, help="replace only marked block")
    mode.add_argument("--check-library", type=Path, help="check without writing")
    args = parser.parse_args()
    root = args.oscar_root.resolve()
    surfaces, provenance = load_catalogue(root)
    fragment = render(surfaces, provenance)
    destination = args.output or args.update_library or args.check_library
    require(root not in destination.resolve().parents,
            "refusing to write/check generated data inside the OSCAR source")
    if args.output:
        args.output.write_text(fragment)
    else:
        text, start, end = generated_block(destination)
        check_public_names(text, surfaces)
        if args.check_library:
            require(text[start:end] == fragment, "generated catalogue differs")
        else:
            destination.write_text(text[:start] + fragment + text[end:])
    print("{} surfaces; {} generators; {} terms; {} generated bytes".format(
        len(surfaces), sum(len(surface.generators) for surface in surfaces),
        sum(len(poly) for surface in surfaces for poly in surface.generators),
        len(fragment.encode())))
    print("Characteristics:", dict(sorted(Counter(surface.characteristic
                                                  for surface in surfaces).items())))
    print("Source commit:", provenance["commit"])
    print("Data tree:", provenance["data_tree"])


if __name__ == "__main__":
    try:
        main()
    except (ValueError, KeyError, OSError, subprocess.CalledProcessError) as error:
        print("import_non_general_type_surfaces_p4.py:", error, file=sys.stderr)
        sys.exit(1)
