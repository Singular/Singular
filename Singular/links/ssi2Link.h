/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    ssi2Link.h
 * Purpose: declaration of link routines for binary SSI2
 ***************************************************************/
#ifndef SSI2LINK_H
#define SSI2LINK_H

#include "Singular/links/silink.h"

si_link_extension slInitSsi2Extension(si_link_extension s);
si_link_extension slInitSsi2cExtension(si_link_extension s);
si_link_extension slInitSsi2zExtension(si_link_extension s);
si_link_extension slInitSsi2zstdExtension(si_link_extension s);
si_link_extension slInitSsi2lz4Extension(si_link_extension s);
si_link_extension slInitSsi2eExtension(si_link_extension s);
si_link_extension slInitSsi2fExtension(si_link_extension s);

#endif
