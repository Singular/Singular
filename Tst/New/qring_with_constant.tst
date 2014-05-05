LIB "tst.lib";
tst_init();

ring rng = (integer),(x,y,z),dp;

ideal iq = 14,x2-2;
iq = std(iq);

ideal bla= 7y-28z,yz;

qring rngQ = iq;

def bla1=imap(rng,bla);
bla1;

def bla2=fetch(rng,bla);
bla2;

map mapi=rng,x,y,z;
def bla3=mapi(bla);
bla3;


ring rng1 = (integer,100),(x,y,z),dp;

ideal iq = 20,x2-2;
iq = std(iq);

ideal bla= 7y-28z,yz;

qring rngQ = iq;

def bla1=imap(rng1,bla);
bla1;

def bla2=fetch(rng1,bla);
bla2;

map mapi=rng1,x,y,z;
def bla3=mapi(bla);
bla3;


ring rng2 = (integer,2,5),(x,y,z),dp;

ideal iq = 28,x2-2;
iq = std(iq);

ideal bla= 7y-28z,yz;

qring rngQ = iq;

def bla1=imap(rng2,bla);
bla1;

def bla2=fetch(rng2,bla);
bla2;

map mapi=rng2,x,y,z;
def bla3=mapi(bla);
bla3;

ring rng3 = (integer,7,5),(x,y,z),dp;

ideal iq = 49,x2-2;
iq = std(iq);

ideal bla= 7y-96z,yz;

qring rngQ = iq;

def bla1=imap(rng3,bla);
bla1;

def bla2=fetch(rng3,bla);
bla2;

map mapi=rng3,x,y,z;
def bla3=mapi(bla);
bla3;

tst_status(1);
$
