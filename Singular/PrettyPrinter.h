#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#if defined(HAVE_MINOR) || defined(HAVE_WRAPPERS)

#include <fstream>
#include <string>

class PrettyPrinter
{
private:
  int m_indents;
  char* m_baseIndent;
  char* m_indent;
  char* m_fileName1;    // file name for outputs to primary output file
  char* m_fileName2;    // file name for outputs to secondary output file
  int m_console;        // == -1: no output to console
                        // == 0:  just output to console, i.e. no file output
                        // == 1:  identical outputs to file1 and console;
                        // == 2:  identical outputs to file2 and console;
                        // (file1 contains at least as much output as file2)
  std::ofstream m_file1;
  std::ofstream m_file2;
public:
  PrettyPrinter (const char* fileName1, const char* fileName2,   // choose filename "" for no output file
                 const bool append1, const bool append2,
                 const int console, const char* baseIndent);
  ~PrettyPrinter ();
  PrettyPrinter& operator+ ();
  PrettyPrinter& operator++ ();
  PrettyPrinter& operator> (const char* s);
  PrettyPrinter& operator>> (const char* s);
  PrettyPrinter& operator> (const int i);
  PrettyPrinter& operator>> (const int i);
  PrettyPrinter& operator> (const long l);
  PrettyPrinter& operator>> (const long l);
  PrettyPrinter& operator> (const unsigned long ul);
  PrettyPrinter& operator>> (const unsigned long ul);
  PrettyPrinter& operator> (const std::string s);
  PrettyPrinter& operator>> (const std::string s);
  PrettyPrinter& operator< (const char* s);
  PrettyPrinter& operator<< (const char* s);
  PrettyPrinter& operator< (const int i);
  PrettyPrinter& operator<< (const int i);
  PrettyPrinter& operator< (const long l);
  PrettyPrinter& operator<< (const long l);
  PrettyPrinter& operator< (const unsigned long ul);
  PrettyPrinter& operator<< (const unsigned long ul);
  PrettyPrinter& operator< (const std::string s);
  PrettyPrinter& operator<< (const std::string s);
  PrettyPrinter& operator+ (const int i);
  PrettyPrinter& operator- (const int i);
  void setBaseIndent (const char* baseIndent);
  char* getBaseIndent () const;
  void setConsole (const int console);
  int getConsole () const;
};

#endif // defined(HAVE_MINOR) || defined(HAVE_WRAPPERS)

#endif
/* PRETTY_PRINTER_H */