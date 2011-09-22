LIB "tst.lib"; tst_init();
proc xxx
{
  int k=4;        //defines a local variable k
  int result=k+2;
  export(result);  //defines the global variable "result".
}
xxx();
listvar(all);
tst_status(1);$
