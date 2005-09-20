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
