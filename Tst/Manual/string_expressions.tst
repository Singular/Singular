LIB "tst.lib"; tst_init();
// string_expression[start, length] : a substring
// (possibly filled up with blanks)
// the substring of s starting at position 2
// with a length of 4
string s="123456";
s[2,4];
"abcd"[2,2];
// string_expression[position] : a character from a string
s[3];
// string_expression[position..position] :
// a substring starting at the first position up to the second
// given position
s[2..4];
// a function returning a string
typeof(s);
tst_status(1);$
