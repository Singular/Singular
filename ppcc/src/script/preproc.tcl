proc readfile {filename} {
  set result ""
  catch {
    set fp [open $filename r]
    set result [read -nonewline $fp]
    close $fp
  }
  return [split $result "\n"]
}

proc writefile {filename data} {
  catch {
    set fp [open $filename w]
    puts -nonewline $fp $data
    close $fp
  }
}

proc preprocess {in out} {
  set result ""
  foreach line [readfile $in] {
    switch -glob -- $line {
      "#script *" {
        set script [string range $line 8 end]
	set script [string trim $script]
        append result [exec [info nameofexecutable] src/script/$script] "\n"
      }
      * {
        append result $line "\n"
      }
    }
  }
  writefile $out $result
}

foreach arg $argv {
  switch -glob -- $arg {
    *.in {
      set out [string range $arg 0 end-3]
      puts "$arg => $out"
      preprocess $arg $out
    }
    * {
      puts "skipping: $arg"
    }
  }
}
