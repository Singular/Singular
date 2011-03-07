echo = 2;

LIB("./bigintm.so");

printBlackboxTypes();

bigintm_setup();

printBlackboxTypes();

bigintm_setup();

printBlackboxTypes();


bigintm a = 666;
bigintm b = -666;


typeof(a); // TODO!
a;


typeof(b); // TODO!
b;

a + b;

typeof(_);

a - b;



a * b;

typeof(_);

a();

typeof(_);


a(b);

typeof(_);


def s = "sssss";
typeof(s);

a(s);


a("bbbbbb");



def i = 6;
typeof(i);

a(i);


a(6);


b(2);


bigintm c = 666;


a==a; // BUG?
a==b; // BUG?
a==c; // BUG?

c == 666;

c == -666;


a - s;

c == s;

c.s;
c.sss;
c.0;



$$$

