#!/usr/bin/env python3
"""Translate a strict, typed Singular library call closure to native C++.

No evaluation, interpreter dispatch, or source execution is used by the compiler.
The typed subset returns int/intvec/poly/number/string/ideal/list and borrows native call arguments.
Unsupported reachable code is an error, with its complete dependency chain.
"""

import argparse
import bisect
from concurrent.futures import ThreadPoolExecutor
from contextlib import contextmanager
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import stat
import subprocess
import sys
import tempfile

HERE = Path(__file__).resolve().parent
VERSION = "singular-native-v10"
CACHE_FORMAT = "cache-v1"
ABI_SNAPSHOT_FORMAT = "abi-snapshot-v3"
ARTIFACT_RECEIPT_FORMAT = "artifact-receipt-v2"
DEFAULT_JOBS = max(1, min(8, os.cpu_count() or 1))
BUILD_ENVIRONMENT_NAMES = (
    "PATH", "CPATH", "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH", "OBJC_INCLUDE_PATH",
    "LIBRARY_PATH", "COMPILER_PATH", "GCC_EXEC_PREFIX", "GCC_SPECS", "LD_RUN_PATH",
    "LD_LIBRARY_PATH", "LD_PRELOAD", "LD_AUDIT", "LD_HWCAP_MASK", "LD_ORIGIN_PATH",
    "GLIBC_TUNABLES", "PKG_CONFIG_PATH", "PKG_CONFIG_LIBDIR", "PKG_CONFIG_SYSROOT_DIR",
    "PKG_CONFIG_DISABLE_UNINSTALLED", "PKG_CONFIG_ALLOW_SYSTEM_CFLAGS",
    "PKG_CONFIG_ALLOW_SYSTEM_LIBS", "PKG_CONFIG_SYSTEM_INCLUDE_PATH",
    "PKG_CONFIG_SYSTEM_LIBRARY_PATH", "PKG_CONFIG_TOP_BUILD_DIR",
    "PKG_CONFIG_DONT_DEFINE_PREFIX", "PKG_CONFIG_MSVC_SYNTAX",
    "PKG_CONFIG_FDO_SYSROOT_RULES", "PKG_CONFIG_PURE_DEPGRAPH",
    "PKG_CONFIG_DEBUG_SPEW", "PKG_CONFIG_LOG", "SOURCE_DATE_EPOCH", "LANG", "LC_ALL")
LOADER_ENVIRONMENT_NAMES = (
    "LD_PRELOAD", "LD_AUDIT", "LD_LIBRARY_PATH", "LD_HWCAP_MASK",
    "GLIBC_TUNABLES", "LD_ORIGIN_PATH")
LOADER_CONFIGURATION_PATHS = (Path("/etc/ld.so.cache"), Path("/etc/ld.so.preload"))
IDENT = re.compile(r"[A-Za-z][A-Za-z0-9_]*\Z")
TABLE = HERE/"table.h" if (HERE/"table.h").exists() else HERE.parent/"table.h"
RESERVED = set(re.findall(r'\{\s*"([A-Za-z][A-Za-z0-9_]*)"', TABLE.read_text()))
RESERVED.update(("basering", "return", "if", "else", "break", "continue", "for", "while"))


def identifier(name):
    return bool(IDENT.fullmatch(name)) and name not in RESERVED


class CompileError(Exception):
    pass


STAT_KEYS = ("libraries_indexed", "procedures_parsed", "objects_compiled",
             "objects_reused", "modules_linked", "modules_reused",
             "abi_snapshots_created", "abi_snapshots_reused")
_UNSET = object()


def empty_stats():
    return {key: 0 for key in STAT_KEYS}


def add_stats(target, source):
    for key, value in source.items():
        target[key] = target.get(key, 0) + value


def digest(value):
    if not isinstance(value, bytes):
        value = json.dumps(value, sort_keys=True).encode()
    return hashlib.sha256(value).hexdigest()


def file_digest(path):
    h = hashlib.sha256()
    with open(path, "rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def atomic(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(dir=path.parent, prefix="."+path.name+".",
                                         suffix=".tmp", delete=False) as stream:
            temporary = Path(stream.name)
            stream.write(data if isinstance(data, bytes) else data.encode())
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, path)
        temporary = None
    finally:
        if temporary is not None:
            try:
                temporary.unlink()
            except FileNotFoundError:
                pass


def save_json(path, value):
    atomic(path, json.dumps(value, indent=2, sort_keys=True) + "\n")


@contextmanager
def key_lock(cache, category, key):
    """Lock one cache entry without serializing unrelated build work."""
    path = cache/"locks"/category/(digest(key)+".lock")
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a") as stream:
        fcntl.flock(stream, fcntl.LOCK_EX)
        try:
            yield
        finally:
            fcntl.flock(stream, fcntl.LOCK_UN)


def cache_namespace(root):
    """Create the current namespace under a short lifecycle lock.

    Previous layouts remain untouched and are never consulted.  The lock only
    protects namespace creation; compilation uses independent per-key locks.
    """
    root.mkdir(parents=True, exist_ok=True)
    lock_path = root/("."+CACHE_FORMAT+".lifecycle.lock")
    with lock_path.open("a") as stream:
        fcntl.flock(stream, fcntl.LOCK_EX)
        namespace = root/CACHE_FORMAT
        namespace.mkdir(parents=True, exist_ok=True)
        marker = namespace/"format.json"
        expected = {"format": CACHE_FORMAT}
        try:
            current = json.loads(marker.read_text())
        except (OSError, ValueError, UnicodeError):
            current = None
        if current != expected:
            save_json(marker, expected)
        fcntl.flock(stream, fcntl.LOCK_UN)
    return namespace


def tokens(source, first_line=1):
    pattern = re.compile(r'\s+|//[^\n]*|/\*[\s\S]*?\*/|"(?:\\[\s\S]|[^"\\])*"'
                         r'|[A-Za-z_][A-Za-z_0-9]*|[0-9]+|\.\.|<=|>=|==|!=|<>|&&|\|\||\+\+|--|\*\*|::|.', re.S)
    result = []
    line = first_line
    for match in pattern.finditer(source):
        value = match.group()
        if not (value.isspace() or value.startswith(("//", "/*"))):
            result.append([value, line])
        line += value.count("\n")
    return result


class Reader:
    def __init__(self, items):
        self.items = items + [["<eof>", items[-1][1] if items else 1]]
        self.pos = 0

    def peek(self):
        return self.items[self.pos][0]

    def take(self, expected=None):
        value, line = self.items[self.pos]
        if expected is not None and value != expected:
            raise CompileError(f"line {line}: expected {expected!r}, got {value!r}")
        if value == "<eof>":
            raise CompileError(f"line {line}: unexpected end of source")
        self.pos += 1
        return value

    def accept(self, value):
        if self.peek() == value:
            self.take()
            return True
        return False

    def group(self, opening, closing):
        self.take(opening)
        start = self.pos
        depth = 1
        while depth:
            value = self.take()
            depth += (value == opening) - (value == closing)
        return self.items[start:self.pos-1]


class LibraryScanner:
    """Index top-level library declarations without tokenizing help text.

    Singular's library loader accepts both quoted help strings with escaped
    quotes and legacy help containing literal, embedded quotes.  Those quotes
    are documentation delimiters only in the top-level procedure header; using
    the executable tokenizer for that region can therefore invent tokens and
    braces.  This scanner is deliberately scoped to imports, procedure
    signatures and balanced bodies.  The existing tokenizer and Parser still
    validate every selected executable body.
    """

    NAME_START = frozenset("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_@'")
    NAME_PART = NAME_START | frozenset("0123456789")

    def __init__(self, source):
        self.source = source
        self.length = len(source)
        self.pos = 0
        self.newlines = [i for i, value in enumerate(source) if value == "\n"]
        self.procedures = {}
        self.imports = []

    def line(self, position=None):
        return bisect.bisect_left(self.newlines, self.pos if position is None else position) + 1

    def error(self, message, position=None):
        raise CompileError(f"line {self.line(position)}: {message}")

    def skip_comment(self, position):
        if self.source.startswith("//", position):
            newline = self.source.find("\n", position + 2)
            return self.length if newline < 0 else newline + 1
        if self.source.startswith("/*", position):
            closing = self.source.find("*/", position + 2)
            if closing < 0:
                self.error("unterminated comment", position)
            return closing + 2
        return position

    def skip_trivia_at(self, position):
        while position < self.length:
            if self.source[position].isspace():
                position += 1
                continue
            after = self.skip_comment(position)
            if after != position:
                position = after
                continue
            break
        return position

    def skip_trivia(self):
        self.pos = self.skip_trivia_at(self.pos)

    def skip_initial_trivia_at(self, position):
        """Skip INITIAL whitespace/comments, including column-zero shebangs."""
        while position < self.length:
            if self.source[position].isspace():
                position += 1
                continue
            if ((position == 0 or self.source[position - 1] == "\n") and
                    self.source.startswith("#!", position)):
                newline = self.source.find("\n", position)
                position = self.length if newline < 0 else newline + 1
                continue
            after = self.skip_comment(position)
            if after != position:
                position = after
                continue
            break
        return position

    def skip_top_trivia(self, line_start, stop_at_help_delimiter=False):
        """Skip top-level trivia and retain declaration-line eligibility."""
        while self.pos < self.length:
            if (not stop_at_help_delimiter and
                    (self.pos == 0 or self.source[self.pos - 1] == "\n") and
                    self.source.startswith("#!", self.pos)):
                newline = self.source.find("\n", self.pos)
                self.pos = self.length if newline < 0 else newline + 1
                line_start = True
                continue
            if (stop_at_help_delimiter and
                    (self.pos == 0 or self.source[self.pos - 1] == "\n") and
                    self.source.startswith("//", self.pos)):
                break
            value = self.source[self.pos]
            if value == "\n":
                self.pos += 1
                line_start = True
                continue
            if value.isspace():
                self.pos += 1
                continue
            if stop_at_help_delimiter and self.source.startswith("/*", self.pos):
                break
            after = self.skip_comment(self.pos)
            if after == self.pos:
                break
            if "\n" in self.source[self.pos:after]:
                line_start = True
            self.pos = after
        return line_start

    def skip_header_trivia(self, line_start):
        """Skip only trivia which Singular's exclusive HEADER state accepts."""
        while self.pos < self.length:
            if self.source[self.pos] == "\n":
                self.pos += 1
                line_start = True
                continue
            column_zero = self.pos == 0 or self.source[self.pos - 1] == "\n"
            if column_zero and self.source.startswith(("//", "#!"), self.pos):
                newline = self.source.find("\n", self.pos)
                self.pos = self.length if newline < 0 else newline + 1
                line_start = True
                continue
            break
        return line_start

    def word_at(self, position):
        if position >= self.length or self.source[position] not in self.NAME_START:
            return None, position
        end = position + 1
        while end < self.length and self.source[end] in self.NAME_PART:
            end += 1
        return self.source[position:end], end

    def take_word(self):
        value, end = self.word_at(self.pos)
        if value is None:
            self.error("expected identifier")
        self.pos = end
        return value

    def signature_trivia_at(self, position):
        """Skip pdef trivia without consuming a plain source newline.

        The native scanner leaves pdef at a newline unless it is hidden inside
        a block comment.  That distinction separates legacy ``proc p`` (the
        implicit ``list #`` form) from an explicit parenthesized signature.
        """
        while position < self.length:
            while position < self.length and self.source[position] in " \t\r":
                position += 1
            if self.source.startswith("/*", position):
                position = self.skip_comment(position)
                continue
            break
        return position

    def quoted_end(self, position):
        if position >= self.length or self.source[position] != '"':
            self.error("expected string literal", position)
        cursor = position + 1
        while cursor < self.length:
            value = self.source[cursor]
            if value == "\\":
                cursor += 2
            elif value == '"':
                return cursor + 1
            else:
                cursor += 1
        self.error("unterminated string literal", position)

    def balanced_end(self, position, opening, closing):
        if position >= self.length or self.source[position] != opening:
            self.error(f"expected {opening!r}", position)
        depth = 1
        cursor = position + 1
        while cursor < self.length:
            after = self.skip_comment(cursor)
            if after != cursor:
                cursor = after
                continue
            value = self.source[cursor]
            if value == '"':
                cursor = self.quoted_end(cursor)
            elif value == opening:
                depth += 1
                cursor += 1
            elif value == closing:
                depth -= 1
                cursor += 1
                if depth == 0:
                    return cursor
            else:
                cursor += 1
        self.error(f"missing closing {closing!r}", position)

    def header_assignment_end(self, position):
        """Return the end of a new-style quoted header value, if present."""
        cursor = position
        while cursor < self.length and self.source[cursor].isspace():
            cursor += 1
        if cursor >= self.length or self.source[cursor] != "=":
            return None
        cursor += 1
        while cursor < self.length and self.source[cursor].isspace():
            cursor += 1
        if cursor >= self.length or self.source[cursor] != '"':
            return None
        return self.quoted_end(cursor)

    def example_end(self, position):
        """Skip from an ``example`` keyword through its balanced body."""
        cursor = position
        while cursor < self.length:
            after = self.skip_comment(cursor)
            if after != cursor:
                cursor = after
                continue
            value = self.source[cursor]
            if value == '"':
                cursor = self.quoted_end(cursor)
            elif value == "{":
                return self.balanced_end(cursor, "{", "}")
            else:
                cursor += 1
        self.error("example body is missing", position)

    def help_body(self, position):
        """Return the opening body brace following a procedure header.

        First recognize a well-formed quoted help string, in which braces are
        ordinary documentation.  If its first unescaped quote is embedded text
        rather than the delimiter before the body, match Singular's legacy
        rule instead: the first unescaped opening brace starts the body.  Legacy
        help can contain literal quotes, but must spell a documentation brace as
        ``\\{``; this is the same distinction made by libparse.l.
        """
        cursor = position
        if self.source[position] == '"':
            try:
                quoted = self.quoted_end(position)
            except CompileError:
                quoted = None
            if quoted is not None:
                after = self.skip_trivia_at(quoted)
                if after < self.length and self.source[after] == "{":
                    return after
            cursor += 1
        while cursor < self.length:
            if self.source.startswith("\\{", cursor):
                cursor += 2
            elif self.source[cursor] == "{":
                return cursor
            else:
                cursor += 1
        self.error("procedure body is missing", position)

    def add_import(self):
        self.skip_trivia()
        if self.pos >= self.length or self.source[self.pos] != '"':
            raise CompileError("dynamic LIB declaration is unsupported")
        end = self.quoted_end(self.pos)
        self.imports.append(self.source[self.pos + 1:end - 1])
        self.pos = self.skip_trivia_at(end)
        if self.pos >= self.length or self.source[self.pos] != ";":
            self.error("expected ';' after LIB declaration")
        self.pos += 1

    def add_procedure(self, private):
        while self.pos < self.length and self.source[self.pos] in " \t":
            self.pos += 1
        name_position = self.pos
        name = self.take_word()
        params = [["list", self.line(name_position)], ["#", self.line(name_position)]]
        signature = self.signature_trivia_at(self.pos)
        if signature < self.length and self.source[signature] == "(":
            end = self.balanced_end(signature, "(", ")")
            params = tokens(self.source[signature + 1:end - 1], self.line(signature + 1))
            self.pos = end
        else:
            self.pos = signature
        self.skip_trivia()
        if self.pos < self.length and self.source[self.pos] == ";":
            self.pos += 1
            self.skip_trivia()
        if self.pos >= self.length:
            self.error(f"procedure {name} has no body")
        if self.source[self.pos] == "=":
            self.error("dynamic procedure declaration is unsupported")
        body_start = self.pos if self.source[self.pos] == "{" else self.help_body(self.pos)
        body_end = self.balanced_end(body_start, "{", "}")
        body = tokens(self.source[body_start + 1:body_end - 1], self.line(body_start + 1))
        if name in self.procedures:
            raise CompileError(f"duplicate procedure {name}")
        self.procedures[name] = dict(name=name, private=private, params=params, body=body)
        self.pos = body_end

    def scan(self):
        # The native library scanner starts in legacy header/help mode.  Quotes
        # and braces there are prose; only declarations beginning a physical
        # line are significant.  A declaration or new-style header assignment
        # switches to INITIAL-like top-level scanning, where bodies, strings and
        # examples need their own balanced skipping.
        preamble = True
        header = True
        line_start = True
        while True:
            if preamble and header:
                line_start = self.skip_header_trivia(line_start)
            else:
                line_start = self.skip_top_trivia(line_start, preamble and not header)
            if self.pos >= self.length:
                break
            column_zero = self.pos == 0 or self.source[self.pos - 1] == "\n"
            if preamble and column_zero and self.source.startswith(("//", "#!"), self.pos):
                newline = self.source.find("\n", self.pos)
                self.pos = self.length if newline < 0 else newline + 1
                line_start = True
                if not header:
                    preamble = False
                continue
            value = self.source[self.pos]
            if value == '"':
                self.pos = self.pos + 1 if preamble else self.quoted_end(self.pos)
                if preamble:
                    header = False
                line_start = False
                continue
            if value == "{":
                self.pos = self.pos + 1 if preamble else self.balanced_end(self.pos, "{", "}")
                if preamble:
                    header = False
                line_start = False
                continue
            word, end = self.word_at(self.pos)
            if word is None:
                self.pos += 1
                if preamble:
                    header = False
                line_start = False
                continue
            self.pos = end
            horizontal = self.pos
            while horizontal < self.length and self.source[horizontal] in " \t":
                horizontal += 1
            # header/help are exclusive native lexer states: only these exact
            # column-zero forms leave legacy preamble mode.  In particular,
            # prose beginning with LIB, proc(...), an indented proc, or static
            # proc is still documentation.  INITIAL mode below is deliberately
            # broader, matching the native scanner after its first declaration.
            preamble_proc = (preamble and column_zero and word == "proc" and
                             self.pos < self.length and self.source[self.pos] == " " and
                             self.word_at(horizontal)[0] is not None)
            preamble_lib = (preamble and column_zero and word == "LIB" and
                            horizontal > self.pos and horizontal < self.length and
                            self.source[horizontal] == '"')
            line_end = self.source.find("\n", self.pos)
            if line_end < 0:
                line_end = self.length
            lib_tail = self.source[self.pos:line_end]
            preamble_dynamic_lib = (preamble and column_zero and word == "LIB" and
                                    (lib_tail.lstrip().startswith("(") or
                                     re.fullmatch(r"[ \t]+[A-Za-z_@'][A-Za-z0-9_@']*[ \t]*;[ \t]*(?://.*)?",
                                                  lib_tail) is not None))
            initial_proc = (not preamble and word == "proc" and
                            horizontal > self.pos and
                            self.word_at(horizontal)[0] is not None)
            if preamble_lib or (not preamble and word == "LIB"):
                self.add_import()
                preamble = False
            elif preamble_dynamic_lib:
                raise CompileError("dynamic LIB declaration is unsupported")
            elif preamble_proc or initial_proc:
                self.add_procedure(False)
                preamble = False
            elif not preamble and word == "static":
                after = self.skip_initial_trivia_at(self.pos)
                following, following_end = self.word_at(after)
                if following == "proc":
                    self.pos = following_end
                    self.add_procedure(True)
                    preamble = False
            elif preamble and word in ("info", "version", "category", "url"):
                assignment_end = self.header_assignment_end(self.pos)
                if assignment_end is not None:
                    self.pos = assignment_end
                    preamble = False
            elif not preamble and word == "example":
                self.pos = self.example_end(self.pos)
            elif preamble:
                header = False
            line_start = False
        return dict(procedures=self.procedures, imports=self.imports)


def _literal_mod_init_newstructs(procedures):
    """Return unconditional literal newstruct declarations run at LIB load."""
    procedure = procedures.get("mod_init")
    if procedure is None:
        return []
    values = [token for token, _ in procedure["body"]]
    result, seen, depth = [], set(), 0
    for index, value in enumerate(values):
        if value == "{":
            depth += 1
            continue
        if value == "}":
            depth -= 1
            continue
        # A declaration after a straight-line terminator is not installed by
        # mod_init and therefore cannot safely define a nominal ABI type.
        if depth == 0 and value in ("return", "ERROR"):
            break
        if depth != 0 or value != "newstruct" or index+6 >= len(values):
            continue
        call = values[index:index+7]
        if call[1] != "(" or call[3] != "," or call[5:] != [")", ";"]:
            continue
        if not (call[2].startswith('"') and call[4].startswith('"')):
            continue
        try:
            name = json.loads(call[2])
            json.loads(call[4])
        except (TypeError, ValueError):
            continue
        if not isinstance(name, str) or not IDENT.fullmatch(name) or name in seen:
            continue
        seen.add(name)
        result.append(name)
    return result


def library_index(source):
    result = LibraryScanner(source).scan()
    result["newstructs"] = _literal_mod_init_newstructs(result["procedures"])
    return result


PRECEDENCE = {"or": 1, "||": 1, "and": 2, "&&": 2,
              "==": 3, "!=": 3, "<>": 3, "<": 4, "<=": 4, ">": 4, ">=": 4,
              "..": 5, ":": 6, "+": 6, "-": 6, "*": 7, "/": 7, "%": 7,
              "div": 7, "mod": 7, "^": 9, "**": 9}
KERNELS = {"nvars", "ncols", "nrows", "deg", "size", "poly", "var", "lead", "leadmonom", "jet",
           "number", "int", "diff", "numerator", "denominator", "subst", "list", "delete",
           "insert", "intvec", "ideal", "string", "leadcoef", "leadexp", "typeof", "ringlist"}
OWNED = {"poly": "snative::Poly", "number": "snative::Number", "list": "snative::List",
         "paramnames": "snative::List", "intvec": "snative::Intvec",
         "string": "snative::String", "ideal": "snative::Ideal"}
CPP_TYPES = {"int": "long", "poly": "poly", "number": "number", "list": "lists",
             "value": "leftv", "paramname": "leftv", "string": "const char *",
             "ideal": "snative::IdealView",
             "intvec": "intvec *", "varargs": "lists"}


def exits(tree):
    """Conservative control-flow exits, including paths bypassing later returns."""
    if tree[0] in ("return", "error"):
        return {"return"}
    if tree[0] in ("break", "continue"):
        return {tree[0]}
    if tree[0] == "block":
        paths = {"next"}
        for child in tree[1]:
            if "next" not in paths:
                break
            paths = (paths-{"next"}) | exits(child)
        return paths
    if tree[0] == "if":
        return exits(tree[2]) | exits(tree[3])
    if tree[0] in ("for", "while"):
        return {"next"} | (exits(tree[-1]) & {"return"})
    return {"next"}


def terminates(tree):
    return exits(tree)=={"return"}


def trees(tree):
    """Walk AST nodes, not declaration pairs or argument/statement lists."""
    yield tree
    if tree[0] == "declare":
        children = [value for _, value in tree[2] if value is not None]
    elif tree[0] == "block":
        children = tree[1]
    elif tree[0] == "call":
        children = tree[2]
    elif tree[0] == "sequence":
        children = tree[1]
    else:
        children = [child for child in tree[1:] if isinstance(child, list)]
    for child in children:
        yield from trees(child)


class Parser(Reader):
    def sequence(self):
        """Parse Singular's lowest-precedence comma expression.

        Calls use ``expression`` for each argument, so their commas remain
        argument separators. Parenthesized and assignment contexts use this
        method and retain the sequence explicitly until a destination type is
        known by the emitter.
        """
        values = [self.expression()]
        while self.accept(","):
            values.append(self.expression())
        values = [child for value in values
                  for child in (value[1] if value[0]=="sequence" else [value])]
        return values[0] if len(values)==1 else ["sequence", values]

    def expression(self, level=0):
        if self.peek() in ("-", "!", "not"):
            left = ["unary", self.take(), self.expression(8)]
        elif self.accept("("):
            left = self.sequence()
            self.take(")")
        else:
            value = self.take()
            if value.isdigit():
                if int(value) > 2147483647 or (len(value)>1 and value[0]=="0"):
                    raise CompileError("integer literal outside the supported int syntax")
                left = ["int", int(value)]
            elif value.startswith('"') and "\\" not in value:
                left = ["string", value[1:-1]]
            elif value == "#":
                left = ["varargs"]
            elif IDENT.fullmatch(value):
                if self.accept("("):
                    args = []
                    if self.peek() != ")":
                        argument = self.expression()
                        args.append(argument)
                        while self.accept(","):
                            argument = self.expression()
                            args.append(argument)
                    # User procedures receive an exprlist, as do the list and
                    # intvec constructors: f((1,2)) supplies two arguments.
                    # Fixed-arity kernel slots are different.  For example,
                    # insert(L,(1,2)) consumes the head of the parenthesized
                    # expression as one value; keep that shape so the emitter
                    # can conservatively reject it instead of moving position.
                    if value not in KERNELS or value in ("list", "intvec", "ideal", "string"):
                        args = [child for argument in args
                                for child in (argument[1] if argument[0]=="sequence" else [argument])]
                    self.take(")")
                    left = ["call", value, args]
                else:
                    left = ["var", value]
            else:
                raise CompileError(f"unsupported expression token {value!r}")
        while self.peek() in ("[", "."):
            if self.accept("["):
                first = self.expression()
                if self.accept(","):
                    left = ["index2", left, first, self.expression()]
                else:
                    left = ["index", left, first]
                self.take("]")
            else:
                self.take(".")
                left = ["field", left, self.take()]
        while self.peek() in PRECEDENCE and PRECEDENCE[self.peek()] >= level:
            op = self.take()
            left = ["binary", op, left, self.expression(PRECEDENCE[op]+1)]
        return left

    def simple(self):
        if self.peek() in ("int", "intvec", "poly", "number", "string", "ideal", "list", "def"):
            kind = self.take()
            declarations = []
            while True:
                name = self.take()
                if not identifier(name):
                    raise CompileError(f"unsupported identifier {name!r}")
                value = (self.sequence() if kind in ("intvec", "ideal", "list") else self.expression()) if self.accept("=") else None
                declarations.append([name, value])
                if not self.accept(","):
                    break
            # Singular declares every name first and then distributes one
            # common comma RHS.  Until that pairwise assignment is modeled,
            # do not accept the superficially similar C declaration shape.
            if len(declarations)>1 and any(value is not None for _, value in declarations):
                raise CompileError("initialized multi-name declarations require contextual comma assignment")
            return ["declare", kind, declarations]
        lhs = self.expression()
        if self.accept("="):
            return ["assign", lhs, self.sequence()]
        if self.peek() in ("++", "--"):
            return ["increment", lhs, 1 if self.take()=="++" else -1]
        raise CompileError("only declarations, assignments and postfix increments are supported here")

    def block(self):
        self.take("{")
        statements = []
        while self.peek() != "}":
            statements.append(self.statement())
        self.take("}")
        return ["block", statements]

    def statement(self):
        if self.peek() == "{":
            return self.block()
        if self.accept(";"):
            return ["empty"]
        if self.accept("execute"):
            self.take("(")
            value = self.expression()
            self.take(")")
            self.take(";")
            # Only a literal number declaration followed by a proven ring
            # parameter name is lowered. Arbitrary source execution stays out.
            if value[0:2] != ["binary", "+"] or value[2][0] != "string":
                raise CompileError("execute requires a statically known number declaration")
            match = re.fullmatch(r"number\s+([A-Za-z][A-Za-z0-9_]*)\s*=\s*", value[2][1])
            if not match or not identifier(match[1]):
                raise CompileError("execute requires a statically known number declaration")
            return ["parameter_declare", match[1], value[3]]
        if self.accept("kill"):
            name = self.take()
            self.take(";")
            return ["kill", name]
        if self.accept("ERROR"):
            self.take("(")
            message = self.take()
            # A literal message is sufficient for terminal error paths. Do not
            # accept Singular escape sequences with different C++ semantics.
            if not message.startswith('"') or "\\" in message:
                raise CompileError("ERROR requires a plain literal message")
            self.take(")")
            self.take(";")
            return ["error", message[1:-1]]
        if self.accept("return"):
            self.take("(")
            result = ["return", self.sequence()]
            self.take(")")
            self.take(";")
            return result
        if self.accept("if"):
            self.take("(")
            condition = self.expression()
            self.take(")")
            yes = self.block()
            no = self.block() if self.accept("else") else ["empty"]
            return ["if", condition, yes, no]
        if self.accept("while"):
            self.take("(")
            condition = self.expression()
            self.take(")")
            return ["while", condition, self.block()]
        if self.accept("for"):
            self.take("(")
            initial = self.simple()
            self.take(";")
            condition = self.expression()
            self.take(";")
            step = self.simple()
            self.take(")")
            return ["for", initial, condition, step, self.block()]
        if self.peek() in ("break", "continue"):
            result = [self.take()]
            self.take(";")
            return result
        result = self.simple()
        self.take(";")
        return result

    def parse(self):
        result = []
        while self.peek() != "<eof>":
            result.append(self.statement())
        return ["block", result]


def signature(proc, newstructs=(), include_nominal=False):
    if [t[0] for t in proc["params"]] == ["list", "#"]:
        # The interpreter presents variadic arguments as the procedure-local
        # list '#'. Materialize the complete heterogeneous list at the entry.
        # The internal name is deliberately unavailable to source declarations.
        result = [["varargs", "__snative_varargs"]]
        return (result, {}) if include_nominal else result
    r = Reader(proc["params"])
    params = []
    nominal = {}
    newstructs = set(newstructs)
    while r.peek() != "<eof>":
        kind, name = r.take(), r.take()
        if (kind not in ("int", "intvec", "poly", "number", "string", "ideal", "list", "def")
                and kind not in newstructs) or not identifier(name):
            raise CompileError("supported parameters are int/intvec/poly/number/string/ideal/list and checked def")
        if name in [n for _, n in params] or name == "basering":
            raise CompileError(f"duplicate/reserved parameter {name}")
        if kind in newstructs:
            nominal[name] = kind
            kind = "value"
        params.append(["value" if kind=="def" else kind, name])
        if not r.accept(","):
            break
    if r.peek() != "<eof>":
        raise CompileError("unsupported parameter list")
    return (params, nominal) if include_nominal else params


class Closure:
    def __init__(self, cache, search):
        self.cache, self.search = cache, search
        self.libraries, self.nodes, self.newstruct_types = {}, {}, {}
        self.stack = []
        self.stats = empty_stats()

    def cached(self, category, key, producer, counter):
        path = self.cache / category / (digest([file_digest(__file__), key]) + ".json")
        try:
            return json.loads(path.read_text())
        except (ValueError, OSError, UnicodeError):
            pass
        with key_lock(self.cache, category, key):
            try:
                return json.loads(path.read_text())
            except (ValueError, OSError, UnicodeError):
                pass
            value = producer()
            save_json(path, value)
            self.stats[counter] += 1
            return value

    def library(self, path):
        path = str(Path(path).resolve())
        if path not in self.libraries:
            source = Path(path).read_bytes()
            self.libraries[path] = self.cached("index", digest([VERSION, digest(source)]),
                                               lambda: library_index(source.decode()), "libraries_indexed")
        return self.libraries[path]

    def import_path(self, parent, name):
        for directory in [Path(parent).parent] + self.search:
            candidate = directory / name
            if candidate.is_file():
                return str(candidate.resolve())
        raise CompileError(f"cannot resolve imported library {name}")

    def available_newstructs(self, path):
        path = str(Path(path).resolve())
        if path in self.newstruct_types:
            return self.newstruct_types[path]
        found, seen, pending = set(), set(), [path]
        while pending:
            current = pending.pop()
            if current in seen:
                continue
            seen.add(current)
            library = self.library(current)
            found.update(library.get("newstructs", ()))
            for name in library["imports"]:
                if not name.endswith(".lib"):
                    continue
                try:
                    pending.append(self.import_path(current, name))
                except CompileError:
                    # Type discovery must not make an otherwise irrelevant,
                    # unavailable optional import part of this closure.
                    continue
        self.newstruct_types[path] = frozenset(found)
        return self.newstruct_types[path]

    def resolve(self, path, name):
        if name in self.library(path)["procedures"]:
            return path, name
        found, seen = set(), set()
        def visit(current):
            if current in seen:
                return
            seen.add(current)
            lib = self.library(current)
            candidate = lib["procedures"].get(name)
            if current != path and candidate and not candidate["private"]:
                found.add((current, name))
            for imported in lib["imports"]:
                visit(self.import_path(current, imported))
        visit(path)
        if len(found) != 1:
            raise CompileError(f"{'ambiguous' if found else 'unresolved'} library/kernel call {name}")
        return next(iter(found))

    def visit(self, path, name):
        key = (path, name)
        if key in self.nodes:
            return self.nodes[key]
        self.stack.append(f"{Path(path).name}::{name}")
        try:
            proc = self.library(path)["procedures"].get(name)
            if proc is None:
                raise CompileError(f"procedure {name} not found")
            if not identifier(name) or name=="compilerCalls":
                raise CompileError(f"unsupported/reserved procedure name {name}")
            params, nominal_params = signature(
                proc, self.available_newstructs(path), include_nominal=True)
            if any(t[0]=="#" for t in proc["body"]) and not (len(params)==1 and params[0][0]=="varargs"):
                raise CompileError("# is supported only in a list # procedure")
            ast = self.cached("ast", digest([VERSION, [t[0] for t in proc["body"]]]),
                              lambda: Parser(proc["body"]).parse(), "procedures_parsed")
            node = dict(path=path, name=name, params=params, nominal_params=nominal_params,
                        ast=ast, dependencies=set(), calls={},
                        returns=set(), chain=list(self.stack), private=proc["private"],
                        symbol="sn_" + name + "_" + digest([path, name, params, nominal_params])[:12])
            parameter_names = {name for kind, name in params if kind!="varargs"}
            node["mutated"] = {tree[1][1] if tree[1][0]=="var" else tree[1][1][1]
                               for tree in trees(ast)
                               if tree[0] in ("assign", "increment") and
                               tree[1][0] in ("var", "index") and
                               (tree[1][0]=="var" or tree[1][1][0]=="var") and
                               (tree[1][1] if tree[1][0]=="var" else tree[1][1][1]) in parameter_names}
            self.nodes[key] = node  # forward declarations permit recursive closures
            for tree in trees(ast):
                if tree[0] == "call" and tree[1] not in KERNELS:
                    target = self.resolve(path, tree[1])
                    self.visit(*target)
                    node["dependencies"].add(target)
                    node["calls"][tree[1]] = target
            return node
        except CompileError as exc:
            if "call chain:" in str(exc):
                raise
            raise CompileError(f"{exc}\ncall chain: {' -> '.join(self.stack)}") from exc
        finally:
            self.stack.pop()

    def generate(self):
        # Monotone return-type inference over the whole closure. Base returns
        # seed recursive components; ungrounded cycles and mixed types reject.
        def infer(tree, types, node):
            op = tree[0]
            if op == "int":
                return {"int"}
            if op == "string":
                return {"string"}
            if op == "field":
                return {"value"}
            if op == "var":
                if tree[1] not in types:
                    return set()
                return {"list" if types[tree[1]]=="varargs" else types[tree[1]]}
            if op == "varargs":
                return {"list"}
            if op == "index":
                indexes = infer(tree[2], types, node)
                # An intvec index denotes an interpreter expression list.  It
                # becomes one intvec only in an explicitly typed intvec
                # assignment/declaration, handled contextually by the emitter.
                if "intvec" in indexes:
                    return set()
                return {"poly" if kind in ("poly", "ideal") else "string" if kind=="string"
                        else "value" if kind in ("list", "value") else "int"
                        for kind in infer(tree[1], types, node)}
            if op == "index2":
                return set()
            if op == "sequence":
                # A bare comma expression remains an interpreter exprlist.
                # Only an explicit intvec destination may collect its values.
                return set()
            if op == "unary":
                if tree[2][0]=="sequence":
                    return set()
                return infer(tree[2], types, node) if tree[1]=="-" else {"int"}
            if op == "binary":
                if tree[2][0]=="sequence" or tree[3][0]=="sequence":
                    return set()
                if tree[1] in ("..", ":"):
                    return {"intvec"}
                if tree[1] not in ("+", "-", "*", "/", "%", "div", "mod", "^", "**"):
                    return {"int"}
                left, right = infer(tree[2], types, node), infer(tree[3], types, node)
                if tree[1] in ("^", "**"):
                    return left
                return {"string" if a==b=="string" else "list" if "list" in (a,b)
                        else "ideal" if "ideal" in (a,b) else "poly" if "poly" in (a,b) else "number" if "number" in (a,b)
                        else "intvec" if "intvec" in (a,b) else "value" if "value" in (a,b) else "int"
                        for a in left for b in right}
            if op == "call":
                if tree[1] in KERNELS:
                    if tree[1]=="delete" and tree[2]:
                        return infer(tree[2][0], types, node) & {"list", "intvec"}
                    return {"list" if tree[1] in ("list", "delete", "insert", "ringlist") else "string" if tree[1] in ("typeof", "string")
                            else "ideal" if tree[1]=="ideal"
                            else "poly" if tree[1] in ("poly", "var", "lead", "leadmonom", "jet", "subst")
                            else "number" if tree[1] in ("number", "diff", "numerator", "denominator", "leadcoef")
                            else "intvec" if tree[1] in ("intvec", "leadexp") else "int"}
                return self.nodes[node["calls"][tree[1]]]["returns"]
            return set()
        changed = True
        while changed:
            changed = False
            for node in self.nodes.values():
                types = dict((n,t) for t,n in node["params"] if t!="varargs")
                for tree in trees(node["ast"]):
                    if tree[0] == "parameter_declare":
                        types[tree[1]] = "number"
                    if tree[0] == "declare":
                        for n, value in tree[2]:
                            if tree[1] != "def":
                                types[n] = tree[1]
                            elif value is not None:
                                candidates = infer(value, types, node)
                                if len(candidates)==1:
                                    types[n] = next(iter(candidates))
                before = set(node["returns"])
                for tree in trees(node["ast"]):
                    if tree[0] == "return":
                        node["returns"].update(infer(tree[1], types, node))
                changed |= before != node["returns"]
        for node in sorted(self.nodes.values(), key=lambda n: len(n["chain"]), reverse=True):
            if len(node["returns"])!=1 or not node["returns"] <= {"int", "intvec", "poly", "number", "string", "ideal", "list"}:
                raise CompileError("cannot infer one int/intvec/poly/number/string/ideal/list return type (mixed types or ungrounded recursion)\n"
                                   + "call chain: " + " -> ".join(node["chain"]))
            node["result"] = next(iter(node["returns"]))
            node["contextual_sequence_result"] = any(
                tree[0] == "return" and tree[1][0] == "sequence"
                for tree in trees(node["ast"]))
            node["needs_ring"] = (node["result"] in ("poly", "number", "ideal") or any(t in ("poly", "number", "ideal") for t,_ in node["params"]) or
                                  any((t[0]=="var" and t[1]=="basering") or
                                      (t[0]=="declare" and t[1] in ("poly", "number", "ideal")) or
                                      (t[0]=="call" and t[1] in KERNELS-{"size", "nrows", "list", "delete", "insert", "int", "intvec", "string", "typeof"})
                                      for t in trees(node["ast"])))
        changed = True
        while changed:
            changed = False
            for node in self.nodes.values():
                if not node["needs_ring"] and any(self.nodes[d]["needs_ring"] for d in node["dependencies"]):
                    node["needs_ring"] = changed = True
        for node in self.nodes.values():
            try:
                node["code"] = Emitter(self, node).generate()
            except CompileError as exc:
                raise CompileError(f"{exc}\ncall chain: {' -> '.join(node['chain'])}") from exc


def declaration(node):
    params = [f"{CPP_TYPES[t]} v_{n}" for t, n in node["params"]]
    return OWNED.get(node["result"], "long") + " " + node["symbol"] + "(" + ", ".join(["snative::Context &ctx"]+params) + ")"


class Emitter:
    def __init__(self, closure, node):
        self.closure, self.node = closure, node
        self.types = dict((n, t) for t, n in node["params"] if t!="varargs")
        self.varargs_name = next((name for kind, name in node["params"] if kind=="varargs"), None)
        self.locals, self.lines = [], []
        self.poly_locals, self.owned = set(), {}
        self.bound_params = {}
        for name, nominal in node.get("nominal_params", {}).items():
            self.locals.append(
                f"v_{name}=snative::newstruct_argument(v_{name}, {json.dumps(nominal)});")
        for name, kind in self.types.items():
            if kind == "poly":
                self.locals.append(f"snative::PolyBinding b_{name}(v_{name}, ctx.r);")
                self.bound_params[name] = f"b_{name}.get()"
            elif kind == "ideal":
                self.locals.append(f"snative::IdealBinding b_{name}(v_{name}, ctx.r);")
                self.bound_params[name] = f"b_{name}.get()"
        self.cow_params = {name: kind for name, kind in self.types.items()
                           if name in node["mutated"] and kind in ("poly", "number", "string", "ideal", "list", "intvec")}
        for name, kind in self.cow_params.items():
            owner = {"poly":"CowPoly", "number":"CowNumber", "string":"CowString", "ideal":"CowIdeal",
                     "list":"CowList", "intvec":"CowIntvec"}[kind]
            suffix = ", ctx.r" if kind in ("poly", "number", "ideal", "list") else ""
            source = self.bound_params.get(name, f"v_{name}")
            self.locals.append(f"snative::{owner} c_{name}({source}{suffix});")
        self.declared = set(self.types)
        self.guarded_declarations = set()
        self.parameter_locals = set()
        self.poly_scans = {}
        self.serial, self.indent, self.loop = 0, 1, 0

    def out(self, text):
        self.lines.append("  "*self.indent + text)

    def lexical_state(self):
        """Return the name/type state which may not escape a C++ block."""
        return (dict(self.types), set(self.declared), set(self.poly_locals),
                set(self.parameter_locals))

    def restore_lexical_state(self, state):
        self.types, self.declared, self.poly_locals, self.parameter_locals = state

    def temp(self, code, kind="int"):
        self.serial += 1
        name = "t" + str(self.serial)
        if kind in OWNED:
            self.out(f"{OWNED[kind]} {name} = {code};")
            self.out('if (errorreported) throw std::runtime_error("native algebra kernel failed");')
            self.owned[name+".get()"] = name
            return kind, name+".get()"
        self.out(f"const {CPP_TYPES.get(kind, kind)} {name} = {code};")
        return kind, name

    def own(self, value, kind="poly"):
        # This generic path consumes only expression temporaries. The guarded
        # accumulator exception is implemented by owned_arguments below;
        # procedure arguments and all other named values remain borrowed.
        if value in self.owned:
            return "std::move("+self.owned[value]+")"
        return f"{OWNED[kind]}::copy({value}{', ctx.r' if kind in ('poly', 'number', 'ideal', 'list', 'paramnames') else ''})"

    def owned_arguments(self, values, kind, consume=None):
        """Give destructive kernels owners, optionally reusing an accumulator.

        A local on the left of an assignment may replace one of the copies in
        the outermost RHS operation.  All RHS children have already been
        evaluated when this helper is called.  If the local occurs in more
        than one destructive operand, materialize the other copies first so
        C++'s unspecified argument-evaluation order cannot observe a moved-from
        local.
        """
        values = list(values)
        selected = None
        if consume is not None and consume[0] == kind:
            pointer, owner = consume[1], consume[2]
            matches = [i for i, value in enumerate(values) if value == pointer]
            if matches:
                selected = matches[0]
                for i in matches[1:]:
                    _, values[i] = self.temp(f"{OWNED[kind]}::copy({values[i]}, ctx.r)", kind)
        return [f"std::move({consume[2]})" if i == selected else self.own(value, kind)
                for i, value in enumerate(values)]

    def commutative_owner(self, values, kind, consume=None):
        """Select one existing owner for an in-place commutative operation.

        Unlike polynomial kernels, coefficient kernels only consume one
        operand. Prefer the assignment target, then an expression temporary;
        retain the ordinary non-destructive kernel when neither is owned.
        Duplicate aliases are copied before the selected owner is moved.
        """
        values = list(values)
        selected, owner = None, None
        if consume is not None and consume[0] == kind:
            matches = [i for i, value in enumerate(values) if value == consume[1]]
            if matches:
                selected, owner = matches[0], consume[2]
        if selected is None:
            for i, value in enumerate(values):
                if value in self.owned:
                    selected, owner = i, self.owned[value]
                    break
        if selected is None:
            return None, None, values
        for i, value in enumerate(values):
            if i != selected and value == values[selected]:
                _, values[i] = self.temp(f"{OWNED[kind]}::copy({value}, ctx.r)", kind)
        return selected, f"std::move({owner})", values

    def left_owner(self, values, kind, consume=None):
        """Own the left operand while keeping later aliased operands valid."""
        values = list(values)
        moving = None
        if consume is not None and consume[0] == kind and values[0] == consume[1]:
            moving = consume[2]
        elif values[0] in self.owned:
            moving = self.owned[values[0]]
        if moving is not None:
            for i in range(1, len(values)):
                if values[i] == values[0]:
                    _, values[i] = self.temp(f"{OWNED[kind]}::copy({values[i]}, ctx.r)", kind)
            return f"std::move({moving})", values
        return self.own(values[0], kind), values

    def polynomial(self, kind, value):
        if kind == "int":
            return self.temp(f"snative::Poly(p_ISet((int)({value}), ctx.r), ctx.r)", "poly")[1]
        if kind == "number":
            return self.temp(f"snative::polynomial({value}, ctx.r)", "poly")[1]
        if kind != "poly":
            raise CompileError(f"expected poly or int, got {kind}")
        return value

    def number(self, kind, value, explicit=False):
        if kind == "number":
            return value
        if kind == "int" or (kind == "poly" and explicit):
            return self.temp(f"snative::number_from_{kind}({value}, ctx.r)", "number")[1]
        if kind == "value":
            return self.temp(f"snative::number_argument({value}, ctx.r)", "number")[1]
        raise CompileError(f"expected number or int, got {kind}")

    def string_value(self, kind, value):
        if kind == "string":
            return value
        if kind == "value":
            return self.temp(f"snative::exact_string_argument({value})", "string")[1]
        raise CompileError(f"expected string, got {kind}")

    def converted_string_argument(self, value):
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"snative::StringArgument {name}({value});")
        return name+".get()"

    def ideal_value(self, kind, value):
        if kind == "ideal":
            return value
        if kind == "poly":
            return self.temp(f"snative::ideal_from_poly({value}, ctx.r)", "ideal")[1]
        if kind == "int":
            return self.temp(f"snative::ideal_from_int({value}, ctx.r)", "ideal")[1]
        if kind == "number":
            return self.temp(f"snative::ideal_from_number({value}, ctx.r)", "ideal")[1]
        if kind == "value":
            self.serial += 1
            name = "t"+str(self.serial)
            self.out(f"snative::IdealArgument {name}({value}, ctx.r);")
            return name+".get()"
        raise CompileError(f"expected ideal, poly, number, or int, got {kind}")

    def integer(self, expression):
        kind, value = self.expr(expression, expected="int")
        if kind == "value":
            return self.temp(f"snative::int_argument({value})")[1]
        if kind != "int":
            raise CompileError(f"expected int, got {kind}")
        return value

    def narrow_index(self, value):
        """Apply Singular's Subexpr::start machine-int narrowing exactly once."""
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"const int {name} = (int)({value});")
        return name

    def list_value(self, kind, value):
        if kind == "value":
            return self.temp(f"snative::plain_list_argument({value})", "lists")[1]
        if kind != "list":
            raise CompileError(f"expected list, got {kind}")
        return value

    def intvec_value(self, kind, value):
        if kind == "intvec":
            return value
        if kind != "int":
            raise CompileError(f"expected intvec or int, got {kind}")
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"snative::Intvec {name}(0);")
        self.out(f"snative::append({name}, {value});")
        self.owned[name+".get()"] = name
        return name+".get()"

    def borrowed_intvec(self, value):
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"intvec *{name} = snative::intvec_argument({value});")
        return name

    def converted_intvec_argument(self, value):
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"snative::IntvecArgument {name}({value});")
        return name+".get()"

    def converted_poly_argument(self, value):
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"snative::PolyArgument {name}({value}, ctx.r);")
        return name+".get()"

    def convert(self, kind, expression, consume=None):
        if kind=="list" and expression[0]=="sequence":
            return self.list_literal([self.expr(value) for value in expression[1]])[1]
        if kind=="ideal" and expression[0]=="sequence":
            return self.ideal_literal([self.expr(value) for value in expression[1]], flatten_ideals=True)[1]
        if kind=="intvec" and expression[0]=="index":
            indexed = self.index_expression(expression, intvec_context=True)
            return self.intvec_value(*indexed)
        if kind == "number" and expression[0] == "index":
            indexed = self.index_expression(expression, number=True)
            if indexed[0] == "number":
                return indexed[1]
            return self.number(*indexed)
        # A private comma-returning procedure may be lowered to one intvec
        # only when this expression is the complete RHS of an explicitly
        # typed intvec destination.  Pass that proof down to the call emitter;
        # an untyped use (notably list(f(...))) must retain exprlist semantics.
        args = self.expr(expression, consume, expected="intvec" if kind=="intvec" else None)
        return (self.list_value if kind=="list" else self.polynomial if kind=="poly" else
                self.intvec_value if kind=="intvec" else self.string_value if kind=="string" else
                self.ideal_value if kind=="ideal" else self.number)(*args)

    def box(self, kind, value):
        if kind == "value":
            return value
        if kind not in ("int", "number", "poly", "string", "ideal", "list", "intvec", "ring"):
            raise CompileError(f"cannot box native {kind}")
        self.serial += 1
        name = "t"+str(self.serial)
        if kind == "ideal":
            self.out(f"snative::View {name}({value});")
        else:
            self.out(f"snative::View {name}({kind.upper()}_CMD, (void*)({value}));")
        return name+".get()"

    def owned_value(self, code):
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"snative::Value {name} = {code};")
        return "value", name+".get()"

    def list_literal(self, args, variadic_arguments=False):
        """Build a list after evaluating every element, moving owned values."""
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"snative::List {name}(ctx.r);")
        self.out(f"{name}.reserve({len(args)}L);")
        for index, (kind, value) in enumerate(args, 1):
            if kind in ("poly", "number", "string", "ideal", "list", "intvec") and value in self.owned:
                owner = self.owned.pop(value)
                self.out(f"snative::list_set_owned({name}, {index}L, std::move({owner}), ctx.r);")
            else:
                setter = "list_set_argument" if variadic_arguments else "list_set"
                self.out(f"snative::{setter}({name}, {index}L, {self.box(kind, value)}, ctx.r);")
        self.out('if (errorreported) throw std::runtime_error("native algebra kernel failed");')
        self.owned[name+".get()"] = name
        return "list", name+".get()"

    def intvec_literal(self, args):
        if any(kind not in ("int", "intvec") for kind, _ in args):
            raise CompileError("intvec construction requires int/intvec elements")
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"snative::Intvec {name}(0);")
        for _, value in args:
            self.out(f"snative::append({name}, {value});")
        self.owned[name+".get()"] = name
        return "intvec", name+".get()"

    def ideal_literal(self, args, flatten_ideals=False):
        if not flatten_ideals and len(args)==1 and args[0][0]=="ideal":
            return args[0]
        allowed = ("int", "number", "poly") + (("ideal",) if flatten_ideals else ())
        if any(kind not in allowed for kind, _ in args):
            raise CompileError("ideal construction requires int/number/poly elements")
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"snative::Ideal {name}(idInit(0,1), ctx.r);")
        for kind, value in args:
            if kind == "ideal":
                self.out(f"snative::append({name}, {value});")
                continue
            polynomial = self.polynomial(kind, value)
            if polynomial in self.owned:
                owner = self.owned.pop(polynomial)
                self.out(f"snative::append({name}, std::move({owner}));")
            else:
                self.out(f"snative::append({name}, {polynomial});")
        self.owned[name+".get()"] = name
        return "ideal", name+".get()"

    def string_literal(self, args):
        self.serial += 1
        name = "t"+str(self.serial)
        self.out(f"snative::String {name}=snative::String::copy(\"\");")
        for kind, value in args:
            boxed = self.box(kind, value)
            _, converted = self.temp(f"snative::string_from_value({boxed})", "string")
            self.out(f"{name}=snative::string_add({name}.get(), {converted});")
        self.owned[name+".get()"] = name
        return "string", name+".get()"

    def index_expression(self, tree, number=False, intvec_context=False):
        if tree[1][0] == "var" and tree[2][0] == "var":
            cursor = self.poly_scans.get((tree[1][1], tree[2][1]))
            if cursor is not None:
                return self.temp(f"snative::lead({cursor}, ctx.r)", "poly")
        if tree == ["index", ["index", ["call", "ringlist", [["var", "basering"]]], ["int", 1]], ["int", 2]]:
            return self.temp("snative::parameter_names(ctx.r)", "paramnames")
        if tree[1][0] == "sequence":
            # (a,b)[i] maps indexing over both exprlist elements; it is not an
            # intvec concatenation followed by one scalar selection.
            raise CompileError("comma expression indexing source requires interpreted exprlist semantics")
        if tree[2][0] == "sequence":
            # expr[(1,2)] indexes by the head expression in the interpreter;
            # it is not the vector selector denoted by expr[intvec(1,2)].
            raise CompileError("comma expression indexing requires interpreted exprlist semantics")
        kind, vector = self.expr(tree[1])
        index_kind, index = self.expr(tree[2])
        if index_kind == "value":
            if kind == "intvec":
                if not intvec_context:
                    raise CompileError("dynamic intvec selector requires an explicit intvec assignment")
                named = str(tree[1][0]=="var").lower()
                return self.temp(f"snative::intvec_select_argument({vector}, {index}, {named})", "intvec")
            index_kind, index = self.temp(f"snative::int_argument({index})")
        if index_kind == "intvec":
            if kind != "intvec" or not intvec_context:
                raise CompileError("vector-valued indexing requires an explicit intvec assignment")
            if tree[1][0] != "var":
                raise CompileError("vector-valued indexing requires a named source")
            return self.temp(f"snative::intvec_select({vector}, {index})", "intvec")
        if index_kind != "int":
            raise CompileError("vector-valued indexing is not supported yet")
        # jjINDEX_P consumes the full Singular long.  The remaining scalar
        # index paths go through Subexpr::start (and therefore narrow to int).
        if kind == "poly":
            return self.temp(f"snative::term({vector}, {index}, ctx.r)", "poly")
        index = self.narrow_index(index)
        if kind == "value":
            if number:
                return self.temp(f"snative::indexed_number({vector}, {index}, ctx.r)", "number")
            return self.owned_value(f"snative::indexed_value({vector}, {index}, ctx.r)")
        if kind in ("list", "paramnames"):
            return self.temp(f"snative::get({vector}, {index})", "paramname" if kind=="paramnames" else "value")
        if kind == "string":
            return self.temp(f"snative::string_index({vector}, {index})", "string")
        if kind == "ideal":
            return self.temp(f"snative::ideal_get({vector}, {index}, ctx.r)", "poly")
        if kind != "intvec":
            raise CompileError("only intvec/poly/string/ideal indexing is supported")
        return self.temp(f"snative::get({vector}, {index})")

    def expr(self, tree, consume=None, expected=None):
        op = tree[0]
        if op == "int":
            return "int", str(tree[1])+"L"
        if op == "string":
            return "string", json.dumps(tree[1])
        if op == "var":
            if tree[1] == "basering":
                return "ring", "ctx.r"
            if tree[1] not in self.types:
                raise CompileError(f"unknown or unsupported global identifier {tree[1]}")
            name = tree[1]
            if name in self.cow_params:
                return self.types[name], "c_"+name+".get()"
            if name in self.bound_params:
                return self.types[name], self.bound_params[name]
            return self.types[name], "v_"+name+(".get()" if name in self.poly_locals else "")
        if op == "varargs":
            if self.varargs_name is None:
                raise CompileError("# is supported only in a list # procedure")
            return "list", "v_"+self.varargs_name
        if op == "sequence":
            values = [self.expr(value) for value in tree[1]]
            return self.intvec_literal(values)
        if op == "index":
            if tree[1][0]=="var" and tree[1][1] in self.node.get("nominal_params", {}):
                raise CompileError("direct newstruct indexing requires interpreted operator dispatch")
            return self.index_expression(tree)
        if op == "index2":
            # Keep matrix/intmat multidimensional indexing on the interpreted
            # fallback until those static types have native owners.
            raise CompileError("two-dimensional indexing requires matrix/intmat support")
        if op == "field":
            kind, value = self.expr(tree[1])
            if kind != "value" or not IDENT.fullmatch(tree[2]):
                raise CompileError("field access requires a dynamic newstruct value")
            return self.owned_value(f"snative::struct_field({value}, {json.dumps(tree[2])}, ctx.r)")
        if op == "unary":
            if tree[2][0]=="sequence":
                raise CompileError("arithmetic on a comma expression requires interpreted exprlist semantics")
            kind, value = self.expr(tree[2])
            if kind=="value" and tree[1]=="-":
                kind, value = self.owned_value(f"snative::unary_value({value}, '-', ctx.r)")
                return (self.temp(f"snative::int_argument({value})")
                        if expected=="int" else (kind, value))
            if kind == "intvec" and tree[1] == "-":
                return self.temp(f"snative::intvec_neg({value})", "intvec")
            if kind == "poly" and tree[1] == "-":
                owner = self.owned_arguments([value], "poly", consume)[0]
                return self.temp(f"snative::neg({owner})", "poly")
            if kind == "number" and tree[1] == "-":
                if ((consume is not None and consume[0] == "number" and value == consume[1]) or
                        value in self.owned):
                    owner = (f"std::move({consume[2]})" if consume is not None and
                             consume[0] == "number" and value == consume[1]
                             else f"std::move({self.owned[value]})")
                    return self.temp(f"snative::number_neg({owner}, ctx.r)", "number")
                return self.temp(f"snative::number_neg({value}, ctx.r)", "number")
            if kind != "int":
                raise CompileError(f"unsupported unary {tree[1]}({kind})")
            code = f"snative::neg({value})" if tree[1]=="-" else f"!snative::truth({value})"
            return self.temp(code)
        if op == "binary":
            # Evaluate operands separately: Singular logical operators are eager.
            operator = tree[1]
            if tree[2][0]=="sequence" or tree[3][0]=="sequence":
                raise CompileError("arithmetic on a comma expression requires interpreted exprlist semantics")
            # Removing the head from one named polynomial is a pointer update,
            # not a general subtraction. Restrict the rewrite to this exact,
            # side-effect-free shape; borrowed values are copied first.
            if (operator == "-" and tree[2][0] == "var" and
                    tree[3] == ["call", "lead", [tree[2]]]):
                lt, lhs = self.expr(tree[2])
                if lt == "poly":
                    owner = self.owned_arguments([lhs], "poly", consume)[0]
                    return self.temp(f"snative::drop_lead({owner})", "poly")
            typeof, literal = None, None
            if operator in ("==", "!=", "<>"):
                if tree[2][0:2] == ["call", "typeof"] and len(tree[2][2])==1 and tree[3][0]=="string":
                    typeof, literal = tree[2], tree[3][1]
                elif tree[2][0]=="string" and tree[3][0:2] == ["call", "typeof"] and len(tree[3][2])==1:
                    literal, typeof = tree[2][1], tree[3]
            if typeof is not None:
                kind, value = self.expr(typeof[2][0])
                comparison = f"snative::type_is({self.box(kind, value)}, {json.dumps(literal)})"
                return self.temp(("!" if operator!="==" else "")+comparison)
            integer_context = (expected=="int" and operator in
                               ("+", "-", "*", "%", "div", "mod", "^", "**"))
            # Expected type constrains this operation's result, not either
            # operand.  An overloaded child may legitimately return a custom
            # value consumed by the enclosing overloaded operation.
            lt, lhs = self.expr(tree[2])
            rt, rhs = self.expr(tree[3])
            if operator in ("^", "**") and rt=="value" and lt in ("poly", "number", "ideal"):
                if lt=="poly":
                    return self.temp(f"snative::poly_power_value({lhs}, {rhs}, ctx.r)", "poly")
                if lt=="number":
                    return self.temp(f"snative::number_power_value({lhs}, {rhs}, ctx.r)", "number")
                return self.temp(f"snative::ideal_power_value({lhs}, {rhs}, ctx.r)", "ideal")
            if operator == "..":
                if lt!=rt or lt!="int":
                    raise CompileError("range requires two int operands")
                return self.temp(f"snative::intvec_range({lhs}, {rhs})", "intvec")
            if operator == ":":
                if lt!=rt or lt!="int":
                    raise CompileError("repetition requires two int operands")
                return self.temp(f"snative::intvec_repeat({lhs}, {rhs})", "intvec")
            if lt==rt=="string" and operator == "+":
                return self.temp(f"snative::string_add({lhs}, {rhs})", "string")
            if lt==rt=="string" and operator in ("==", "!=", "<>", "<", "<=", ">", ">="):
                comparison = {"<":"'<'", ">":"'>'", "<=":"1", ">=":"2",
                              "==":"3", "!=":"4", "<>":"4"}[operator]
                return self.temp(f"snative::string_compare({lhs}, {rhs}, {comparison})")
            if operator=="+" and "list" in (lt, rt):
                lhs, rhs = self.list_value(lt,lhs), self.list_value(rt,rhs)
                owner, values = self.left_owner([lhs, rhs], "list", consume)
                return self.temp(f"snative::list_add({owner}, {values[1]}, ctx.r)", "list")
            if operator in ("==", "!=", "<>", "<", "<=", ">", ">="):
                comparison = {"<":"'<'", ">":"'>'", "<=":"LE", ">=":"GE",
                              "==":"EQUAL_EQUAL", "!=":"NOTEQUAL",
                              "<>":"NOTEQUAL"}[operator]
                if lt==rt=="value":
                    return self.temp(
                        f"snative::compare_values({lhs}, {rhs}, {comparison}, ctx.r)")
                if lt=="value" and rt=="int":
                    return self.temp(
                        f"snative::compare_value_int({lhs}, {rhs}, {comparison}, true, ctx.r)")
                if lt=="int" and rt=="value":
                    return self.temp(
                        f"snative::compare_value_int({rhs}, {lhs}, {comparison}, false, ctx.r)")
            if operator in ("+", "-", "*", "%", "mod", "div", "^", "**"):
                operation = {"mod":"%", "div":"INTDIV_CMD", "**":"^"}.get(operator, operator)
                operation = operation if operation=="INTDIV_CMD" else repr(operation)
                if lt=="value" and rt=="int":
                    kind, value = self.owned_value(
                        f"snative::binary_value_int({lhs}, {rhs}, {operation}, true, ctx.r)")
                    return (self.temp(f"snative::int_argument({value})")
                            if integer_context else (kind, value))
                if lt=="int" and rt=="value":
                    kind, value = self.owned_value(
                        f"snative::binary_value_int({rhs}, {lhs}, {operation}, false, ctx.r)")
                    return (self.temp(f"snative::int_argument({value})")
                            if integer_context else (kind, value))
            # The procedure's eventual return type says nothing about the
            # runtime types of two values selected from lists/newstructs.  In
            # particular, `return(a[i]==b[i]);` is not evidence that the
            # selected values are integers: Singular may select the intvec,
            # string, number, or polynomial equality kernel.  Reject that
            # unresolved dispatch instead of silently narrowing it to int.
            if "intvec" in (lt, rt):
                if lt==rt=="intvec":
                    if operator in ("+", "-"):
                        return self.temp(f"snative::intvec_binary({lhs}, {rhs}, {str(operator=='-').lower()})", "intvec")
                    if operator in ("==", "!=", "<>", "<", "<=", ">", ">="):
                        comparison = {"<":"'<'", ">":"'>'", "<=":"1", ">=":"2",
                                      "==":"3", "!=":"4", "<>":"4"}[operator]
                        return self.temp(f"snative::intvec_compare({lhs}, {rhs}, {comparison})")
                if lt=="intvec" and rt=="int" and operator in ("+", "-", "*", "/", "div", "%", "mod"):
                    operation = {"div":"/", "mod":"%"}.get(operator, operator)
                    return self.temp(f"snative::intvec_scalar({lhs}, {rhs}, '{operation}')", "intvec")
                # Singular has scalar-right intvec order/equality overloads,
                # but deliberately no intvec != int/<> overload.
                if lt=="intvec" and rt=="int" and operator in ("==", "<", "<=", ">", ">="):
                    comparison = {"<":"'<'", ">":"'>'", "<=":"1", ">=":"2",
                                  "==":"3"}[operator]
                    return self.temp(f"snative::intvec_compare({lhs}, {rhs}, {comparison})")
                if lt=="int" and rt=="intvec" and operator in ("+", "*"):
                    return self.temp(f"snative::intvec_scalar({rhs}, {lhs}, '{operator}')", "intvec")
                raise CompileError(f"unsupported intvec operator {operator}({lt}, {rt})")
            if operator in ("^", "**"):
                if lt==rt=="int":
                    return self.temp(f"snative::integer_power({lhs}, {rhs})")
                if lt=="ideal" and rt=="int":
                    return self.temp(f"snative::ideal_power({lhs}, {rhs}, ctx.r)", "ideal")
                if lt=="number" and rt=="int":
                    return self.temp(f"snative::number_power({lhs}, {rhs}, ctx.r)", "number")
                if lt!="poly" or rt!="int":
                    raise CompileError("power currently requires poly/number^int")
                owner = self.owned_arguments([lhs], "poly", consume)[0]
                return self.temp(f"snative::power({owner}, {rhs})", "poly")
            if "ideal" in (lt, rt):
                if operator=="+" and lt in ("ideal", "poly", "int") and rt in ("ideal", "poly", "int"):
                    lhs, rhs = self.ideal_value(lt,lhs), self.ideal_value(rt,rhs)
                    return self.temp(f"snative::ideal_add({lhs}, {rhs}, ctx.r)", "ideal")
                if operator=="*" and lt==rt=="ideal":
                    return self.temp(f"snative::ideal_mul({lhs}, {rhs}, ctx.r)", "ideal")
                if operator=="*" and lt=="ideal" and rt in ("poly", "number", "int"):
                    rhs = self.polynomial(rt,rhs)
                    return self.temp(f"snative::ideal_mul_poly({lhs}, {rhs}, ctx.r)", "ideal")
                if operator=="*" and rt=="ideal" and lt in ("poly", "number", "int"):
                    lhs = self.polynomial(lt,lhs)
                    return self.temp(f"snative::poly_mul_ideal({lhs}, {rhs}, ctx.r)", "ideal")
                raise CompileError(f"unsupported ideal operator {operator}({lt}, {rt})")
            if "poly" in (lt, rt):
                lhs, rhs = self.polynomial(lt, lhs), self.polynomial(rt, rhs)
                if operator in ("+", "-", "*"):
                    method = {"+":"add", "-":"sub", "*":"mul"}[operator]
                    lhs, rhs = self.owned_arguments([lhs, rhs], "poly", consume)
                    return self.temp(f"snative::{method}({lhs}, {rhs})", "poly")
                if operator in ("==", "!=", "<>"):
                    return self.temp(("!" if operator!="==" else "")+f"p_EqualPolys({lhs}, {rhs}, ctx.r)")
                if operator == "/":
                    return self.temp(f"snative::divide({lhs}, {rhs}, ctx.r)", "poly")
                raise CompileError(f"unsupported polynomial operator {operator}")
            if "number" in (lt, rt):
                lhs, rhs = self.number(lt, lhs), self.number(rt, rhs)
                if operator in ("+", "-", "*", "/"):
                    method = {"+":"add", "-":"sub", "*":"mul", "/":"div"}[operator]
                    if operator in ("+", "*"):
                        selected, owner, values = self.commutative_owner(
                            [lhs, rhs], "number", consume)
                        if selected is not None:
                            values[selected] = owner
                            return self.temp(
                                f"snative::number_{method}({values[0]}, {values[1]}, ctx.r)",
                                "number")
                    return self.temp(f"snative::number_{method}({lhs}, {rhs}, ctx.r)", "number")
                if operator in ("==", "!=", "<>"):
                    return self.temp(("!" if operator!="==" else "")+f"n_Equal({lhs}, {rhs}, ctx.r->cf)")
                if operator in ("<", "<=", ">", ">="):
                    greater = f"n_Greater({lhs if operator[0]=='>' else rhs}, {rhs if operator[0]=='>' else lhs}, ctx.r->cf)"
                    if len(operator)==2:
                        greater = f"({greater} || n_Equal({lhs}, {rhs}, ctx.r->cf))"
                    return self.temp(greater)
                raise CompileError(f"unsupported number operator {operator}")
            if lt!="int" or rt!="int":
                raise CompileError(f"unsupported operator {operator}({lt}, {rt})")
            if operator == "/":
                raise CompileError("int division with / requires source-line warning support; use a typed number")
            if operator in ("%", "mod"):
                code = f"snative::remainder({lhs}, {rhs})"
            elif operator == "div":
                code = f"snative::quotient({lhs}, {rhs})"
            elif operator in ("+", "-", "*"):
                method = {"+":"add", "-":"sub", "*":"mul"}[operator]
                code = f"snative::{method}({lhs}, {rhs})"
            elif operator in ("and", "&&", "or", "||"):
                operator = "&" if operator in ("and", "&&") else "|"
                code = f"(({lhs})!=0) {operator} (({rhs})!=0)"
            else:
                operator = "!=" if operator=="<>" else operator
                code = f"(int)({lhs}) {operator} (int)({rhs})"
            return self.temp(code)
        if op == "call":
            name = tree[1]
            if name in KERNELS and any(argument[0]=="sequence" for argument in tree[2]):
                raise CompileError("comma expression in a fixed-arity kernel requires interpreted exprlist semantics")
            callee = None if name in KERNELS else self.closure.nodes[self.node["calls"][name]]
            if (callee is not None and callee.get("contextual_sequence_result") and
                    expected != "intvec" and
                    not (expected == "recursive_sequence_component" and callee is self.node)):
                raise CompileError(
                    "contextual comma-return procedure requires an explicit intvec destination")
            expected_args = ([None]*len(tree[2]) if callee is None or
                             (len(callee["params"])==1 and callee["params"][0][0]=="varargs") else
                             ["int" if kind=="int" else None for kind, _ in callee["params"]])
            args = [self.expr(argument, expected=expected_args[i] if i<len(expected_args) else None)
                    for i, argument in enumerate(tree[2])]
            types, values = [t for t, _ in args], [v for _, v in args]
            # Exact overload mappings. No iiExprArith/iiPStart/interpreter call.
            if name == "typeof" and len(args)==1:
                return self.temp(f"snative::type_name({self.box(*args[0])})", "string")
            if name == "list":
                return self.list_literal(args)
            if name == "intvec":
                return self.intvec_literal(args)
            if name == "ideal":
                return self.ideal_literal(args)
            if name == "string":
                return self.string_literal(args)
            if name == "delete" and types == ["list", "int"]:
                return self.temp(f"snative::list_delete({values[0]}, {values[1]}, ctx.r)", "list")
            if name == "delete" and types == ["list", "value"]:
                position = self.temp(f"snative::int_argument({values[1]})")[1]
                return self.temp(f"snative::list_delete({values[0]}, {position}, ctx.r)", "list")
            if name == "delete" and types == ["intvec", "int"]:
                return self.temp(f"snative::intvec_delete({values[0]}, {values[1]})", "intvec")
            if name == "insert" and len(args) in (2, 3) and types[0]=="list" and (len(args)==2 or types[2]=="int"):
                inserted = self.box(*args[1])
                position = values[2] if len(args)==3 else "0L"
                return self.temp(f"snative::list_insert({values[0]}, {inserted}, {position}, ctx.r)", "list")
            if name == "poly" and len(args)==1 and types[0] in ("int", "poly", "number"):
                return "poly", self.polynomial(*args[0])
            if name == "number" and len(args)==1 and types[0] in ("int", "poly", "number", "value"):
                return "number", self.number(*args[0], explicit=True)
            if name == "int" and types == ["int"]:
                return args[0]
            if name == "int" and types == ["number"]:
                return self.temp(f"snative::number_to_int({values[0]}, ctx.r)")
            if name == "int" and types == ["string"]:
                return self.temp(f"snative::string_to_int({values[0]})")
            if name in ("numerator", "denominator") and len(args)==1 and types[0] in ("int", "number"):
                value = self.number(*args[0])
                return self.temp(f"snative::fraction_part({value}, {str(name=='denominator').lower()}, ctx.r)", "number")
            if name == "diff" and types == ["number", "number"]:
                # With int/poly operands the interpreter can select a different
                # overload. Only map the exact coefficient overload here.
                a, b = values
                return self.temp(f"snative::number_diff({a}, {b}, ctx.r)", "number")
            if name == "subst" and len(args)==3 and all(t in ("int", "number", "poly") for t in types):
                values = [self.polynomial(*arg) for arg in args]
                return self.temp("snative::substitute("+", ".join(values+["ctx.r"])+")", "poly")
            if name == "var" and types == ["int"]:
                return self.temp(f"snative::variable({values[0]}, ctx.r)", "poly")
            if name in ("lead", "leadmonom") and types == ["poly"]:
                return self.temp(f"snative::{name}({values[0]}, ctx.r)", "poly")
            if name == "leadcoef" and types == ["poly"]:
                return self.temp(f"snative::leadcoef({values[0]}, ctx.r)", "number")
            if name == "leadexp" and types == ["poly"]:
                return self.temp(f"snative::leadexp({values[0]}, ctx.r)", "intvec")
            if name == "jet" and types == ["poly", "int"]:
                owner = self.owned_arguments([values[0]], "poly", consume)[0]
                return self.temp(f"snative::jet({owner}, {values[1]})", "poly")
            if name == "nvars" and types == ["ring"]:
                return self.temp(f"{values[0]}->N")
            if name == "ncols" and types == ["ideal"]:
                return self.temp(f"snative::ideal_columns({values[0]})")
            if name == "nrows" and types == ["intvec"]:
                return self.temp(f"{values[0]}->rows()")
            if name == "deg" and types in (["poly"], ["poly", "intvec"]):
                return self.temp("snative::degree("+", ".join(values+["ctx.r"])+")")
            if name == "size" and types == ["int"]:
                return self.temp(f"snative::integer_size({values[0]})")
            if name == "size" and types == ["poly"]:
                return self.temp(f"pLength({values[0]})")
            if name == "size" and types == ["intvec"]:
                return self.temp(f"{values[0]}->length()")
            if name == "size" and types == ["list"]:
                return self.temp(f"lSize({values[0]})+1")
            if name == "size" and types == ["string"]:
                return self.temp(f"strlen({values[0]})")
            if name == "size" and types == ["ideal"]:
                return self.temp(f"snative::ideal_size({values[0]})")
            if name == "size" and types == ["value"]:
                return self.temp(f"snative::size_value({values[0]}, ctx.r)")
            if name in KERNELS:
                raise CompileError(f"unsupported kernel overload {name}({', '.join(types)})")
            if len(callee["params"])==1 and callee["params"][0][0]=="varargs":
                _, arguments = self.list_literal(args, variadic_arguments=True)
                values = [arguments]
            else:
                if len(args)!=len(callee["params"]):
                    raise CompileError(f"argument count does not match {name}")
                for i, ((kind, value), (expected, _)) in enumerate(zip(args, callee["params"])):
                    if expected=="number":
                        values[i] = self.number(kind, value)
                    elif expected=="list":
                        values[i] = self.list_value(kind, value)
                    elif expected=="intvec":
                        if kind=="value":
                            values[i] = self.converted_intvec_argument(value)
                        else:
                            values[i] = self.intvec_value(kind, value)
                    elif expected=="string":
                        values[i] = (self.converted_string_argument(value)
                                     if kind=="value" else self.string_value(kind,value))
                    elif expected=="ideal":
                        values[i] = self.ideal_value(kind,value)
                    elif expected=="poly" and kind=="value":
                        values[i] = self.converted_poly_argument(value)
                    elif expected=="int" and kind=="value":
                        values[i] = self.temp(f"snative::int_argument({value})")[1]
                    elif expected=="value":
                        values[i] = self.box(kind, value)
                    elif kind!=expected:
                        raise CompileError(f"argument types do not match {name}")
            return self.temp(callee["symbol"]+"("+", ".join(["ctx"]+values)+")", callee["result"])
        raise CompileError(f"unsupported expression {op}")

    def canonical_poly_scan(self, tree):
        """Recognize a side-effect-free one-based traversal of one polynomial."""
        if tree[0] != "for":
            return None
        initial, condition, step, body = tree[1], tree[2], tree[3], tree[4]
        index = None
        if (initial[0] == "declare" and initial[1] == "int" and
                len(initial[2]) == 1 and initial[2][0][1] == ["int", 1]):
            index = initial[2][0][0]
        elif (initial[0] == "assign" and initial[1][0] == "var" and
              initial[2] == ["int", 1] and self.types.get(initial[1][1]) == "int"):
            index = initial[1][1]
        if index is None:
            return None
        if (condition[0:2] != ["binary", "<="] or
                condition[2] != ["var", index] or condition[3][0:2] != ["call", "size"] or
                len(condition[3][2]) != 1 or condition[3][2][0][0] != "var"):
            return None
        source = condition[3][2][0][1]
        if self.types.get(source) != "poly":
            return None
        increment = step == ["increment", ["var", index], 1]
        assignment = step == ["assign", ["var", index],
                              ["binary", "+", ["var", index], ["int", 1]]]
        if not (increment or assignment):
            return None
        wanted = ["index", ["var", source], ["var", index]]
        if not any(node == wanted for node in trees(body)):
            return None
        for node in trees(body):
            if node[0] == "continue":
                return None
            if (node[0] in ("assign", "increment") and node[1][0] == "var" and
                    node[1][1] in (source, index)):
                return None
        return source, index

    def statement(self, tree, guarded=False):
        op = tree[0]
        if self.parameter_locals and op in ("if", "for", "while", "break", "continue", "return", "error"):
            raise CompileError("lowered execute declarations require straight-line use before kill")
        if op == "block":
            if guarded:
                self.out("{")
                self.indent += 1
            before_scope = self.lexical_state() if guarded else None
            before_parameters = set(self.parameter_locals)
            for child in tree[1]:
                self.statement(child, guarded)
            if self.parameter_locals != before_parameters:
                raise CompileError("lowered execute declarations must be killed in the same block")
            if before_scope is not None:
                self.restore_lexical_state(before_scope)
                self.indent -= 1
                self.out("}")
        elif op == "parameter_declare":
            name = tree[1]
            if name in self.declared:
                raise CompileError(f"duplicate local {name}")
            kind, value = self.expr(tree[2])
            if kind != "paramname":
                raise CompileError("execute number initializer must come from ringlist(basering)[1][2]")
            locals_ = ", ".join(json.dumps(n) for n in sorted(self.declared | {name}))
            self.out(f"snative::Number v_{name}=snative::parameter_number({value}, ctx.r, {{{locals_}}});")
            self.types[name] = "number"
            self.declared.add(name)
            self.poly_locals.add(name)
            self.parameter_locals.add(name)
        elif op == "kill":
            name = tree[1]
            if name not in self.parameter_locals:
                raise CompileError("kill currently requires a lowered execute declaration")
            self.out(f"v_{name} = snative::Number(nullptr, ctx.r->cf);")
            self.parameter_locals.remove(name)
            self.types.pop(name)
            # Keep the C++ declaration reserved within this lexical block.
        elif op == "declare":
            for name, value in tree[2]:
                declared_as_def = tree[1] == "def"
                kind = tree[1]
                typed_ideal = kind == "ideal"
                if name in self.declared or not identifier(name):
                    raise CompileError(f"duplicate/reserved local {name}")
                # Initializer is resolved before this identifier is introduced.
                initializer_start = len(self.lines)
                initial = None
                if kind == "def":
                    if value is None:
                        raise CompileError("def requires a statically typed initializer")
                    if value[0] == "sequence":
                        raise CompileError("def comma initializer requires interpreted exprlist assignment")
                    kind, initial = self.expr(value)
                    if kind not in ("int", "intvec", "poly", "number", "string", "ideal", "list"):
                        raise CompileError("def initializer has an unsupported static type")
                    if kind in OWNED:
                        initial = self.own(initial, kind)
                elif value is not None:
                    if kind=="list" and value == ["index", ["index", ["call", "ringlist", [["var", "basering"]]], ["int", 1]], ["int", 2]]:
                        kind, initial = self.expr(value)
                        initial = self.own(initial, kind)
                    elif kind in OWNED:
                        converted = self.convert(kind, value)
                        initial = self.own(converted, kind)
                        if typed_ideal:
                            initial = f"snative::ideal_assign({initial}, ctx.r)"
                    else:
                        initial = self.integer(value)
                if guarded:
                    initializer_lines = self.lines[initializer_start:]
                    del self.lines[initializer_start:]
                    if name not in self.guarded_declarations:
                        self.locals.append(f"int sn_declared_{name}=0;")
                        self.guarded_declarations.add(name)
                    token = "LIST_CMD" if kind=="paramnames" else kind.upper()+"_CMD"
                    self.out(f"snative::declare_local(sn_declared_{name}, {token}, "
                             f"{str(declared_as_def).lower()}, {json.dumps(name)});")
                    self.lines.extend(initializer_lines)
                self.types[name] = kind
                self.declared.add(name)
                if kind in OWNED:
                    self.poly_locals.add(name)
                    if initial is not None:
                        self.out(f"{OWNED[kind]} v_{name} = {initial};")
                    else:
                        default = ("nullptr, ctx.r" if kind=="poly" else "ctx.r" if kind in ("list", "paramnames")
                                   else "1" if kind=="intvec" else "omStrDup(\"\")" if kind=="string"
                                   else "idInit(1,1), ctx.r, Sy_bit(FLAG_STD)" if kind=="ideal"
                                   else "n_Init(0, ctx.r->cf), ctx.r->cf")
                        self.out(f"{OWNED[kind]} v_{name}("+default+");")
                else:
                    self.out(f"long v_{name} = 0;")
                if initial is not None and kind not in OWNED:
                    self.out(f"v_{name} = {initial};")
        elif op == "assign":
            # Native parser evaluates the indexing expression before the RHS.
            lhs = tree[1]
            if lhs[0] == "index":
                kind, target = self.expr(lhs[1])
                index = self.integer(lhs[2])
                index = self.narrow_index(index)
                target_name = lhs[1][1] if lhs[1][0]=="var" else None
                if kind=="list" and target_name is not None and (target_name in self.poly_locals or target_name in self.cow_params):
                    if tree[2][0] == "sequence":
                        raise CompileError("comma expression list-slot assignment requires interpreted exprlist semantics")
                    value_kind, value = self.expr(tree[2])
                    owner = f"v_{target_name}" if target_name in self.poly_locals else f"c_{target_name}.write()"
                    if value_kind in ("poly", "number", "string", "ideal", "list", "intvec") and value in self.owned:
                        owned = self.owned.pop(value)
                        self.out(f"snative::list_set_owned({owner}, {index}, std::move({owned}), ctx.r);")
                    else:
                        value = self.box(value_kind, value)
                        self.out(f"snative::list_set({owner}, {index}, {value}, ctx.r);")
                    return
                if kind=="string" and target_name is not None and (target_name in self.poly_locals or target_name in self.cow_params):
                    value_kind, value = self.expr(tree[2])
                    value = self.string_value(value_kind,value)
                    owner = f"v_{target_name}" if target_name in self.poly_locals else f"c_{target_name}.write()"
                    self.out(f"snative::string_set({owner}, {index}, {value});")
                    return
                if kind=="ideal" and target_name is not None and (target_name in self.poly_locals or target_name in self.cow_params):
                    value_kind, value = self.expr(tree[2])
                    polynomial = self.polynomial(value_kind,value)
                    owner = f"v_{target_name}" if target_name in self.poly_locals else f"c_{target_name}.write()"
                    self.out(f"snative::ideal_set({owner}, {index}, {self.own(polynomial, 'poly')});")
                    return
                if kind != "intvec":
                    raise CompileError("indexed assignment requires a typed list/intvec/string/ideal variable")
                value = self.integer(tree[2])
                if target_name in self.poly_locals:
                    self.out(f"snative::set(v_{target_name}, {index}, {value});")
                elif target_name in self.cow_params:
                    self.out(f"snative::set(c_{target_name}.write(), {index}, {value});")
                else:
                    raise CompileError("intvec element assignment requires a typed variable")
            elif lhs[0] == "var" and self.types.get(lhs[1]) == "int":
                value = self.integer(tree[2])
                self.out(f"v_{lhs[1]} = {value};")
            elif lhs[0] == "var" and lhs[1] in self.cow_params:
                kind = self.types[lhs[1]]
                owner = f"c_{lhs[1]}"
                if kind == "ideal" and tree[2] == ["var", lhs[1]]:
                    # iiAssign clears IDFLAG before its raw-data self-alias
                    # fast path.  No assignment conversion is then applied.
                    self.out(f"{owner}.write().clear_flags();")
                    return
                value = self.convert(kind, tree[2])
                if value != owner+".get()" or kind == "ideal":
                    assigned = self.own(value, kind)
                    if kind == "ideal":
                        assigned = f"snative::ideal_assign({assigned}, ctx.r)"
                    self.out(f"{owner}.assign({assigned});")
            elif lhs[0] == "var" and lhs[1] in self.poly_locals:
                kind = self.types.get(lhs[1])
                if kind not in OWNED:
                    raise CompileError(f"local {lhs[1]} is not available on this path")
                if kind=="paramnames":
                    raise CompileError("ring parameter-name lists must remain read-only")
                owner = f"v_{lhs[1]}"
                if kind == "ideal" and tree[2] == ["var", lhs[1]]:
                    self.out(f"{owner}.clear_flags();")
                    return
                value = self.convert(kind, tree[2], (kind, owner+".get()", owner))
                # Other native value types can elide pointer-identical
                # assignment. Ideal identity conversions cannot: their flag
                # assignment boundary remains observable.
                if value != owner+".get()" or kind == "ideal":
                    assigned = self.own(value, kind)
                    if kind == "ideal":
                        assigned = f"snative::ideal_assign({assigned}, ctx.r)"
                    self.out(f"{owner} = {assigned};")
            else:
                raise CompileError("assignment requires a typed local or parameter")
        elif op == "increment":
            lhs = tree[1]
            if lhs[0] != "var" or self.types.get(lhs[1]) != "int":
                raise CompileError("postfix increment requires an int variable")
            self.out(f"v_{lhs[1]} = snative::increment(v_{lhs[1]}, {tree[2]});")
        elif op == "return":
            if tree[1][0] == "sequence":
                # A comma expression is normally an interpreter exprlist.  It
                # is an intvec collection only when the procedure is already
                # known to return intvec *and* one component is itself an
                # intvec.  The latter keeps an all-int `return((1,2))` dynamic.
                values = [self.expr(child, expected="recursive_sequence_component")
                          for child in tree[1][1]]
                if (not self.node["private"] or self.node["result"]!="intvec" or
                        not any(item[0]=="intvec" for item in values)):
                    raise CompileError("comma expression return requires interpreted exprlist semantics")
                kind, value = self.intvec_literal(values)
            else:
                kind, value = self.expr(tree[1], expected="int" if self.node["result"]=="int" else None)
            if kind=="value" and self.node["result"] in ("int", "number", "string", "ideal", "list", "intvec"):
                expected = self.node["result"]
                if expected=="int":
                    kind, value = self.temp(f"snative::int_argument({value})")
                elif expected=="number":
                    kind, value = self.temp(f"snative::exact_number_argument({value}, ctx.r)", "number")
                elif expected=="list":
                    kind, value = "list", self.list_value(kind, value)
                elif expected=="string":
                    kind, value = "string", self.temp(f"snative::exact_string_argument({value})", "string")[1]
                elif expected=="ideal":
                    kind, value = "ideal", self.temp(f"snative::exact_ideal_argument({value}, ctx.r)", "ideal")[1]
                else:
                    kind, value = "intvec", self.borrowed_intvec(value)
            if kind != self.node["result"]:
                raise CompileError("return type does not match inferred procedure type")
            if kind in OWNED:
                if tree[1][0]=="var" and tree[1][1] in self.poly_locals:
                    value = "std::move(v_"+tree[1][1]+")"
                else:
                    value = self.own(value, kind)
            self.out("return " + value + ";")
        elif op == "if":
            value = self.integer(tree[1])
            self.out(f"if (snative::truth({value})) {{")
            self.indent += 1
            before = self.lexical_state()
            # Even a terminating branch can be reached after another branch
            # declared the same source name on an earlier loop iteration.
            # Keep the runtime declaration check on both sides; lexical state
            # is still restored below so neither branch's locals escape.
            self.statement(tree[2], True)
            self.restore_lexical_state((dict(before[0]), set(before[1]),
                                        set(before[2]), set(before[3])))
            self.indent -= 1
            self.out("} else {")
            self.indent += 1
            self.statement(tree[3], True)
            self.restore_lexical_state(before)
            self.indent -= 1
            self.out("}")
        elif op in ("for", "while"):
            scan = self.canonical_poly_scan(tree) if op == "for" else None
            if op == "for":
                self.statement(tree[1], guarded)
                condition, body = tree[2], tree[4]
            else:
                condition, body = tree[1], tree[2]
            if scan is not None:
                source, index = scan
                _, polynomial = self.expr(["var", source])
                self.serial += 1
                cursor = "t"+str(self.serial)
                self.out(f"poly {cursor} = {polynomial};")
                self.out("while (true) {")
                self.indent += 1
                self.loop += 1
                self.out("ctx.tick();")
                self.out(f"if ({cursor} == nullptr) break;")
                key = (source, index)
                previous = self.poly_scans.get(key)
                self.poly_scans[key] = cursor
                before_body = self.lexical_state()
                self.statement(body, True)
                self.restore_lexical_state(before_body)
                if previous is None:
                    self.poly_scans.pop(key)
                else:
                    self.poly_scans[key] = previous
                self.out("{")
                self.indent += 1
                before_step = self.lexical_state()
                self.statement(tree[3], True)
                self.restore_lexical_state(before_step)
                self.indent -= 1
                self.out("}")
                self.out(f"{cursor} = pNext({cursor});")
                self.loop -= 1
                self.indent -= 1
                self.out("}")
                return
            self.out("while (true) {")
            self.indent += 1
            self.loop += 1
            self.out("ctx.tick();")
            value = self.integer(condition)
            self.out(f"if (!snative::truth({value})) break;")
            before_body = self.lexical_state()
            self.statement(body, True)
            self.restore_lexical_state(before_body)
            if op == "for":
                # Singular continue skips the step, unlike a C++ for-loop.
                self.out("{")
                self.indent += 1
                before_step = self.lexical_state()
                self.statement(tree[3], True)
                self.restore_lexical_state(before_step)
                self.indent -= 1
                self.out("}")
            self.loop -= 1
            self.indent -= 1
            self.out("}")
        elif op in ("continue", "break"):
            if not self.loop:
                raise CompileError(f"{op} outside loop")
            self.out(op + ";")
        elif op == "error":
            self.out("throw std::runtime_error("+json.dumps(tree[1])+");")
        elif op != "empty":
            raise CompileError(f"unsupported statement {op}")

    def generate(self):
        if not terminates(self.node["ast"]):
            raise CompileError("every path must return an int/intvec/poly/number/string/ideal/list or raise ERROR")
        self.statement(self.node["ast"])
        return (declaration(self.node) + " {\n  snative::Call call(ctx);\n" +
                "".join("  "+line+"\n" for line in self.locals) +
                "\n".join(self.lines) + "\n}\n")


def run(command, capture_stderr=False, **kwargs):
    supplied_environment = kwargs.pop("env", None)
    environment = dict(os.environ if supplied_environment is None else supplied_environment)
    # Keep __DATE__/__TIME__ deterministic without sacrificing warm reuse.
    if not environment.get("SOURCE_DATE_EPOCH"):
        environment["SOURCE_DATE_EPOCH"] = "0"
    kwargs["env"] = environment
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, **kwargs)
    if result.returncode:
        raise CompileError(shlex.join(command)+"\n"+result.stderr.decode(errors="replace"))
    return (result.stderr if capture_stderr else result.stdout).decode()


def _stat_fields(value):
    return {name: getattr(value, name) for name in
            ("st_dev", "st_ino", "st_mode", "st_size", "st_mtime_ns", "st_ctime_ns")}


def _file_state(path):
    logical = Path(os.path.abspath(os.fspath(path)))
    link_stat = os.lstat(logical)
    is_link = stat.S_ISLNK(link_stat.st_mode)
    if not is_link and not stat.S_ISREG(link_stat.st_mode):
        raise CompileError(f"ABI input is not a regular file or symlink: {logical}")
    resolved = Path(os.path.realpath(logical))
    target_stat = os.stat(resolved)
    if not stat.S_ISREG(target_stat.st_mode):
        raise CompileError(f"ABI input does not resolve to a regular file: {logical}")
    return {"logical": str(logical), "kind": "symlink" if is_link else "regular",
            "link_target": os.readlink(logical) if is_link else None,
            "resolved": str(resolved), "lstat": _stat_fields(link_stat),
            "stat": _stat_fields(target_stat)}


def file_record(path):
    """Fingerprint a stable regular file while retaining symlink identity."""
    for _ in range(3):
        before = _file_state(path)
        checksum = file_digest(before["resolved"])
        after = _file_state(path)
        if before == after:
            before["sha256"] = checksum
            return before
    raise CompileError(f"ABI input changed while it was being fingerprinted: {path}")


def validate_file_record(record):
    try:
        expected = dict(record)
        expected.pop("sha256")
        return _file_state(record["logical"]) == expected
    except (CompileError, KeyError, OSError, TypeError):
        return False


def _directory_state(path):
    logical = Path(os.path.abspath(os.fspath(path)))
    try:
        link_stat = os.lstat(logical)
    except FileNotFoundError:
        return {"logical": str(logical), "kind": "missing"}
    is_link = stat.S_ISLNK(link_stat.st_mode)
    if not is_link and not stat.S_ISDIR(link_stat.st_mode):
        return {"logical": str(logical), "kind": "other", "lstat": _stat_fields(link_stat)}
    resolved = Path(os.path.realpath(logical))
    try:
        target_stat = os.stat(resolved)
    except FileNotFoundError:
        if not is_link:
            raise CompileError(f"directory changed while it was being fingerprinted: {logical}")
        return {"logical": str(logical), "kind": "broken-symlink",
                "link_target": os.readlink(logical), "resolved": str(resolved),
                "lstat": _stat_fields(link_stat)}
    if not stat.S_ISDIR(target_stat.st_mode):
        return {"logical": str(logical), "kind": "other", "lstat": _stat_fields(link_stat)}
    return {"logical": str(logical), "kind": "symlink" if is_link else "directory",
            "link_target": os.readlink(logical) if is_link else None,
            "resolved": str(resolved), "lstat": _stat_fields(link_stat),
            "stat": _stat_fields(target_stat)}


def _symlink_entry_states(directory, names):
    """Capture links whose targets can appear without changing the directory."""
    result = []
    for name in names:
        path = directory/name
        try:
            link_stat = os.lstat(path)
        except FileNotFoundError:
            result.append({"name": name, "kind": "missing"})
            continue
        if not stat.S_ISLNK(link_stat.st_mode):
            continue
        resolved = Path(os.path.realpath(path))
        entry = {"name": name, "kind": "symlink", "link_target": os.readlink(path),
                 "resolved": str(resolved), "lstat": _stat_fields(link_stat)}
        try:
            target_stat = os.stat(resolved)
        except FileNotFoundError:
            entry["target_kind"] = "missing"
        else:
            entry["target_kind"] = ("regular" if stat.S_ISREG(target_stat.st_mode) else
                                    "directory" if stat.S_ISDIR(target_stat.st_mode) else "other")
            entry["stat"] = _stat_fields(target_stat)
        result.append(entry)
    return result


def _entry_selection_states(directory, names):
    """Capture entry properties which can change search-path selection."""
    result = []
    for name in names:
        path = directory/name
        try:
            entry_stat = os.lstat(path)
        except FileNotFoundError:
            result.append({"name": name, "kind": "missing"})
            continue
        mode = entry_stat.st_mode
        kind = ("symlink" if stat.S_ISLNK(mode) else
                "regular" if stat.S_ISREG(mode) else
                "directory" if stat.S_ISDIR(mode) else "other")
        result.append({"name": name, "kind": kind,
                       "mode": stat.S_IMODE(mode),
                       "readable": os.access(path, os.R_OK),
                       "executable": os.access(path, os.X_OK)})
    return result


def directory_record(path):
    """Capture pkg-config search-directory identity and its .pc namespace."""
    result = _directory_state(path)
    if result["kind"] not in ("directory", "symlink"):
        return result
    logical = Path(result["logical"])
    try:
        pc_entries = sorted(entry.name for entry in logical.iterdir()
                            if entry.name.endswith(".pc"))
        pc_symlinks = _symlink_entry_states(logical, pc_entries)
        pc_files = [file_record(logical/name) for name in pc_entries
                    if (logical/name).is_file()]
    except OSError as error:
        raise CompileError(
            f"cannot enumerate pkg-config directory safely: {logical}: {error}") from error
    if _directory_state(logical) != result:
        raise CompileError(f"directory changed while it was being fingerprinted: {logical}")
    result["pc_entries"] = pc_entries
    result["pc_symlinks"] = pc_symlinks
    result["pc_files"] = pc_files
    return result


def validate_directory_record(record):
    try:
        expected = dict(record)
        has_namespace = all(name in expected for name in
                            ("pc_entries", "pc_symlinks", "pc_files"))
        entries = expected.pop("pc_entries", None)
        symlinks = expected.pop("pc_symlinks", None)
        files = expected.pop("pc_files", None)
        current = _directory_state(record["logical"])
        if current != expected:
            return False
        if current["kind"] not in ("directory", "symlink"):
            return True
        return (has_namespace and
                (entries is None or
                 (_symlink_entry_states(Path(record["logical"]), entries) == symlinks and
                  all(validate_file_record(entry) for entry in files))))
    except (CompileError, KeyError, OSError, TypeError):
        return False


def link_directory_record(path):
    result = _directory_state(path)
    if result["kind"] in ("directory", "symlink"):
        logical = Path(result["logical"])
        try:
            entries = sorted(
                entry.name for entry in logical.iterdir()
                if entry.name.startswith("lib") and
                (entry.name.endswith(".a") or ".so" in entry.name))
            result["link_entries"] = entries
            result["link_symlinks"] = _symlink_entry_states(logical, entries)
            result["link_entry_states"] = _entry_selection_states(logical, entries)
        except OSError as error:
            raise CompileError(
                f"cannot enumerate linker directory safely: {logical}: {error}") from error
        state = dict(result)
        state.pop("link_entries")
        state.pop("link_symlinks")
        state.pop("link_entry_states")
        if _directory_state(logical) != state:
            raise CompileError(f"directory changed while it was being fingerprinted: {logical}")
    return result


def validate_link_directory_record(record):
    try:
        expected = dict(record)
        has_namespace = all(name in expected for name in
                            ("link_entries", "link_symlinks", "link_entry_states"))
        entries = expected.pop("link_entries", None)
        symlinks = expected.pop("link_symlinks", None)
        states = expected.pop("link_entry_states", None)
        current = _directory_state(record["logical"])
        if current != expected:
            return False
        if current["kind"] not in ("directory", "symlink"):
            return True
        return (has_namespace and
                (entries is None or
                 (_symlink_entry_states(Path(record["logical"]), entries) == symlinks and
                  _entry_selection_states(Path(record["logical"]), entries) == states)))
    except (CompileError, KeyError, OSError, TypeError):
        return False


def namespace_directory_record(path):
    result = _directory_state(path)
    if result["kind"] in ("directory", "symlink"):
        logical = Path(result["logical"])
        try:
            entries = sorted(entry.name for entry in logical.iterdir())
            result["entries"] = entries
            result["entry_symlinks"] = _symlink_entry_states(logical, entries)
            result["entry_states"] = _entry_selection_states(logical, entries)
        except OSError:
            result["entries"] = None
            result["entry_symlinks"] = None
            result["entry_states"] = None
        state = dict(result)
        state.pop("entries")
        state.pop("entry_symlinks")
        state.pop("entry_states")
        if _directory_state(logical) != state:
            raise CompileError(f"directory changed while it was being fingerprinted: {logical}")
    return result


def validate_namespace_directory_record(record):
    try:
        expected = dict(record)
        has_namespace = all(name in expected for name in
                            ("entries", "entry_symlinks", "entry_states"))
        entries = expected.pop("entries", None)
        symlinks = expected.pop("entry_symlinks", None)
        states = expected.pop("entry_states", None)
        current = _directory_state(record["logical"])
        if current != expected:
            return False
        if current["kind"] not in ("directory", "symlink"):
            return True
        return (has_namespace and
                (entries is None or
                 (_symlink_entry_states(Path(record["logical"]), entries) == symlinks and
                  _entry_selection_states(Path(record["logical"]), entries) == states)))
    except (CompileError, KeyError, OSError, TypeError):
        return False


def candidate_file_record(path):
    """Record whether a path can become a selected tool/spec candidate."""
    logical = Path(os.path.abspath(os.fspath(path)))
    try:
        link_stat = os.lstat(logical)
    except FileNotFoundError:
        return {"logical": str(logical), "kind": "missing"}
    is_link = stat.S_ISLNK(link_stat.st_mode)
    if not is_link:
        return {"logical": str(logical),
                "kind": "regular" if stat.S_ISREG(link_stat.st_mode) else "other",
                "lstat": _stat_fields(link_stat),
                "readable": os.access(logical, os.R_OK),
                "executable": os.access(logical, os.X_OK)}
    resolved = Path(os.path.realpath(logical))
    result = {"logical": str(logical), "kind": "symlink",
              "link_target": os.readlink(logical), "resolved": str(resolved),
              "lstat": _stat_fields(link_stat)}
    try:
        target_stat = os.stat(resolved)
    except FileNotFoundError:
        result["target_kind"] = "missing"
        result["readable"] = False
        result["executable"] = False
    else:
        result["target_kind"] = ("regular" if stat.S_ISREG(target_stat.st_mode) else
                                 "directory" if stat.S_ISDIR(target_stat.st_mode) else "other")
        result["stat"] = _stat_fields(target_stat)
        result["readable"] = os.access(logical, os.R_OK)
        result["executable"] = os.access(logical, os.X_OK)
    return result


def validate_candidate_file_record(record):
    try:
        return candidate_file_record(record["logical"]) == record
    except (CompileError, KeyError, OSError, TypeError):
        return False


def _macro_expands_has_include(source):
    """Detect a local macro expansion which synthesizes __has_include."""
    macros = {}
    function_definition = re.compile(
        r"^[ \t]*#[ \t]*define[ \t]+([A-Za-z_]\w*)\(([^()]*)\)[ \t]*(.*)$")
    object_definition = re.compile(
        r"^[ \t]*#[ \t]*define[ \t]+([A-Za-z_]\w*)[ \t]+(.*)$")
    lines = source.splitlines()
    for line in lines:
        match = function_definition.match(line)
        if match:
            parameters = tuple(value.strip() for value in match.group(2).split(",")
                               if value.strip())
            if all(re.fullmatch(r"[A-Za-z_]\w*", value) for value in parameters):
                macros[match.group(1)] = (parameters, match.group(3))
            continue
        match = object_definition.match(line)
        if match:
            macros[match.group(1)] = (None, match.group(2))

    identifier_pattern = re.compile(r"[A-Za-z_]\w*")
    paste_pattern = re.compile(
        r"([A-Za-z_]\w*)[ \t]*##[ \t]*([A-Za-z_]\w*)")

    def arguments(text, opening):
        depth, start, result, index = 1, opening+1, [], opening+1
        while index < len(text):
            if text[index] == "(":
                depth += 1
            elif text[index] == ")":
                depth -= 1
                if depth == 0:
                    result.append(text[start:index])
                    return result, index+1
            elif text[index] == "," and depth == 1:
                result.append(text[start:index])
                start = index+1
            index += 1
        return None, opening+1

    def expand(text, depth=0, active=frozenset()):
        if depth > 32:
            return text, False
        output, index, paste_derived_builtin = [], 0, False
        while index < len(text):
            match = identifier_pattern.match(text, index)
            if match is None:
                output.append(text[index])
                index += 1
                continue
            name = match.group(0)
            macro = macros.get(name)
            if macro is None or name in active:
                output.append(name)
                index = match.end()
                continue
            parameters, body = macro
            end = match.end()
            if parameters is None:
                replacement = body
            else:
                opening = end
                while opening < len(text) and text[opening].isspace():
                    opening += 1
                if opening >= len(text) or text[opening] != "(":
                    output.append(name)
                    index = end
                    continue
                values, end = arguments(text, opening)
                if values is None or len(values) != len(parameters):
                    output.append(text[index:end])
                    index = end
                    continue
                mapping = dict(zip(parameters, values))
                replacement = identifier_pattern.sub(
                    lambda token: mapping.get(token.group(0), token.group(0)), body)
            pasted_values = []
            previous = None
            while previous != replacement:
                previous = replacement
                def paste(pair):
                    value = pair.group(1)+pair.group(2)
                    pasted_values.append(value)
                    return value
                replacement = paste_pattern.sub(paste, replacement)
            expanded, nested_paste = expand(replacement, depth+1, active | {name})
            paste_derived_builtin = paste_derived_builtin or nested_paste
            for value in pasted_values:
                expanded_value, nested_value_paste = expand(
                    value, depth+1, active | {name})
                if (nested_value_paste or
                        any(target in expanded_value for target in targets)):
                    paste_derived_builtin = True
            output.append(expanded)
            index = end
        return "".join(output), paste_derived_builtin

    targets = ("__has_include", "__has_include_next")
    for line in lines:
        if re.match(r"^[ \t]*#[ \t]*define\b", line):
            continue
        _, paste_derived_builtin = expand(line)
        if paste_derived_builtin:
            return True
    # A fixed paste in a macro body is unsafe even if its invocation is in a
    # different dependency header.  Generic concatenators are handled at each
    # locally visible invocation above and remain usable by system headers.
    for parameters, body in macros.values():
        if ("##" in body and
                (parameters is None or
                 not any(parameter in body for parameter in parameters))):
            previous = None
            while previous != body:
                previous = body
                body = paste_pattern.sub(lambda pair: pair.group(1)+pair.group(2), body)
            if any(target in body for target in targets):
                return True
    return False


def _literal_has_include_operands(path):
    """Return literal __has_include operands, rejecting untrackable uses.

    Dependency output does not list a missing header tested by __has_include.
    Only literal operands have a finite candidate set which a warm snapshot can
    validate without invoking the preprocessor again.
    """
    try:
        source = Path(path).read_text(errors="replace")
    except OSError as error:
        raise CompileError(f"cannot scan header for __has_include: {path}: {error}") from error

    # This compiler emits C++11, where phase 1 trigraph replacement can create
    # a backslash which phase 2 then uses to splice an operator token.
    trigraphs = {"??=": "#", "??/": "\\", "??'": "^", "??(": "[",
                "??)": "]", "??!": "|", "??<": "{", "??>": "}", "??-": "~"}
    for spelling, replacement in trigraphs.items():
        source = source.replace(spelling, replacement)
    # Translation phase 2 removes escaped newlines before tokenization; doing
    # the same prevents an operator or its call from being hidden by splicing.
    source = re.sub(r"\\\r?\n", "", source)
    result = []
    masked = list(source)
    size = len(source)

    def quoted_end(index, quote):
        index += 1
        while index < size:
            if source[index] == "\\":
                index += 2
            elif source[index] == quote:
                return index+1
            elif source[index] == "\n" and quote == '"':
                return index
            else:
                index += 1
        return index

    def raw_end(index):
        match = re.match(r'(?:u8|u|U|L)?R"([^ ()\\\t\r\n]{0,16})\(', source[index:])
        if not match:
            return None
        terminator = ")"+match.group(1)+'"'
        end = source.find(terminator, index+match.end())
        return size if end < 0 else end+len(terminator)

    def skip_space_and_comments(index):
        while index < size:
            if source[index].isspace():
                index += 1
            elif source.startswith("//", index):
                end = source.find("\n", index+2)
                index = size if end < 0 else end+1
            elif source.startswith("/*", index):
                end = source.find("*/", index+2)
                if end < 0:
                    raise CompileError(f"unterminated comment while scanning {path}")
                index = end+2
            else:
                break
        return index

    def mask(start, end):
        for offset in range(start, min(end, size)):
            if source[offset] not in "\r\n":
                masked[offset] = " "

    index = 0
    while index < size:
        if source.startswith("//", index):
            end = source.find("\n", index+2)
            next_index = size if end < 0 else end+1
            mask(index, next_index)
            index = next_index
            continue
        if source.startswith("/*", index):
            end = source.find("*/", index+2)
            if end < 0:
                raise CompileError(f"unterminated comment while scanning {path}")
            mask(index, end+2)
            index = end+2
            continue
        raw = raw_end(index)
        if raw is not None:
            mask(index, raw)
            index = raw
            continue
        if source[index] in "\"'":
            end = quoted_end(index, source[index])
            mask(index, end)
            index = end
            continue
        name = next((value for value in ("__has_include_next", "__has_include")
                     if source.startswith(value, index)), None)
        if (name is not None and
                (index == 0 or not (source[index-1].isalnum() or source[index-1] == "_")) and
                (index+len(name) == size or
                 not (source[index+len(name)].isalnum() or source[index+len(name)] == "_"))):
            line_start = source.rfind("\n", 0, index)+1
            if re.fullmatch(r"\s*#\s*define\s*", source[line_start:index]):
                # Compatibility fallbacks commonly define the builtin itself.
                # They do not perform a filesystem query at this location.
                index += len(name)
                continue
            operand = skip_space_and_comments(index+len(name))
            if operand >= size or source[operand] != "(":
                prefix = source[line_start:index]
                feature_guard = (re.fullmatch(r"\s*#\s*ifn?def\s*", prefix) is not None or
                                 re.search(r"\bdefined\s*(?:\(\s*)?\Z", prefix) is not None)
                if feature_guard:
                    index += len(name)
                    continue
                raise CompileError(
                    f"standalone {name} alias is unsupported in {path}")
            operand = skip_space_and_comments(operand+1)
            quoted = operand < size and source[operand] == '"'
            if quoted:
                end = quoted_end(operand, '"')
                if end <= operand+1 or end > size or source[end-1] != '"':
                    raise CompileError(f"malformed __has_include operand in {path}")
                value = source[operand+1:end-1]
                if "\\" in value:
                    raise CompileError(f"escaped __has_include operand is unsupported in {path}")
            elif operand < size and source[operand] == "<":
                end = source.find(">", operand+1)
                if end < 0 or "\n" in source[operand+1:end]:
                    raise CompileError(f"malformed __has_include operand in {path}")
                value = source[operand+1:end]
                end += 1
            else:
                raise CompileError(f"non-literal __has_include operand is unsupported in {path}")
            close = skip_space_and_comments(end)
            if close >= size or source[close] != ")":
                raise CompileError(f"malformed __has_include operand in {path}")
            if not value:
                raise CompileError(f"empty __has_include operand is unsupported in {path}")
            result.append((value, quoted))
            index = close+1
            continue
        index += 1
    if _macro_expands_has_include("".join(masked)):
        raise CompileError(
            f"token-pasted __has_include is unsupported in {path}")
    return result


def _implicit_pch_files(headers, working_directory, candidates):
    """Track GCC's implicit ``header.gch`` selection conservatively."""
    cwd = Path(working_directory)
    result = []
    for value in headers:
        header = Path(value)
        if not header.is_absolute():
            header = cwd/header
        for suffix in (".gch", ".pch"):
            candidate = Path(str(Path(os.path.abspath(header)))+suffix)
            candidates.add(candidate)
            if candidate.is_dir():
                raise CompileError(
                    f"directory-form implicit precompiled header is unsupported: {candidate}")
            if candidate.is_file():
                result.append(str(candidate))
    return list(dict.fromkeys(result))


def _include_shadow_directories(headers, include_directories, candidates=None):
    """Track directories and exact negative candidates affecting preprocessing."""
    roots = [Path(value).absolute() for value in include_directories]
    directories = set(roots)
    include_candidates = set()
    for value in headers:
        header = Path(value).absolute()
        directories.add(header.parent)
        resolved_header = Path(os.path.realpath(header))
        for root in roots:
            try:
                relative = resolved_header.relative_to(Path(os.path.realpath(root)))
            except ValueError:
                continue
            for search_root in roots:
                directories.add(search_root/relative.parent)
                # Directory enumeration may be unavailable while exact lookup
                # through an execute-only parent still succeeds.  Track every
                # ordinary selected-header spelling as an exact candidate too.
                include_candidates.add(search_root/relative)
        for spelling, quoted in _literal_has_include_operands(header):
            operand = Path(spelling)
            if operand.is_absolute():
                include_candidates.add(operand)
                continue
            if any(part == ".." for part in operand.parts):
                raise CompileError(f"parent-relative __has_include operand is unsupported in {header}")
            if quoted:
                include_candidates.add(header.parent/operand)
            include_candidates.update(root/operand for root in roots)
            # Parent namespaces catch path-component kind changes, while the
            # exact candidate catches a new file in an already-existing tree.
            directories.update(path.parent for path in include_candidates)
    if candidates is not None:
        candidates.update(include_candidates)
    records = [namespace_directory_record(value) for value in
               sorted(directories, key=lambda path: str(path))]
    for record in records:
        if record["kind"] in ("directory", "symlink") and record.get("entries") is None:
            raise CompileError(
                f"cannot enumerate include search directory safely: {record['logical']}")
    return records


def _candidate_selection_identity(records):
    """Stable selection state, excluding timestamps and candidate contents."""
    return [(entry.get("logical"), entry.get("kind"), entry.get("link_target"),
             entry.get("resolved"), entry.get("target_kind"),
             entry.get("readable"), entry.get("executable"))
            for entry in records]


def _namespace_selection_identity(records):
    return [(entry.get("logical"), entry.get("kind"), entry.get("resolved"),
             entry.get("link_target"), entry.get("entries"),
             entry.get("entry_symlinks"), entry.get("entry_states"))
            for entry in records]


def _records(paths):
    result = {}
    for path in paths:
        logical = str(Path(os.path.abspath(os.fspath(path))))
        if logical not in result:
            result[logical] = file_record(logical)
    return [result[path] for path in sorted(result)]


def _record_identity(records):
    # Stat data validates a snapshot but does not invalidate compiled output
    # merely because an identical file was reinstalled at a new inode.
    return [(entry["logical"], entry["resolved"], entry["link_target"], entry["sha256"])
            for entry in records]


def _which(command):
    found = shutil.which(command)
    if not found:
        raise CompileError(f"required build tool not found: {command}")
    return str(Path(os.path.abspath(found)))


def configuration_request(prefix, cxx, flags):
    prefix = Path(prefix).resolve()
    return {"format": ABI_SNAPSHOT_FORMAT, "translator": VERSION,
            "working_directory": os.getcwd(), "prefix": str(prefix),
            "compiler": _which(cxx), "pkg_config": _which("pkg-config"),
            "ldd": _which("ldd"), "flags": list(flags),
            "environment": {name: os.environ.get(name) for name in BUILD_ENVIRONMENT_NAMES}}


def _validate_loader_environment(environment):
    for name in ("LD_PRELOAD", "LD_AUDIT"):
        if environment.get(name):
            raise CompileError(
                f"{name} loader injection is unsupported for cached native builds")


def _linker_script_tokens(path, max_size=16*1024*1024):
    """Tokenize a textual GNU linker script; return None for binary inputs."""
    try:
        with open(path, "rb") as stream:
            data = stream.read(max_size+1)
    except OSError as error:
        raise CompileError(f"cannot inspect linker input {path}: {error}") from error
    binary_magic = (b"\x7fELF", b"!<arch>\n", b"BC\xc0\xde",
                    b"\xcf\xfa\xed\xfe", b"\xfe\xed\xfa\xcf",
                    b"\xca\xfe\xba\xbe")
    if data.startswith(b"!<thin>\n"):
        raise CompileError(f"GNU thin archive inputs are unsupported: {path}")
    if data.startswith(binary_magic) or b"\0" in data[:4096]:
        return None
    if len(data) > max_size:
        raise CompileError(f"textual linker input is too large to inspect safely: {path}")
    source = data.decode("utf-8", errors="surrogateescape")
    # Strip C comments before shlex; GNU scripts commonly begin with one.
    if re.search(r"/\*", source):
        if re.search(r"/\*(?:(?!\*/).)*\Z", source, re.S):
            raise CompileError(f"unterminated comment in linker script: {path}")
        source = re.sub(r"/\*.*?\*/", " ", source, flags=re.S)
    parenthesized_commands = r"INPUT|GROUP|LIB|AS_NEEDED|SEARCH_DIR|STARTUP"
    if not (re.search(r"\b(?:"+parenthesized_commands+r")\s*\(", source, re.I) or
            re.search(r"\bINCLUDE\s+[^\s(),;]+", source, re.I)):
        return None
    lexer = shlex.shlex(source, posix=True, punctuation_chars="(),;")
    lexer.whitespace_split = True
    lexer.commenters = "#"
    try:
        values = list(lexer)
    except ValueError as error:
        raise CompileError(f"cannot parse linker script {path}: {error}") from error
    return [part for value in values
            for part in (list(value) if value and set(value) <= set("(),;") else [value])]


def _link_inputs(tokens, compiler_directories, working_directory=None,
                 candidates=None, runtime_directories=None):
    """Conservatively close over files a linker invocation can select."""
    cwd = Path(working_directory or os.getcwd())
    directories, rpath_link_directories = [], []
    libraries, result, direct_basenames = [], [], []

    def absolute(value):
        path = Path(value)
        return path if path.is_absolute() else cwd/path

    sysroot = None
    index = 0
    while index < len(tokens):
        token = tokens[index]
        if token == "--sysroot" and index+1 < len(tokens):
            index += 1
            sysroot = absolute(tokens[index])
        elif token.startswith("--sysroot="):
            sysroot = absolute(token.split("=", 1)[1])
        index += 1

    def search_path(value):
        text = os.fspath(value)
        if text.startswith("="):
            if sysroot is None:
                raise CompileError("sysroot-relative linker path without --sysroot")
            return sysroot/text[1:].lstrip(os.sep)
        if text == "$SYSROOT" or text.startswith("$SYSROOT/"):
            if sysroot is None:
                raise CompileError("$SYSROOT linker path without --sysroot")
            return sysroot/text[len("$SYSROOT"):].lstrip(os.sep)
        return absolute(text)

    def add_runtime_path(value):
        for item in os.fspath(value).split(os.pathsep):
            if not item:
                item = os.curdir
            if "$" in item and not (item == "$SYSROOT" or item.startswith("$SYSROOT/")):
                raise CompileError(f"dynamic linker runtime path is unsupported: {item}")
            if runtime_directories is not None:
                runtime_directories.add(search_path(item))

    def add_input(value, shadowable=False):
        path = absolute(value)
        if candidates is not None:
            candidates.add(path)
        if path.is_file():
            result.append(str(path))
            if shadowable:
                direct_basenames.append(path.name)

    long_file_options = ("--version-script", "--dynamic-list",
                         "--retain-symbols-file", "--just-symbols")
    script_options = ("--script", "--default-script")
    address_options = ("-Ttext", "-Ttext-segment", "-Trodata-segment",
                       "-Tdata", "-Tbss")

    index = 0
    while index < len(tokens):
        token = tokens[index]
        if (token in ("--audit", "--depaudit", "--filter", "--auxiliary",
                      "-P", "-F", "-f") or
                any(token.startswith(name+"=") for name in
                    ("--audit", "--depaudit", "--filter", "--auxiliary"))):
            raise CompileError(
                f"loader-injecting linker option is unsupported: {token}")
        if token == "--sysroot" and index+1 < len(tokens):
            index += 1
        elif token.startswith("--sysroot="):
            pass
        if token in ("-L", "--library-path") and index+1 < len(tokens):
            index += 1
            directories.append(search_path(tokens[index]))
        elif token.startswith("-L") and len(token)>2:
            directories.append(search_path(token[2:]))
        elif token.startswith("--library-path="):
            directories.append(search_path(token.split("=", 1)[1]))
        elif token in ("-rpath-link", "--rpath-link") and index+1 < len(tokens):
            index += 1
            rpath_link_directories += [search_path(value) for value in
                                       tokens[index].split(os.pathsep) if value]
        elif token.startswith(("-rpath-link=", "--rpath-link=")):
            rpath_link_directories += [search_path(value) for value in
                                       token.split("=", 1)[1].split(os.pathsep) if value]
        elif token in ("-rpath", "--rpath") and index+1 < len(tokens):
            index += 1
            add_runtime_path(tokens[index])
        elif token.startswith(("-rpath=", "--rpath=")):
            add_runtime_path(token.split("=", 1)[1])
        elif token in ("-l", "--library") and index+1 < len(tokens):
            index += 1
            libraries.append(tokens[index])
        elif token.startswith("-l") and len(token)>2:
            libraries.append(token[2:])
        elif token.startswith("--library="):
            libraries.append(token.split("=", 1)[1])
        elif token in ("--mri-script", "-c"):
            raise CompileError("MRI linker scripts are unsupported")
        elif token.startswith("--mri-script="):
            raise CompileError("MRI linker scripts are unsupported")
        elif token in address_options and index+1 < len(tokens):
            index += 1
        elif any(token.startswith(name+"=") for name in address_options):
            pass
        elif token in script_options+("-T", "-dT") and index+1 < len(tokens):
            index += 1
            add_input(tokens[index])
        elif any(token.startswith(name+"=") for name in script_options):
            add_input(token.split("=", 1)[1])
        elif token.startswith("-dT") and len(token)>3:
            add_input(token[3:])
        elif token.startswith("-T") and len(token)>2:
            add_input(token[2:])
        elif token in long_file_options and index+1 < len(tokens):
            index += 1
            add_input(tokens[index])
        elif any(token.startswith(name+"=") for name in long_file_options):
            add_input(token.split("=", 1)[1])
        elif token == "-R" and index+1 < len(tokens):
            index += 1
            if absolute(tokens[index]).is_dir():
                add_runtime_path(tokens[index])
            else:
                add_input(tokens[index])
        elif token.startswith("-R") and len(token)>2:
            if absolute(token[2:]).is_dir():
                add_runtime_path(token[2:])
            else:
                add_input(token[2:])
        elif token.startswith(("-plugin=", "--plugin=", "-plugin-opt=")):
            add_input(token.split("=", 1)[1], shadowable=True)
        elif not token.startswith("-"):
            add_input(token, shadowable=True)
        index += 1
    directories += [search_path(value) for value in
                    os.environ.get("LIBRARY_PATH", "").split(os.pathsep) if value]
    directories += [search_path(value) for value in compiler_directories]
    directories += rpath_link_directories
    directories = list(dict.fromkeys(directories))
    if candidates is not None:
        # GCC can choose startup objects and other implicit inputs from the
        # same ordered prefixes.  Watch earlier basename shadows of every
        # concrete dry-run input, not only the currently selected absolute.
        candidates.update(directory/name for directory in directories
                          for name in direct_basenames)
    # Fingerprint every existing spelling, not only the usual shared-library
    # choice.  -static and linker state can select an archive, and an earlier
    # directory/sibling becoming usable is covered by directory validation.
    for name in libraries:
        spellings = ([name[1:]] if name.startswith(":") else
                     ["lib"+name+suffix for suffix in (".so", ".a")])
        for directory in directories:
            for spelling in spellings:
                path = directory/spelling
                if candidates is not None:
                    candidates.add(path)
                if path.is_file():
                    result.append(str(path))
    for directory in dict.fromkeys(rpath_link_directories):
        try:
            for entry in directory.iterdir():
                if (entry.name.startswith("lib") and
                        (entry.name.endswith(".a") or ".so" in entry.name) and
                        entry.is_file()):
                    result.append(str(entry))
        except OSError:
            pass

    # Shared libraries selected by -l are sometimes GNU linker scripts (for
    # example libc.so), so close over script dependencies for every input, not
    # only an explicitly named -T script.
    pending = list(dict.fromkeys(result))
    inspected = set()

    def parenthesized(values, start, script):
        if start >= len(values) or values[start] != "(":
            raise CompileError(f"malformed linker script command in {script}")
        depth = 1
        current = start+1
        body = []
        while current < len(values) and depth:
            value = values[current]
            if value == "(":
                depth += 1
            elif value == ")":
                depth -= 1
                if not depth:
                    return body, current+1
            body.append(value)
            current += 1
        raise CompileError(f"unterminated linker script command in {script}")

    def dependency_words(values):
        commands = {"INPUT", "GROUP", "LIB", "AS_NEEDED", "STARTUP"}
        return [value for value in values
                if value not in ("(", ")", ",", ";") and
                value.upper() not in commands]

    def dependency_candidates(name, script):
        if name.startswith("-l") and len(name)>2:
            library = name[2:]
            spellings = ([library[1:]] if library.startswith(":") else
                         ["lib"+library+suffix for suffix in (".so", ".a")])
            return [directory/spelling for directory in directories
                    for spelling in spellings]
        value = Path(name)
        if value.is_absolute() or name.startswith(("=", "$SYSROOT")):
            return [search_path(name)]
        search = [cwd, script.parent]+directories
        return list(dict.fromkeys(directory/value for directory in search))

    def add_dependency(name, script):
        if not name or name.startswith("-") and not name.startswith("-l"):
            raise CompileError(f"unsupported linker script input {name!r} in {script}")
        for path in dependency_candidates(name, script):
            if candidates is not None:
                candidates.add(path)
            if path.is_file():
                logical = str(path)
                if logical not in result:
                    result.append(logical)
                    pending.append(logical)

    while pending:
        script = Path(pending.pop(0))
        logical = str(script.absolute())
        if logical in inspected:
            continue
        inspected.add(logical)
        if len(inspected) > 10000:
            raise CompileError("linker script dependency closure is too large")
        values = _linker_script_tokens(script)
        if values is None:
            continue
        # SEARCH_DIR affects every dependency in the script, regardless of its
        # textual position in common GNU scripts, so collect it first.
        index = 0
        while index < len(values):
            if values[index].upper() == "SEARCH_DIR":
                body, index = parenthesized(values, index+1, script)
                for value in dependency_words(body):
                    directory = search_path(value)
                    if directory not in directories:
                        directories.append(directory)
                continue
            index += 1
        index = 0
        while index < len(values):
            command = values[index].upper()
            if command in ("INPUT", "GROUP", "LIB", "AS_NEEDED", "STARTUP"):
                body, index = parenthesized(values, index+1, script)
                for value in dependency_words(body):
                    add_dependency(value, script)
                continue
            if command == "INCLUDE":
                if index+1 >= len(values) or values[index+1] in ("(", ")", ",", ";"):
                    raise CompileError(f"malformed INCLUDE in linker script {script}")
                add_dependency(values[index+1], script)
                index += 2
                continue
            index += 1
    return list(dict.fromkeys(result)), directories


def _compiler_search_directories(compiler, options, category):
    environment = dict(os.environ)
    environment.update({"LC_ALL": "C", "LANG": "C"})
    output = run([compiler]+options+["-print-search-dirs"], env=environment)
    line = next((line for line in output.splitlines() if line.startswith(category+":")), "")
    if "=" not in line:
        raise CompileError(f"compiler did not report its {category} search directories")
    value = line.split("=", 1)[1]
    return [Path(item) for item in value.split(os.pathsep) if item]


def _compiler_install_directory(compiler, options):
    environment = dict(os.environ)
    environment.update({"LC_ALL": "C", "LANG": "C"})
    output = run([compiler]+options+["-print-search-dirs"], env=environment)
    line = next((line for line in output.splitlines() if line.startswith("install:")), "")
    if not line:
        # Clang reports programs/libraries but no GCC installation prefix.
        return None
    value = line.split(":", 1)[1].strip()
    if value.startswith("="):
        value = value[1:]
    if not value:
        raise CompileError("compiler reported an empty installation directory")
    return Path(value)


def _compiler_target_machine(compiler, options):
    value = run([compiler]+options+["-dumpmachine"]).strip()
    if not value or os.sep in value:
        raise CompileError("compiler did not report a valid target machine")
    return value


def _compiler_dump_version(compiler, options):
    value = run([compiler]+options+["-dumpversion"]).strip()
    if not value or os.sep in value:
        raise CompileError("compiler did not report a valid dump version")
    return value


def _compiler_dry_run(compiler, options, link=False):
    """Return a locale-stable GCC/Clang -### trace without executing tools."""
    command = [compiler]+list(options)+["-###"]
    if link:
        command += ["-shared", "-Wl,-z,defs"]
    else:
        command.append("-c")
    command += ["-x", "c++", "/dev/null", "-o", "/dev/null"]
    environment = dict(os.environ)
    environment.update({"LC_ALL": "C", "LANG": "C"})
    return run(command, capture_stderr=True, env=environment)


def _compiler_reported_spec_files(compiler, options, link=False,
                                  working_directory=None, trace=None):
    """Extract spec files the GCC driver reports loading.

    -### exercises driver/spec expansion without executing cc1/as/ld.  Candidate
    paths are tracked separately so this authoritative active-file list can be
    validated without rerunning the probe on a warm hit.
    """
    output = (_compiler_dry_run(compiler, options, link) if trace is None else trace)
    cwd = Path(working_directory or os.getcwd())
    result = []
    prefix = "Reading specs from "
    for line in output.splitlines():
        if not line.startswith(prefix):
            continue
        path = Path(line[len(prefix):])
        if not path.is_absolute():
            path = cwd/path
        result.append(str(Path(os.path.abspath(path))))
    return list(dict.fromkeys(result))


def _compiler_trace_commands(trace):
    """Parse shell-quoted command lines from a GCC/Clang -### trace."""
    result = []
    for line in trace.splitlines():
        # Driver diagnostics and configuration metadata start in column zero;
        # the commands which -### would execute are indented.
        if not line[:1].isspace():
            continue
        try:
            command = shlex.split(line, comments=False, posix=True)
        except ValueError as error:
            raise CompileError(f"cannot parse compiler -### command: {error}") from error
        if command:
            result.append(command)
    return result


def _compiler_link_trace_tokens(trace, expected_programs=()):
    """Return only linker/collect commands from a compiler dry-run trace."""
    expected = {Path(value).name for value in expected_programs}
    expected.update({"collect2", "ld", "ld.bfd", "ld.gold", "ld.lld",
                     "ld64.lld", "lld", "mold"})
    commands = [command for command in _compiler_trace_commands(trace)
                if Path(command[0]).name in expected]
    if not commands:
        raise CompileError("compiler -### trace did not report a link command")
    return [value for command in commands for value in command]


def _compiler_trace_input_files(trace, working_directory=None):
    """Find concrete file-valued inputs passed to compiler subprocesses."""
    cwd = Path(working_directory or os.getcwd())
    result = []

    def add(value):
        path = Path(value)
        if not path.is_absolute():
            path = cwd/path
        if path.is_file():
            result.append(str(path))

    split_file_options = {"-include", "-imacros", "-include-pch", "--config"}
    directory_profile_options = {"-fprofile-dir", "-fprofile-use"}
    configuration_prefix = "Configuration file: "
    for line in trace.splitlines():
        if line.startswith(configuration_prefix):
            add(line[len(configuration_prefix):].strip())
    for command in _compiler_trace_commands(trace):
        command, response_files = _expand_response_files(command, cwd)
        result += response_files
        index = 0
        while index < len(command):
            token = command[index]
            if token in split_file_options and index+1 < len(command):
                index += 1
                add(command[index])
            elif not token.startswith("-"):
                add(token)
            elif "=" in token:
                # GCC/Clang file-bearing options include -fplugin=,
                # -fauto-profile=, -fprofile-sample-use= and --config=.
                # Testing every RHS which names an existing regular file is
                # conservative and also covers plugin-defined file options.
                option, values = token.split("=", 1)[0], token.split("=")[1:]
                for value in values:
                    path = Path(value)
                    if not path.is_absolute():
                        path = cwd/path
                    if (option == "-fprofile-dir" or
                            option in directory_profile_options and not path.is_file()):
                        raise CompileError(
                            f"directory-valued profile input is unsupported: {token}")
                    add(value)
            index += 1
    return list(dict.fromkeys(result))


def _validate_profile_inputs(options, working_directory=None):
    """Reject profile consumers whose input set cannot be snapshotted."""
    cwd = Path(working_directory or os.getcwd())
    implicit = {"-fprofile-use", "-fbranch-probabilities", "-fauto-profile",
                "-fprofile-sample-use", "-fprofile-instr-use", "-fcs-profile-use"}
    result = []
    for option in options:
        if option in implicit or option == "-fprofile-dir" or option.startswith("-fprofile-dir="):
            raise CompileError(f"implicit or directory profile input is unsupported: {option}")
        name, separator, value = option.partition("=")
        if separator and name in implicit:
            path = Path(value)
            if not path.is_absolute():
                path = cwd/path
            if not path.is_file():
                raise CompileError(f"profile input is not a regular file: {option}")
            result.append(str(path.absolute()))
    return list(dict.fromkeys(result))


def _compiler_include_directories(compiler, options):
    environment = dict(os.environ)
    environment.update({"LC_ALL": "C", "LANG": "C"})
    output = run([compiler]+options+["-E", "-x", "c++", "/dev/null", "-v"],
                 capture_stderr=True, env=environment)
    active, result = False, []
    for line in output.splitlines():
        text = line.strip()
        if "search starts here:" in text:
            active = True
            continue
        if active and text == "End of search list.":
            break
        if active:
            suffix = " (framework directory)"
            if text.endswith(suffix):
                text = text[:-len(suffix)]
            if text:
                result.append(Path(text))
    if not result:
        raise CompileError("compiler did not report its include search directories")
    return result


def _path_directories(value):
    if value is None:
        value = os.defpath
    return [Path(part or os.curdir) for part in value.split(os.pathsep)]


def _compiler_program(compiler, name, options, path_value=None, candidates=None):
    value = run([compiler]+options+["-print-prog-name="+name]).strip()
    value = value or name
    candidate = Path(value)
    if not candidate.is_absolute() and candidates is not None:
        if candidate.parent == Path(os.curdir):
            candidates.update(directory/candidate for directory in _path_directories(path_value))
        else:
            candidates.add(candidate.absolute())
        candidates.add(Path(compiler).parent/candidate)
    if value == name:
        found = shutil.which(value, path=path_value)
        return found
    if not candidate.is_absolute():
        local = Path(compiler).parent/candidate
        if local.is_file():
            candidate = local
        elif not candidate.is_file() and candidate.parent == Path(os.curdir):
            found = shutil.which(value, path=path_value)
            if found:
                candidate = Path(found)
    return str(candidate) if candidate.is_file() else None


def _option_values(options, name):
    return _option_alias_values(options, (name,))


def _option_alias_values(options, names):
    """Collect values for equivalent driver options in original argv order."""
    result = []
    index = 0
    while index < len(options):
        option = options[index]
        for name in names:
            if option == name and index+1 < len(options):
                index += 1
                result.append(options[index])
                break
            if option.startswith(name+"="):
                result.append(option.split("=", 1)[1])
                break
        index += 1
    return result


def _expand_response_files(options, working_directory, max_depth=32):
    """Return GCC's effective argv and the stable recursive @file closure."""
    root = Path(working_directory).absolute()
    files, active = [], set()
    token_count = 0

    def expand(values, depth):
        nonlocal token_count
        if depth > max_depth:
            raise CompileError(f"compiler response nesting exceeds {max_depth}")
        result = []
        for value in values:
            # libiberty's expandargv has no @@ escape: @@name names a
            # response file whose filename starts with '@'.
            if not value.startswith("@") or value == "@":
                result.append(value)
                token_count += 1
                continue
            path = Path(value[1:])
            path = path if path.is_absolute() else root/path
            path = Path(os.path.abspath(path))
            try:
                state = _file_state(path)
            except (CompileError, OSError) as error:
                raise CompileError(f"cannot read compiler response file {path}: {error}") from error
            key = (state["stat"]["st_dev"], state["stat"]["st_ino"])
            if key in active:
                raise CompileError(f"recursive compiler response file: {path}")
            if state["stat"]["st_size"] > 64*1024*1024:
                raise CompileError(f"compiler response file is too large: {path}")
            try:
                text = Path(state["resolved"]).read_text(errors="surrogateescape")
            except OSError as error:
                raise CompileError(f"cannot read compiler response file {path}: {error}") from error
            try:
                changed = _file_state(path) != state
            except (CompileError, OSError) as error:
                raise CompileError(
                    f"compiler response file changed while being read: {path}") from error
            if "\0" in text or changed:
                raise CompileError(f"compiler response file changed while being read: {path}")
            try:
                nested = shlex.split(text, comments=False, posix=True)
            except ValueError as error:
                raise CompileError(f"cannot parse compiler response file {path}: {error}") from error
            files.append(str(path))
            active.add(key)
            result += expand(nested, depth+1)
            active.remove(key)
            if token_count > 1000000:
                raise CompileError("compiler response expansion is too large")
        return result

    expanded = expand(list(options), 0)
    return expanded, list(dict.fromkeys(files))


def _compiler_spec_files(compiler, options, candidates=None, effective_options=None,
                         search_directories=None, install_directory=_UNSET,
                         target_machine=None, dump_version=None,
                         working_directory=None, reported_files=()):
    """Resolve GCC specs through the driver's effective startfile search.

    GCC does not resolve spec includes beside the including file.  Default
    specs have no cwd fallback; user -specs, %include, and %:include fall back
    to their bare name only after every startfile prefix misses.
    """
    parsed_options = options if effective_options is None else effective_options
    search = ([Path(value) for value in search_directories]
              if search_directories is not None else
              _compiler_search_directories(compiler, options, "libraries"))
    if install_directory is _UNSET:
        install = _compiler_install_directory(compiler, options)
    else:
        install = (None if install_directory is None else Path(install_directory))
    target = target_machine or _compiler_target_machine(compiler, options)
    version = dump_version or _compiler_dump_version(compiler, options)
    cwd = Path(working_directory or os.getcwd())
    result, seen, scanned, pending = [], set(), set(), []

    def usable(path):
        try:
            return path.is_file() and os.access(path, os.R_OK)
        except OSError:
            return False

    def resolve(name, allow_cwd):
        value = Path(name)
        paths = ([value] if value.is_absolute() else
                 [directory/value for directory in search])
        if not value.is_absolute() and allow_cwd:
            paths.append(cwd/value)
        unique = set()
        for value in paths:
            value = Path(os.path.abspath(value))
            if value in unique:
                continue
            unique.add(value)
            if candidates is not None:
                candidates.add(value)
            if usable(value):
                return value
        return None

    def add(path, scan):
        if path is None:
            return
        logical = str(Path(os.path.abspath(path)))
        if logical not in seen:
            seen.add(logical)
            result.append(logical)
        if not scan or logical in scanned:
            return
        scanned.add(logical)
        try:
            text = Path(logical).read_text(errors="replace")
        except OSError:
            return
        external_function = re.compile(
            r"%:(?:if-exists(?:-else)?|find-file|find-plugindir|getenv)\s*\(")
        if external_function.search(text):
            raise CompileError("filesystem-dependent GCC spec functions are unsupported")
        for kind, name in re.findall(
                r"(?m)^\s*%(include_noerr|include)[ \t]+<([^>\n]+)>", text):
            pending.append((name, kind != "include_noerr"))
        # Track constant %:include(name) uses conservatively even when they
        # occur in a conditional spec which may not be expanded by this run.
        include_function = re.compile(r"%:include\(\s*([^()%{}\s'\"]+)\s*\)")
        for name in include_function.findall(text):
            pending.append((name, True))
        if "%:include(" in include_function.sub("", text):
            raise CompileError("dynamic or quoted GCC %:include is unsupported")

    # The primary default lookup searches startfile prefixes only.  GCC then
    # independently reads exec_prefix/spec_machine_suffix/specs.  The normal
    # installation spelling ends in target/version; GCC_EXEC_PREFIX omits that
    # suffix.  Record exact ordinary and accelerator candidates so a missing or
    # broken override becoming readable invalidates a warm snapshot.
    add(resolve("specs", False), True)
    second_defaults = []
    if install is not None:
        install = Path(os.path.abspath(install))
        second_defaults += [install/"specs", install/target/"specs"]
        exec_prefix = os.environ.get("GCC_EXEC_PREFIX")
        if exec_prefix:
            second_defaults.append(Path(exec_prefix)/target/"specs")
        elif install.name == version:
            second_defaults.append(install.parent.parent/target/"specs")
        else:
            second_defaults += [ancestor/target/"specs" for ancestor in install.parents]
    reported_paths = {
        str(Path(os.path.abspath((cwd/Path(value)) if not Path(value).is_absolute()
                                 else Path(value))))
        for value in reported_files
    }
    for path in dict.fromkeys(Path(os.path.abspath(value)) for value in second_defaults):
        if candidates is not None:
            candidates.add(path)
        if usable(path) and str(path) in reported_paths:
            add(path, True)

    # Every successful load_specs call is reported under -v, including a
    # target-specific second default override and branch-exercised %:include.
    for path in reported_files:
        path = Path(path)
        if not path.is_absolute():
            path = cwd/path
        path = Path(os.path.abspath(path))
        if candidates is not None:
            candidates.add(path)
        if usable(path):
            add(path, True)

    pending += [(name, True) for name in
                _option_alias_values(parsed_options, ("-specs", "--specs"))]
    raw_environment = os.environ.get("GCC_SPECS", "")
    if raw_environment:
        environment_path = Path(raw_environment)
        if not environment_path.is_absolute():
            environment_path = cwd/environment_path
        if usable(environment_path):
            pending.append((raw_environment, True))
        else:
            pending += [(part, True) for part in raw_environment.split(os.pathsep) if part]
    while pending:
        name, allow_cwd = pending.pop(0)
        add(resolve(name, allow_cwd), True)
    return result


def _linker_program_names(options):
    selection = next(reversed(_option_values(options, "-fuse-ld")), None)
    if not selection or selection == "default":
        return ["collect2", "ld"]
    if os.sep in selection:
        return ["collect2", selection]
    return ["collect2", "ld."+selection]


def _pkg_config_files(tool, env):
    pending, seen, directories = ["Singular"], set(), []
    while pending:
        package = pending.pop(0)
        if package in seen:
            continue
        seen.add(package)
        # `--path` is a pkgconf extension.  The pcfiledir variable is provided
        # by both pkgconf and the reference freedesktop.org pkg-config.
        directory = run([tool, "--variable=pcfiledir", package], env=env).strip()
        if directory:
            directories.append(str(Path(directory).absolute()))
        requires = run([tool, "--print-requires", "--print-requires-private", package],
                       env=env)
        for line in requires.splitlines():
            dependency = line.split()[0] if line.split() else ""
            if dependency and dependency not in seen:
                pending.append(dependency)
    directories = list(dict.fromkeys(directories))
    # A query may select package-uninstalled.pc, or a differently named file
    # through Provides:.  Fingerprint every usable .pc file in each directory
    # actually reported by pkg-config rather than guessing a module filename.
    files = []
    for directory in directories:
        try:
            files += [str(entry.absolute()) for entry in Path(directory).iterdir()
                      if entry.name.endswith(".pc") and entry.is_file()]
        except OSError as error:
            raise CompileError(
                f"cannot enumerate selected pkg-config directory: {directory}: {error}") from error
    return sorted(set(files)), directories


def _ldd_inputs(output, working_directory=None):
    cwd = Path(working_directory or os.getcwd())
    result = []
    for line in output.splitlines():
        if re.search(r"=>\s*not found(?:\s|$)", line):
            raise CompileError(f"unresolved dynamic dependency: {line.strip()}")
        match = re.search(r"=>\s*(.+?)\s+\(0x[0-9a-fA-F]+\)\s*$", line)
        if match is None:
            match = re.match(r"\s*((?:/|\./|\.\./).+?)\s+"
                             r"\(0x[0-9a-fA-F]+\)\s*$", line)
        if match:
            path = Path(match.group(1))
            if not path.is_absolute():
                path = cwd/path
            path = Path(os.path.abspath(path))
            if not path.is_file():
                raise CompileError(f"dynamic dependency disappeared: {path}")
            result.append(str(path))
    return result


def _dynamic_dependencies(ldd, paths, working_directory=None):
    """Return the ldd-reported transitive DSO closure for build tools."""
    result = []
    for path in dict.fromkeys(path for path in paths if path):
        environment = dict(os.environ)
        environment.update({"LC_ALL": "C", "LANG": "C"})
        result += _ldd_inputs(run([ldd, str(path)], env=environment), working_directory)
    return list(dict.fromkeys(result))


def _loader_search_candidates(ldd, paths, working_directory=None):
    """Capture successful and unsuccessful loader search spellings."""
    environment = dict(os.environ)
    environment.pop("LD_DEBUG_OUTPUT", None)
    environment.update({"LC_ALL": "C", "LANG": "C", "LD_DEBUG": "libs"})
    result = []
    for path in dict.fromkeys(path for path in paths if path):
        output = run([ldd, str(path)], capture_stderr=True, env=environment)
        attempts = []
        for line in output.splitlines():
            match = re.search(r"\btrying file=(.*?)\s*$", line)
            if match:
                candidate = Path(match.group(1))
                if not candidate.is_absolute():
                    candidate = Path(working_directory or os.getcwd())/candidate
                attempts.append(str(Path(os.path.abspath(candidate))))
        if not attempts:
            selected = _ldd_inputs(output, working_directory)
            if not selected:
                selected = _ldd_inputs(run([ldd, str(path)], env={
                    name: value for name, value in environment.items()
                    if name != "LD_DEBUG"}), working_directory)
            if selected:
                raise CompileError(
                    f"dynamic loader did not report its search candidates for {path}")
        result += attempts
    return list(dict.fromkeys(result))


def _elf_dynamic_inputs(paths):
    """Return ELF ET_EXEC/ET_DYN files for which ldd has dependencies."""
    result = []
    for value in dict.fromkeys(path for path in paths if path):
        try:
            with open(value, "rb") as stream:
                header = stream.read(18)
        except OSError:
            continue
        if len(header) < 18 or header[:4] != b"\x7fELF":
            continue
        byteorder = "little" if header[5] == 1 else "big" if header[5] == 2 else None
        if byteorder is not None and int.from_bytes(header[16:18], byteorder) in (2, 3):
            result.append(value)
    return result


def seal_abi_snapshot(snapshot):
    result = dict(snapshot)
    result.pop("payload_digest", None)
    result["payload_digest"] = digest(result)
    return result


def valid_snapshot_payload(snapshot):
    try:
        expected = snapshot["payload_digest"]
        value = dict(snapshot)
        value.pop("payload_digest")
        return expected == digest(value)
    except (KeyError, TypeError):
        return False


def probe_build_configuration(request):
    if sys.platform != "linux":
        raise CompileError("native module building is initially supported on Linux only")
    prefix = Path(request["prefix"])
    compiler = request["compiler"]
    env = dict(os.environ)
    env["PKG_CONFIG_PATH"] = (str(prefix/"lib/pkgconfig")+os.pathsep+
                              env.get("PKG_CONFIG_PATH", ""))
    cflags = shlex.split(run([request["pkg_config"], "--cflags", "Singular"], env=env))
    library_flags = shlex.split(run([request["pkg_config"], "--libs", "Singular"], env=env))
    default_pc_path = run([request["pkg_config"], "--variable=pc_path", "pkg-config"],
                          env=env).strip()
    configured_pc_path = env.get("PKG_CONFIG_LIBDIR") or default_pc_path
    pc_directories = ([prefix/"lib/pkgconfig"]+
                      [Path(value) for value in env.get("PKG_CONFIG_PATH", "").split(os.pathsep) if value]+
                      [Path(value) for value in configured_pc_path.split(os.pathsep) if value])
    pc_files, selected_pc_directories = _pkg_config_files(request["pkg_config"], env)
    pc_directories += [Path(value) for value in selected_pc_directories]
    directory_records = [directory_record(value) for value in
                         dict.fromkeys(str(Path(value).absolute()) for value in pc_directories)]
    link_options = ["-O2"] + request["flags"]
    compile_options = (["-std=c++11", "-fPIC", "-fvisibility=hidden"]+
                       link_options+cflags+["-I", str(HERE)])
    working_directory = request.get("working_directory", os.getcwd())
    effective_compile_options, compile_response_files = _expand_response_files(
        compile_options, working_directory)
    profile_files = _validate_profile_inputs(effective_compile_options, working_directory)
    include_directories = _compiler_include_directories(compiler, compile_options)
    deps = run([compiler]+compile_options+["-M", "-x", "c++", str(HERE/"native_runtime.h")])
    if ":" not in deps:
        raise CompileError("compiler dependency output has no target separator")
    headers = shlex.split(deps.replace("\\\n", " ").split(":", 1)[1])
    include_candidates = set()
    include_directory_records = _include_shadow_directories(
        headers, include_directories, include_candidates)
    pch_files = _implicit_pch_files(headers, working_directory, include_candidates)
    include_candidate_records = [candidate_file_record(value) for value in
                                 sorted(include_candidates, key=lambda path: str(path))]
    singular = prefix/"bin/Singular"
    link_driver_options = link_options+library_flags
    effective_link_driver_options, link_response_files = _expand_response_files(
        link_driver_options, working_directory)
    compile_trace = _compiler_dry_run(compiler, effective_compile_options)
    link_trace = _compiler_dry_run(compiler, effective_link_driver_options, link=True)
    compile_trace_inputs = _compiler_trace_input_files(
        compile_trace, working_directory)
    compile_program_names = ("cc1plus", "as")
    linker_program_names = tuple(_linker_program_names(effective_link_driver_options))
    # Plugin-capable GCC configurations may load these while linking archive
    # LTO objects even when this invocation has no explicit -flto option.
    # Overtracking the helpers is safe; omitting them can reuse a stale module.
    lto_program_names = ("lto-wrapper", "lto1", "liblto_plugin.so")
    program_names = compile_program_names+linker_program_names+lto_program_names
    compile_program_directories = _compiler_search_directories(
        compiler, compile_options, "programs")
    link_program_directories = _compiler_search_directories(
        compiler, link_driver_options, "programs")
    program_directories = compile_program_directories+link_program_directories
    program_directory_records = [namespace_directory_record(value) for value in
                                 dict.fromkeys(str(Path(value).absolute())
                                               for value in program_directories)]
    program_candidates = set()
    for directory in program_directories:
        for name in program_names:
            candidate = Path(name)
            program_candidates.add(candidate if candidate.is_absolute() else directory/candidate)
    request_environment = request.get("environment", {})
    _validate_loader_environment(request_environment)
    path_value = (request_environment["PATH"] if "PATH" in request_environment
                  else os.environ.get("PATH"))
    if path_value is None:
        path_value = os.defpath
    compile_startfile_directories = _compiler_search_directories(
        compiler, compile_options, "libraries")
    link_startfile_directories = _compiler_search_directories(
        compiler, link_driver_options, "libraries")
    link_trace_tokens, linker_response_files = _expand_response_files(
        _compiler_link_trace_tokens(link_trace, linker_program_names),
        working_directory)
    link_candidates = set()
    module_runtime_directories = set()
    resolved_link_inputs, link_directories = _link_inputs(
        effective_link_driver_options+link_trace_tokens,
        link_startfile_directories, working_directory, link_candidates,
        module_runtime_directories)
    raw_ld_run_path = (request_environment.get("LD_RUN_PATH")
                       if "LD_RUN_PATH" in request_environment
                       else os.environ.get("LD_RUN_PATH"))
    for value in ([] if raw_ld_run_path is None else raw_ld_run_path.split(os.pathsep)):
        if not value:
            continue
        if "$" in value:
            raise CompileError(f"dynamic LD_RUN_PATH is unsupported: {value}")
        path = Path(value)
        module_runtime_directories.add(
            path if path.is_absolute() else Path(working_directory)/path)
    raw_ld_library_path = (request_environment.get("LD_LIBRARY_PATH")
                           if "LD_LIBRARY_PATH" in request_environment
                           else os.environ.get("LD_LIBRARY_PATH"))
    loader_environment_directories = ([] if raw_ld_library_path is None else
                                      [Path(value or os.curdir) for value in
                                       raw_ld_library_path.split(os.pathsep)])
    module_runtime_directories.update(
        path if path.is_absolute() else Path(working_directory)/path
        for path in loader_environment_directories)
    # The generated module always receives this absolute rpath at link time.
    module_runtime_directories.add(prefix/"lib")
    module_runtime_directory_records = [namespace_directory_record(value) for value in
                                        sorted(module_runtime_directories,
                                               key=lambda path: str(path))]
    for record in module_runtime_directory_records:
        if record["kind"] in ("directory", "symlink") and record.get("entries") is None:
            raise CompileError(
                f"cannot enumerate module runtime directory safely: {record['logical']}")
    link_directory_records = [link_directory_record(value) for value in
                              dict.fromkeys(str(Path(value).absolute()) for value in link_directories)]
    ldd_environment = dict(os.environ)
    ldd_environment.update({"LC_ALL": "C", "LANG": "C"})
    linked_runtime_files = _ldd_inputs(
        run([request["ldd"], str(singular)], env=ldd_environment), working_directory)
    loader_configuration_paths = list(LOADER_CONFIGURATION_PATHS)
    loader_configuration_files = [str(path) for path in loader_configuration_paths
                                  if path.is_file()]
    runtime_files = ([str(singular)]+resolved_link_inputs+linked_runtime_files+
                     loader_configuration_files)
    runtime_directories = (loader_environment_directories+
                           [prefix/"lib"]+link_directories+
                           [Path(value).parent for value in linked_runtime_files]+
                           [Path(value) for value in ("/lib", "/lib64", "/usr/lib", "/usr/lib64")])
    compile_programs = [_compiler_program(compiler, name, compile_options, path_value,
                                          program_candidates)
                        for name in compile_program_names]
    linker_programs = [_compiler_program(compiler, name, link_driver_options, path_value,
                                         program_candidates)
                       for name in linker_program_names]
    lto_files = [_compiler_program(compiler, name, link_driver_options, path_value,
                                   program_candidates)
                 for name in lto_program_names]
    # Compiler/pkg-config wrappers are commonly executable scripts.  Track the
    # wrapper files themselves below, but ask ldd only about actual ELF
    # drivers; ldd rejects scripts with "not a dynamic executable".
    driver_targets = _elf_dynamic_inputs([compiler, request["pkg_config"]])
    compile_dynamic_inputs = _elf_dynamic_inputs(compile_trace_inputs)
    link_dynamic_inputs = _elf_dynamic_inputs(resolved_link_inputs)
    driver_dependencies = _dynamic_dependencies(
        request["ldd"], driver_targets, working_directory)
    compile_tool_dependencies = _dynamic_dependencies(
        request["ldd"], compile_programs+compile_dynamic_inputs, working_directory)
    link_tool_dependencies = _dynamic_dependencies(
        request["ldd"], linker_programs+lto_files+link_dynamic_inputs, working_directory)
    driver_loader_candidates = _loader_search_candidates(
        request["ldd"], driver_targets, working_directory)
    compile_loader_candidates = _loader_search_candidates(
        request["ldd"], compile_programs+compile_dynamic_inputs, working_directory)
    link_loader_candidates = _loader_search_candidates(
        request["ldd"], linker_programs+lto_files+link_dynamic_inputs,
        working_directory)
    runtime_loader_candidates = _loader_search_candidates(
        request["ldd"], [singular], working_directory)
    loader_configuration_candidates = [str(path) for path in loader_configuration_paths]
    driver_loader_candidates += loader_configuration_candidates
    runtime_loader_candidates += loader_configuration_candidates
    runtime_directories += [Path(value).parent for value in
                            driver_dependencies+compile_tool_dependencies+
                            link_tool_dependencies]
    runtime_directory_records = [link_directory_record(value) for value in
                                 dict.fromkeys(str(Path(value).absolute())
                                               for value in runtime_directories)]
    spec_candidates = set()
    target_machine = _compiler_target_machine(compiler, effective_compile_options)
    dump_version = _compiler_dump_version(compiler, effective_compile_options)
    compile_install = _compiler_install_directory(compiler, compile_options)
    link_install = _compiler_install_directory(compiler, link_driver_options)
    compile_reported_specs = _compiler_reported_spec_files(
        compiler, effective_compile_options, working_directory=working_directory,
        trace=compile_trace)
    link_reported_specs = _compiler_reported_spec_files(
        compiler, effective_link_driver_options, link=True,
        working_directory=working_directory, trace=link_trace)
    compile_specs = _compiler_spec_files(compiler, compile_options, spec_candidates,
                                         effective_compile_options,
                                         compile_startfile_directories,
                                         compile_install, target_machine, dump_version,
                                         working_directory, compile_reported_specs)
    link_specs = _compiler_spec_files(compiler, link_driver_options, spec_candidates,
                                      effective_link_driver_options,
                                      link_startfile_directories,
                                      link_install, target_machine, dump_version,
                                      working_directory, link_reported_specs)
    program_candidate_records = [candidate_file_record(value) for value in
                                 sorted(program_candidates, key=lambda path: str(path))]
    spec_candidate_records = [candidate_file_record(value) for value in
                              sorted(spec_candidates, key=lambda path: str(path))]
    link_candidate_records = [candidate_file_record(value) for value in
                              sorted(link_candidates, key=lambda path: str(path))]
    compile_loader_candidate_records = [candidate_file_record(value) for value in
                                        sorted(set(driver_loader_candidates+
                                                   compile_loader_candidates))]
    link_loader_candidate_records = [candidate_file_record(value) for value in
                                     sorted(set(driver_loader_candidates+
                                                link_loader_candidates))]
    runtime_loader_candidate_records = [candidate_file_record(value) for value in
                                        sorted(set(runtime_loader_candidates))]
    link_files = ([compiler]+[path for path in linker_programs+lto_files if path]+link_specs+
                  driver_dependencies+link_tool_dependencies+link_response_files+
                  linker_response_files+resolved_link_inputs+loader_configuration_files)
    compile_files = ([compiler]+[path for path in compile_programs if path]+compile_specs+
                     driver_dependencies+compile_tool_dependencies+compile_response_files+
                     compile_trace_inputs+profile_files+pch_files+headers+
                     loader_configuration_files)
    validation_files = (compile_files+link_files+runtime_files+pc_files+
                        loader_configuration_files+
                        [request["pkg_config"], request["ldd"]])
    all_records = _records(validation_files)
    records_by_path = {entry["logical"]: entry for entry in all_records}
    def select(paths):
        keys = sorted({str(Path(os.path.abspath(os.fspath(path)))) for path in paths})
        return [records_by_path[key] for key in keys]
    compile_records, link_records, runtime_records = (
        select(compile_files), select(link_files), select(runtime_files))
    compiler_version = run([compiler, "--version"])
    loader_environment = [(name, request.get("environment", {}).get(name))
                          for name in LOADER_ENVIRONMENT_NAMES]
    source_date_epoch = request.get("environment", {}).get("SOURCE_DATE_EPOCH") or "0"
    identities = {
        "compile": digest([VERSION, "compile", compile_options, compiler_version,
                           _record_identity(compile_records),
                           _namespace_selection_identity(include_directory_records),
                           _candidate_selection_identity(include_candidate_records),
                           _candidate_selection_identity(compile_loader_candidate_records),
                           loader_environment, source_date_epoch]),
        "link": digest([VERSION, "link", link_options, library_flags, compiler_version,
                        _record_identity(link_records),
                        _candidate_selection_identity(link_loader_candidate_records),
                        loader_environment, source_date_epoch]),
        "runtime": digest([VERSION, "runtime", _record_identity(runtime_records),
                           _candidate_selection_identity(runtime_loader_candidate_records),
                           _namespace_selection_identity(module_runtime_directory_records),
                           loader_environment]),
    }
    return seal_abi_snapshot(
        {"format": ABI_SNAPSHOT_FORMAT, "request": request, "files": all_records,
         "directories": directory_records,
         "include_directories": include_directory_records,
         "include_candidates": include_candidate_records,
         "program_directories": program_directory_records,
         "program_candidates": program_candidate_records,
         "spec_candidates": spec_candidate_records,
         "link_candidates": link_candidate_records,
         "loader_candidates": {"compile": compile_loader_candidate_records,
                               "link": link_loader_candidate_records,
                               "runtime": runtime_loader_candidate_records},
         "module_runtime_directories": module_runtime_directory_records,
         "link_directories": link_directory_records,
         "runtime_directories": runtime_directory_records,
         "file_sets": {"compile": [r["logical"] for r in compile_records],
                       "link": [r["logical"] for r in link_records],
                       "runtime": [r["logical"] for r in runtime_records]},
         "compiler": compiler, "compile_options": compile_options,
         "link_options": link_options,
         "libraries": library_flags+["-Wl,-rpath,"+str(prefix/"lib")],
         "compiler_version": compiler_version, "identities": identities})


def validate_abi_snapshot(snapshot, request):
    try:
        return (valid_snapshot_payload(snapshot) and
                snapshot["format"] == ABI_SNAPSHOT_FORMAT and snapshot["request"] == request and
                set(snapshot["identities"]) == {"compile", "link", "runtime"} and
                all(validate_file_record(entry) for entry in snapshot["files"]) and
                all(validate_directory_record(entry) for entry in snapshot["directories"]) and
                all(validate_namespace_directory_record(entry)
                    for entry in snapshot["include_directories"]) and
                all(validate_candidate_file_record(entry)
                    for entry in snapshot["include_candidates"]) and
                all(validate_namespace_directory_record(entry)
                    for entry in snapshot["program_directories"]) and
                all(validate_candidate_file_record(entry)
                    for entry in snapshot["program_candidates"]) and
                all(validate_candidate_file_record(entry)
                    for entry in snapshot["spec_candidates"]) and
                all(validate_candidate_file_record(entry)
                    for entry in snapshot["link_candidates"]) and
                set(snapshot["loader_candidates"]) == {"compile", "link", "runtime"} and
                all(validate_candidate_file_record(entry)
                    for phase in ("compile", "link", "runtime")
                    for entry in snapshot["loader_candidates"][phase]) and
                all(validate_namespace_directory_record(entry)
                    for entry in snapshot["module_runtime_directories"]) and
                all(validate_link_directory_record(entry) for entry in snapshot["link_directories"]) and
                all(validate_link_directory_record(entry) for entry in snapshot["runtime_directories"]))
    except (KeyError, TypeError):
        return False


def refresh_abi_snapshot(snapshot, request):
    """Refresh changed stat data without probing when paths and bytes agree."""
    try:
        if (not valid_snapshot_payload(snapshot) or
                snapshot["format"] != ABI_SNAPSHOT_FORMAT or snapshot["request"] != request or
                set(snapshot["identities"]) != {"compile", "link", "runtime"}):
            return None
        refreshed_files = [entry if validate_file_record(entry) else file_record(entry["logical"])
                           for entry in snapshot["files"]]
        refreshed_directories = [entry if validate_directory_record(entry)
                                 else directory_record(entry["logical"])
                                 for entry in snapshot["directories"]]
        refreshed_include_directories = [
            entry if validate_namespace_directory_record(entry)
            else namespace_directory_record(entry["logical"])
            for entry in snapshot["include_directories"]]
        refreshed_include_candidates = [
            entry if validate_candidate_file_record(entry)
            else candidate_file_record(entry["logical"])
            for entry in snapshot["include_candidates"]]
        refreshed_program_directories = [
            entry if validate_namespace_directory_record(entry)
            else namespace_directory_record(entry["logical"])
            for entry in snapshot["program_directories"]]
        refreshed_program_candidates = [
            entry if validate_candidate_file_record(entry)
            else candidate_file_record(entry["logical"])
            for entry in snapshot["program_candidates"]]
        refreshed_spec_candidates = [
            entry if validate_candidate_file_record(entry)
            else candidate_file_record(entry["logical"])
            for entry in snapshot["spec_candidates"]]
        refreshed_link_candidates = [
            entry if validate_candidate_file_record(entry)
            else candidate_file_record(entry["logical"])
            for entry in snapshot["link_candidates"]]
        refreshed_loader_candidates = {
            phase: [entry if validate_candidate_file_record(entry)
                    else candidate_file_record(entry["logical"])
                    for entry in snapshot["loader_candidates"][phase]]
            for phase in ("compile", "link", "runtime")}
        refreshed_module_runtime_directories = [
            entry if validate_namespace_directory_record(entry)
            else namespace_directory_record(entry["logical"])
            for entry in snapshot["module_runtime_directories"]]
        refreshed_link_directories = [
            entry if validate_link_directory_record(entry)
            else link_directory_record(entry["logical"])
            for entry in snapshot["link_directories"]]
        refreshed_runtime_directories = [
            entry if validate_link_directory_record(entry)
            else link_directory_record(entry["logical"])
            for entry in snapshot["runtime_directories"]]
        def directory_identity(entry):
            files = entry.get("pc_files")
            return (entry.get("logical"), entry.get("kind"), entry.get("resolved"),
                    entry.get("link_target"), entry.get("pc_entries"),
                    entry.get("pc_symlinks"),
                    None if files is None else _record_identity(files))
        if (_record_identity(refreshed_files) != _record_identity(snapshot["files"]) or
                [directory_identity(entry) for entry in refreshed_directories] !=
                [directory_identity(entry) for entry in snapshot["directories"]]):
            return None
        link_directory_identity = lambda entry: (entry.get("logical"), entry.get("kind"),
                                                  entry.get("resolved"), entry.get("link_target"),
                                                  entry.get("link_entries"), entry.get("link_symlinks"),
                                                  entry.get("link_entry_states"))
        if ([link_directory_identity(entry) for entry in refreshed_link_directories] !=
                [link_directory_identity(entry) for entry in snapshot["link_directories"]]):
            return None
        if ([link_directory_identity(entry) for entry in refreshed_runtime_directories] !=
                [link_directory_identity(entry) for entry in snapshot["runtime_directories"]]):
            return None
        if (_namespace_selection_identity(refreshed_include_directories) !=
                _namespace_selection_identity(snapshot["include_directories"])):
            return None
        if (_candidate_selection_identity(refreshed_include_candidates) !=
                _candidate_selection_identity(snapshot["include_candidates"])):
            return None
        if (_namespace_selection_identity(refreshed_program_directories) !=
                _namespace_selection_identity(snapshot["program_directories"])):
            return None
        if (refreshed_program_candidates != snapshot["program_candidates"] or
                refreshed_spec_candidates != snapshot["spec_candidates"] or
                refreshed_link_candidates != snapshot["link_candidates"]):
            return None
        if any(refreshed_loader_candidates[phase] !=
               snapshot["loader_candidates"][phase]
               for phase in ("compile", "link", "runtime")):
            return None
        if (_namespace_selection_identity(refreshed_module_runtime_directories) !=
                _namespace_selection_identity(snapshot["module_runtime_directories"])):
            return None
        result = dict(snapshot)
        result["files"] = refreshed_files
        result["directories"] = refreshed_directories
        result["include_directories"] = refreshed_include_directories
        result["include_candidates"] = refreshed_include_candidates
        result["program_directories"] = refreshed_program_directories
        result["program_candidates"] = refreshed_program_candidates
        result["spec_candidates"] = refreshed_spec_candidates
        result["link_candidates"] = refreshed_link_candidates
        result["loader_candidates"] = refreshed_loader_candidates
        result["module_runtime_directories"] = refreshed_module_runtime_directories
        result["link_directories"] = refreshed_link_directories
        result["runtime_directories"] = refreshed_runtime_directories
        return seal_abi_snapshot(result)
    except (CompileError, KeyError, OSError, TypeError):
        return None


def load_build_configuration(cache, prefix, cxx, flags, stats):
    request = configuration_request(prefix, cxx, flags)
    key = digest(request)
    path = cache/"abi"/(key+".json")
    with key_lock(cache, "abi", key):
        try:
            snapshot = json.loads(path.read_text())
        except (OSError, ValueError, UnicodeError):
            snapshot = None
        if snapshot is not None and validate_abi_snapshot(snapshot, request):
            stats["abi_snapshots_reused"] += 1
            return snapshot
        if snapshot is not None:
            refreshed = refresh_abi_snapshot(snapshot, request)
            if refreshed is not None:
                save_json(path, refreshed)
                stats["abi_snapshots_reused"] += 1
                return refreshed
        snapshot = probe_build_configuration(request)
        save_json(path, snapshot)
        stats["abi_snapshots_created"] += 1
        return snapshot


def _artifact_receipt(path):
    return path.with_suffix(path.suffix+".receipt.json")


def valid_artifact(path, kind="object"):
    receipt = _artifact_receipt(path)
    try:
        value = os.lstat(path)
        receipt_stat = os.lstat(receipt)
        if not stat.S_ISREG(value.st_mode) or stat.S_ISLNK(value.st_mode):
            return False
        if not stat.S_ISREG(receipt_stat.st_mode) or stat.S_ISLNK(receipt_stat.st_mode):
            return False
        data = json.loads(receipt.read_text())
        if (not isinstance(data, dict) or not isinstance(data.get("artifact"), dict) or
                data.get("format") != ARTIFACT_RECEIPT_FORMAT or
                data.get("kind") != kind or
                not isinstance(data.get("dynamic_dependencies"), list) or
                not isinstance(data.get("loader_candidates"), list) or
                data["artifact"].get("logical") != str(Path(path).absolute())):
            return False
        changed = False
        refreshed = data["artifact"]
        if not validate_file_record(refreshed):
            refreshed = file_record(path)
            if _record_identity([refreshed]) != _record_identity([data["artifact"]]):
                return False
            changed = True
        dependencies = []
        for entry in data["dynamic_dependencies"]:
            dependency = entry
            if not validate_file_record(dependency):
                dependency = file_record(entry["logical"])
                if _record_identity([dependency]) != _record_identity([entry]):
                    return False
                changed = True
            dependencies.append(dependency)
        dependency_paths = {entry["logical"] for entry in dependencies}
        loader_candidates = []
        for entry in data["loader_candidates"]:
            candidate = entry
            if not validate_candidate_file_record(candidate):
                candidate = candidate_file_record(entry["logical"])
                # A selected candidate is also content-fingerprinted as a
                # dependency.  Permit metadata-only refreshes for that exact
                # file, while any unselected candidate change forces relink.
                if (entry["logical"] not in dependency_paths or
                        _candidate_selection_identity([candidate]) !=
                        _candidate_selection_identity([entry])):
                    return False
                changed = True
            loader_candidates.append(candidate)
        if changed:
            save_json(receipt, {"format": ARTIFACT_RECEIPT_FORMAT, "kind": kind,
                                "artifact": refreshed,
                                "dynamic_dependencies": dependencies,
                                "loader_candidates": loader_candidates})
        return True
    except (AttributeError, CompileError, KeyError, OSError, TypeError, ValueError, UnicodeError):
        return False


def publish_artifact(path, kind="object", dynamic_dependencies=(), loader_candidates=()):
    save_json(_artifact_receipt(path),
              {"format": ARTIFACT_RECEIPT_FORMAT, "kind": kind,
               "artifact": file_record(path),
               "dynamic_dependencies": _records(dynamic_dependencies),
               "loader_candidates": [candidate_file_record(value) for value in
                                     sorted(set(loader_candidates))]})


@contextmanager
def unique_output(target):
    target.parent.mkdir(parents=True, exist_ok=True)
    descriptor, name = tempfile.mkstemp(dir=target.parent, prefix="."+target.name+".",
                                        suffix=".tmp")
    os.close(descriptor)
    temporary = Path(name)
    try:
        yield temporary
    finally:
        try:
            temporary.unlink()
        except FileNotFoundError:
            pass


def compile_object(cache, key, source, target, code, compiler, options):
    with key_lock(cache, "object", key):
        try:
            source_current = source.read_text()==code
        except (OSError, UnicodeError):
            source_current = False
        if not source_current:
            atomic(source, code)
        if valid_artifact(target):
            return "reused"
        with unique_output(target) as temporary:
            run([compiler]+options+["-c", str(source), "-o", str(temporary)])
            os.replace(temporary, target)
        publish_artifact(target)
        return "compiled"


def _compile_task(task):
    return compile_object(**task)


def link_module(cache, key, target, compiler, options, objects, libraries,
                ldd, working_directory):
    with key_lock(cache, "module", key):
        if valid_artifact(target, "module"):
            return "reused"
        with unique_output(target) as temporary:
            run([compiler]+options+["-shared", "-Wl,-z,defs", "-o", str(temporary)]+
                [str(path) for path in objects]+libraries)
            dynamic_dependencies = _dynamic_dependencies(
                ldd, [temporary], working_directory)
            loader_candidates = _loader_search_candidates(
                ldd, [temporary], working_directory)
            os.replace(temporary, target)
        publish_artifact(target, "module", dynamic_dependencies, loader_candidates)
        return "linked"


def module_source(roots, export_prefix, aliases):
    lines = ['#include "native_runtime.h"'] + [declaration(n)+";" for n in roots]
    lines += ["static unsigned long compiled_entries=0;",
              "static BOOLEAN compiled_calls(leftv result, leftv args) {",
              '  if (args!=NULL) { WerrorS("compilerCalls expects no arguments"); return TRUE; }',
              "  result->rtyp=INT_CMD; result->data=(void*)compiled_entries; return FALSE;", "}"]
    for i, node in enumerate(roots):
        lines += [f"static BOOLEAN entry_{i}(leftv result, leftv args) {{", "  try {"]
        if node["needs_ring"]:
            lines += ["    if (currRing==NULL) throw std::runtime_error(\"native entry requires a basering\");"]
        lines += ["    leftv arg=args;"]
        values = []
        for j, (kind, _) in enumerate(node["params"]):
            if kind == "varargs":
                lines += [f"    snative::List a{j}_owner=snative::arguments_list(arg,currRing);",
                          f"    lists a{j}=a{j}_owner.get();", "    arg=nullptr;"]
                values.append(f"a{j}")
                continue
            if kind == "number":
                lines += [f"    snative::NumberArgument a{j}(arg,currRing);",
                          "    if (errorreported) return TRUE;", "    arg=arg->next;"]
                values.append(f"a{j}.get()")
                continue
            if kind == "list":
                helper = "plain_list_argument" if node["result"]=="list" else "list_argument"
                lines += [f"    lists a{j}=snative::{helper}(arg);", "    arg=arg->next;"]
                values.append(f"a{j}")
                continue
            if kind == "value":
                lines += [f"    sleftv a{j}_materialized;",
                          f"    char a{j}_string_value[2];",
                          f"    leftv a{j}=snative::scalar_index_argument(arg,a{j}_materialized,a{j}_string_value);",
                          f"    a{j}=snative::value_argument(a{j});", "    arg=arg->next;"]
                values.append(f"a{j}")
                continue
            if kind == "intvec":
                lines += [f"    snative::IntvecArgument a{j}(arg);", "    arg=arg->next;"]
                values.append(f"a{j}.get()")
                continue
            if kind == "string":
                lines += [f"    snative::StringArgument a{j}(arg);", "    arg=arg->next;"]
                values.append(f"a{j}.get()")
                continue
            if kind == "ideal":
                lines += [f"    snative::IdealArgument a{j}(arg,currRing);", "    arg=arg->next;"]
                values.append(f"a{j}.get()")
                continue
            if kind == "poly":
                lines += [f"    sleftv a{j}_materialized;",
                          f"    char a{j}_string_value[2];",
                          f"    leftv a{j}_input=snative::scalar_index_argument(arg,a{j}_materialized,a{j}_string_value);",
                          f"    a{j}_input=snative::value_argument(a{j}_input);"]
                if node["result"]=="poly":
                    lines += [f'    if (snative::unsupported_poly_result_metadata(a{j}_input)) throw std::runtime_error("attributed poly arguments for polynomial returns are unsupported");']
                lines += [f"    snative::PolyArgument a{j}(a{j}_input,currRing);",
                          "    arg=arg->next;"]
                values.append(f"a{j}.get()")
                continue
            token = {"int":"INT_CMD"}[kind]
            argument = "arg"
            if kind == "int":
                argument = f"a{j}_input"
                lines += [f"    sleftv a{j}_materialized;",
                          f"    char a{j}_string_value[2];",
                          f"    leftv {argument}=snative::scalar_index_argument(arg,a{j}_materialized,a{j}_string_value);",
                          f"    {argument}=snative::value_argument({argument});"]
            mismatch = f"{argument}->Typ()!={token}"
            lines += [f"    if ({argument}==NULL || {mismatch} || {argument}->e!=NULL || {argument}->rtyp==ALIAS_CMD)",
                      f'      throw std::runtime_error("native argument {j+1} requires {kind}; got token " + std::to_string({argument}==NULL ? -1 : {argument}->Typ()) + ({argument}!=NULL && {argument}->e!=NULL ? " (indexed value)" : ""));',
                      *([f'    if (snative::metadata({argument})) throw std::runtime_error("attributed int arguments are unsupported");'] if kind=="int" else []),
                      f"    {CPP_TYPES[kind]} a{j}=({CPP_TYPES[kind]}){argument}->Data();",
                      "    if (errorreported) return TRUE;",
                      "    arg=arg->next;"]
            values.append(f"a{j}")
        lines += ['    if (arg!=NULL) throw std::runtime_error("native entry argument count mismatch");',
                  "    snative::Context ctx(currRing);",
                  "    ++compiled_entries;",
                  "    "+OWNED.get(node["result"], "const long")+" value="+node["symbol"]+"("+", ".join(["ctx"]+values)+");",
                  "    if (errorreported) return TRUE;",
                  "    if (sLastPrinted.rtyp!=0) sLastPrinted.CleanUp();",
                  ("    result->rtyp=INT_CMD; result->data=(void*)value;" if node["result"]=="int" else
                   ("    result->rtyp=IDEAL_CMD; result->flag=value.flags(); result->data=(void*)value.release();"
                    if node["result"]=="ideal" else
                  f"    result->rtyp={node['result'].upper()}_CMD; result->data=(void*)value.release();")),
                  "    return FALSE;",
                  "  } catch (const snative::ReportedInterpreterError &) { return TRUE; }",
                  "  catch (const std::exception &e) { WerrorS(e.what()); return TRUE; }", "}"]
    lines += ['extern "C" __attribute__((visibility("default"))) int mod_init(SModulFunctions *api) {']
    lines += ["  try {"]
    native_names = [export_prefix+"compilerCalls"]+[export_prefix+n["name"] for n in roots]
    for i, alias in enumerate(aliases):
        lines.append(f"    procinfov alias_{i}=snative::alias_source("+
                     json.dumps(alias["library"])+", "+json.dumps(alias["procedure"])+");")
    for i, node in enumerate(roots):
        lines.append(f"    snative::SourceProc source_{i}=snative::native_source("+
                     json.dumps(node["path"])+", "+json.dumps(node["name"])+");")
    for name in native_names+[export_prefix+a["procedure"] for a in aliases]:
        lines.append("    snative::check_export("+json.dumps(name)+");")
    for name in native_names:
        lines.append("    snative::prepare_native("+json.dumps(name)+");")
    for i, alias in enumerate(aliases):
        lines.append("    snative::install_alias("+json.dumps(export_prefix+alias["procedure"])+f", alias_{i});")
    counter = export_prefix+"compilerCalls"
    lines.append("    if (!api->iiAddCproc(\"compiled.so\", "+json.dumps(counter)+
                 ", FALSE, compiled_calls)) throw std::runtime_error("+
                 json.dumps("cannot register native entry "+counter)+");")
    for i, node in enumerate(roots):
        # Keep the source library identity in procinfo.  Singular's `example`
        # command follows this field to find the original procedure help; a
        # synthetic `compiled.so` identity makes an otherwise-correct rebound
        # procedure try to open a nonexistent library.
        origin = Path(node["path"]).name
        exported = export_prefix+node["name"]
        lines.append("    if (!api->iiAddCproc("+json.dumps(origin)+", "+
                     json.dumps(exported)+f", FALSE, entry_{i})) "+
                     "throw std::runtime_error("+
                     json.dumps("cannot register native entry "+exported)+");")
        lines.append("    snative::attach_native_source("+json.dumps(exported)+
                     f", entry_{i}, source_{i});")
    lines += ['  } catch (const std::exception &e) { WerrorS(e.what()); return MAX_TOK; }']
    lines += ["  return MAX_TOK;", "}"]
    return "\n".join(lines)+"\n"


def _node_identity(node):
    return {"path": node["path"], "name": node["name"], "params": node["params"],
            "nominal_params": node.get("nominal_params", {}),
            "dependencies": sorted(node["dependencies"]), "calls": node["calls"],
            "private": node["private"], "symbol": node["symbol"],
            "result": node["result"], "needs_ring": node["needs_ring"], "code": node["code"]}


def analyze_root(closure, path, name):
    """Analyze and emit exactly one root in an isolated mutable closure."""
    root = closure.visit(path, name)
    closure.generate()
    return root


def build(args):
    cache = cache_namespace(args.cache_dir.resolve())
    search = [p.resolve() for p in args.library_path]
    search += [Path(p).resolve() for p in args.library_search.split(os.pathsep) if p]
    search += [args.singular_prefix.resolve()/"share/singular/LIB"]
    candidates = [args.library]+[p/args.library for p in search]
    library = next((p for p in candidates if p.is_file()), None)
    if library is None:
        raise CompileError(f"library not found: {args.library}")
    stats = empty_stats()
    discovery = Closure(cache, search)
    path = str(library.resolve())
    library_index_value = discovery.library(path)
    add_stats(stats, discovery.stats)
    requested = sorted(set(args.entry or [name for name, proc in library_index_value["procedures"].items()
                                          if not proc["private"]]))
    root_keys, skipped, nodes = [], [], {}
    for name in requested:
        closure = Closure(cache, search)
        closure.libraries = discovery.libraries
        try:
            node = analyze_root(closure, path, name)
            for key, candidate in closure.nodes.items():
                if key in nodes and _node_identity(nodes[key]) != _node_identity(candidate):
                    raise CompileError(f"inconsistent isolated analysis for {Path(key[0]).name}::{key[1]}")
                nodes.setdefault(key, candidate)
            root_keys.append((node["path"], node["name"]))
        except CompileError as exc:
            source_proc = library_index_value["procedures"].get(name)
            if args.strict or source_proc is None or source_proc["private"] or name=="compilerCalls":
                raise
            skipped.append(dict(library=path, procedure=name, reason=str(exc), mode="interpreted-alias"))
        finally:
            add_stats(stats, closure.stats)
    if not root_keys and not skipped:
        raise CompileError("no public procedures could be compiled")
    # Public reachable helpers must also be callable from the interpreter.
    # Static helpers remain internal unless explicitly selected as roots.
    exports = {key: nodes[key] for key in root_keys}
    exports.update((key, node) for key, node in nodes.items() if not node["private"])
    roots = [exports[key] for key in sorted(exports)]
    names = [node["name"] for node in roots]+[entry["procedure"] for entry in skipped]
    if len(set(names))!=len(names):
        raise CompileError("public procedures in the closure have colliding export names")

    configuration = load_build_configuration(cache, args.singular_prefix.resolve(),
                                             args.cxx, args.cxxflag, stats)
    compiler = configuration["compiler"]
    options = configuration["compile_options"]
    identities = configuration["identities"]
    objects, manifest_nodes, tasks = [], [], []
    for key, node in sorted(nodes.items()):
        dependencies = [nodes[dependency] for dependency in sorted(node["dependencies"])]
        code = '#include "native_runtime.h"\n' + "\n".join(declaration(dep)+";" for dep in dependencies)
        code += "\n// Source: " + json.dumps(node["path"]) + "::" + node["name"] + "\n" + node["code"]
        object_id = digest([CACHE_FORMAT, identities["compile"], code])
        object_directory = cache/"objects"/object_id
        source = object_directory/(node["name"]+".cc")
        obj = object_directory/(node["name"]+".o")
        tasks.append(dict(cache=cache, key=object_id, source=source, target=obj, code=code,
                          compiler=compiler, options=options))
        objects.append(obj)
        manifest_nodes.append(dict(library=node["path"], procedure=node["name"],
                                   symbol=node["symbol"], result=node["result"], private=node["private"],
                                   parameters=node["params"], nominal_parameters=node.get("nominal_params", {}),
                                   source=str(source), object=str(obj),
                                   key=object_id, dependencies=[dep["symbol"] for dep in dependencies]))
    wrapper = module_source(roots, args.export_prefix, skipped)
    wrapper_id = digest([CACHE_FORMAT, identities["compile"], wrapper])
    wrapper_directory = cache/"objects"/wrapper_id
    entry_source, entry_object = wrapper_directory/"entrypoints.cc", wrapper_directory/"entrypoints.o"
    tasks.append(dict(cache=cache, key=wrapper_id, source=entry_source, target=entry_object,
                      code=wrapper, compiler=compiler, options=options))

    # executor.map retains task order, so diagnostics and statistics are
    # deterministic even though independent compilers run concurrently.
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        outcomes = list(executor.map(_compile_task, tasks))
    stats["objects_compiled"] += outcomes.count("compiled")
    stats["objects_reused"] += outcomes.count("reused")

    object_keys = [entry["key"] for entry in manifest_nodes]+[wrapper_id]
    module_id = digest([CACHE_FORMAT, identities["link"], identities["runtime"],
                        sorted(object_keys)])
    directory = cache/"modules"/module_id
    module = directory/("native_"+module_id[:16]+".so")
    outcome = link_module(cache, module_id, module, compiler,
                          configuration["link_options"], objects+[entry_object],
                          configuration["libraries"], configuration["request"]["ldd"],
                          configuration["request"]["working_directory"])
    stats["modules_linked" if outcome=="linked" else "modules_reused"] += 1
    build_id = digest([identities["compile"], identities["link"], identities["runtime"]])
    manifest = dict(format=VERSION, cache_format=CACHE_FORMAT, module=str(module), build=build_id,
                    abi=identities, exports=[args.export_prefix+name for name in names],
                    native_exports=[args.export_prefix+node["name"] for node in roots],
                    aliases=skipped, procedures=manifest_nodes,
                    counter=args.export_prefix+"compilerCalls", requested=requested, skipped=skipped,
                    entry_source=str(entry_source), entry_object=str(entry_object))
    manifest_key = digest([module_id, requested, skipped])
    manifest_path = directory/("manifest_"+manifest_key[:16]+".json")
    with key_lock(cache, "manifest", manifest_key):
        try:
            current = json.loads(manifest_path.read_text())
        except (OSError, ValueError, UnicodeError):
            current = None
        if current != manifest:
            save_json(manifest_path, manifest)
    return dict(module=str(module), manifest=str(manifest_path), exports=manifest["exports"],
                native_exports=manifest["native_exports"], aliases=skipped, skipped=skipped,
                warnings=["WARNING: "+args.export_prefix+entry["procedure"]+
                          " is an interpreted alias, not native C++" for entry in skipped],
                stats=stats)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("library", type=Path)
    parser.add_argument("--entry", action="append", help="procedure root; omit to attempt every public procedure")
    parser.add_argument("--strict", action="store_true", help="fail the whole library if any public procedure is unsupported")
    parser.add_argument("--singular-prefix", type=Path)
    parser.add_argument("--singular-binary", type=Path, help="infer installation prefix from this interpreter")
    parser.add_argument("--cache-dir", type=Path, default=Path(os.environ.get("XDG_CACHE_HOME", str(Path.home()/".cache")))/"singular/native")
    parser.add_argument("--library-path", type=Path, action="append", default=[])
    parser.add_argument("--library-search", default="", help="interpreter library search path")
    parser.add_argument("--export-prefix", default="c_")
    parser.add_argument("--receipt", type=Path, help="write module path for compiler.lib after a successful build")
    parser.add_argument("--cxx", default=os.environ.get("CXX", "c++"))
    parser.add_argument("--cxxflag", action="append", default=[])
    parser.add_argument("--jobs", type=int, default=DEFAULT_JOBS,
                        help="maximum concurrent C++ object compilations")
    args = parser.parse_args()
    if args.singular_prefix is None:
        binary = args.singular_binary or shutil.which("Singular")
        if not binary:
            parser.error("--singular-prefix or --singular-binary is required")
        args.singular_prefix = Path(binary).resolve().parent.parent
    if not re.fullmatch(r"[A-Za-z][A-Za-z0-9_]*", args.export_prefix):
        parser.error("export prefix must be a nonempty identifier")
    if args.jobs < 1:
        parser.error("--jobs must be at least 1")
    try:
        result = build(args)
        if args.receipt:
            if "\n" in result["module"] or "\r" in result["module"]:
                raise CompileError("module path contains a newline")
            atomic(args.receipt, result["module"])
        print(json.dumps(result, indent=2))
    except (CompileError, OSError, RecursionError, UnicodeError) as exc:
        print(f"singular2cpp: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
