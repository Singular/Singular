#include "SDDebug.h"
#include "SDLogger.h"
#include "SDDebugger.h"

#ifdef KDEBUG
//TODO: Is that a good premise?
#define SD_HAVE_DEBUG
#endif

#ifdef SD_HAVE_DEBUG

#include <map>
#include <cstring>

namespace SDDebug = ShiftDVec::Debug;

namespace SD_Debug_Debugger
{
  struct cmp_c_str;

  SD_Debug_Debugger_END
}

struct SDDebug::Debugger::cmp_c_str
{
  public:
    bool operator() ( const char *a, const char *b ) const
    { return strcmp(a,b) < 0; }
};

namespace SD_Debug_Debugger
{
  namespace // Private Area
  {
    typedef
      std::map
      <const char*, ALogger*, cmp_c_str> LoggerMap;
    static LoggerMap*        loggers;
  }

  SD_Debug_Debugger_END
}

void SDDebug::Debugger::Init()
{
  if( !loggers ) loggers = new LoggerMap();
  add_logger("Default");
}

void SDDebug::Debugger::Free()
{
  delete_all_loggers();
  if( loggers ){ delete loggers; loggers = NULL; }
}

SDDebug::ALogger*
SDDebug::Debugger::add_logger( const char* name )
{
  ALogger* logger = get_logger( name );
  if( logger != &dummy_logger ) return logger;

  logger = new DefaultLogger;
  char*    lgname = new char[strlen(name)];
  strcpy(lgname, name);
  loggers->insert( LoggerMap::value_type(lgname, logger) );

  return logger;
}

SDDebug::ALogger*
SDDebug::Debugger::get_logger( const char* name )
{
  if( !loggers ) Init();
  LoggerMap::iterator logger = loggers->find( name );
  if( logger != loggers->end() ) return logger->second;
  else                           return &dummy_logger;
}

bool SDDebug::Debugger::has_logger( const char* name )
{
  if( !loggers ) Init();
  LoggerMap::iterator logger = loggers->find( name );
  return logger != loggers->end();
}

void SDDebug::Debugger::delete_all_loggers()
{
  if( !loggers ) return;
  LoggerMap::iterator it = loggers->begin();
  for(; it != loggers->end(); ++it )
  {
    delete it->first;
    delete it->second;
  }
}

#else  // SD_HAVE_DEBUG is unset

void SDDebug::Debugger::Init(){}

void SDDebug::Debugger::Free(){}

SDDebug::ALogger*
SDDebug::Debugger::add_logger( const char* name )
{ return &dummy_logger; }

SDDebug::ALogger*
SDDebug::Debugger::get_logger( const char* name )
{ return &dummy_logger; }

bool SDDebug::Debugger::has_logger( const char* name )
{ return false; }

void SDDebug::Debugger::delete_all_loggers(){}

#endif
