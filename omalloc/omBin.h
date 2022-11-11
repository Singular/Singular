/*******************************************************************
 *  File:    omBin.h
 *  Purpose: declaration of routines related to Bins
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#ifndef OM_BIN_H
#define OM_BIN_H

#define omGetAlignedSpecBin(size) _omGetSpecBin(size, 1, 0)
#define omGetSpecBin(size) _omGetSpecBin(size, 0, 0)
omBin _omGetSpecBin(size_t size, int align, int track);

#define omUnGetSpecBin(bin_ptr) _omUnGetSpecBin(bin_ptr, 0)
#define omDeleteSpecBin(bin_ptr) _omUnGetSpecBin(bin_ptr, 1)
void _omUnGetSpecBin(omBin *bin, int force);
long omGetUsedBinBytes(void);

omBin omGetStickyBinOfBin(omBin bin);
void omMergeStickyBinIntoBin(omBin sticky_bin, omBin into_bin);

unsigned long omGetNewStickyBinTag(omBin bin);
void omSetStickyBinTag(omBin bin, unsigned long sticky);
void omUnSetStickyBinTag(omBin bin, unsigned long sticky);
void omDeleteStickyBinTag(omBin bin, unsigned long sticky);

unsigned long omGetNewStickyAllBinTag(void);
void omSetStickyAllBinTag(unsigned long sticky);
void omUnSetStickyAllBinTag(unsigned long sticky);
void omDeleteStickyAllBinTag(unsigned long sticky);

void omPrintBinStats(FILE* fd);
#define omIsStickyBin(bin) (bin->sticky >= SIZEOF_VOIDP)

/*BEGINPRIVATE*/
#ifndef OM_NDEBUG
int omIsKnownTopBin(omBin bin, int normal_bin);
#endif
#ifdef OM_HAVE_TRACK
#define omGetSpecTrackBin(size) _omGetSpecBin(size, 0, 1)
#endif

#define omIsStaticNormalBin(bin)                                            \
 ((unsigned long) bin >= ((unsigned long) &om_StaticBin[0]) &&              \
  (unsigned long) bin <= ((unsigned long) &om_StaticBin[OM_MAX_BIN_INDEX]))
#define omIsNormalBin(bin) omIsStaticNormalBin(bin) || omFindInGList(om_SpecBin, next, bin, bin) || omIsStickyBin(bin)
#define omIsSpecBin(bin) (!omIsStaticNormalBin(bin) && !omIsStickyBin(bin))

#ifdef OM_HAVE_TRACK
#define omIsStaticTrackBin(bin)                                                  \
 ((unsigned long) bin >= ((unsigned long) &om_StaticTrackBin[0]) &&              \
  (unsigned long) bin <= ((unsigned long) &om_StaticTrackBin[OM_MAX_BIN_INDEX]))
#define omIsTrackBin(bin) omIsStaticTrackBin(bin) || omFindInGList(om_SpecTrackBin, next, bin, bin)
#else
#define omIsKnownTopBin(bin, normal_bin)  1
#define omIsStaticTrackBin(bin) 0
#define omIsTrackBin(bin)       0
#endif
#define omIsStaticBin(bin) omIsStaticNormalBin(bin) || omIsStaticTrackBin(bin)
/*ENDPRIVATE*/

#endif /* OM_BIN_H */
