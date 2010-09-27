LIB "tst.lib";

tst_init();
tst_ignore("CVS: $Id$");

"==============================================";
" DBM link tests ";

proc read_dbm_all (link l)
{
  string s="";
  s=read(l);
  while( s != "" ) 
  {
    s,"=",read(l,s);
    s=read(l);
  }
}
proc read_dbm (link l)
{
  string s="";
  s=read(l);
  if( s != "" ) { s,"=",read(l,s); }
}

link l1="DBM:rw db1";
link l2="DBM:rw db2";
l1;

int i;

for(i=1;i<=5; i++)
{
  write(l1,"Key"+string(i), "Value"+string(i));
  write(l2,"Key"+string(i), "DB 2 : Value"+string(i));
}
read_dbm(l1);
read_dbm(l1);
read_dbm(l2);
read_dbm(l1);
"**********************************************";
read_dbm_all(l2);
"**********************************************";
read_dbm_all(l1);
"**********************************************";
write(l1,"Key4", "NewValue4");
read(l1,"Key4");
write(l1,"Key2");
read(l1,"Key2");
l2="DBM:r db2";
write(l2,"Key4"); // should fail
l1="DBM: db1";
write(l1,"Key4"); // should not fail
close(l1);
close(l2);
kill l1,l2;
"**********************************************";
string s="12345678901234567890123456789012345678901234567890";
string t500=s+s+s+s+s+s+s+s+s+s;
string t1k=t500+t500;
link l="DBM:rw test";
write(l,"a2",s);
read(l,"a2");
write(l,"a1",t1k);
read(l,"a2");
read(l,"a1");
i=system("sh", "/bin/rm -f db1.dir db1.pag db2.dir db2.pag");
i=system("sh", "/bin/rm -f test.dir test.pag");

tst_status(1);$
