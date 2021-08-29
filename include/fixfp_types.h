#pragma once

#include <fixfp_templ.h>
#include <fixfp_vec_templ.h>

static constexpr u32 FPT_SHIFT = 16;

// extern template struct fixfp<FPT_SHIFT>;
// extern template struct fixfp3<FPT_SHIFT>;

using fpt = fixfp<FPT_SHIFT>;
using fpt3 = fixfp3<FPT_SHIFT>;

template struct fixfp<FPT_SHIFT>;
template struct fixfp3<FPT_SHIFT>;
