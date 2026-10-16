LIB "tst.lib";
tst_init();

// [compat-parenthesized-htable]
// Parenthesized and computed htable keys retain their spielwiese meaning.
string key="actual";
htable parenthesized=htable((key)->1);
ASSUME(0,parenthesized["actual"]==1);
htable computed=htable(string(5)->2);
ASSUME(0,computed["5"]==2);
htable nested=htable((string(2+3))->3);
ASSUME(0,nested["5"]==3);

// [compat-comment-text]
// Lambda-looking text inside each comment form stays inert.
ASSUME(0,(1 /* ) -> {} */ + 2)==3);
ASSUME(0,(1 // ) -> {}
  + 2)==3);
int hashbang=(1
#! ) -> { }
  + 2);
ASSUME(0,hashbang==3);
int midline_hashbang=(1 #! ) -> { }
  + 2);
ASSUME(0,midline_hashbang==3);

// [compat-parentheses-and-division]
ring compatibility=0,t,dp;
ASSUME(0,(8/2)==4);
ASSUME(0,(t /* comment */ /2)==t/2);
ASSUME(0,(8) - (3)==5);
ASSUME(0,(8)
  + (3)==11);

// [compat-expression-lists]
int a=1;
int b=2;
list pair=(a,b);
ASSUME(0,size(pair)==2);
ASSUME(0,pair[1]==1);
ASSUME(0,pair[2]==2);
ASSUME(0,((((((((a))))))))==1);

// [compat-large-fallback]
// Exercise scanner fallback beyond Flex's default 16 KiB buffer.
string padding="a";
for (int padding_step=1;padding_step<=15;padding_step++)
{
  padding=padding+padding;
}
execute("ASSUME(0,(1 /*"+padding+"*/ + 2)==3);");

// [compat-procedure-calls]
proc ordinary(int x,int y){return(x+y);};
ASSUME(0,ordinary(a,b)==3);
proc empty(){return(7);};
ASSUME(0,empty()==7);
ASSUME(0,empty( )==7);

// [compat-legacy-lambda]
// Legacy lambdas keep implicit returns and statement bodies.
proc single=x->{x*x};
ASSUME(0,single(6)==36);
intvec squares=apply(1..3,x->{x*x});
ASSUME(0,squares==intvec(1,4,9));
proc steps=x->{int next=x+1;next*next};
ASSUME(0,steps(5)==36);
proc text=s->{size(s)};
ASSUME(0,text("abcdef")==6);

print("spielwiese compatibility tests passed");
tst_status(1);$
