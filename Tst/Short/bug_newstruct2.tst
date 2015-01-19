LIB "tst.lib";
tst_init();

proc divisor_print(divisor D)
{
"Divisor = ("+string(D.den)+") - ("+string(D.num)+")";
}
proc divisor_status(divisor D)
{
  return ("status");
}
proc divisor_test(divisor D)
{
  return ("test");
}
proc divisor_gcd(divisor D, divisor D2)
{
  return ("gcd");
}
proc divisor_extgcd(divisor D, divisor D2)
{
  return ("extgcd");
}
proc divisor_diff(divisor D, divisor D2)
{
  return ("diff");
}

newstruct("divisor","ideal den,ideal num");
newstruct("formaldivisor","list summands");
newstruct("pdivisor","list summands, cone tail");

system("install","divisor","status",divisor_status, 4);
system("install","divisor","test",divisor_test,4);
system("install","divisor","gcd",divisor_gcd,2);
system("install","divisor","extgcd",divisor_extgcd,2);


system("install","divisor","print",divisor_print,1);

ring r=0,(x,y),dp;

divisor C;
C.num = ideal(x);
C.den = ideal(y);

C;
test(C);
status(C);
gcd(C,C);
extgcd(C,C);
// -------------------------------------------------------
newstruct("myunion", "poly p,int i");
proc to_poly(myunion uni)
{
  return (uni.p);
}

proc to_int(myunion uni)
{
  return (uni.i);
}

ring r=0,x,dp;

myunion uni;
uni.p = x+1;
uni.i = 17;

system("install", "myunion", "poly", to_poly, 1);
system("install", "myunion", "int", to_int, 1);

poly(uni); // -> x+1
int(uni);   // -> 17

// ---------------------------------------------------------

newstruct("stringifiable", "int i");
stringifiable stry;
stry;
string(stry);

proc to_str(def arg) { return ("(overloaded)"); }
system("install", "stringifiable", "string", to_str,1);
stry;  // print falls back to string
string(stry);

proc to_str_wrong(def arg) { return (17); }
system("install", "stringifiable", "string", to_str_wrong,1);
stry;  // incorrectly converted, fall back to default
string(stry);

proc printing(def arg) { ("(overloaded)"); }
system("install", "stringifiable", "print", printing,1);

stry;  // now only print works correctly
string(stry); // incorrectly converted, fall back to default

tst_status(1);$
