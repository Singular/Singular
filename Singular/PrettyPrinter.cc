#include "mod2.h"

#if defined(HAVE_MINOR) || defined(HAVE_WRAPPERS)

#include <string>
#include <fstream>
#include "febase.h"
#include "PrettyPrinter.h"

PrettyPrinter::PrettyPrinter (const char* fileName1, const char* fileName2,   // choose filename "" for no output file
                              const bool append1, const bool append2,
                              const int console, const char* baseIndent)
{
  m_baseIndent = new char[200];
  m_fileName1 = new char[200];
  m_fileName2 = new char[200];
  m_indent = new char[10];

  strcpy(m_baseIndent, baseIndent);
  strcpy(m_fileName1, fileName1);
  strcpy(m_fileName2, fileName2);
  strcpy(m_indent, "");
  if (strcmp(m_fileName1, "") != 0)
  {
    if (append1) m_file1.open(m_fileName1, std::fstream::app);
    else         m_file1.open(m_fileName1);
  }
  if (strcmp(m_fileName2, "") != 0)
  { 
    if (append2) m_file2.open(m_fileName2, std::fstream::app);
    else         m_file2.open(m_fileName2);
  }
  m_console = console;
  m_indents = 0;
}

PrettyPrinter::~PrettyPrinter ()
{
  if (strcmp(m_fileName1, "") != 0) m_file1.close();
  if (strcmp(m_fileName2, "") != 0) m_file2.close();
  delete m_baseIndent;
  delete m_fileName1;
  delete m_fileName2;
  delete m_indent;
}

PrettyPrinter& PrettyPrinter::operator< (const char* s)
{
  if (strcmp(m_fileName1, "") != 0)
  {
    m_file1 << s;
  }
  if (m_console == 0 || m_console == 1)
  {
    PrintS(s);
  }
  return *this;
}

PrettyPrinter& PrettyPrinter::operator<< (const char* s)
{
  if (strcmp(m_fileName1, "") != 0)
  {
    m_file1 << s;
  }
  if (strcmp(m_fileName2, "") != 0)
  {
    m_file2 << s;
  }
  if (m_console == 0 || m_console == 1 || m_console == 2)
  {
    PrintS(s);
  }
  return *this;
}

PrettyPrinter& PrettyPrinter::operator< (const std::string s)
{
  return (*this) < s.c_str();
}

PrettyPrinter& PrettyPrinter::operator<< (const std::string s)
{
  return (*this) << s.c_str();
}

PrettyPrinter& PrettyPrinter::operator> (const std::string s)
{
  return (*this) > s.c_str();
}

PrettyPrinter& PrettyPrinter::operator>> (const std::string s)
{
  return (*this) >> s.c_str();
}

PrettyPrinter& PrettyPrinter::operator< (const int i)
{
  char h[10];
  sprintf(h, "%d", i);
  return (*this) < h;
}

PrettyPrinter& PrettyPrinter::operator<< (const int i)
{
  char h[10];
  sprintf(h, "%d", i);
  return (*this) << h;
}

PrettyPrinter& PrettyPrinter::operator> (const int i)
{
  char h[10];
  sprintf(h, "%d", i);
  return (*this) > h;
}

PrettyPrinter& PrettyPrinter::operator>> (const int i)
{
  char h[10];
  sprintf(h, "%d", i);
  return (*this) >> h;
}

PrettyPrinter& PrettyPrinter::operator< (const long l)
{
  char h[10];
  sprintf(h, "%ld", l);
  return (*this) < h;
}

PrettyPrinter& PrettyPrinter::operator<< (const long l)
{
  char h[10];
  sprintf(h, "%ld", l);
  return (*this) << h;
}

PrettyPrinter& PrettyPrinter::operator> (const long l)
{
  char h[10];
  sprintf(h, "%ld", l);
  return (*this) > h;
}

PrettyPrinter& PrettyPrinter::operator>> (const long l)
{
  char h[10];
  sprintf(h, "%ld", l);
  return (*this) >> h;
}

PrettyPrinter& PrettyPrinter::operator< (const unsigned long ul)
{
  char h[10];
  sprintf(h, "%lu", ul);
  return (*this) < h;
}

PrettyPrinter& PrettyPrinter::operator<< (const unsigned long ul)
{
  char h[10];
  sprintf(h, "%lu", ul);
  return (*this) << h;
}

PrettyPrinter& PrettyPrinter::operator> (const unsigned long ul)
{
  char h[10];
  sprintf(h, "%lu", ul);
  return (*this) > h;
}

PrettyPrinter& PrettyPrinter::operator>> (const unsigned long ul)
{
  char h[10];
  sprintf(h, "%lu", ul);
  return (*this) >> h;
}

PrettyPrinter& PrettyPrinter::operator+ ()
{
  if (strcmp(m_fileName1, "") != 0)
  {
    m_file1 << "\n";
  }
  if (m_console == 0 || m_console == 1)
  {
    PrintLn();
  }
  return *this;
}

PrettyPrinter& PrettyPrinter::operator++ ()
{
  if (strcmp(m_fileName1, "") != 0)
  {
    m_file1 << "\n";
  }
  if (strcmp(m_fileName2, "") != 0)
  {
    m_file2 << "\n";
  }
  if (m_console == 0 || m_console == 1 || m_console == 2)
  {
    PrintLn();
  }
  return *this;
}

PrettyPrinter& PrettyPrinter::operator> (const char* s)
{
  if (strcmp(m_fileName1, "") != 0)
  {
    m_file1 << m_indent;
    m_file1 << s;
  }
  if (m_console == 0 || m_console == 1)
  {
    PrintS(m_indent);
    PrintS(s);
  }
  return *this;
}

PrettyPrinter& PrettyPrinter::operator>> (const char* s)
{
  if (strcmp(m_fileName1, "") != 0)
  {
    m_file1 << m_indent;
    m_file1 << s;
  }
  if (strcmp(m_fileName2, "") != 0)
  {
    m_file2 << m_indent;
    m_file2 << s;
  }
  if (m_console == 0 || m_console == 1 || m_console == 2)
  {
    PrintS(m_indent);
    PrintS(s);
  }
  return *this;
}

PrettyPrinter& PrettyPrinter::operator+ (const int i)
{
  m_indents += i;
  if (m_indents < 0) m_indents = 0;
  delete m_indent;
  m_indent = new char[0];
  strcpy(m_indent, "");
  for (int j = 0; j < m_indents; j++) strcat(m_indent, m_baseIndent);
  return *this;
}

PrettyPrinter& PrettyPrinter::operator- (const int i)
{
  m_indents -= i;
  if (m_indents < 0) m_indents = 0;
  delete m_indent;
  m_indent = new char[0];
  strcpy(m_indent, "");
  for (int j = 0; j < m_indents; j++) strcat(m_indent, m_baseIndent);
  return *this;
}

void PrettyPrinter::setBaseIndent (const char* baseIndent)
{
  strcpy(m_baseIndent, baseIndent);
}

char* PrettyPrinter::getBaseIndent () const
{
  return m_baseIndent;
}

void PrettyPrinter::setConsole (const int console)
{
  m_console = console;
}

int PrettyPrinter::getConsole () const
{
  return m_console;
}

#endif // defined(HAVE_MINOR) || defined(HAVE_WRAPPERS)