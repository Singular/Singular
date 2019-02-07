source src/script/gen.tcl

emit "const char *SymbolNames\[] = {
  \"SymNone\",
"
foreach kw "$keywords $other" {
  emit "  \"Sym[capitalize $kw]\",\n"
}

emit "};\n"
