LIB "tst.lib"; tst_init();
  ring r=0,(x(1..500)),dp;
  poly p=(x(1)+x(500))^50;
  proc ReportMemoryUsage()
  {  "Memory currently used by SINGULAR     :",memory(0),"Byte (",
     int(memory(0)/1023), "KByte)" +newline+
     "Memory currently allocated from system:",memory(1), "Byte (",
     int(memory(1)/1023), "KByte)";
     "Maximal memory allocated from system  :",memory(2), "Byte (",
     int(memory(2)/1023), "KByte)";
  }
  ReportMemoryUsage();
  kill p;
  ReportMemoryUsage(); // less memory used: p killed
  kill r;
  ReportMemoryUsage(); // even less memory: r killed
tst_status(1);$
