//$Id: ring_wrap.h,v 1.6 2005-11-07 08:39:16 bricken Exp $
#ifndef RING_WRAP_HEADER
#define RING_WRAP_HEADER
#include "ring.h"
#include "grammar.h"
#include "subexpr.h"
#include "ipid.h"
class Ring{
  public:
    ring pimpl;
    Ring(ring r=currRing): pimpl(r){
      if (r!=NULL)
        ++(r->ref);
    }
    Ring(const Ring& r2):pimpl(r2.pimpl){
      pimpl->ref++;
    }
    ~Ring(){
      if (pimpl!=NULL)
        --(pimpl->ref);
    }
    void set(){
      //FIXME: only a hack, no solution
      char name_buffer[100];
      static int ending=0;
      ending++;
      sprintf(name_buffer, "PYTHON_RING_VAR%d",ending);
      idhdl shadow_hdl=enterid(name_buffer,0,RING_CMD,&IDROOT);
      pimpl->ref++;
      shadow_hdl->data.uring=pimpl;
      rChangeCurrRing(pimpl);
      currRingHdl=shadow_hdl;
      
    }
};
void export_ring();
#endif
