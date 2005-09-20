//$Id: ring_wrap.h,v 1.4 2005-09-20 12:22:59 bricken Exp $
#ifndef RING_WRAP_HEADER
#define RING_WRAP_HEADER
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
};
void export_ring();
#endif
