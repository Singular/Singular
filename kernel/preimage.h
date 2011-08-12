#ifndef KERNEL_PREIMAGE_H
#define KERNEL_PREIMAGE_H

struct ip_sring; typedef struct ip_sring * ring;
struct sip_sideal; typedef struct sip_sideal * ideal;
struct sip_smap; typedef struct sip_smap *          map;

ideal maGetPreimage(ring theImageRing, map theMap, ideal id, const ring dst_r);

#endif // KERNEL_PREIMAGE_H
