list l1=1,2,3;
list l2="aa","bb",list();
list l3=list(list(list()));
l1;
l2;
l3;
list l=l1,l2,l3,list();
l;
insert(l1,"-------",1);
insert(l1,"-------",2);
insert(l1,"-------",3);
insert(l1,"-------",4);
insert(l1,"-------",0);
list L;
list L1 = 1,2,3;
L[1]= L1;
L[1][2];  
L[1][2] = 0;
L[1][2];  
L;
L[1][1] = 0;
L;
L[1][1];
L;
LIB "tst.lib";tst_status(1);$
LIB "tst.lib";tst_status(1);$
