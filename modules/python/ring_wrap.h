//$Id: ring_wrap.h,v 1.3 2005-09-20 08:39:44 bricken Exp $
#ifndef RING_WRAP_HEADER
#define RING_WRAP_HEADER
class Ring{
  public:
    ring pimpl;
    Ring(ring r=currRing): pimpl(r){
      if (r!=NULL)
        ++r->ref;
    }
    ~Ring(){
      if (pimpl!=NULL)
        --pimpl->ref;
    }
};
void export_ring();
#endif
