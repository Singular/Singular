LIB "tst.lib";
tst_init();

ring R = 0,(x,y,z),dp;
ideal I = x^2+2*y+1/3,x*y-z^3,5*x^3*y+7/11;
string s = string(I);

link l = "ssi2:w ssi2_options_plain.ssi2";
write(l,I);
close(l);
kill l;
link l = "ssi2:r ssi2_options_plain.ssi2";
def J = read(l);
close(l);
kill l;
s == string(J);
kill J;

link l = "ssi2:w ssi2_options_gzip.ssi2.gz";
write(l,I);
close(l);
kill l;
link l = "ssi2:r ssi2_options_gzip.ssi2.gz";
def G = read(l);
close(l);
kill l;
s == string(G);
kill G;

echo=0;
if (size(system("executable","zstd"))>0)
{
  link l = "ssi2c:w ssi2_options_c.ssi2c";
  write(l,I);
  close(l);
  kill l;
  link l = "ssi2c:r ssi2_options_c.ssi2c";
  def C = read(l);
  close(l);
  kill l;
  if (s!=string(C)) { ERROR("ssi2c value"); }
  kill C;

  link l = "ssi2:w,zstd,long=23 ssi2_options_explicit_zstd";
  write(l,I);
  close(l);
  def CZ = read(l);
  close(l);
  if (s!=string(CZ)) { ERROR("explicit zstd value"); }
  kill CZ;
  kill l;
}
echo=1;

link l = "ssi2:w,plain ssi2_options_plain_suffix.ssi2.gz";
write(l,I);
close(l);
kill l;
link l = "ssi2:r,plain ssi2_options_plain_suffix.ssi2.gz";
def P = read(l);
close(l);
kill l;
s == string(P);
kill P;

link l = "ssi2:w,plain ssi2_options_reopen_plain.ssi2.zst";
write(l,I);
close(l);
def RP = read(l);
close(l);
s == string(RP);
kill RP;
kill l;

link l = "ssi2:w ssi2_options_append.ssi2";
write(l,I);
close(l);
kill l;
link l = "ssi2:a ssi2_options_append.ssi2";
write(l,I);
close(l);
kill l;
link l = "ssi2:r ssi2_options_append.ssi2";
def A1 = read(l);
def A2 = read(l);
close(l);
kill l;
s == string(A1);
s == string(A2);
kill A1;
kill A2;

link l = "ssi:w ssi2_options_legacy_named.ssi2";
write(l,I);
close(l);
kill l;
string legacy_stream=read("ssi2_options_legacy_named.ssi2");
string(legacy_stream[1..5]) == "98 16";
int legacy_line_end=find(legacy_stream,newline);
string(legacy_stream[legacy_line_end+1..legacy_line_end+2]) == "15";
link l = "ssi:r ssi2_options_legacy_named.ssi2";
def O = read(l);
close(l);
kill l;
s == string(O);
kill O;

if (system("sh",
  "rm -f ssi2_options_explicit_zstd ssi2_options_reopen_plain.ssi2.zst ssi2_options_append.ssi2") != 0)
{
  ERROR("remove reopen and append test files");
}

tst_status(1);$
