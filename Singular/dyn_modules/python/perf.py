from Singular import number
n=number(1);
s=number(0)
for i in range(1000000):
	s=s+n
print(s)
