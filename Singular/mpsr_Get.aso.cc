#include "aso.h"
#include "mod2.h"
#include "mpsr_Get.h"
main()
{
#ifdef HAVE_MPSR
  ASO(mpsr_sleftv);
#endif
}
