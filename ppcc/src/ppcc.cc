#include "../adlib/lib.h"
#include "parser.h"

void Error(const char *message) {
  printf("ppcc: %s\n", message);
  exit(1);
}

#define LINK 1
#define COMP 2
#define CPP  4
#define DEP  8
#define ALL  (LINK | COMP | CPP)

typedef enum {
  NoArg,
  IntegralArg,
  FollowArg,
  IntegralOrFollowArg
} OptArgType;

typedef void (*OptFunc)(Str *, const char *);

struct OptSpec {
  const char *name;
  int mode;
  OptArgType argtype;
  OptFunc handler;
};

int debug = 0;
int mode = LINK;
bool depend = false;
StrArr *CPPArgs, *CompArgs, *LinkArgs, *DepArgs, *ExtraArgs;
Str *Language = NULL;
Str *Compiler = NULL;
Str *Output = NULL;
StrArr *CInput = NULL;
StrArr *CCInput = NULL;
StrArr *ObjInput = NULL;
StrArr *AsmInput = NULL;

void AddArg(int mode, Str *arg) {
  if (mode & CPP) CPPArgs->add(arg);
  if (mode & COMP) CompArgs->add(arg);
  if (mode & LINK) LinkArgs->add(arg);
}

void ModeCPP(Str *opt, const char *arg) {
  mode = CPP;
}

void ModeComp(Str *opt, const char *arg) {
  mode = COMP;
}

void ModeDepend(Str *opt, const char *arg) {
  depend = true;
}

void SelectLang(Str *opt, const char *arg) {
  Language = new Str(arg);
  AddArg(ALL, opt);
  AddArg(ALL, Language);
}

void SaveOutput(Str *opt, const char *arg) {
  Output = new Str(arg);
  AddArg(COMP|LINK, opt);
  AddArg(COMP|LINK, Output);
}


OptSpec optspecs[] = {
  { "-E", 0, NoArg, ModeCPP },
  { "-c", 0, NoArg, ModeComp },
  { "-S", 0, NoArg, ModeComp },
  { "-x", ALL, FollowArg, SelectLang },
  { "-f", ALL, IntegralArg },
  { "-O", COMP, IntegralArg },
  { "-g", COMP|LINK, IntegralArg },
  { "-W", ALL, IntegralArg },
  { "-X", ALL, IntegralArg },
  { "-w", ALL, IntegralArg },
  { "-D", CPP, IntegralArg },
  { "-U", CPP, IntegralArg },
  { "-MM", CPP, NoArg },
  { "-MD", CPP, NoArg },
  { "-MMD", CPP, NoArg },
  { "-MG", CPP, NoArg },
  { "-MP", CPP, NoArg },
  { "-MF", CPP, FollowArg },
  { "-MT", CPP, FollowArg },
  { "-MQ", CPP, FollowArg },
  { "-M", CPP, NoArg },
  { "-o", COMP|LINK, FollowArg, SaveOutput },
  { "-I", CPP, IntegralOrFollowArg },
  { "-I-", CPP, IntegralOrFollowArg },
  { "-I", CPP, IntegralOrFollowArg },
  { "-include", CPP, FollowArg },
  { "-idirafter", CPP, FollowArg },
  { "-imacros", CPP, FollowArg },
  { "-iquote", CPP, FollowArg },
  { "-isystem", CPP, FollowArg },
  { NULL }
};

int ParseOpt(Str *opt, char *arg) {
  for (OptSpec *spec = optspecs; spec->name; spec++) {
    Word len = strlen(spec->name);
    if (len <= opt->len()) {
      if (strncmp(spec->name, opt->c_str(), len) == 0) {
        OptFunc handler = spec->handler;
        switch (spec->argtype) {
          case NoArg:
          case IntegralArg:
            if (handler) {
              handler(opt, NULL);
            } else {
              AddArg(spec->mode, opt);
            }
            return 0;
          case FollowArg:
            if (!arg)
              Error("missing argument");
            if (handler) {
              handler(opt, arg);
            } else {
              AddArg(spec->mode, opt);
              AddArg(spec->mode, new Str(arg));
            }
            return 1;
          case IntegralOrFollowArg:
            if (len == opt->len()) {
              if (!arg)
                Error("missing argument");
              if (handler) {
                handler(opt, arg);
              } else {
                AddArg(spec->mode, opt);
                AddArg(spec->mode, new Str(arg));
              }
              return 1;
            } else {
              if (handler) {
                handler(opt, arg);
              } else {
                AddArg(spec->mode, opt);
              }
              return 0;
            }
        }
      }
    }
  }
  AddArg(ALL, opt);
  return 0;
}

void TestOutputExecutable() {
  if (mode == LINK) {
    Str *path = S("factory/threadsupport.cc");
    for (Int i = 0; i < 4; i++) {
      if (FileStat(path)) {
        ExtraArgs->add(path);
        return;
      }
      path = S("../")->add(path);
    }
  }
}

void ParseArgs() {
  if (ArgC <= 1) {
    Error("no arguments");
  }
  Compiler = new Str(ArgV[1]);
  for (int i = 2; i < ArgC; i++) {
    char *p = ArgV[i];
    if (p[0] == '-') {
      i += ParseOpt(new Str(p), ArgV[i+1]);
    } else {
      Str *arg = new Str(p);
      if (arg->ends_with(".c")) {
        CInput->add(arg);
      } else if (arg->ends_with(".cc") || arg->ends_with(".cpp")) {
        CCInput->add(arg);
      } else if (arg->ends_with(".o")) {
        ObjInput->add(arg);
      } else if (arg->ends_with(".s")) {
        AsmInput->add(arg);
      } else {
        AddArg(ALL, arg);
      }
    }
  }
}

Str *RunCPP() {
  Str *input;
  if (CInput->len())
    input = CInput->at(0);
  else
    input = CCInput->at(0);
  StrArr *args = A("-E", "-DPSINGULAR");
  args->add(CPPArgs)->add(input);
  if (debug) {
    printf("Preprocessor: %s %s\n",
      Compiler->c_str(),
      StrJoin(args, " ")->c_str());
  }
  Str *result = ReadProcess(Compiler, args);
  if (!result)
    Error("preprocessor failed");
  if (debug >= 3)
    Print(result);
  result = RunPreProcessor(input, result);
  return result;
}

void PassThroughCompiler() {
  if (debug)
    PrintLn(S("Pass through: ")->
      add(StrJoin(Args->clone()->add(ExtraArgs), S(" "))));
  if (ArgC <= 1) {
    Error("no arguments");
  }
  Compiler = new Str(ArgV[1]);
  StrArr *args = new StrArr(ArgC);
  for (int i = 2; i < ArgC; i++) {
    args->add(S(ArgV[i]));
  }
  args->add(ExtraArgs);
  int exitcode = System(Compiler, args);
  exit(exitcode);
}

void ExecCompiler() {
  int exitcode;
  Int inputs;
  StrArr *args = new StrArr(1);
  switch (inputs = CInput->len() + CCInput->len()) {
    case 0:
      if (mode == COMP)
        Error("no input files");;
      break;
    case 1:
      break;
    default:
      Error("too many C/C++ input files");
      break;
  }
  switch (mode) {
    case CPP:
      PassThroughCompiler();
      break;
    case COMP:
      args->add(S("-c"))->add(CompArgs);
      break;
    case LINK:
      args->add(LinkArgs);
      break;
  }
  if (inputs) {
    Str *cppOutput = RunCPP();
    const char *lang = CInput->len() == 1 ? "c" : "c++";
    args->add(S("-x"))->add(S(lang))->add(S("-"));
    if (debug) {
      printf("%s: %s %s\n",
        mode == COMP ? "Compiler" : "Linker",
        Compiler->c_str(),
        StrJoin(args, " ")->c_str());
      if (debug >= 2)
        Print(cppOutput);
      fflush(stdout);
    }
    int success = WriteProcess(Compiler, args, cppOutput);
    if (!success)
      exit(1);
  } else {
    TestOutputExecutable();
    PassThroughCompiler();
  }
}

INIT(Args, {
  GCVar(Language);
  GCVar(Output);
  GCVar(Compiler);

  GCVar(CPPArgs, new StrArr());
  GCVar(CompArgs, new StrArr());
  GCVar(LinkArgs, new StrArr());
  GCVar(DepArgs, new StrArr());
  GCVar(ExtraArgs, new StrArr());

  GCVar(CInput, new StrArr());
  GCVar(CCInput, new StrArr());
  GCVar(ObjInput, new StrArr());
  GCVar(AsmInput, new StrArr());
});

void Main() {
  char *envDebug = getenv("DEBUG_PPCC");
  if (envDebug) {
    switch (envDebug[0]) {
      case '0': debug = 0; break;
      case '1': debug = 1; break;
      case '2': debug = 2; break;
      default:
        Error("invalid DEBUG_PPCC value, must be 0, 1, or 2");
        break;
    }
  }
  if (Args->len() >= 1 && Args->at(0)->eq(S("--test"))) {
    for (Int i = 1; i < Args->len(); i++) {
      Str *filename = Args->at(i);
      PrintLn(S("=== ")->add(filename));
      Print(TestPreProcessor(filename));
    }
    return;
  }
  if (getenv("RUN_PPCC")) {
    if (debug) {
      PrintLn(S("Args: ")->add(StrJoin(Args, " ")));
    }
    ParseArgs();
    ExecCompiler();
  } else {
    PassThroughCompiler();
  }
}
