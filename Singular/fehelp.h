#ifndef FEHELP_H
#define FEHELP_H


/*!
!
****************************************************************
 *
 * help system (fehelp.cc)
 *
 ****************************************************************

*/
// if str != NULL display help for str
// display general help, otherwise
void feHelp(char* str = NULL);
// if browser != NULL or feOpt("browser") != NULL
//    set HelpBrowser to browser
// otherwise, if browser was already set, leave as is,
//            if not, choose first available browser
// return string identifying current browser
// keeps feOpt("browser") up-to-date
// Optional warn argument is as in feResource
const char* feHelpBrowser(char* browser = NULL, int warn = -1);

void feStringAppendBrowsers(int warn = -1);

#endif /*!
!
  FEHELP_H 

*/
