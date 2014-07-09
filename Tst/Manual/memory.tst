LIB "tst.lib"; tst_init();
  ring r=0,(x(1..500)),dp;
  poly p=(x(1)+x(500))^50;
  proc ReportMemoryUsage()
  {  tst_ignore("Memory currently used by SINGULAR     :" + string(memory(0)) + "Byte (" + string(int(memory(0)div 1024)) + "KByte)" );
     tst_ignore("Memory currently allocated from system:" + string(memory(1)) + "Byte (" + string(int(memory(1)div 1024)) + "KByte)" );
     tst_ignore("Maximal memory allocated from system  :" + string(memory(2)) + "Byte (" + string(int(memory(2)div 1024)) + "KByte)" );
  }
  ReportMemoryUsage();
  kill p;
  ReportMemoryUsage(); // less memory used: p killed
  kill r;
  ReportMemoryUsage(); // even less memory: r killed
tst_status(1);$
