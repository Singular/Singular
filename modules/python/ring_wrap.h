#ifndef RING_WRAP_HEADER
#define RING_WRAP_HEADER
class Ring{
  public:
    ring pimpl;
    Ring(ring r=currRing): pimpl(r){
    }
};
void export_ring();
#endif
