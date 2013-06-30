#include "SDDebug.h"
#include "SDDebugger.h"

#ifdef KDEBUG
//TODO: Is that a good premise?
#define SD_HAVE_DEBUG
#endif

namespace SDDebug = ShiftDVec::Debug;

SDDebug::DummyLogger
SDDebug::dummy_logger = SDDebug::DummyLogger();

SDDebug::PolyFormat SDDebug::pf = SDDebug::PolyFormat();

#ifdef SD_HAVE_DEBUG

#include <ctime>
#include <cstring>

SDDebug::DefaultLogger::DefaultLogger() :
  filename(NULL), output_stream(NULL), pf(NULL) {}

SDDebug::DefaultLogger::~DefaultLogger()
{
  if(output_stream) fclose(output_stream);
  if(filename)      delete filename;
}


SDDebug::ALogger&
SDDebug::DefaultLogger::operator<<( const char* str )
{
  if( !output_stream ) PrintS( str );
  else   fputs( str, output_stream );

  return *this;
}

SDDebug::ALogger&
SDDebug::DefaultLogger::operator<<( int number )
{
  if( !output_stream )   Print("%d", number);
  else fprintf( output_stream, "%d", number);

  return *this;
}

SDDebug::ALogger&
SDDebug::DefaultLogger::operator<<( special spcl )
{
  char* output;
  switch( spcl )
  {
    case Date:
      time_t now; time(&now);
      output = ctime(&now);
      int len_time = strlen( output );
      output[len_time-1] = '\0';
      break;
    case Flush:
      if( output_stream ) fflush( output_stream );
      return *this;
  }

  if( !output_stream ) PrintS( output );
  else   fputs( output, output_stream );

  return *this;
}

SDDebug::ALogger&
SDDebug::DefaultLogger::operator<<( void* addr )
{
  if( !output_stream )   Print("%p", addr );
  else fprintf( output_stream, "%p", addr );

  return *this;
}

SDDebug::ALogger&
SDDebug::DefaultLogger::operator<<( poly p )
{
  if( pf )
  {
    ring lm_ring = pf->get_lm_ring();
    ring tl_ring = pf->get_tail_ring();

    if( !output_stream ) p_Write0( p, lm_ring, tl_ring );
    else fputs( p_String(p, lm_ring, tl_ring), output_stream );
  }
  else
    operator<<("(Cannot output poly - no formatter specified)");

  return *this;
}

SDDebug::ALogger&
SDDebug::DefaultLogger::operator<<( PolyFormat* pf )
{
  this->pf = pf;
  return *this;
}


void SDDebug::DefaultLogger::set_output_stream( FILE* stream )
{
  if( filename )
  {
    delete filename;
    fclose( output_stream );
    filename = NULL;
  }

  output_stream = stream;
}

void SDDebug::DefaultLogger::set_output_stream
  ( const char* filename_, const char* mode )
{
  if( this->filename )
  {
    delete this->filename;
    fclose(output_stream);
  }
  this->filename = new char[strlen(filename_)+1];
  strcpy( this->filename, filename_ );
  output_stream = fopen( filename_, mode );
}

#endif
