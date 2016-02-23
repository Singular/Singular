LIB "tst.lib"; tst_init();
bigint(5)*2, bigint(2)^100-10;
bigint(-5) div 2, bigint(-5) mod 2;

proc check_divmod(bigint a, bigint b)
{
  if ((a div b)*b+(a mod b)==a)
  {
    "sum okay";
  }
  else
  {
    "div:",a div b;
    "mod:",a mod b;
    "sum:",(a div b)*b+(a mod b)," a:",a," b:",b;
  }
  if((a mod b)>=0) {"sign okay";}
  else { "a mod b:",a mod b; }
}
// small-small
check_divmod(7,3);
check_divmod(7,-3);
check_divmod(-7,-3);
check_divmod(-7,3);
// small -large
check_divmod(7,30000000000);
check_divmod(7,-30000000000);
check_divmod(-7,-30000000000);
check_divmod(-7,30000000000);
// large-small
check_divmod(70000000000,3);
check_divmod(70000000000,-3);
check_divmod(-70000000000,-3);
check_divmod(-70000000000,3);
// large-large
check_divmod(70000000000,30000000000);
check_divmod(70000000000,-30000000000);
check_divmod(-70000000000,-30000000000);
check_divmod(-70000000000,30000000000);
tst_status(1);$
