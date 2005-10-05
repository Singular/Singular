#include "mod2.h"
#include "structs.h"
#include "subexpr.h"
#include "wrapper.h"

void initPySingular() {
  initSingular();
  init_Singular();
}
