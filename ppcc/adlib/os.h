#pragma once

Str *ReadFile(FILE *fp);
Str *ReadFile(const char *filename);
Str *ReadFile(Str *filename);
StrArr *ReadLines(const char *filename);
StrArr *ReadLines(Str *filename);
Str *ReadProcess(Str *prog, StrArr *args);
StrArr *ReadProcessLines(Str *prog, StrArr *args);
int WriteFile(FILE *fp, Str *data);
int WriteFile(Str *filename, Str *data);
int WriteFile(const char *filename, Str *data);
int WriteProcess(Str *prog, StrArr *args, Str *data);
int System(Str *prog, StrArr *args);

void Print(Str *str);
void Print(const char *str);
void Print(Int i);
void PrintErr(Str *str);
void PrintErr(const char *str);
void PrintErr(Int i);
void PrintLn(Str *str);
void PrintLn(const char *str);
void PrintLn(Int i);
void PrintErrLn(Str *str);
void PrintErrLn(const char *str);
void PrintErrLn(Int i);

enum ListFilesMode {
  ListFilesAndDirs = 1,
  ListFilesRelative = 2,
};

Str *Pwd();
bool ChDir(Str *path);
bool ChDir(const char *path);
StrArr *ListFiles(const char *path);
StrArr *ListFiles(Str *path);
StrArr *ListFiles(const char *path);
StrArr *ListFileTree(Str *path, int mode = 0);
StrArr *ListFileTree(const char *path, int mode = 0);

struct FileInfo : PtrFreeGC {
  bool is_dir;
  bool is_file;
  bool is_link;
  bool is_other;
  double atime; // currently no nano-second resolution
  double mtime;
  double ctime;
#ifdef HAVE_OFF_T
  Offset size;
#else
  Int size;
#endif
};

static const char *PathSeparator = "/";

bool FileStat(FileInfo &info, const char *path, bool follow_links = false);
bool FileStat(FileInfo &info, Str *path, bool follow_links = false);
FileInfo *FileStat(const char *path, bool follow_links = false);
FileInfo *FileStat(Str *path, bool follow_links = false);
Str *DirName(Str *path);
Str *BaseName(Str *path);
Str *FileExtension(Str *path);
bool MakeDir(Str *path, bool recursive = false);
bool MakeDir(const char *path, bool recursive = false);
bool RemoveDir(Str *path);
bool RemoveDir(const char *path);
bool RemoveFile(Str *path);
bool RemoveFile(const char *path);
bool Rename(Str *path, Str *path2);
bool Rename(const char *path, const char *path2);
