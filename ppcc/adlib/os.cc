#include "lib.h"

#include <sys/stat.h>
#include <limits.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

Str *ReadFile(FILE *fp) {
  char buffer[65536];
  if (fp == NULL)
    return NULL;
  Str *result = new Str(1024);
  for (;;) {
    size_t nbytes = fread(buffer, 1, sizeof(buffer), fp);
    if (nbytes == 0)
      break;
    result->add(buffer, nbytes);
  }
  return result;
}

Str *ReadFile(const char *filename) {
  FILE *fp = fopen(filename, "r");
  Str *result = ReadFile(fp);
  fclose(fp);
  return result;
}

Str *ReadFile(Str *filename) {
  return ReadFile(filename->c_str());
}

int WriteFile(FILE *fp, Str *data) {
  char *p = data->c_str();
  Int len = data->len();
  while (len != 0) {
    Int written = fwrite(p, 1, len, fp);
    if (written == 0)
      return 0;
    len -= written;
    p += written;
  }
  return 1;
}

int WriteFile(const char *filename, Str *data) {
  FILE *fp = fopen(filename, "w");
  if (!fp)
    return 0;
  int success = WriteFile(fp, data);
  fclose(fp);
  return success;
}

int WriteFile(Str *filename, Str *data) {
  return WriteFile(filename->c_str(), data);
}

StrArr *ReadLines(const char *filename) {
  Str *contents = ReadFile(filename);
  return contents->splitLines();
}

StrArr *ReadLines(Str *filename) {
  return ReadLines(filename->c_str());
}

static char safe_chars[256];

void InitSafeChars() {
  for (int i = 'a'; i <= 'z'; i++)
    safe_chars[i] = 1;
  for (int i = 'A'; i <= 'Z'; i++)
    safe_chars[i] = 1;
  for (int i = '0'; i <= '9'; i++)
    safe_chars[i] = 1;
  const char *p = ",._+:@%/-";
  while (*p)
    safe_chars[(unsigned char) *p++] = 1;
}

INIT(_AdLibOS, InitSafeChars(););

Str *ShellEscape(Str *arg) {
  int safe = 1;
  for (Int i = 0; i < arg->len(); i++) {
    if (!safe_chars[arg->byte(i)]) {
      safe = 0;
      break;
    }
  }
  if (safe)
    return arg;
  Str *result = new Str(arg->len());
  result->add('\'');
  for (Str::Each it(arg); it; ++it) {
    if (*it == '\'')
      result->add("'\\''");
    else
      result->add(*it);
  }
  result->add('\'');
  return result;
}

Str *BuildCommand(Str *prog, StrArr *args) {
  Str *command = new Str(1024);
  command->add(ShellEscape(prog));
  for (Int i = 0; i < args->len(); i++) {
    command->add(' ')->add(ShellEscape(args->at(i)));
  }
  return command;
}

Str *ReadProcess(Str *prog, StrArr *args) {
  Str *command = BuildCommand(prog, args);
  FILE *pipe = popen(command->c_str(), "r");
  if (!pipe)
    return NULL;
  Str *result = ReadFile(pipe);
  if (pclose(pipe))
    return NULL;
  return result;
}

StrArr *ReadProcessLines(Str *prog, StrArr *args) {
  Str *output = ReadProcess(prog, args);
  return output->splitLines();
}

int WriteProcess(Str *prog, StrArr *args, Str *data) {
  Str *command = BuildCommand(prog, args);
  FILE *pipe = popen(command->c_str(), "w");
  if (!pipe)
    return 0;
  int success = WriteFile(pipe, data);
  if (pclose(pipe))
    return 0;
  return success;
}

int System(Str *prog, StrArr *args) {
  Str *command = BuildCommand(prog, args);
  int result = system(command->c_str());
  if (result >= 256)
    result >>= 8;
  return result;
}

void Print(Str *str) {
  printf("%s", str->c_str());
}

void PrintLn(Str *str) {
  printf("%s\n", str->c_str());
}

void PrintErr(Str *str) {
  fprintf(stderr, "%s", str->c_str());
}

void PrintErrLn(Str *str) {
  fprintf(stderr, "%s\n", str->c_str());
}

void Print(const char *str) {
  printf("%s", str);
}

void PrintLn(const char *str) {
  printf("%s\n", str);
}

void PrintErr(const char *str) {
  fprintf(stderr, "%s", str);
}

void PrintErrLn(const char *str) {
  fprintf(stderr, "%s\n", str);
}

void Print(Int i) {
  printf("%" WORD_FMT "d", i);
}

void PrintLn(Int i) {
  printf("%" WORD_FMT "d\n", i);
}

void PrintErr(Int i) {
  fprintf(stderr, "%" WORD_FMT "d", i);
}

void PrintErrLn(Int i) {
  fprintf(stderr, "%" WORD_FMT "d\n", i);
}

Str *Pwd() {
#ifdef PATH_MAX
  char *path = getcwd(NULL, PATH_MAX);
#else
  char *path = getcwd(NULL, 8192);
#endif
  Str *result = new Str(path);
  free(path);
  return result;
}

bool ChDir(const char *path) {
  return !chdir(path);
}

bool ChDir(Str *path) {
  return ChDir(path->c_str());
}

bool FileStat(FileInfo &info, const char *path, bool follow_links) {
  struct stat st;
  if (follow_links) {
    if (stat(path, &st) < 0)
      return false;
  } else {
    if (lstat(path, &st) < 0)
      return false;
  }
  memset(&info, 0, sizeof(info));
  if (S_ISDIR(st.st_mode)) {
    info.is_dir = true;
  } else if (S_ISREG(st.st_mode)) {
    info.is_file = true;
  } else if (S_ISLNK(st.st_mode)) {
    info.is_link = true;
  } else {
    info.is_other = true;
  }
  info.atime = st.st_atime;
  info.mtime = st.st_mtime;
  info.ctime = st.st_ctime;
  info.size = st.st_size;
  return true;
}

bool FileStat(FileInfo &info, Str *path, bool follow_links) {
  return FileStat(info, path->c_str(), follow_links);
}

FileInfo *FileStat(const char *path, bool follow_links) {
  FileInfo info;
  if (FileStat(info, path, follow_links))
    return new FileInfo(info);
  else
    return NULL;
}

FileInfo *FileStat(Str *path, bool follow_links) {
  return FileStat(path->c_str(), follow_links);
}

StrArr *ListFiles(const char *path) {
#ifdef HAVE_DIRENT_H
  StrArr *result = new StrArr();
  DIR *dir = opendir(path);
  if (!dir)
    return NULL;
  for (;;) {
    struct dirent *entry = readdir(dir);
    if (!entry)
      break;
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    result->add(new Str(entry->d_name));
  }
  closedir(dir);
  return result;
#else
  return NULL;
#endif
}

StrArr *ListFiles(Str *path) {
  return ListFiles(path->c_str());
}

static void WalkDir(StrArr *acc, const char *path, int mode) {
  StrArr *files = ListFiles(path);
  if (!files)
    return;
  FileInfo info;
  for (Int i = 0; i < files->len(); i++) {
    Str *newpath = new Str(path);
    if (!newpath->ends_with(PathSeparator))
      newpath->add(PathSeparator);
    newpath->add(files->at(i));
    if (FileStat(info, newpath)) {
      if (info.is_dir) {
        newpath->add(PathSeparator);
        WalkDir(acc, newpath->c_str(), mode);
        if (mode & ListFilesAndDirs)
          acc->add(newpath);
      } else {
        acc->add(newpath);
      }
    }
  }
}

StrArr *ListFileTree(const char *path, int mode) {
  StrArr *result = new StrArr();
  FileInfo info;
  if (FileStat(info, path, true)) {
    if (info.is_dir) {
      Str *dir = new Str(path);
      if (!dir->ends_with(PathSeparator))
        dir->add(PathSeparator);
      Int prefixlen = dir->len();
      result->add(dir);
      WalkDir(result, dir->c_str(), mode);
      if (mode & ListFilesRelative) {
        for (Int i = 0; i < result->len(); i++) {
          result->at(i)->remove(0, prefixlen);
        }
      }
    } else {
      if ((mode & ListFilesRelative) == 0)
        result->add(S(path));
    }
  }
  return result;
}

StrArr *ListFileTree(Str *path, int mode) {
  return ListFileTree(path->c_str(), mode);
}

Str *DirName(Str *path) {
  Int pos = path->rfind(PathSeparator);
  if (pos == NOWHERE) {
    return path->clone();
  } else {
    return path->range_excl(0, pos);
  }
}

Str *BaseName(Str *path) {
  Int pos = path->rfind(PathSeparator);
  if (pos == NOWHERE) {
    return path->clone();
  } else {
    return path->range_excl(pos+1, path->len());
  }
}

Str *FileExtension(Str *path) {
  Int slash = path->rfind(PathSeparator);
  Int dot = path->rfind('.');
  if (dot == NOWHERE || dot < slash) {
    return S("");
  } else {
    return path->range_excl(dot, path->len());
  }
}


static bool MakeDirRec(Str *path) {
  FileInfo info;
  path = path->clone();
  for (Int i = 1; i < path->len();) {
    Int p = path->find(PathSeparator, i);
    if (p == NOWHERE)
      break;
    path->at(p) = '\0';
    if (FileStat(info, path, true)) {
      if (!info.is_dir)
        return false;
    } else if (mkdir(path->c_str(), 0777) < 0) {
      return false;
    }
    path->at(p) = PathSeparator[0];
    i = p+1;
  }
  if (FileStat(info, path, true)) {
    if (!info.is_dir)
      return false;
  } else if (mkdir(path->c_str(), 0777) < 0) {
    return false;
  }
  return true;
}

bool MakeDir(const char *path, bool recursive) {
  if (!recursive)
    return mkdir(path, 0777) >= 0;
  return MakeDirRec(S(path));
}

bool MakeDir(Str *path, bool recursive) {
  if (!recursive)
    return mkdir(path->c_str(), 0777) >= 0;
  return MakeDirRec(path);
}

bool RemoveDir(const char *path) {
  return rmdir(path) >= 0;
}

bool RemoveDir(Str *path) {
  return RemoveDir(path->c_str());
}

bool RemoveFile(const char *path) {
  return unlink(path) >= 0;
}

bool RemoveFile(Str *path) {
  return RemoveFile(path->c_str());
}


bool Rename(const char *path, const char *path2) {
  return rename(path, path2) >= 0;
}

bool Rename(Str *path, Str *path2) {
  return Rename(path->c_str(), path2->c_str());
}
