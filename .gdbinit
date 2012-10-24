break dErrorBreak
break dReportError



### http://sourceware.org/gdb/onlinedocs/gdb/Define.html#Define

# print poly
define pp

if $argc > 0
  print $arg0
  if $arg0 != 0 
    print *$arg0

if $argc == 2
  call p_Write($arg0, $arg1)
end
  end
end


end
     
