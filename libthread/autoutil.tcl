proc shell-quote {arg} {
  switch -regexp -- $arg {
    {^$} {return "''"}
    {^[-a-zA-Z0-9_@%+=:,./]+$} {
      return $arg
    }
    default {
      return "'[string map {' '\"'\"'} $arg]'"
    }
  }
}

proc opt-list {prefix paths} {
  set result {}
  foreach path $paths {
    append result " $prefix[shell-quote $path]"
  }
  return [string trim $result]
}

proc -L {args} {
  return [opt-list -L $args]
}

proc -I {args} {
  return [opt-list -I $args]
}

proc -l {args} {
  return [opt-list -l $args]
}

proc -D {args} {
  return [opt-list -D $args]
}
