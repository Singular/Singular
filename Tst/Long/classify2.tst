LIB "tst.lib";
tst_init();

LIB "classify2.lib";
printlevel=0;


ring R = 0,(x,y),ds;

list individuals = list("E_6",x^3+y^4,list()),
list("E_7",x^3+x*y^3,list()),
list("E_8",x^3+y^5,list()),
list("X_9",x^4+x^2*y^2+y^4,list(x^2*y^2)),
list("J_10",x^3+x^2*y^2+y^6,list(x^2*y^2)),
list("E_12",x^3+y^7,list(x*y^5)),
list("E_13",x^3+x*y^5,list(y^8)),
list("E_14",x^3+y^8,list(x*y^6)),
list("E_18",x^3+y^10,list(x*y^7,x*y^8)),
list("E_19",x^3+x*y^7,list(y^11,y^12)),
list("E_20",x^3+y^11,list(x*y^8,x*y^9)),
list("Z_11",x^3*y+y^5,list(x*y^4)),
list("Z_12",x^3*y+x*y^4,list(x^2*y^3)),
list("Z_13",x^3*y+y^6,list(x*y^5)),
list("Z_17",x^3*y+y^8,list(x*y^6,x*y^7)),
list("Z_18",x^3*y+x*y^6,list(y^9,y^10)),
list("Z_19",x^3*y+y^9,list(x*y^7,x*y^8)),
list("W_12",x^4+y^5,list(x^2*y^3)),
list("W_13",x^4+x*y^4,list(y^6)),
list("W_17",x^4+x*y^5,list(y^7,y^8)),
list("W_18",x^4+y^7,list(x^2*y^4,x^2*y^5)),
list("J_(3,0)",x^3+x^2*y^3+y^9,list(x^2*y^3,x*y^7)),
list("Z_(1,0)",x^3*y+x^2*y^3+y^7,list(x^2*y^3,x*y^6)),
list("W_(1,0)",x^4+x^2*y^3+y^6,list(x^2*y^3,x^2*y^4));

poly f0,f1;
Poly f;
NormalFormEquation F;
list cf = 11,13;
map phi;
int j;

int tp=0;
for (int i = 1;i<=size(individuals);i++){
if (i<>4){
"---------------------------";
individuals[i][1];
f0=individuals[i][2];
for (j=1;j<=size(individuals[i][3]);j++){
   f0=f0+cf[j]*individuals[i][3][j];
}
phi = basering,x+y+x^2+y^2+y^3+y^4,3*x+2*y+x*y;
f1=phi(f0);
f = makePoly(f1);
if (tp==1){complexType(f);} else {
F = complexClassify(f);
F;
}
"input "+string(f0);
}
}

tp=1;
for (i = 1;i<=size(individuals);i++){
"---------------------------";
individuals[i][1];
f0=individuals[i][2];
for (j=1;j<=size(individuals[i][3]);j++){
   f0=f0+cf[j]*individuals[i][3][j];
}
phi = basering,x+y+x^2+y^2+y^3+y^4,3*x+2*y+x*y;
f1=phi(f0);
f = makePoly(f1);
if (tp==1){complexType(f);} else {
F = complexClassify(f);
F;
"input "+string(f0);
}
}







list families;
int mu=18;
if (mu>=1){families[size(families)+1]=list("A_"+string(mu), x^2+y^(mu+1),list())};

if (mu>=4){families[size(families)+1]=list("D_"+string(mu), x^2*y+y^(mu-1),list())};

if (mu>=11){families[size(families)+1] = list("J_"+string(mu), x^3+x^2*y^2+y^(mu-4), list(y^(mu-4)));
         for (i=1;i<mu-9;i++){families[size(families)+1] = list("Y_("+string(i+4)+","+string(4+mu-9-i)+")", x^(4+i)+x^2*y^2+y^(4+(mu-9-i)),list(x^2*y^2));};
}
//
if (mu>=10){families[size(families)+1]=list("X_"+string(mu), x^4+x^2*y^2+y^(mu-5),list(y^(mu-5)))};
//
if (mu>=17){families[size(families)+1]=list("J_(3,"+string(mu-16)+")",x^3+x^2*y^3+y^(mu-7)+y^(mu-6) ,list(y^(mu-7),y^(mu-6)))};
//
if(mu>=16){families[size(families)+1]=list("Z_(1,"+string(mu-15)+")",x^3*y+x^2*y^3+y^(mu-8)+y^(mu-7) ,list(y^(mu-8),y^(mu-7)));
//
families[size(families)+1]=list("W_(1,"+string(mu-15)+")",x^4+x^2*y^3+y^(mu-9)+y^(mu-8) ,list(y^(mu-9),y^(mu-8)));
//
if (mu mod 2 == 0) {families[size(families)+1]=list("W#_(1,"+string(mu-15)+")",(x^2+y^3)^2+x*y^((mu div 2)-3)+x*y^((mu div 2)-2) ,list(x*y^((mu div 2)-3),x*y^((mu div 2)-2)));}
//
mu=mu+1;
if (mu mod 2 == 1) {families[size(families)+1]=list("W#_(1,"+string(mu-15)+")",(x^2+y^3)^2+x^2*y^((mu-9) div 2)+x^2*y^((mu-7) div 2) ,list(x^2*y^((mu-9) div 2),x^2*y^((mu-7) div 2)));}
};



int j;

tp=0;
for (i = 1;i<=size(families);i++){
"---------------------------";
"Input Type "+families[i][1];
//families[i];
f0=families[i][2];
for (j=1;j<=size(families[i][3]);j++){
   f0=f0+cf[j]*families[i][3][j];
}
//phi = basering,x+y+x^2+y^2+y^3,3*x+2*y+x*y;
phi = basering,x+y+x*y+x^2+y^2+y^3+y^4,3*x+2*y+x*y;
f1=phi(f0);
f = makePoly(f1);
if (tp==1){complexType(f);} else {
F = complexClassify(f);
F;
}
"input "+string(f0);
}


tp=1;
for (i = 1;i<=size(families);i++){
"---------------------------";
"Input Type "+families[i][1];
//families[i];
f0=families[i][2];
for (j=1;j<=size(families[i][3]);j++){
   f0=f0+cf[j]*families[i][3][j];
}
//phi = basering,x+y+x^2+y^2+y^3,3*x+2*y+x*y;
phi = basering,x+y+x*y+x^2+y^2+y^3+y^4,3*x+2*y+x*y;
f1=phi(f0);
f = makePoly(f1);
if (tp==1){complexType(f);} else {
F = complexClassify(f);
F;
}
"input "+string(f0);
}



tst_status(1);