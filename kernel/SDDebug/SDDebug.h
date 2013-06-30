#ifndef SD_DEBUG_H
#define SD_DEBUG_H

#ifdef KDEBUG
//TODO: Is that a good premise?
#define SD_HAVE_DEBUG
#endif

#define ShiftDVec_Debug ShiftDVec{ namespace Debug
#define ShiftDVec_Debug_END }

#define SD_Debug_Debugger \
  ShiftDVec{ namespace Debug{ namespace Debugger
#define SD_Debug_Debugger_END }}

#include "SDLogger.h"
#include "SDDebugger.h"

namespace ShiftDVec_Debug
{
  struct PolyFormat;

  extern DummyLogger dummy_logger;

  extern PolyFormat pf;

  template <typename ptr_type> void* addr( ptr_type some_ptr );

  ShiftDVec_Debug_END
}

template <class ptr_type>
void* SDDebug::addr( ptr_type some_ptr )
{ return static_cast<void*>(some_ptr); }

struct ShiftDVec::Debug::PolyFormat
{
  public:
    PolyFormat* operator()(){ return this; }

    PolyFormat* operator()( ring r )
    { lm_ring = r; tl_ring = r; return this; }

    PolyFormat* operator()( ring rlm, ring rtl )
    { lm_ring = rlm; tl_ring = rtl; return this; }

    ring get_lm_ring()   { return lm_ring; }
    ring get_tail_ring() { return tl_ring; }

  private:
    ring lm_ring;
    ring tl_ring;
};

#ifdef SD_HAVE_DEBUG

#define SD_DEBUG_LOG(logger_name)                         \
if( logger_name &&                                        \
    ShiftDVec::Debug::Debugger::has_logger(logger_name) ) \
  *ShiftDVec::Debug::Debugger::get_logger( logger_name )
#define SD_DEBUG_SEC if(1)

namespace ShiftDVec_Debug
{
  class DefaultLogger;

  ShiftDVec_Debug_END
}

class ShiftDVec::Debug::DefaultLogger :
  public ShiftDVec::Debug::AbstractLogger
{
  protected:
    char* filename;
    PolyFormat* pf;

  public:
    DefaultLogger();

    virtual ~DefaultLogger();

    virtual ALogger& operator<<( const char* str );
    virtual ALogger& operator<<( int number );
    virtual ALogger& operator<<( special spcl );
    virtual ALogger& operator<<( void* addr );
    virtual ALogger& operator<<( poly p );
    virtual ALogger& operator<<( PolyFormat* pf );

    virtual void set_output_stream( FILE* stream );
    virtual void set_output_stream
      ( const char* filename_, const char* mode );
    virtual void set_output_stream( ALogger* logger );
};

#else  // SD_HAVE_DEBUG is unset

#define SD_DEBUG_LOG(log_obj_addr) \
   if(1); else ShiftDVec::Debug::dummy_logger
#define SD_DEBUG_SEC if(0)

#endif // #ifdef SD_HAVE_DEBUG
#endif // #ifndef SD_DEBUG_HPP
