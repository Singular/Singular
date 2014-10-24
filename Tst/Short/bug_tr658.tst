LIB "tst.lib";
tst_init();

newstruct("Tnewstruct", "proc uninitializedProc ");

Tnewstruct st;
st;
st;
st;
st; // should not crash ! 
st;
st;
st;
st;
st;
st;

st.uninitializedProc = groebner;
def st2 = st;
st2; // should not crash ! 


newstruct("TSaltedStruct", "proc uninitializedProc, int salt");

TSaltedStruct st3;

st3; //  ok
st3; //  ok
st3; //  ok

st3; //  should not crash ! 


tst_status(1);$
