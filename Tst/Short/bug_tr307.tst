LIB "tst.lib";
tst_init();

proc huhu(int i)
{
 print(i);
}
proc hihi()
{
 return(huhu);
}
hihi()(1);

list ll=(huhu);
proc(ll[1])(2);
def ll_1=ll[1];
ll_1(3);

huhu;
hihi;
proc huhu1()
{
 print(17);
}
proc hihi1(int i)
{
 return(huhu1);
}
proc(hihi1(1))();

ll=(huhu1);
proc(ll[1])();
def ll_2=ll[1];
ll_2();

hihi1;
huhu1;

tst_status(1);$
