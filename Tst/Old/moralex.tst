ring sr=0,(x,y,z),(c,ls);
ideal j=x2+y2+z2,x4+y5+z6;
sr;
// std von jacob(j) klemmte
proc mi
{
 ideal @id=#[1];
 string @s;
 int @n=size(@id);
 matrix @mat[1][@n]=@id;
 int @mnr;
 matrix @ff[1][@n]=@mat;
 ideal @tid;
 ideal @tmod;
 matrix @tmat[1][1];
 for (int @l=@n; @l>0; @l=@l-1)
 {
    @tid=@ff;
    @tmat=jacob(@tid);
    @tmod=minor(@tmat,@l);
    @ff[1,@l]=0;
    @tmod=@tmod,@ff;
    "jetzt kommt std von";
    @tmod;
    @tmod=std(@tmod);
    "geschafft";
    vdim(@tmod);
    @mnr=vdim(@tmod)-@mnr;
    return(@tmod);
 }
}
mi(j);
LIB "tst.lib";tst_status(1);$
