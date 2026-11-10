#include <errno.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *singular_path(void)
{
  uint32_t size = 0;
  if (_NSGetExecutablePath(NULL, &size) != -1 || size == 0)
    return NULL;

  char *launcher = malloc(size);
  if (launcher == NULL || _NSGetExecutablePath(launcher, &size) != 0)
  {
    free(launcher);
    return NULL;
  }

  char *resolved = realpath(launcher, NULL);
  free(launcher);
  if (resolved == NULL)
    return NULL;

  char *separator = strrchr(resolved, '/');
  if (separator == NULL)
  {
    free(resolved);
    return NULL;
  }
  *separator = '\0';

  const char suffix[] = "/../bin/Singular";
  size_t length = strlen(resolved) + sizeof(suffix);
  char *singular = malloc(length);
  if (singular != NULL)
    snprintf(singular, length, "%s%s", resolved, suffix);
  free(resolved);
  return singular;
}

int main(int argc, char **argv)
{
  (void)argc;
  char *singular = singular_path();
  if (singular == NULL || access(singular, X_OK) != 0)
  {
    fprintf(stderr, "Cannot locate the Singular executable: %s\n",
            singular == NULL ? strerror(errno) : singular);
    free(singular);
    return 1;
  }

  if (isatty(STDIN_FILENO) || isatty(STDOUT_FILENO))
  {
    argv[0] = singular;
    execv(singular, argv);
    perror("Cannot start Singular");
    free(singular);
    return 1;
  }

  execl("/usr/bin/open", "open", "-a", "Terminal", singular, (char *)NULL);
  perror("Cannot open Terminal");
  free(singular);
  return 1;
}
