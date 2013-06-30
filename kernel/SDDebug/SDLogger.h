#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#ifdef KDEBUG
//TODO: Is that a good premise?
#define SD_HAVE_DEBUG
#endif

#include <cstdio>
#include <kernel/polys.h>
#include <kernel/SDDebug.h>

namespace ShiftDVec_Debug
{
  struct PolyFormat;
  class AbstractLogger;
  class DummyLogger;

  typedef AbstractLogger ALogger;

  extern PolyFormat pf;

  ShiftDVec_Debug_END
}

namespace SDDebug = ShiftDVec::Debug;

class SDDebug::AbstractLogger
{
  public:
    typedef AbstractLogger ALogger;

    virtual ~AbstractLogger() {}

    enum special { Date, Flush };

    virtual ALogger& operator<<( const char* str ) = 0;
    virtual ALogger& operator<<( int number ) = 0;
    virtual ALogger& operator<<( special spcl ) = 0;

    virtual ALogger& operator<<( void* addr ) = 0;
    virtual ALogger& operator<<( poly p ) = 0;
    virtual ALogger& operator<<( PolyFormat* ) = 0;

    virtual void set_output_stream( FILE* stream ) = 0;
    virtual void set_output_stream
      ( const char* filename, const char* mode ) = 0;
    virtual void set_output_stream( ALogger* logger ) = 0;

  protected:
    FILE* output_stream;
};

class SDDebug::DummyLogger : public AbstractLogger
{
  public:
    virtual ~DummyLogger() {}

    virtual ALogger& operator<<(const char* str){return *this;}
    virtual ALogger& operator<<( int number ){ return *this; }
    virtual ALogger& operator<<( special spcl ){return *this;}
    virtual ALogger& operator<<( void* addr ){ return *this;}
    virtual ALogger& operator<<( poly p ){ return *this; }
    virtual ALogger& operator<<( PolyFormat* ){ return *this; }

    virtual void set_output_stream( FILE* stream ) {}
    virtual void set_output_stream
      ( const char* filename, const char* mode ) {}
    virtual void set_output_stream( ALogger* logger ) {}
};

#endif
