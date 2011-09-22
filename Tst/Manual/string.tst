LIB "tst.lib"; tst_init();
  string s="Hi";
  string s1="a string with new line at the end"+newline;
  string s2="another string with new line at the end
  ";
  s;s1;s2;
  ring r; ideal i=std(ideal(x,y^3));
  "dimension of i =",dim(i),", multiplicity of i =",mult(i);
  "dimension of i = "+string(dim(i))+", multiplicity of i = "+string(mult(i));
  "a"+"b","c";
tst_status(1);$
