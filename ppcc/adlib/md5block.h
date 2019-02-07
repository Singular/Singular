  #define F(x, y, z) ((x & y) | ((~(x) & z)))
  #define G(x, y, z) ((x & z) | ((y & ~(z))))
  #define H(x, y, z) ((x) ^ (y) ^(z))
  #define I(x, y, z) ((y) ^ ((x) | ~(z)))
  #define ROT(x, s) ((x) << (s) | (x) >> (32-s))
  // Round 1
  #define U(a, b, c, d, k, s, t) \
    a = b + ROT(a + F(b,c,d) + buf[k] + t, s)
  U(a, b, c, d, 0, 7, 0xd76aa478);
  U(d, a, b, c, 1, 12, 0xe8c7b756);
  U(c, d, a, b, 2, 17, 0x242070db);
  U(b, c, d, a, 3, 22, 0xc1bdceee);
  U(a, b, c, d, 4, 7, 0xf57c0faf);
  U(d, a, b, c, 5, 12, 0x4787c62a);
  U(c, d, a, b, 6, 17, 0xa8304613);
  U(b, c, d, a, 7, 22, 0xfd469501);
  U(a, b, c, d, 8, 7, 0x698098d8);
  U(d, a, b, c, 9, 12, 0x8b44f7af);
  U(c, d, a, b, 10, 17, 0xffff5bb1);
  U(b, c, d, a, 11, 22, 0x895cd7be);
  U(a, b, c, d, 12, 7, 0x6b901122);
  U(d, a, b, c, 13, 12, 0xfd987193);
  U(c, d, a, b, 14, 17, 0xa679438e);
  U(b, c, d, a, 15, 22, 0x49b40821);
  #undef U
  // Round 2
  #define U(a, b, c, d, k, s, t) \
    a = b + ROT(a + G(b,c,d) + buf[k] + t, s)
  U(a, b, c, d, 1, 5, 0xf61e2562);
  U(d, a, b, c, 6, 9, 0xc040b340);
  U(c, d, a, b, 11, 14, 0x265e5a51);
  U(b, c, d, a, 0, 20, 0xe9b6c7aa);
  U(a, b, c, d, 5, 5, 0xd62f105d);
  U(d, a, b, c, 10, 9, 0x2441453);
  U(c, d, a, b, 15, 14, 0xd8a1e681);
  U(b, c, d, a, 4, 20, 0xe7d3fbc8);
  U(a, b, c, d, 9, 5, 0x21e1cde6);
  U(d, a, b, c, 14, 9, 0xc33707d6);
  U(c, d, a, b, 3, 14, 0xf4d50d87);
  U(b, c, d, a, 8, 20, 0x455a14ed);
  U(a, b, c, d, 13, 5, 0xa9e3e905);
  U(d, a, b, c, 2, 9, 0xfcefa3f8);
  U(c, d, a, b, 7, 14, 0x676f02d9);
  U(b, c, d, a, 12, 20, 0x8d2a4c8a);
  #undef U
  // Round 3
  #define U(a, b, c, d, k, s, t) \
    a = b + ROT(a + H(b,c,d) + buf[k] + t, s)
  U(a, b, c, d, 5, 4, 0xfffa3942);
  U(d, a, b, c, 8, 11, 0x8771f681);
  U(c, d, a, b, 11, 16, 0x6d9d6122);
  U(b, c, d, a, 14, 23, 0xfde5380c);
  U(a, b, c, d, 1, 4, 0xa4beea44);
  U(d, a, b, c, 4, 11, 0x4bdecfa9);
  U(c, d, a, b, 7, 16, 0xf6bb4b60);
  U(b, c, d, a, 10, 23, 0xbebfbc70);
  U(a, b, c, d, 13, 4, 0x289b7ec6);
  U(d, a, b, c, 0, 11, 0xeaa127fa);
  U(c, d, a, b, 3, 16, 0xd4ef3085);
  U(b, c, d, a, 6, 23, 0x4881d05);
  U(a, b, c, d, 9, 4, 0xd9d4d039);
  U(d, a, b, c, 12, 11, 0xe6db99e5);
  U(c, d, a, b, 15, 16, 0x1fa27cf8);
  U(b, c, d, a, 2, 23, 0xc4ac5665);
  #undef U
  // Round 4
  #define U(a, b, c, d, k, s, t) \
    a = b + ROT(a + I(b,c,d) + buf[k] + t, s)
  U(a, b, c, d, 0, 6, 0xf4292244);
  U(d, a, b, c, 7, 10, 0x432aff97);
  U(c, d, a, b, 14, 15, 0xab9423a7);
  U(b, c, d, a, 5, 21, 0xfc93a039);
  U(a, b, c, d, 12, 6, 0x655b59c3);
  U(d, a, b, c, 3, 10, 0x8f0ccc92);
  U(c, d, a, b, 10, 15, 0xffeff47d);
  U(b, c, d, a, 1, 21, 0x85845dd1);
  U(a, b, c, d, 8, 6, 0x6fa87e4f);
  U(d, a, b, c, 15, 10, 0xfe2ce6e0);
  U(c, d, a, b, 6, 15, 0xa3014314);
  U(b, c, d, a, 13, 21, 0x4e0811a1);
  U(a, b, c, d, 4, 6, 0xf7537e82);
  U(d, a, b, c, 11, 10, 0xbd3af235);
  U(c, d, a, b, 2, 15, 0x2ad7d2bb);
  U(b, c, d, a, 9, 21, 0xeb86d391);
  #undef U
  #undef F
  #undef G
  #undef H
  #undef I
  #undef ROT
