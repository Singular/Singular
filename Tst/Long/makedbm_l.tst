LIB "tst.lib";
LIB "makedbm.lib";

tst_init();
tst_ignore("CVS: $Id: makedbm_l.tst,v 1.2 1998-07-01 10:10:50 obachman Exp $");

// ===============================================
int i;
create_sing_dbm();
read_sing_dbm();
"//////////////////////////////////////////////////////////////////////";

// ===============================================
link l="DBM: NFlist";
dbm_read(l);
kill l;
i=system("sh", "/bin/rm -f NFlist.dir NFlist.pag");
tst_status(1);$
