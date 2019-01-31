if {[catch {
  msg-checking "Checking for extra configuration in src..."
  # We generate the .cc and .h files so that they get picked
  # up during dependency checking.
  set output [exec [info nameofexecutable] src/script/preproc.tcl {*}[glob src/*.in]]
} output]} {
  msg-result "failed"
  exit 1
}
msg-result "ok"
puts $output
