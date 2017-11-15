LIB "tst.lib";
tst_init();

LIB "methods.lib";
printlevel=0;


HashTable H = hashTable(list("a","b","c"),list("aa","bb","cc"));
H;

HashTable H = hashTable(list("a","b","c"),list("aa","bb","cc"));
selectKey(H,"b");
H*"b";

HashTable H = hashTable(list("a","b","c"),list("aa","bb","cc"));
HashTable W = hashTable(list("d","e","f"),list("aa","bb","cc"));
addHashTables(H,W);
H+W;

proc square(int i){return(i^2);};
HashTable H1 = hashTable(list(list("int")),list("square"));
Method m1 = method(H1);
installMethod(m1,"tst");
tst(2);
proc kbase2(ideal I,int n){return(kbase(I,n));}
HashTable H2 = hashTable(list(list("ideal","int")),list("kbase2"));
Method m2=method(H2);
Method m=m1+m2;
installMethod(m,"tst");
ring R=0,(x,y),dp;
ideal I = x^3,y^7;
I=std(I);
tst(I,2);
HashTable F = hashTable(list(list("int"),list("ideal","int")),list("square","kbase2"));
Method m3 = method(F);
installMethod(m3,"tst");
tst(I,2);
tst(3);

tst_status(1);$
