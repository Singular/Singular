//
// test script for regularity command
//
ring r1 = 31991,(t,x,y,z,w),(c,ls);
r1;
"-------------------------------";
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
i;
list t3=mres(i,0);
t3;

"------------------------------";
regularity(t3);
"------------------------------";
listvar(all);
kill r1;
ring R=0,(r,s,t,v,w,x,y,z),dp;
//
ideal I = v-r9, w-r7st, x-r3s2t4, y-r9-s3t6, z-s6r3;
//
ideal J = eliminate( I, rst);
J = std(J);
list R1=nres(J,0);
print( R1);
regularity( R1);
LIB "tst.lib";tst_status(1);$;
