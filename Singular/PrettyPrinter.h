#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#if defined(HAVE_MINOR) || defined(HAVE_WRAPPERS)

#include <fstream>
#include <string>

/*! \class PrettyPrinter
 *  \brief Class PrettyPrinter is a utility for pretty-printing any kind of
 *         output to one or two files and/or the console.
 *
 *  The user may use an instance of PrettyPrinter to define two files to
 *  which output can be directed. Then, whenever an item is written to the
 *  PrettyPrinter, the user can decide whether the output goes only to the
 *  first of the files or to both. Thereby, the user may write, e.g., <em>all</em>
 *  output to the first file, and only <em>asorted</em> parts to the second, e.g.,
 *  only the most important bits, for instance to generate some kind of overview
 *  of the data that is written to the first file.<br>
 *  Furthermore, the user can decide whether the output will also go to the
 *  console. If so, he/she can decide whether the console will display
 *  all that output which goes to the first file, or only that which goes
 *  to the second one.
 *  Moreover, PrettyPrinter offers functionality to easily include linefeeds
 *  and tabs in the output to give it a better readable format.
 *  \author Frank Seelisch, http://www.mathematik.uni-kl.de/~seelisch
 */
class PrettyPrinter
{
private:
  /*! the number of times, the m_baseIndent is used to make up the current
      indent string; see e.g. PrettyPrinter::operator> (const char* s);
      Example: if m_baseIndent is "~~~" and m_indents == 2, then the current
      indent string is set to "~~~~~~" */
  int m_indents;

  /*! the base string to define the entire current indent string;
      see description of m_indents */
  char* m_baseIndent;

  /*! the container for the entire current indent string;
      see descriptions of m_indents and m_baseIndent */
  char* m_indent;

  /*! file name of primary output file m_file1;
      m_file1 will always contain at least as much output as m_file2 */
  char* m_fileName1;

  /*! file name of secondary output file m_file2;
      m_file1 will always contain at least as much output as m_file2 */
  char* m_fileName2;

  /*! for controlling output to the console;<br>
      if == -1: no output to the console<br>
      if == 0:  just output to the console, i.e. no file output<br>
      if == 1:  identical outputs to file1 and console<br>
      if == 2:  identical outputs to file2 and console */
  int m_console;

  /*! file handle to primary output file */
  std::ofstream m_file1;

  /*! file handle to secondary output file */
  std::ofstream m_file2;
public:
  /*!
   *  A constructor for PrettyPrinter.<br>
   *  Set filenames equal to "" in order to omit the usage of that file. I.e.,
   *  the user should set fileName2 to "" when he/she wants to use only one
   *  output file and set both filenames to "" when he/she does not want any file output.<br>
   *  Set console to -1, if no console output is desired; to 0 if there should be
   *  console output but no file output; to 1 if console output should be identical to
   *  output to primary file; and to 2 if console output should be identical to
   *  output to secondary file.<br>
   *  The baseIndent will be used to build the current indent string which will be included
   *  in the output using e.g. PrettyPrinter::operator> (const char* s).
   *  @param fileName1 a file name for the primary output file
   *  @param fileName2 a file name for the secondary output file
   *  @param append1 if true, m_file1 is being appended, otherwise initially cleared
   *  @param append2 if true, m_file2 is being appended, otherwise initially cleared
   *  @param console control integer for setting up console output
   *  @param baseIndent the base string for the indent string
   *  @see PrettyPrinter::operator+ (const int i)
   */
  PrettyPrinter (const char* fileName1, const char* fileName2,
                 const bool append1, const bool append2,
                 const int console, const char* baseIndent);

  /*!
   *  A destructor for PrettyPrinter.
   */
  ~PrettyPrinter ();
  
  /*!
   *  A method for writing all pending output to all defined output files.
   *  This works simply by closing all output files and re-opening them in
   *  append mode.
   */
  void flush ();

  /*!
   *  A method for including a linefeed in the output to the primary file
   *  (if any) and to the console (if console output has been declared accordingly).
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator++ ()
   */
  PrettyPrinter& operator+ ();
  
  /*!
   *  A method for including a linefeed in the output to the primary file
   *  (if any), the secondary file (if any), and to the console (if console
   *  output has been declared accordingly).
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator+ ()
   */
  PrettyPrinter& operator++ ();
  
  /*!
   *  A method for including the current indent string followed by the argument string
   *  in the output to the primary file (if any) and to the console (if console output
   *  has been declared accordingly).
   *  @param s the string to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator>> (const char* s)
   */
  PrettyPrinter& operator> (const char* s);

  /*!
   *  A method for including the current indent string followed by the argument string
   *  in the output to the primary file (if any), to the secondary file (if any), and to
   *  the console (if console output has been declared accordingly).
   *  @param s the string to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator> (const char* s)
   */
  PrettyPrinter& operator>> (const char* s);

  /*!
   *  A method for including the current indent string followed by the argument integer
   *  in the output to the primary file (if any) and to the console (if console output
   *  has been declared accordingly).
   *  @param i the integer to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator>> (const int i)
   */
  PrettyPrinter& operator> (const int i);
  
  /*!
   *  A method for including the current indent string followed by the argument integer
   *  in the output to the primary file (if any), to the secondary file (if any), and to
   *  the console (if console output has been declared accordingly).
   *  @param i the integer to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator> (const int i)
   */
  PrettyPrinter& operator>> (const int i);
  
  /*!
   *  A method for including the current indent string followed by the argument long
   *  in the output to the primary file (if any) and to the console (if console output
   *  has been declared accordingly).
   *  @param l the long to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator>> (const long l)
   */
  PrettyPrinter& operator> (const long l);
  
  /*!
   *  A method for including the current indent string followed by the argument long
   *  in the output to the primary file (if any), to the secondary file (if any), and to
   *  the console (if console output has been declared accordingly).
   *  @param l the long to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator> (const long l)
   */
  PrettyPrinter& operator>> (const long l);
  
  /*!
   *  A method for including the current indent string followed by the argument unsigned long
   *  in the output to the primary file (if any) and to the console (if console output
   *  has been declared accordingly).
   *  @param ul the unsigned long to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator>> (const unsigned long ul)
   */
  PrettyPrinter& operator> (const unsigned long ul);
  
  /*!
   *  A method for including the current indent string followed by the argument unsigned long
   *  in the output to the primary file (if any), to the secondary file (if any), and to
   *  the console (if console output has been declared accordingly).
   *  @param ul the unsigned long to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator> (const unsigned long ul)
   */
  PrettyPrinter& operator>> (const unsigned long ul);
  
  /*!
   *  A method for including the current indent string followed by the argument string
   *  in the output to the primary file (if any) and to the console (if console output
   *  has been declared accordingly).
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator>> (const std::string s)
   */
  PrettyPrinter& operator> (const std::string s);
  
  /*!
   *  A method for including the current indent string followed by the argument string
   *  in the output to the primary file (if any), to the secondary file (if any), and to
   *  the console (if console output has been declared accordingly).
   *  @param s the string to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator> (const std::string s)
   */
  PrettyPrinter& operator>> (const std::string s);
  
  /*!
   *  A method for including the argument string in the output to the primary file
   *  (if any) and to the console (if console output has been declared accordingly).
   *  @param s the string to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator<< (const char* s)
   */
  PrettyPrinter& operator< (const char* s);
  
  /*!
   *  A method for including the argument string in the output to the primary file
   *  (if any), to the secondary file (if any), and to the console (if console
   *  output has been declared accordingly).
   *  @param s the string to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator< (const char* s)
   */
  PrettyPrinter& operator<< (const char* s);
  
  /*!
   *  A method for including the argument integer in the output to the primary file
   *  (if any) and to the console (if console output has been declared accordingly).
   *  @param i the integer to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator<< (const int i)
   */
  PrettyPrinter& operator< (const int i);
  
  /*!
   *  A method for including the argument integer in the output to the primary file
   *  (if any), to the secondary file (if any), and to the console (if console
   *  output has been declared accordingly).
   *  @param i the integer to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator< (const int i)
   */
  PrettyPrinter& operator<< (const int i);
  
  /*!
   *  A method for including the argument long in the output to the primary file
   *  (if any) and to the console (if console output has been declared accordingly).
   *  @param l the long to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator<< (const long l)
   */
  PrettyPrinter& operator< (const long l);

  /*!
   *  A method for including the argument long in the output to the primary file
   *  (if any), to the secondary file (if any), and to the console (if console
   *  output has been declared accordingly).
   *  @param l the long to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator< (const long l)
   */
  PrettyPrinter& operator<< (const long l);
  
  /*!
   *  A method for including the argument unsigned long in the output to the primary file
   *  (if any) and to the console (if console output has been declared accordingly).
   *  @param ul the unsigned long to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator<< (const unsigned long ul)
   */
  PrettyPrinter& operator< (const unsigned long ul);
  
  /*!
   *  A method for including the argument unsigned long in the output to the primary file
   *  (if any), to the secondary file (if any), and to the console (if console
   *  output has been declared accordingly).
   *  @param ul the unsigned long to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator< (const unsigned long ul)
   */
  PrettyPrinter& operator<< (const unsigned long ul);
  
  /*!
   *  A method for including the argument string in the output to the primary file
   *  (if any) and to the console (if console output has been declared accordingly).
   *  @param s the string to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator<< (const std::string s)
   */
  PrettyPrinter& operator< (const std::string s);
  
  /*!
   *  A method for including the argument string in the output to the primary file
   *  (if any), to the secondary file (if any), and to the console (if console
   *  output has been declared accordingly).
   *  @param s the string to be written to the output devices
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator< (const std::string s)
   */
  PrettyPrinter& operator<< (const std::string s);

  /*!
   *  A method for incrementing the number of times the baseIndent string is
   *  concatenated in order to form the current entire indent string. The increment equals
   *  the argument integer.<br>
   *  Use PrettyPrinter::setBaseIndent (const char* baseIndent) to define the
   *  baseIndent string. If e.g. baseIndent == "~~", and the incremented number of times
   *  this baseIndent will be used equals 3, then the current indent string will be set to
   *  "~~~~~~".
   *  @param i the number of times the baseIndent is use to form the current indent string
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator- (const int i)
   */
  PrettyPrinter& operator+ (const int i);
  
  /*!
   *  A method for decrementing the number of times the baseIndent string is
   *  concatenated in order to form the current entire indent string. The decrement equals
   *  the argument integer.<br>
   *  Use PrettyPrinter::setBaseIndent (const char* baseIndent) to define the
   *  baseIndent string. If e.g. baseIndent == "~~", and the decremented number of times
   *  this baseIndent will be used equals 3, then the current indent string will be set to
   *  "~~~~~~".
   *  @param i the number of times the baseIndent is use to form the current indent string
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::operator+ (const int i)
   */
  PrettyPrinter& operator- (const int i);

  /*!
   *  A method for setting the baseIndent string.<br>
   *  Note that this string is used to form the current entire indent string.
   *  @param baseIndent the new baseIndent string
   *  @return a reference to the modified instance of PrettyPrinter
   *  @see PrettyPrinter::getBaseIndent () const
   */
  void setBaseIndent (const char* baseIndent);
  
  /*!
   *  A method for retrieving the baseIndent string.<br>
   *  Note that this string is used to form the current entire indent string.
   *  @return the current baseIndent string
   *  @see PrettyPrinter::setBaseIndent (const char* baseIndent)
   */
  char* getBaseIndent () const;

  /*!
   *  A method for controling console output.<br>
   *  Use the parameter -1, if no console output is desired; 0 if there should be
   *  console output but no file output; 1 if console output should be identical to
   *  output to the primary file; and 2 if console output should be identical to
   *  output to the secondary file.
   *  @param console the control number as described in the method comment
   *  @see PrettyPrinter::getConsole () const
   */
  void setConsole (const int console);
  
  /*!
   *  A method for retrieving the console output control parameter.<br>
   *  @see PrettyPrinter::setConsole (const int console)
   */
  int getConsole () const;
};

#endif
/* defined(HAVE_MINOR) || defined(HAVE_WRAPPERS) */

#endif
/* PRETTY_PRINTER_H */