LIB "tst.lib"; tst_init();
proc p1
{
  int i,j;
  exportto(Current,i);
  intmat m;
  listvar(Current);
  exportto(Top,m);
}
p1();
package Test1;
exportto(Test1,p1);
listvar(Top);
listvar(Test1);
Test1::p1();
listvar(Top);
listvar(Test1);
tst_status(1);$
