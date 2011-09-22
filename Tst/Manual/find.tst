LIB "tst.lib"; tst_init();
  find("Aac","a");
  find("abab","a"+"b");
  find("abab","a"+"b",2);
  find("abab","ab",3);
  find("0123","abcd");
tst_status(1);$
