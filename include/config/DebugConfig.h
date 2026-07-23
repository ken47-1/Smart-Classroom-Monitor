/* ==================== DebugConfig.h ==================== */
#pragma once

/* =============== DEBUG =============== */
#define DEBUG_ENABLED  1   // Master toggle

#if DEBUG_ENABLED   // EDIT BELOW
    #define DEBUG_FRAMEBUFFER_DUMP  0
    #define DEBUG_DUMP_EVERY_N      1
#else   // DO NOT EDIT BELOW
    #define DEBUG_FRAMEBUFFER_DUMP  0
    #define DEBUG_DUMP_EVERY_N      0
#endif
