LIB "tst.lib";
tst_init();

ring A1=0,(a,b,c),dp;
ideal P=ab-c2;
qring A=std(P);                
poly p=abc;

ring B1=0,(x,y,z,a,b,c),dp;
ideal I=x2,y,ab-c2;
qring B=std(I);               
map ib=A,a,b,c;                

ring C1=0,(u,v,a,b,c),lp;
ideal J=uv,ab-c2;
qring C=std(J);               
map ic=A,a,b,c;              

ring T1=0,(x,y,z,u,v,a,b,c),dp;
ideal K=imap(C1,J)+imap(B1,I);
qring T=std(K);                
map jb=B,x,y,z,a,b,c;       
map jc=C,u,v,a,b,c;           

map psi=jc(ic);                //
map phi=jb(ib);
psi(p);

phi(p);


tst_status(1);$
