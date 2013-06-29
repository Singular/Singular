#ifndef SD_DEBUGGER_H
#define SD_DEBUGGER_H

#ifdef KDEBUG
//TODO: Is that a good premise?
#define SD_HAVE_DEBUG
#endif

#include "SDDebug.h"
#include "SDLogger.h"

namespace SD_Debug_Debugger
{
  void Init();
  void Free();

  ALogger* add_logger( const char* name );
  ALogger* get_logger( const char* name );
  ALogger* get_logger();

  bool has_logger( const char* name );
  
  void delete_all_loggers();

  SD_Debug_Debugger_END
}

#endif
