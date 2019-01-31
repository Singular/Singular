source src/script/gen.tcl

emit "enum Symbol {
  SymNone,
"
foreach kw "$keywords $other" {
  emit "  Sym[capitalize $kw],\n"
}

emit "};\n"
