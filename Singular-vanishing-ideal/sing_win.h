#ifndef SING_WIN_H
#define SING_WIN_H

#ifdef ix86_Win

void heOpenWinHtmlHelp(const char* keyw, char* helppath );
void heOpenWinntHlp(const char* keyw, char* helppath );
void heOpenWinntUrl(const char* url, int local);

#endif /*ix86_Win */
#endif /* SING_WIN_H */
