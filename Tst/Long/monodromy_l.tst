LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: monodromy_l.tst,v 1.11 2004-02-23 19:06:14 Singular Exp $");

LIB "mondromy.lib";

list unimodal=
"P[8]","x,y,z","x3+y3+z3+xyz",
"X[9]","x,y","x4+y4+x2y2",
"J[10]","x,y","x3+y6+x2y2",
"T[3,4,5]","x,y,z","x3+y4+z5+xyz",
"T[3,4,6]","x,y,z","x3+y4+z6+xyz",
"T[4,5,6]","x,y,z","x4+y5+z6+xyz",
"E[12]","x,y","x3+y7+xy5",
"E[13]","x,y","x3+xy5+y8",
"E[14]","x,y","x3+y8+xy6",
"Z[11]","x,y","x3y+y5+xy4",
"Z[12]","x,y","x3y+xy4+x2y3",
"Z[13]","x,y","x3y+y6+xy5",
"W[12]","x,y","x4+y5+x2y3",
"W[13]","x,y","x4+xy4+y6",
"Q[10]","x,y,z","x3+y4+yz2+xy3",
"Q[11]","x,y,z","x3+y2z+xz3+z5",
"Q[12]","x,y,z","x3+y5+yz2+xy4",
"S[11]","x,y,z","x4+y2z+xz2+x3z",
"S[12]","x,y,z","x2y+y2z+xz3+z5",
"U[12]","x,y,z","x3+y3+z4+xyz2";

list bimodal=
//"J[3,0]","x,y","x3+x2y3+y9+xy7",
"J[3,1]","x,y","x3+x2y3+y10",
"J[3,2]","x,y","x3+x2y3+y11",
//"Z[1,0]","x,y","x3y+x2y3+xy6+y7",
"Z[1,1]","x,y","x3y+x2y3+y8",
"Z[1,2]","x,y","x3y+x2y3+y9",
"W[1,0]","x,y","x4+x2y3+y6",
"W[1,1]","x,y","x4+x2y3+y7",
"W[1,2]","x,y","x4+x2y3+y8",
"W#[1,1]","x,y","(x2+y3)^2+xy5",
"W#[1,2]","x,y","(x2+y3)^2+x2y4",
"W#[1,3]","x,y","(x2+y3)^2+xy6",
//"W#[1,4]","x,y","(x2+y3)^2+x2y5",
"Q[2,0]","x,y,z","x3+yz2+x2y2+xy4",
"Q[2,1]","x,y,z","x3+yz2+x2y2+y7",
"Q[2,2]","x,y,z","x3+yz2+x2y2+y8",
"S[1,0]","x,y,z","x2z+yz2+y5+zy3",
"S[1,1]","x,y,z","x2z+yz2+x2y2+y6",
"S[1,2]","x,y,z","x2z+yz2+x2y2+y7",
"S#[1,1]","x,y,z","x2z+yz2+zy3+xy4",
"S#[1,2]","x,y,z","x2z+yz2+zy3+x2y3",
"S#[1,3]","x,y,z","x2z+yz2+zy3+xy5",
//"S#[1,4]","x,y,z","x2z+yz2+zy3+x2y4",
"U[1,0]","x,y,z","x3+xz2+xy3+y3z",
"U[1,1]","x,y,z","x3+xz2+xy3+y2z2",
"U[1,2]","x,y,z","x3+xz2+xy3+y4z",
"U[1,3]","x,y,z","x3+xz2+xy3+y3z2",
"U[1,4]","x,y,z","x3+xz2+xy3+y5z",
"E[18]","x,y","x3+y10+xy7",
"E[19]","x,y","x3+xy7+y11",
"E[20]","x,y","x3+y11+xy8",
"Z[17]","x,y","x3y+y8+xy6",
"Z[18]","x,y","x3y+xy6+y9",
"Z[19]","x,y","x3y+y9+xy7",
"W[17]","x,y","x4+xy5+y7",
"W[18]","x,y","x4+y7+x2y4",
"Q[16]","x,y,z","x3+yz2+y7+xy5",
//"Q[17]","x,y,z","x2z+yz2+xy5+y8",
"Q[18]","x,y,z","x3+yz2+y8+xy6",
"S[16]","x,y,z","x2z+yz2+xy4+y6",
"S[17]","x,y,z","x2z+yz2+y6+zy4",
"U[16]","x,y,z","x3+xz2+y5+x2y2";

proc tst_monodromy(list data)
{
  int i;
  string s,typ;
  for(i=1;i<=size(data);i=i+3)
  {
    s="typ=\""+data[i]+"\";";
    execute(s);
    s="ring R=0,("+data[i+1]+"),ds;";
    execute(s);
    export(R);
    s="poly f="+data[i+2]+";";
    execute(s);
    typ;
    jordan(monodromyB(f));
    tst_status();
    kill R;
  }
}

tst_monodromy(unimodal);

tst_status(1); $
