LIB "tst.lib";
tst_init();

proc check(int ok, string label)
{
  if (!ok)
  {
    ERROR(label);
  }
}

proc ssi2_types_proc(int a)
{
  return(a+1);
}

proc ssi2_types_roundtrip(string write_link, string read_link)
{
  ring R=0,(x,y,z),dp;
  int ii=42;
  string ss="abc";
  bigint bi=123456789012345678901234567890;
  number nn=3/7;
  poly pp=x2+2/3*y*z-5;
  vector vv=[x+y,z2,1];
  ideal id=pp,x*y-z;
  module mo=[x,y,z],[z2,1,x+y];
  matrix ma[2][2]=x,y,z,x+y+1;
  smatrix sma=ma;
  intvec iv=1,-2,3,5;
  intmat im[2][3]=1,2,3,4,5,6;
  bigintmat bm[2][2]=1,224553233465,-3,4;
  bigintvec bv=2,5,224553233465;
  list li=ii,ss,pp,id,iv;

  string ii_s=string(ii);
  string ss_s=ss;
  string bi_s=string(bi);
  string nn_s=string(nn);
  string ring_s=string(R);
  string pp_s=string(pp);
  string vv_s=string(vv);
  string id_s=string(id);
  string mo_s=string(mo);
  string ma_s=string(ma);
  string sma_s=string(matrix(sma));
  string iv_s=string(iv);
  string im_s=string(im);
  string bm_s=string(bm);
  string bv_s=string(bv);
  string li_s=string(li);

  link out=write_link;
  write(out,ii);
  write(out,ss);
  write(out,bi);
  write(out,nn);
  write(out,R);
  write(out,pp);
  write(out,vv);
  write(out,id);
  write(out,mo);
  write(out,ma);
  write(out,sma);
  write(out,iv);
  write(out,im);
  write(out,bm);
  write(out,bv);
  write(out,ssi2_types_proc);
  write(out,li);
  close(out);
  kill out;

  link in=read_link;
  def rii=read(in);
  check(typeof(rii)=="int", "int type");
  check(string(rii)==ii_s, "int value");
  def rss=read(in);
  check(typeof(rss)=="string", "string type");
  check(rss==ss_s, "string value");
  def rbi=read(in);
  check(typeof(rbi)=="bigint", "bigint type");
  check(string(rbi)==bi_s, "bigint value");
  def rnn=read(in);
  check(typeof(rnn)=="number", "number type");
  check(string(rnn)==nn_s, "number value");
  def rring=read(in);
  check(typeof(rring)=="ring", "ring type");
  check(string(rring)==ring_s, "ring value");
  def rpp=read(in);
  check(typeof(rpp)=="poly", "poly type");
  check(string(rpp)==pp_s, "poly value");
  def rvv=read(in);
  check(typeof(rvv)=="vector", "vector type");
  check(string(rvv)==vv_s, "vector value");
  def rid=read(in);
  check(typeof(rid)=="ideal", "ideal type");
  check(string(rid)==id_s, "ideal value");
  def rmo=read(in);
  check(typeof(rmo)=="module", "module type");
  check(string(rmo)==mo_s, "module value");
  def rma=read(in);
  check(typeof(rma)=="matrix", "matrix type");
  check(string(rma)==ma_s, "matrix value");
  def rsma=read(in);
  check(typeof(rsma)=="smatrix", "smatrix type");
  check(string(matrix(rsma))==sma_s, "smatrix value");
  def riv=read(in);
  check(typeof(riv)=="intvec", "intvec type");
  check(string(riv)==iv_s, "intvec value");
  def rim=read(in);
  check(typeof(rim)=="intmat", "intmat type");
  check(string(rim)==im_s, "intmat value");
  def rbm=read(in);
  check(typeof(rbm)=="bigintmat", "bigintmat type");
  check(string(rbm)==bm_s, "bigintmat value");
  def rbv=read(in);
  check(typeof(rbv)=="bigintvec", "bigintvec type");
  check(string(rbv)==bv_s, "bigintvec value");
  def rproc=read(in);
  check(typeof(rproc)=="proc", "proc type");
  check(rproc(4)==ssi2_types_proc(4), "proc value");
  def rli=read(in);
  check(typeof(rli)=="list", "list type");
  check(string(rli)==li_s, "list value");
  close(in);
  kill in;
}

ssi2_types_roundtrip("ssi2:w ssi2_types_plain.ssi2", "ssi2:r ssi2_types_plain.ssi2");
if (size(system("executable","zstd"))>0)
{
  ssi2_types_roundtrip("ssi2zstd:w ssi2_types_zstd.ssi2.zst", "ssi2zstd:r ssi2_types_zstd.ssi2.zst");
}

tst_status(1);$
