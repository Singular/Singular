// #define DEBUG
// #define DEBUGALL

// B19 - egcs automatically defines CYGWIN32 but not CYGWIN
// B20 - egcs automatically defines both CYGWIN32 and CYGWIN
// Bfuture - (???) defines CYGWIN but not CYGWIN32
#if defined(__CYGWIN32__)
#if !defined(__CYGWIN__)
 #define B19
 #define __CYGWIN__
#else
 #define B20
#endif
#else
#if defined(__CYGWIN__)
 #define B21
#endif
#endif
// Now: use __CYGWIN__ to represent any version
// distinguish using B19, B20, or B21

#if defined(__CYGWIN__)
#ifdef B19
#define CYGWIN_ATTACH_HANDLE_TO_FD(a)      cygwin32_attach_handle_to_fd      a
#define CYGWIN_CONV_TO_FULL_POSIX_PATH(a)  cygwin32_conv_to_full_posix_path  a
#define CYGWIN_CONV_TO_FULL_WIN32_PATH(a)  cygwin32_conv_to_full_win32_path  a
#define CYGWIN_CONV_TO_POSIX_PATH(a)       cygwin32_conv_to_posix_path       a
#define CYGWIN_CONV_TO_WIN32_PATH(a)       cygwin32_conv_to_win32_path       a
#define CYGWIN_DETACH_DLL(a)               cygwin32_detach_dll               a
#define CYGWIN_GETSHARED(a)                cygwin32_getshared                a
#define CYGWIN_INTERNAL(a)                 cygwin32_internal                 a
#define CYGWIN_POSIX_PATH_LIST_P(a)        cygwin32_posix_path_list_p        a
#define CYGWIN_POSIX_TO_WIN32_PATH_LIST(a) cygwin32_posix_to_win32_path_list a
#define CYGWIN_POSIX_TO_WIN32_PATH_LIST_BUF_SIZE(a) cygwin32_posix_to_win32_path_list_buf_size a
#define CYGWIN_SPLIT_PATH(a)               cygwin32_split_path               a
#define CYGWIN_WIN32_TO_POSIX_PATH_LIST(a) cygwin32_win32_to_posix_path_list a
#define CYGWIN_WIN32_TO_POSIX_PATH_LIST_BUF_SIZE(a) cygwin32_win32_to_posix_path_list_buf_size a
#define CYGWIN_WINPID_TO_PID(a)            cygwin32_winpid_to_pid            a
#else
#define CYGWIN_ATTACH_HANDLE_TO_FD(a)      cygwin_attach_handle_to_fd      a
#define CYGWIN_CONV_TO_FULL_POSIX_PATH(a)  cygwin_conv_to_full_posix_path  a
#define CYGWIN_CONV_TO_FULL_WIN32_PATH(a)  cygwin_conv_to_full_win32_path  a
#define CYGWIN_CONV_TO_POSIX_PATH(a)       cygwin_conv_to_posix_path       a
#define CYGWIN_CONV_TO_WIN32_PATH(a)       cygwin_conv_to_win32_path       a
#define CYGWIN_DETACH_DLL(a)               cygwin_detach_dll               a
#define CYGWIN_GETSHARED(a)                cygwin_getshared                a
#define CYGWIN_INTERNAL(a)                 cygwin_internal                 a
#define CYGWIN_POSIX_PATH_LIST_P(a)        cygwin_posix_path_list_p        a
#define CYGWIN_POSIX_TO_WIN32_PATH_LIST(a) cygwin_posix_to_win32_path_list a
#define CYGWIN_POSIX_TO_WIN32_PATH_LIST_BUF_SIZE(a) cygwin_posix_to_win32_path_list_buf_size a
#define CYGWIN_SPLIT_PATH(a)               cygwin_split_path               a
#define CYGWIN_WIN32_TO_POSIX_PATH_LIST(a) cygwin_win32_to_posix_path_list a
#define CYGWIN_WIN32_TO_POSIX_PATH_LIST_BUF_SIZE(a) cygwin_win32_to_posix_path_list_buf_size a
#define CYGWIN_WINPID_TO_PID(a)            cygwin_winpid_to_pid            a
#endif
#endif

#if defined(__CYGWIN__)
 #define PATH_SEP_CHAR_STR "/"
 #define SEP_CHARS ":"
#else
 #define PATH_SEP_CHAR_STR "\\"
 #define SEP_CHARS ";"
#endif

#ifndef RC_INVOKED

#define MAX_ARGS 20

#ifdef DEBUG
 #define Trace(x)   Trace_ x
#else
 #define Trace(x)
#endif

#define NUM_EXTENSIONS 2
const char* exts[NUM_EXTENSIONS] = { "", ".exe" };

char* pfopen(char *retval, const char *name, const char *dirs);
void error(char* fmt, ...);
void message(char* fmt, ...);
void Trace_(char* fmt, ...);
int get_exec_name_and_path(char* execname, char* execpath);
char* my_strtok(char* s, const char* delim, char** lasts);
int parse_cmdline_to_arg_array(char* argv[MAX_ARGS], char* cmdline);
void strip_exe(char* s);
int start_child(char* cmdline, int wait_for_child);
void xemacs_special(char* exec);
int build_cmdline(char* new_cmdline, char* exec, int argc, char* argv[]);
void process_execname(char *exec, const char* execname, const char* execpath);
int fileExists(char* fullname, const char* path, const char* name);
int endsWith(const char* s1, const char* s2);
int fileExistsMulti(char* fullname, const char* path,
                    const char* name_noext, const char* exts[],
                    const int extcnt);

#endif /* RC_INVOKED */



