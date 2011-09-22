LIB "tst.lib"; tst_init();
  string s1="Now I know";
  string s2="how to encode a \" in a string...";
  string s=s1+" "+s2; // concatenation of 3 strings
  s;
  s1,s2;   // 2 strings, separated by a blank in the output:
tst_status(1);$
