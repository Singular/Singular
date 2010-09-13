ring r= 32003,(x,y,z),lp;

proc printid
{
  int @usage = size(#) != 2;
  if (@usage == 0)
  {
    @usage = typeof(#[1]) != "ideal";
    if (@usage)
    {
      typeof(#[1]), " not allowed for printid";
    }
  }
  if (@usage) {
    "usage : printid(ideal,prefix)",newline,
    "        prints nonzero elements, w/ or w/o pref";
    return();
  }

  #[2], size(#[1]);
  int @i;
  for (@i=1; @i<=size(#[1]); @i=@i+1)
  {
    if (#[2] != "")
    {
      #[2] , "(", @i, ") ", #[1][@i];
    }
    if (#[2] == "")
    {
      #[1][@i];
    }
  }
}

poly px = x;
px;
poly p1 = 1x2;
p1;

poly p2 = 3x2yz5;
p2;

poly p3 =  3 * 1x2yz5;
p3;

poly p4 =  (2x2yz5)^2;
p4;

poly p5 =  (2x2yz5)^2 - 1x;
p5;

poly p6 =  (2x2yz5)^2 - p3;
p6;

poly p6 =  x;
p6;
kill p6;
poly p6 =  x;
p6;

1a 2x;
1y 2x;
y 2x;
1y2x;

"p1", p1;
"p2", p2;
"p3", p3;
"p4", p4;
matrix m[2][2] = p1,p2,p3,p4;
m;
kill m;
matrix m[2][2] = p1,p2,p3,p4;
m;
m[1,2];
"-------------------------------------";
ideal id = m, xy3;
id;

ideal sid = std(id);
sid;

sid[3]+0 == 0;
sid;
".....................";
sid[2] == 1x4y2z10;
sid[2] == x2;
sid;
sid[2] = x2z;
sid;
"-------------------";
printid("?");
printid(sid,"; ");
sid;
printid(sid,"");
sid;
printid(m,"");
listvar(all);
// lamm@mathematik.uni-kl.de (0.9.2e):
poly null;
ord(null);
"-------------------";
ring R = 0,x,dp;
poly f = (x+3)*(x+2);
poly q = (x+2)^2;
poly h = 20x + 40;
(f*q)/h;
LIB "tst.lib";tst_status(1);$;
