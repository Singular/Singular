LIB "tst.lib";
tst_init();


proc MyPrint(def what)
{
   "what";
   what;
   "print(what)";
   print(what);
   "string(what)";
   string(what);
   "print(what, \"%s\")";
   print(what, "%s");
   "print(what, \"%p\")";
   print(what, "%p");
   "print(what, \"%l\")";
   print(what, "%l");
   "print(what, \"%;\")";
   print(what, "%;");
   "print(what, \"%2s\")";
   print(what, "%2s");
   "print(what, \"%2l\")";
   print(what, "%2l");
   "print(what, \"%t\")";
   print(what, "%t");
}

list l = 1, 2; 
MyPrint(l);

list ll;
MyPrint(ll);

l = l + ll;
MyPrint(l);

l = insert(l, ll);
MyPrint(l);

l[10] = 5;

string(l);
MyPrint(l);

l[3] = intvec(1,2,3);
l[4] = "hi";
MyPrint(l);

l[6] = list("ho", "he");
MyPrint(l);

l[8] = MyPrint;
MyPrint(l);

link li = "/tmp";
l[8] = li;
MyPrint(l);

ring r;
MyPrint(r);

ideal i;
MyPrint(i);
 
number n = 12345;
MyPrint(n);
 
vector v = [x+1, y];
MyPrint(v);
 
poly p = x2+y3+n;
MyPrint(p);

l[8] = r;
l[9] = i;
l[7] = n;
// l[1] = v;
l[5] = p;
MyPrint(l);

matrix ma[2][2] = x,y,z,x2;
MyPrint(ma);

module m=[1,y],[0,x+z];
MyPrint(m);

l[1]= list(ma, m);
MyPrint(l);

intmat M=betti(mres(m,0));
MyPrint(M);
print(M,"betti");
print(M, "%b");
l[2] = M;
MyPrint(l);
 
ideal j = z2,x;
resolution re = res(j,0);
MyPrint(re);
l[11] = re;
MyPrint(re);

matrix B(1..3);
B(1)[1,1] = 1;
B(2)[1,1] = 2;
B(3)[1,1] = 3;

B(1..3);
print(B(1..3));
string(B(1..3));

qring qr = std(ideal(x,y));
MyPrint(qr);

list l = 1, qr, list(2, qr);
MyPrint(l);


map phi = r, maxideal(2);
MyPrint(phi);

l[5] = phi;
MyPrint(l);

example sprintf;
example printf;
example fprintf;

tst_status(1);$

