#pragma once

#include <assert.h>
#include "Types.h"
#include "Math.h"

#ifdef DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define newp new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define newp new
#endif