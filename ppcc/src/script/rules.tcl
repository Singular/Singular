source src/script/gen.tcl

foreach kw $keywords {
  emit [format "    %-15s { PUSH_TOKEN(Sym[capitalize $kw]); }\n" "\"$kw\""]
}
