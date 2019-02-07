#pragma once

struct MD5Digest {
  Word32 a, b, c, d;
};

class MD5 : public PtrFreeGC {
private:
  MD5Digest state;
  unsigned char fragment[64];
  Word fragment_length;
  Word total_length;
  bool finished;
public:
  MD5Digest digest();
  Str *bytedigest();
  Str *hexdigest();
  static MD5Digest digest(Str *str);
  static Str *bytedigest(Str *str);
  static Str *hexdigest(Str *str);
  void reset() {
    state.a = 0x67452301;
    state.b = 0xefcdab89;
    state.c = 0x98badcfe;
    state.d = 0x10325476;
    fragment_length = 0;
    total_length = 0;
    finished = false;
  }
  void update(const void *data, Int len);
  void update(Str *str) {
    update(str->c_str(), str->len());
  }
  void update_block(const unsigned char *data);
  void finish();
  MD5() {
    reset();
  }
};

