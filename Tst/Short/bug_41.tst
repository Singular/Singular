LIB "tst.lib";
tst_init();
// attrib for list elements

ring r;
ideal i=x;
list l=i,i;
homog(l[1]);
attrib(l);
attrib(l[1]);
attrib(l[2]);

attrib(l[2],"huhu",1);
attrib(l);
attrib(l[1]);
attrib(l[2]);

l[1][2]=y;
attrib(l);
attrib(l[1]);
attrib(l[2]);

l[2][2]=y;
attrib(l);
attrib(l[1]);
attrib(l[2]);

tst_status(1);$
