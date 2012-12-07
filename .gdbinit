break dErrorBreak
break dReportError
break omReportError


### http://sourceware.org/gdb/onlinedocs/gdb/Define.html#Define

# print poly ring
define pp
  if $argc > 0
    print $arg0
    if $arg0 != 0 
      print *$arg0

      if $argc == 2
        call p_Write($arg0, $arg1)
      else
        call p_Write($arg0, currRing)
      end
    end
  end
end
     
run
backtrace
