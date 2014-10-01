#ifndef SING_WIN_H
#define SING_WIN_H

#ifdef __CYGWIN__

void heOpenWinHtmlHelp(const char* keyw, char* helppath );
void heOpenWinntHlp(const char* keyw, char* helppath );
void heOpenWinntUrl(const char* url, int local);

#endif /*__CYGWIN__ */
#endif /* SING_WIN_H */
