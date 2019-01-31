#pragma once

#include "config.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#ifdef USE_BOEHM_GC
#include <gc/gc.h>
#else
#include "../gclib/gc.h"
#endif

using namespace std;

#include "mem.h"
#include "contract.h"
#include "basic.h"
#include "arr.h"
#include "str.h"
#include "init.h"
#include "hash.h"
#include "os.h"
#include "check.h"
