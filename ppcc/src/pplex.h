#pragma once

enum Symbol {
  SymNone,
  SymClass,
  SymNamespace,
  SymExtern,
  SymVAR,
  SymEXTERN_VAR,
  SymSTATIC_VAR,
  SymINST_VAR,
  SymEXTERN_INST_VAR,
  SymSTATIC_INST_VAR,
  SymIdent,
  SymLiteral,
  SymOp,
  SymSemicolon,
  SymComma,
  SymEqual,
  SymAst,
  SymAnd,
  SymAndAnd,
  SymColonColon,
  SymLPar,
  SymRPar,
  SymLBrkt,
  SymRBrkt,
  SymLBrace,
  SymRBrace,
  SymWS,
  SymEOL,
  SymComment,
  SymBAD,
  SymLineDir,
  SymEOF,
  SymGen,
};

extern const char *SymbolNames[];

#define BIT(n) (((Word64) 1) << (n))
// from 0..n inclusive
#define ALLBITS(n) (BIT(n+1)-1)
// inclusive range
#define BITRANGE(m, n) (ALLBITS(n) & ~ALLBITS(m-1))
#define TEST(set, n) (((set) & BIT(n)) != (Word64) 0)

static const Word64 SymsWS =
  BIT(SymWS) | BIT(SymEOL) | BIT(SymComment);
static const Word64 SymsKeyword =
  BITRANGE(SymNone+1, SymIdent-1);
static const Word64 SymsPunct =
  BIT(SymSemicolon) | BIT(SymLPar) | BIT(SymRPar) |
  BIT(SymLBrkt) | BIT(SymRBrkt) | BIT(SymLBrace) | BIT(SymRBrace);
static const Word64 SymsSpecial =
  BIT(SymVAR) | BIT(SymEXTERN_VAR) | BIT(SymSTATIC_VAR) |
  BIT(SymINST_VAR) | BIT(SymEXTERN_INST_VAR) | BIT(SymSTATIC_INST_VAR) |
  BIT(SymNamespace) | BIT(SymExtern) | BIT(SymLBrace) | BIT(SymRBrace);
static const Word64 SymsEndDecl =
  BIT(SymEqual) | BIT(SymComma) | BIT(SymSemicolon);
static const Word64 SymsTypePrefix =
  BIT(SymIdent) | BIT(SymAst) | BIT(SymAnd) | BIT(SymAndAnd);

struct Token : public GC {
  Token() {
    sym = SymNone;
  }
  Token(Symbol _sym, Str *_str) {
    sym = _sym;
    str = _str;
  }
  Symbol sym;
  Str *str;
};

typedef Arr<Token> TokenList;

struct FuncSpec;

struct SourceFile : public GC {
  Str *filename;
  Str *modulename;
  Str *filedata;
  TokenList *tokens;
};

typedef Arr<SourceFile *> SourceList;

Str *Intern(const char *str, Int len);
SourceFile *ReadSource(Str *filename, Str *filedata = NULL);
bool Tokenize(SourceFile *source);
