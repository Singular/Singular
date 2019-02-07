proc emit {args} {
  foreach str $args {
    puts -nonewline $str
  }
}

proc capitalize {str} {
  return [string toupper [string range $str 0 0]][string range $str 1 end]
}

set keywords "class namespace extern VAR EXTERN_VAR STATIC_VAR
              INST_VAR EXTERN_INST_VAR STATIC_INST_VAR"

set other "Ident Literal Op Semicolon Comma
  Equal Ast And AndAnd ColonColon
  LPar RPar LBrkt RBrkt LBrace RBrace
  WS EOL Comment BAD LineDir EOF Gen"
