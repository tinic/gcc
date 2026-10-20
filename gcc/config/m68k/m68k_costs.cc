/* m68k-costs.cc - unified m68k RTX cost model.

   One engine, one pair of tables (speed, size) per CPU.
   The active pair is chosen once from TUNE_68000_10 / m68k_tune.

   Overrides are loaded from the file named by M68K_COSTS_FILE, into
   the active CPU's active table only. No fallback filenames.

   Adding a new CPU means adding one speed table and one size table,
   plus a case in m68k_cost_select(). The engine does not change. */

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "cfghooks.h"
#include "tree.h"
#include "rtl.h"
#include "flags.h"

#define USE_MOVQ(i)	((unsigned) ((i) + 128) <= 255)

/* ============================================
   Cost index enum - order must match cost_map[]
   ============================================ */
enum m68k_cost_idx {
    IDX_REG_SRC,
    IDX_REG_DST,
    IDX_MEM_PLUS_REG_DISP,
    IDX_MEM_PLUS_REG_REG,
    IDX_MEM_OTHER,
    IDX_CONST_INT_Q,
    IDX_CONST_INT_W,
    IDX_CONST_INT_L_1,
    IDX_CONST_INT_L_2,
    IDX_CONST_DOUBLE,
    IDX_SYMBOL,
    IDX_PLUS_REG_REG,
    IDX_PLUS_REG_CONSTQ,
    IDX_PLUS_REG_CONSTW,
    IDX_PLUS_REG_CONSTL,
    IDX_PLUS_OTHER,
    IDX_LOGIC_REG_CONST,
    IDX_SHIFT_CONST,
    IDX_SHIFT_CONST_W,
    IDX_BRANCH,
    IDX_SET_REG_REG,
    IDX_CALL_OTHER,
    IDX_MULU_L,
    IDX_MULU_W,
    IDX_DIV_L,
    IDX_DIV_W,
    IDX_ADD_L,
    IDX_SUB_L,
    IDX_MOVE_L,
    IDX_MOVEQ,
    IDX_LSL_SHIFT,
    IDX_LSL_SHIFT_STEP,

    /* SBF Structural Constants */
    IDX_MEM_LONG_EXTRA,
    IDX_MEM_PREDEC_EXTRA,
    IDX_CLR_LONG_PENALTY,
    IDX_CLR_WORD_PENALTY,
    IDX_CALL_REG_OFFSET,
    IDX_CALL_PLUS_OFFSET,
    IDX_CALL_SYM_OFFSET,
    IDX_PLUS_LEA_OFFSET,
    IDX_PLUS_LONG_EXTRA,
    IDX_PLUS_SHORT_EXTRA,
    IDX_LOGIC_LONG_EXTRA,
    IDX_LOGIC_WORD_EXTRA,
    IDX_MULT_68000_BASE,
    IDX_MULT_68000_BITS,
    IDX_MULT_68000_TRANS,
    IDX_MULT_68000_WORD_BASE,
    IDX_MULT_68000_LONG_MUL,
    IDX_MULT_68000_WORD_MUL,
    IDX_EXTEND_EXTRA,

    IDX_COUNT
};

/* Globale Tabellenpuffer */
static int cost_speed_68000_10[IDX_COUNT];
static int cost_size_68000_10[IDX_COUNT];

static int cost_speed_68020[IDX_COUNT];
static int cost_size_68020[IDX_COUNT];

static int cost_speed_68030[IDX_COUNT];
static int cost_size_68030[IDX_COUNT];

static int cost_speed_68040[IDX_COUNT];
static int cost_size_68040[IDX_COUNT];

static int cost_speed_68080[IDX_COUNT];
static int cost_size_68080[IDX_COUNT];

static inline void
set_cost (int t[IDX_COUNT], enum m68k_cost_idx idx, int val)
{
    t[idx] = val;
}

/* ============================================
   68000/010 - SPEED / SIZE (Exact SBF Matrices)
   ============================================ */
static void
init_cost_speed_68000_10 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = -1;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_DISP,      4);
    set_cost (t, IDX_MEM_PLUS_REG_REG,       8);
    set_cost (t, IDX_MEM_OTHER,             12);
    set_cost (t, IDX_CONST_INT_Q,            4);
    set_cost (t, IDX_CONST_INT_W,            8);
    set_cost (t, IDX_CONST_INT_L_1,         10);
    set_cost (t, IDX_CONST_INT_L_2,         12);
    set_cost (t, IDX_SYMBOL,                12);
    set_cost (t, IDX_PLUS_REG_REG,           4);
    set_cost (t, IDX_PLUS_REG_CONSTQ,        4);
    set_cost (t, IDX_PLUS_REG_CONSTW,        8);
    set_cost (t, IDX_PLUS_REG_CONSTL,        8);
    set_cost (t, IDX_PLUS_OTHER,            12);
    set_cost (t, IDX_LOGIC_REG_CONST,        8);
    set_cost (t, IDX_SHIFT_CONST,            8);
    set_cost (t, IDX_SHIFT_CONST_W,          6);
    set_cost (t, IDX_CALL_OTHER,            22);
    set_cost (t, IDX_DIV_L,                410);
    set_cost (t, IDX_DIV_W,                136);
    set_cost (t, IDX_LSL_SHIFT,              8);
    set_cost (t, IDX_LSL_SHIFT_STEP,         2);

    set_cost (t, IDX_MEM_LONG_EXTRA,         4);
    set_cost (t, IDX_MEM_PREDEC_EXTRA,       2);
    set_cost (t, IDX_CLR_LONG_PENALTY,       8);
    set_cost (t, IDX_CLR_WORD_PENALTY,       4);
    set_cost (t, IDX_CALL_REG_OFFSET,       -6);
    set_cost (t, IDX_CALL_PLUS_OFFSET,      -4);
    set_cost (t, IDX_CALL_SYM_OFFSET,       -2);
    set_cost (t, IDX_PLUS_LEA_OFFSET,        8);
    set_cost (t, IDX_PLUS_LONG_EXTRA,        6);
    set_cost (t, IDX_PLUS_SHORT_EXTRA,       4);
    set_cost (t, IDX_LOGIC_LONG_EXTRA,       6);
    set_cost (t, IDX_LOGIC_WORD_EXTRA,       6);
    set_cost (t, IDX_MULT_68000_BASE,       24);
    set_cost (t, IDX_MULT_68000_BITS,        6);
    set_cost (t, IDX_MULT_68000_TRANS,       7);
    set_cost (t, IDX_MULT_68000_WORD_BASE,  38);
    set_cost (t, IDX_MULT_68000_LONG_MUL,  150);
    set_cost (t, IDX_MULT_68000_WORD_MUL,   50);
    set_cost (t, IDX_EXTEND_EXTRA,           8);

    /* Not used by SBF -> force return false */
    set_cost (t, IDX_CONST_DOUBLE,          -1);
    set_cost (t, IDX_BRANCH,                -1);
    set_cost (t, IDX_SET_REG_REG,           -1);
    set_cost (t, IDX_MULU_L,                -1);
    set_cost (t, IDX_MULU_W,                -1);
    set_cost (t, IDX_ADD_L,                 -1);
    set_cost (t, IDX_SUB_L,                 -1);
    set_cost (t, IDX_MOVE_L,                -1);
    set_cost (t, IDX_MOVEQ,                 -1);
}

static void
init_cost_size_68000_10 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = -1;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_DISP,      4);
    set_cost (t, IDX_MEM_PLUS_REG_REG,       8);
    set_cost (t, IDX_MEM_OTHER,             12);
    set_cost (t, IDX_CONST_INT_Q,            4);
    set_cost (t, IDX_CONST_INT_W,            8);
    set_cost (t, IDX_CONST_INT_L_1,         10);
    set_cost (t, IDX_CONST_INT_L_2,         12);
    set_cost (t, IDX_SYMBOL,                12);
    set_cost (t, IDX_PLUS_REG_REG,           4);
    set_cost (t, IDX_PLUS_REG_CONSTQ,        4);
    set_cost (t, IDX_PLUS_REG_CONSTW,        8);
    set_cost (t, IDX_PLUS_REG_CONSTL,        8);
    set_cost (t, IDX_PLUS_OTHER,            12);
    set_cost (t, IDX_LOGIC_REG_CONST,        8);
    set_cost (t, IDX_SHIFT_CONST,            8);
    set_cost (t, IDX_SHIFT_CONST_W,          6);
    set_cost (t, IDX_CALL_OTHER,            22);
    set_cost (t, IDX_DIV_L,                108);
    set_cost (t, IDX_DIV_W,                 36);
    set_cost (t, IDX_LSL_SHIFT,              8);
    set_cost (t, IDX_LSL_SHIFT_STEP,         2);

    set_cost (t, IDX_MEM_LONG_EXTRA,         4);
    set_cost (t, IDX_MEM_PREDEC_EXTRA,       2);
    set_cost (t, IDX_CLR_LONG_PENALTY,       8);
    set_cost (t, IDX_CLR_WORD_PENALTY,       4);
    set_cost (t, IDX_CALL_REG_OFFSET,       -6);
    set_cost (t, IDX_CALL_PLUS_OFFSET,      -4);
    set_cost (t, IDX_CALL_SYM_OFFSET,       -2);
    set_cost (t, IDX_PLUS_LEA_OFFSET,        8);
    set_cost (t, IDX_PLUS_LONG_EXTRA,        6);
    set_cost (t, IDX_PLUS_SHORT_EXTRA,       4);
    set_cost (t, IDX_LOGIC_LONG_EXTRA,       6);
    set_cost (t, IDX_LOGIC_WORD_EXTRA,       6);
    set_cost (t, IDX_MULT_68000_BASE,       24);
    set_cost (t, IDX_MULT_68000_BITS,        6);
    set_cost (t, IDX_MULT_68000_TRANS,       7);
    set_cost (t, IDX_MULT_68000_WORD_BASE,  38);
    set_cost (t, IDX_MULT_68000_LONG_MUL,   48);
    set_cost (t, IDX_MULT_68000_WORD_MUL,   16);
    set_cost (t, IDX_EXTEND_EXTRA,           8);
}

/* ============================================
   68020 - SPEED / SIZE
   ============================================ */
static void
init_cost_speed_68020 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = 0;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_DISP,     11);
    set_cost (t, IDX_MEM_PLUS_REG_REG,       4);
    set_cost (t, IDX_MEM_OTHER,              3);
    set_cost (t, IDX_CONST_INT_Q,            0);
    set_cost (t, IDX_CONST_INT_W,           15);
    set_cost (t, IDX_CONST_INT_L_1,         16);
    set_cost (t, IDX_CONST_INT_L_2,         16);
    set_cost (t, IDX_SYMBOL,                 7);
    set_cost (t, IDX_PLUS_REG_REG,           5);
    set_cost (t, IDX_PLUS_REG_CONSTQ,        1);
    set_cost (t, IDX_PLUS_REG_CONSTL,        5);
    set_cost (t, IDX_PLUS_OTHER,            14);
    set_cost (t, IDX_LOGIC_REG_CONST,        7);
    set_cost (t, IDX_SHIFT_CONST,            4);
    set_cost (t, IDX_SET_REG_REG,            1);
    set_cost (t, IDX_CALL_OTHER,             8);
    set_cost (t, IDX_MULU_L,                43);
    set_cost (t, IDX_MULU_W,                27);
    set_cost (t, IDX_DIV_L,                 43);
    set_cost (t, IDX_DIV_W,                 27);
    set_cost (t, IDX_LSL_SHIFT,              4);
    set_cost (t, IDX_LSL_SHIFT_STEP,         2);
}

static void
init_cost_size_68020 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = 0;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_DISP,      1);
    set_cost (t, IDX_MEM_OTHER,              1);
    set_cost (t, IDX_CONST_INT_Q,           20);
    set_cost (t, IDX_CONST_INT_W,            1);
    set_cost (t, IDX_CONST_INT_L_1,          0);
    set_cost (t, IDX_CONST_INT_L_2,          0);
    set_cost (t, IDX_CONST_DOUBLE,           9);
    set_cost (t, IDX_SYMBOL,                12);
    set_cost (t, IDX_PLUS_REG_REG,           5);
    set_cost (t, IDX_PLUS_REG_CONSTQ,       20);
    set_cost (t, IDX_PLUS_REG_CONSTL,        1);
    set_cost (t, IDX_PLUS_OTHER,            12);
    set_cost (t, IDX_LOGIC_REG_CONST,        8);
    set_cost (t, IDX_SHIFT_CONST,            2);
    set_cost (t, IDX_SET_REG_REG,            1);
    set_cost (t, IDX_CALL_OTHER,            11);
    set_cost (t, IDX_MULU_L,                43);
    set_cost (t, IDX_MULU_W,                27);
    set_cost (t, IDX_DIV_L,                 43);
    set_cost (t, IDX_DIV_W,                 27);
    set_cost (t, IDX_ADD_L,                  2);
    set_cost (t, IDX_SUB_L,                  2);
    set_cost (t, IDX_MOVE_L,                 2);
    set_cost (t, IDX_MOVEQ,                  2);
    set_cost (t, IDX_LSL_SHIFT,              2);
    set_cost (t, IDX_LSL_SHIFT_STEP,         2);
}

/* ============================================
   68030 - SPEED / SIZE
   ============================================ */
static void
init_cost_speed_68030 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = 0;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_DISP,     11);
    set_cost (t, IDX_MEM_PLUS_REG_REG,       4);
    set_cost (t, IDX_MEM_OTHER,              3);
    set_cost (t, IDX_CONST_INT_Q,            0);
    set_cost (t, IDX_CONST_INT_W,           15);
    set_cost (t, IDX_CONST_INT_L_1,         16);
    set_cost (t, IDX_CONST_INT_L_2,         16);
    set_cost (t, IDX_SYMBOL,                 7);
    set_cost (t, IDX_PLUS_REG_REG,           5);
    set_cost (t, IDX_PLUS_REG_CONSTQ,        1);
    set_cost (t, IDX_PLUS_REG_CONSTL,        5);
    set_cost (t, IDX_PLUS_OTHER,            14);
    set_cost (t, IDX_LOGIC_REG_CONST,        7);
    set_cost (t, IDX_SHIFT_CONST,            4);
    set_cost (t, IDX_SET_REG_REG,            1);
    set_cost (t, IDX_CALL_OTHER,             8);
    set_cost (t, IDX_MULU_L,                43);
    set_cost (t, IDX_MULU_W,                27);
    set_cost (t, IDX_DIV_L,                 44);
    set_cost (t, IDX_DIV_W,                 27);
    set_cost (t, IDX_LSL_SHIFT,              4);
    set_cost (t, IDX_LSL_SHIFT_STEP,         2);
}

static void
init_cost_size_68030 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = 0;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_DISP,      1);
    set_cost (t, IDX_MEM_OTHER,              1);
    set_cost (t, IDX_CONST_INT_Q,           20);
    set_cost (t, IDX_CONST_INT_W,            1);
    set_cost (t, IDX_CONST_INT_L_1,          0);
    set_cost (t, IDX_CONST_INT_L_2,          0);
    set_cost (t, IDX_CONST_DOUBLE,           9);
    set_cost (t, IDX_SYMBOL,                12);
    set_cost (t, IDX_PLUS_REG_REG,           5);
    set_cost (t, IDX_PLUS_REG_CONSTQ,       20);
    set_cost (t, IDX_PLUS_REG_CONSTL,        1);
    set_cost (t, IDX_PLUS_OTHER,            12);
    set_cost (t, IDX_LOGIC_REG_CONST,        8);
    set_cost (t, IDX_SHIFT_CONST,            2);
    set_cost (t, IDX_SET_REG_REG,            1);
    set_cost (t, IDX_CALL_OTHER,            11);
    set_cost (t, IDX_MULU_L,                44);
    set_cost (t, IDX_MULU_W,                27);
    set_cost (t, IDX_DIV_L,                 44);
    set_cost (t, IDX_DIV_W,                 27);
    set_cost (t, IDX_ADD_L,                  2);
    set_cost (t, IDX_SUB_L,                  2);
    set_cost (t, IDX_MOVE_L,                 2);
    set_cost (t, IDX_MOVEQ,                  2);
    set_cost (t, IDX_LSL_SHIFT,              4);
    set_cost (t, IDX_LSL_SHIFT_STEP,         2);
}

/* ============================================
   68040 - SPEED / SIZE
   ============================================ */
static void
init_cost_speed_68040 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = 0;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_DISP,      1);
    set_cost (t, IDX_MEM_PLUS_REG_REG,       5);
    set_cost (t, IDX_MEM_OTHER,              3);
    set_cost (t, IDX_CONST_INT_Q,            4);
    set_cost (t, IDX_CONST_INT_W,           19);
    set_cost (t, IDX_CONST_INT_L_1,          4);
    set_cost (t, IDX_CONST_INT_L_2,          4);
    set_cost (t, IDX_CONST_DOUBLE,           4);
    set_cost (t, IDX_SYMBOL,                26);
    set_cost (t, IDX_PLUS_REG_REG,          13);
    set_cost (t, IDX_PLUS_REG_CONSTQ,        1);
    set_cost (t, IDX_PLUS_REG_CONSTW,        2);
    set_cost (t, IDX_PLUS_REG_CONSTL,       15);
    set_cost (t, IDX_PLUS_OTHER,             8);
    set_cost (t, IDX_LOGIC_REG_CONST,       11);
    set_cost (t, IDX_SHIFT_CONST,            8);
    set_cost (t, IDX_SHIFT_CONST_W,          1);
    set_cost (t, IDX_SET_REG_REG,           13);
    set_cost (t, IDX_MULU_L,                20);
    set_cost (t, IDX_MULU_W,                14);
    set_cost (t, IDX_DIV_L,                 38);
    set_cost (t, IDX_DIV_W,                 20);
    set_cost (t, IDX_LSL_SHIFT,             22);
    set_cost (t, IDX_LSL_SHIFT_STEP,         0);
}

static void
init_cost_size_68040 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = 0;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_DISP,      1);
    set_cost (t, IDX_MEM_OTHER,              1);
    set_cost (t, IDX_CONST_INT_Q,           20);
    set_cost (t, IDX_CONST_INT_W,            1);
    set_cost (t, IDX_CONST_INT_L_1,          0);
    set_cost (t, IDX_CONST_INT_L_2,          0);
    set_cost (t, IDX_CONST_DOUBLE,           9);
    set_cost (t, IDX_SYMBOL,                12);
    set_cost (t, IDX_PLUS_REG_REG,           5);
    set_cost (t, IDX_PLUS_REG_CONSTQ,       20);
    set_cost (t, IDX_PLUS_REG_CONSTL,        1);
    set_cost (t, IDX_PLUS_OTHER,            12);
    set_cost (t, IDX_LOGIC_REG_CONST,        8);
    set_cost (t, IDX_SHIFT_CONST,            2);
    set_cost (t, IDX_SET_REG_REG,            1);
    set_cost (t, IDX_CALL_OTHER,            11);
    set_cost (t, IDX_MULU_L,                20);
    set_cost (t, IDX_MULU_W,                14);
    set_cost (t, IDX_DIV_L,                 38);
    set_cost (t, IDX_DIV_W,                 20);
    set_cost (t, IDX_ADD_L,                  1);
    set_cost (t, IDX_SUB_L,                  1);
    set_cost (t, IDX_MOVE_L,                 1);
    set_cost (t, IDX_MOVEQ,                  1);
    set_cost (t, IDX_LSL_SHIFT,              2);
    set_cost (t, IDX_LSL_SHIFT_STEP,         0);
}

/* ============================================
   68080 (Apollo) - SPEED / SIZE
   ============================================ */
static void
init_cost_speed_68080 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = 0;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_REG,       3);
    set_cost (t, IDX_MEM_OTHER,              5);
    set_cost (t, IDX_CONST_INT_Q,            8);
    set_cost (t, IDX_CONST_INT_L_1,          3);
    set_cost (t, IDX_CONST_INT_L_2,          3);
    set_cost (t, IDX_PLUS_REG_REG,          13);
    set_cost (t, IDX_PLUS_REG_CONSTL,        2);
    set_cost (t, IDX_PLUS_OTHER,            23);
    set_cost (t, IDX_SHIFT_CONST,            2);
    set_cost (t, IDX_SHIFT_CONST_W,         10);
    set_cost (t, IDX_BRANCH,                10);
    set_cost (t, IDX_SET_REG_REG,            5);
    set_cost (t, IDX_MULU_L,                 4);
    set_cost (t, IDX_MULU_W,                 3);
    set_cost (t, IDX_DIV_L,                 10);
    set_cost (t, IDX_DIV_W,                  6);
    set_cost (t, IDX_LSL_SHIFT,              1);
    set_cost (t, IDX_LSL_SHIFT_STEP,         1);
}

static void
init_cost_size_68080 (int t[IDX_COUNT])
{
    for (int i = 0; i < IDX_COUNT; ++i) t[i] = 0;

    set_cost (t, IDX_REG_SRC,                4);
    set_cost (t, IDX_REG_DST,                0);
    set_cost (t, IDX_MEM_PLUS_REG_DISP,      1);
    set_cost (t, IDX_MEM_OTHER,              1);
    set_cost (t, IDX_CONST_INT_Q,           20);
    set_cost (t, IDX_CONST_INT_W,            1);
    set_cost (t, IDX_CONST_INT_L_1,          0);
    set_cost (t, IDX_CONST_INT_L_2,          0);
    set_cost (t, IDX_CONST_DOUBLE,           9);
    set_cost (t, IDX_SYMBOL,                12);
    set_cost (t, IDX_PLUS_REG_REG,           5);
    set_cost (t, IDX_PLUS_REG_CONSTQ,       20);
    set_cost (t, IDX_PLUS_REG_CONSTL,        1);
    set_cost (t, IDX_PLUS_OTHER,            12);
    set_cost (t, IDX_LOGIC_REG_CONST,        8);
    set_cost (t, IDX_SHIFT_CONST,            2);
    set_cost (t, IDX_SET_REG_REG,            1);
    set_cost (t, IDX_CALL_OTHER,            11);
    set_cost (t, IDX_MULU_L,                 4);
    set_cost (t, IDX_MULU_W,                 3);
    set_cost (t, IDX_DIV_L,                 10);
    set_cost (t, IDX_DIV_W,                  6);
    set_cost (t, IDX_LSL_SHIFT,              1);
    set_cost (t, IDX_LSL_SHIFT_STEP,         1);
}

/* ============================================
   Cost map and initialization
   ============================================ */
#if M68K_SWITCHES_MODE
struct cost_map_entry { const char *name; int index; };
static const struct cost_map_entry cost_map[] = {
    { "cost_reg_src",           IDX_REG_SRC },
    { "cost_reg_dst",           IDX_REG_DST },
    { "cost_mem_plus_reg_disp", IDX_MEM_PLUS_REG_DISP },
    { "cost_mem_plus_reg_reg",  IDX_MEM_PLUS_REG_REG },
    { "cost_mem_other",         IDX_MEM_OTHER },
    { "cost_const_int_q",       IDX_CONST_INT_Q },
    { "cost_const_int_w",       IDX_CONST_INT_W },
    { "cost_const_int_l_1",     IDX_CONST_INT_L_1 },
    { "cost_const_int_l_2",     IDX_CONST_INT_L_2 },
    { "cost_const_double",      IDX_CONST_DOUBLE },
    { "cost_symbol",            IDX_SYMBOL },
    { "cost_plus_reg_reg",      IDX_PLUS_REG_REG },
    { "cost_plus_reg_constq",   IDX_PLUS_REG_CONSTQ },
    { "cost_plus_reg_constw",   IDX_PLUS_REG_CONSTW },
    { "cost_plus_reg_constl",   IDX_PLUS_REG_CONSTL },
    { "cost_plus_other",        IDX_PLUS_OTHER },
    { "cost_logic_reg_const",   IDX_LOGIC_REG_CONST },
    { "cost_shift_const",       IDX_SHIFT_CONST },
    { "cost_shift_const_w",     IDX_SHIFT_CONST_W },
    { "cost_branch",            IDX_BRANCH },
    { "cost_set_reg_reg",       IDX_SET_REG_REG },
    { "cost_call_other",        IDX_CALL_OTHER },
    { "cost_mulu_l",            IDX_MULU_L },
    { "cost_mulu_w",            IDX_MULU_W },
    { "cost_div_l",             IDX_DIV_L },
    { "cost_div_w",             IDX_DIV_W },
    { "cost_add_l",             IDX_ADD_L },
    { "cost_sub_l",             IDX_SUB_L },
    { "cost_move_l",            IDX_MOVE_L },
    { "cost_moveq",             IDX_MOVEQ },
    { "cost_lsl_shift",         IDX_LSL_SHIFT },
    { "cost_lsl_shift_step",    IDX_LSL_SHIFT_STEP },

    { "cost_mem_long_extra",    IDX_MEM_LONG_EXTRA },
    { "cost_mem_predec_extra",  IDX_MEM_PREDEC_EXTRA },
    { "cost_clr_long_penalty",  IDX_CLR_LONG_PENALTY },
    { "cost_clr_word_penalty",  IDX_CLR_WORD_PENALTY },
    { "cost_call_reg_offset",   IDX_CALL_REG_OFFSET },
    { "cost_call_plus_offset",  IDX_CALL_PLUS_OFFSET },
    { "cost_call_sym_offset",   IDX_CALL_SYM_OFFSET },
    { "cost_plus_lea_offset",   IDX_PLUS_LEA_OFFSET },
    { "cost_plus_long_extra",   IDX_PLUS_LONG_EXTRA },
    { "cost_plus_short_extra",  IDX_PLUS_SHORT_EXTRA },
    { "cost_logic_long_extra",  IDX_LOGIC_LONG_EXTRA },
    { "cost_logic_word_extra",  IDX_LOGIC_WORD_EXTRA },
    { "cost_mult_68000_base",   IDX_MULT_68000_BASE },
    { "cost_mult_68000_bits",   IDX_MULT_68000_BITS },
    { "cost_mult_68000_trans",  IDX_MULT_68000_TRANS },
    { "cost_mult_68000_word_base", IDX_MULT_68000_WORD_BASE },
    { "cost_mult_68000_long_mul",  IDX_MULT_68000_LONG_MUL },
    { "cost_mult_68000_word_mul",  IDX_MULT_68000_WORD_MUL },
    { "cost_extend_extra",      IDX_EXTEND_EXTRA },
};

#endif

static int *active_speed;
static int *active_size;

static void
m68k_select_cost_tables (void)
{
    if (TUNE_68000_10)
      {
        init_cost_speed_68000_10 (cost_speed_68000_10);
        init_cost_size_68000_10  (cost_size_68000_10);
        active_speed = cost_speed_68000_10;
        active_size  = cost_size_68000_10;
      }
    else if (m68k_tune == u68020 || m68k_tune == u68020_40)
      {
        init_cost_speed_68020 (cost_speed_68020);
        init_cost_size_68020  (cost_size_68020);
        active_speed = cost_speed_68020;
        active_size  = cost_size_68020;
      }
    else if (m68k_tune == u68030)
      {
        init_cost_speed_68030 (cost_speed_68030);
        init_cost_size_68030  (cost_size_68030);
        active_speed = cost_speed_68030;
        active_size  = cost_size_68030;
      }
    else if (m68k_tune == u68040)
      {
        init_cost_speed_68040 (cost_speed_68040);
        init_cost_size_68040  (cost_size_68040);
        active_speed = cost_speed_68040;
        active_size  = cost_size_68040;
      }
    else
      {
        init_cost_speed_68080 (cost_speed_68080);
        init_cost_size_68080  (cost_size_68080);
        active_speed = cost_speed_68080;
        active_size  = cost_size_68080;
      }
}

#if M68K_SWITCHES_MODE
static void
load_cost_overrides (const char *filename, int table[])
{
    FILE *f = fopen (filename, "r");
    if (!f) return;

    char line[256];
    int  lineno = 0;

    while (fgets (line, sizeof (line), f))
      {
        char *p = line;
        lineno++;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\n' || *p == '\r' || *p == 0) continue;

        char name[128];
        int  value;
        if (sscanf (p, " %127[^= \t] = %d", name, &value) != 2) {
            fprintf (stderr, "%s:%d: syntax error: %s", filename, lineno, line);
            continue;
        }

        for (char *q = name; *q; q++) if (*q >= 'A' && *q <= 'Z') *q += 0x20;

        int found = 0;
        for (size_t i = 0; i < sizeof (cost_map) / sizeof (cost_map[0]); i++) {
            if (strcmp (cost_map[i].name, name) == 0) {
                table[cost_map[i].index] = value;
                found = 1;
                break;
            }
        }
        if (!found) fprintf (stderr, "%s:%d: unknown cost name '%s'\n", filename, lineno, name);
      }
    fclose (f);
}

#endif

static void
m68k_init_costs (bool speed ATTRIBUTE_UNUSED)
{
    m68k_select_cost_tables ();
    #if M68K_SWITCHES_MODE
    const char *env = getenv ("M68K_COSTS_FILE");
    if (env && *env) {
        load_cost_overrides (env, active_speed);
        load_cost_overrides (env, active_size);
        return;
    }
    load_cost_overrides ("m68k-costs-speed.txt", active_speed);
    load_cost_overrides ("m68k-costs-size.txt",  active_size);
    #endif
}

#define COST_TABLE  (speed ? active_speed : active_size)
#define COST_IS_UNDEFINED(idx)  (COST_TABLE[idx] < 0)

#define COST_MEM_PLUS_REG_DISP  (COST_TABLE[IDX_MEM_PLUS_REG_DISP])
#define COST_MEM_PLUS_REG_REG   (COST_TABLE[IDX_MEM_PLUS_REG_REG])
#define COST_MEM_OTHER          (COST_TABLE[IDX_MEM_OTHER])
#define COST_CONST_INT_Q        (COST_TABLE[IDX_CONST_INT_Q])
#define COST_CONST_INT_W        (COST_TABLE[IDX_CONST_INT_W])
#define COST_CONST_INT_L_1      (COST_TABLE[IDX_CONST_INT_L_1])
#define COST_CONST_INT_L_2      (COST_TABLE[IDX_CONST_INT_L_2])
#define COST_CONST_DOUBLE       (COST_TABLE[IDX_CONST_DOUBLE])
#define COST_SYMBOL             (COST_TABLE[IDX_SYMBOL])
#define COST_PLUS_REG_REG       (COST_TABLE[IDX_PLUS_REG_REG])
#define COST_PLUS_REG_CONSTQ    (COST_TABLE[IDX_PLUS_REG_CONSTQ])
#define COST_PLUS_REG_CONSTW    (COST_TABLE[IDX_PLUS_REG_CONSTW])
#define COST_PLUS_REG_CONSTL    (COST_TABLE[IDX_PLUS_REG_CONSTL])
#define COST_PLUS_OTHER         (COST_TABLE[IDX_PLUS_OTHER])
#define COST_LOGIC_REG_CONST    (COST_TABLE[IDX_LOGIC_REG_CONST])
#define COST_SHIFT_CONST        (COST_TABLE[IDX_SHIFT_CONST])
#define COST_SHIFT_CONST_W      (COST_TABLE[IDX_SHIFT_CONST_W])
#define COST_BRANCH             (COST_TABLE[IDX_BRANCH])
#define COST_SET_REG_REG        (COST_TABLE[IDX_SET_REG_REG])
#define COST_CALL_OTHER         (COST_TABLE[IDX_CALL_OTHER])

#define COST_MULU_L             (COST_TABLE[IDX_MULU_L])
#define COST_MULU_W             (COST_TABLE[IDX_MULU_W])
#define COST_DIV_L              (COST_TABLE[IDX_DIV_L]*4)
#define COST_DIV_W              (COST_TABLE[IDX_DIV_W]*4)
#define COST_ADD_L              (COST_TABLE[IDX_ADD_L])
#define COST_SUB_L              (COST_TABLE[IDX_SUB_L])
#define COST_MOVE_L             (COST_TABLE[IDX_MOVE_L])
#define COST_MOVEQ              (COST_TABLE[IDX_MOVEQ])
#define COST_LSL_SHIFT(shift)   (COST_TABLE[IDX_LSL_SHIFT] + (shift) * COST_TABLE[IDX_LSL_SHIFT_STEP])

static int
const_int_cost (HOST_WIDE_INT i)
{
  switch (m68k_const_method (i))
    {
    case MOVQ: return 0;
    case MVZ: case MVS: case NOTB: case NOTW: case NEGW: case SWAP: return 1;
    case MOVL: return 2;
    default: gcc_unreachable ();
    }
}

/* ============================================
   SBF Engine (Fully table-driven)
   ============================================ */
static bool
m68k_68000_10_costs_engine (rtx x, machine_mode mode, int outer_code,
                            int opno, int *total, bool speed)
{
    int code = GET_CODE (x);
    int total2 = 0;

    switch (code)
      {
      case CALL:
        {
          rtx a = XEXP(x, 0);
          if (MEM_P(a))
            {
              rtx b = XEXP(a, 0);
              if (REG_P(b)) { *total = COST_CALL_OTHER + COST_TABLE[IDX_CALL_REG_OFFSET]; return true; }
              if (GET_CODE(b) == PLUS && REG_P(XEXP(b, 0))) { *total = COST_CALL_OTHER + COST_TABLE[IDX_CALL_PLUS_OFFSET]; return true; }
              else if (SYMBOL_REF_P(b) || GET_CODE(b) == CONST_INT) { *total = COST_CALL_OTHER + COST_TABLE[IDX_CALL_SYM_OFFSET]; return true; }
            }
          if (COST_IS_UNDEFINED (IDX_CALL_OTHER)) return false;
          *total = COST_CALL_OTHER;
          return true;
        }
      case EQ: case NE: case GE: case GT: case GTU: case LE: case LT:
        if (COST_IS_UNDEFINED (IDX_BRANCH)) return false;
        *total = COST_BRANCH;
        return true;
      case ZERO_EXTRACT:
        if (outer_code == COMPARE && GET_CODE (XEXP (x, 1)) == CONST_INT && INTVAL (XEXP (x, 1)) == 1) {
            *total = 2; return true;
        }
        return false;
      case POST_INC:
      case TRUNCATE:
        *total = 0;
        return true;
      case PRE_DEC:
        *total = 2;
        return true;
      case REG:
        if (opno) {
            if (COST_IS_UNDEFINED (IDX_REG_SRC)) return false;
            *total = COST_TABLE[IDX_REG_SRC];
        } else {
            if (COST_IS_UNDEFINED (IDX_REG_DST)) return false;
            *total = COST_TABLE[IDX_REG_DST];
        }
        return true;
      case SYMBOL_REF:
        if (COST_IS_UNDEFINED (IDX_SYMBOL)) return false;
        *total = COST_SYMBOL;
        return true;
      case IF_THEN_ELSE:
        *total = 10;
        return true;
      case SET:
        {
          rtx a = XEXP(x, 0);
          rtx b = XEXP(x, 1);
          if (m68k_68000_10_costs_engine (a, mode, code, 0, total, speed)
              && m68k_68000_10_costs_engine (b, mode, code, 1, &total2, speed))
            {
              *total += total2;
              if (MEM_P(a) && GET_CODE(b) == CONST_INT && INTVAL(b) == 0) {
                  *total += GET_MODE_SIZE(mode) > 2 ? COST_TABLE[IDX_CLR_LONG_PENALTY] : COST_TABLE[IDX_CLR_WORD_PENALTY];
                  if (GET_CODE(XEXP(a, 0)) == PRE_DEC) *total += 2;
              }
              return true;
            }
          break;
        }
      case CONST:
        {
          rtx a = XEXP(x, 0);
          if (GET_CODE(a) == PLUS && SYMBOL_REF_P(XEXP(a,0)) && GET_CODE(XEXP(a, 1)) == CONST_INT) {
              if (COST_IS_UNDEFINED (IDX_SYMBOL)) return false;
              *total = COST_SYMBOL;
              return true;
          }
          *total = 0;
          break;
        }
      case MULT:
        {
          rtx a = XEXP(x, 0);
          rtx b = XEXP(x, 1);

          if (GET_CODE(b) == CONST_INT)
            {
              int n = INTVAL(b);
              if (n < 0) n = -n;
              int x_shift = 0;
              unsigned y = 1;
              for (; y < (unsigned)n; y += y, ++x_shift) ;
              if (y == (unsigned)n) {
                  *total = (GET_MODE_SIZE(mode) > 2 ? COST_SHIFT_CONST : COST_SHIFT_CONST_W);
                  *total += COST_TABLE[IDX_LSL_SHIFT_STEP] * x_shift;
                  return true;
              }
            }

          if (GET_CODE (a) == ZERO_EXTEND || GET_CODE (a) == SIGN_EXTEND) {
              *total = 0;
              mode = HImode;
          } else if (!m68k_68000_10_costs_engine (a, mode, code, 0, total, speed)) {
              break;
          }

          if (speed) {
              int f = (GET_MODE_SIZE(mode) > 2) ? COST_TABLE[IDX_MULT_68000_LONG_MUL] : COST_TABLE[IDX_MULT_68000_WORD_MUL];
              if (GET_CODE(b) == CONST_INT && GET_MODE_SIZE(mode) == 2) {
                  int i = INTVAL(b);
                  if (i < 0) i = -i;
                  int bits = 0, l = 0;
                  if (i > 0) {
                      if (GET_CODE (a) == ZERO_EXTEND) {
                          while (i) { ++bits; i>>=1; }
                      } else {
                          while (i || l) { if ((i&1) != l) { l = !l; ++bits; } i >>= 1; }
                      }
                      f = COST_TABLE[IDX_MULT_68000_WORD_BASE] + 2 * bits;
                  }
              }
              *total += f;
          } else {
              *total = GET_MODE_SIZE(mode) > 2 ? COST_TABLE[IDX_MULT_68000_LONG_MUL] : COST_TABLE[IDX_MULT_68000_WORD_MUL];
          }
          return true;
        }
      case DIV:
      case UDIV:
      case MOD:
      case UMOD:
        {
          rtx a = XEXP(x, 0);
          if (m68k_68000_10_costs_engine (a, mode, code, 0, total, speed)) {
              if (speed) *total += GET_MODE_SIZE(mode) > 2 ? COST_DIV_L : COST_DIV_W;
              else *total += GET_MODE_SIZE(mode) > 2 ? COST_DIV_L : COST_DIV_W;
              return true;
          }
          break;
        }
      case ASHIFT:
      case ASHIFTRT:
      case LSHIFTRT:
        {
          rtx a = XEXP(x, 0);
          rtx b = XEXP(x, 1);

          /* An immediate SI register shift by 1..8 is one instruction word
             on the 68000 (including ADD.L for a left shift by one).  Its
             encoded size does not grow with the shift count.  Keep the
             existing timing model and all other targets/modes unchanged.  */
          if (optimize_size && !speed && m68k_tune == u68000 && mode == SImode
              && REG_P (a) && CONST_INT_P (b)
              && INTVAL (b) >= 1 && INTVAL (b) <= 8)
            {
              *total = COSTS_N_INSNS (1);
              return true;
            }

          if (REG_P(a) || SUBREG_P(a)) {

              if (GET_CODE(b) == CONST_INT) {
                  int sv = INTVAL(b);
                  *total = (GET_MODE_SIZE(mode) > 2 ? COST_SHIFT_CONST : COST_SHIFT_CONST_W);

                  // non linear - based on swap
                  if (sv < 16) {
                      *total += COST_TABLE[IDX_LSL_SHIFT_STEP] * sv;
                  } else {
                      *total += COST_TABLE[IDX_LSL_SHIFT_STEP] * (4 + (sv - 16));
                  }
                  return true;
              }
              *total = (GET_MODE_SIZE(mode) > 2 ? COST_SHIFT_CONST : COST_SHIFT_CONST_W) ;
              if (speed) *total += 16;
              return true;
          }
          if (m68k_68000_10_costs_engine (XEXP(x, 0), mode, code, 0, total, speed)) {
              *total += 8;
              return true;
          }
          break;
        }
      case SUBREG:
      case STRICT_LOW_PART:
        if (GET_CODE (XEXP(x, 0)) == SUBREG || GET_CODE (XEXP(x, 0)) == STRICT_LOW_PART) return false;
        return m68k_68000_10_costs_engine (XEXP(x, 0), GET_MODE(XEXP(x, 0)), outer_code, 0, total, speed);
      case ZERO_EXTEND:
      case SIGN_EXTEND:
        if (GET_CODE (XEXP(x, 0)) == ZERO_EXTEND || GET_CODE (XEXP(x, 0)) == SIGN_EXTEND) return false;
        if (m68k_68000_10_costs_engine (XEXP(x, 0), GET_MODE(XEXP(x, 0)), outer_code, 0, total, speed)) {
            *total += COST_TABLE[IDX_EXTEND_EXTRA];
            return true;
        }
        return false;
      case CONST_INT:
        {
          int kind = const_int_cost (INTVAL (x));
          bool is_long = GET_MODE_SIZE (mode) > 2;
          if (kind == 0) {
            if (COST_IS_UNDEFINED (IDX_CONST_INT_Q)) return false;
            *total = COST_TABLE[IDX_CONST_INT_Q];
          } else if (!is_long) {
            if (COST_IS_UNDEFINED (IDX_CONST_INT_W)) return false;
            *total = COST_TABLE[IDX_CONST_INT_W];
          } else if (kind == 1) {
            if (COST_IS_UNDEFINED (IDX_CONST_INT_L_1)) return false;
            *total = COST_TABLE[IDX_CONST_INT_L_1];
          } else {
            if (COST_IS_UNDEFINED (IDX_CONST_INT_L_2)) return false;
            *total = COST_TABLE[IDX_CONST_INT_L_2];
          }
          return true;
        }
      case NEG:
      case NOT:
        {
          rtx a = XEXP(x, 0);
          if (REG_P(a) || SUBREG_P(a)) {
              *total = GET_MODE_SIZE(mode) > 2 ? 6 : 4;
              return true;
          }
          if (m68k_68000_10_costs_engine (a, mode, code, 0, total, speed)) {
              *total += GET_MODE_SIZE(mode) > 2 ? 12 : 8;
              return true;
          }
          break;
        }
      case MINUS:
      case PLUS:
        {
          rtx a = XEXP(x, 0);
          rtx b = XEXP(x, 1);

          if (GET_CODE(a) == PLUS && GET_CODE(b) == CONST_INT && (unsigned)(INTVAL(b) + 128) < 255) {
              if (REG_P(XEXP(a,0)) && REG_P(XEXP(a,1))) {
                  if (COST_IS_UNDEFINED (IDX_PLUS_REG_REG)) return false;
                  *total = COST_PLUS_REG_REG + COST_TABLE[IDX_PLUS_LEA_OFFSET];
                  return true;
              }
          }

          if (SUBREG_P(a)) a = XEXP(a, 0);
          if (SUBREG_P(b)) b = XEXP(b, 0);

          if (REG_P(a) && REGNO(a) >= 8 && REGNO(a) < 16) {
              if (GET_CODE(b) == CONST && GET_CODE(XEXP(b, 0)) == UNSPEC) { *total = 8; return true; }
              if (GET_CODE(b) == CONST_INT && (unsigned)(INTVAL(b) + 32768) < 65535) { *total = 8; return true; }
          }

          if (REG_P(a) && REG_P(b)) {
              if (COST_IS_UNDEFINED (IDX_PLUS_REG_REG)) return false;
              *total = GET_MODE_SIZE(mode) > 2 ? COST_PLUS_REG_REG + 4 : COST_PLUS_REG_REG;
              return true;
          }

          if (GET_CODE(b) == CONST_INT) {
              int i = INTVAL(b);
              if (REG_P(a) && (USE_MOVQ(i) || (REGNO(a) >= 8 && REGNO(a) < 16 && (unsigned)(INTVAL(b) + 32768) < 65535))) {
                  if (COST_IS_UNDEFINED (IDX_PLUS_REG_CONSTQ)) return false;
                  *total = GET_MODE_SIZE(mode) > 2 ? COST_PLUS_REG_CONSTQ + 4 : COST_PLUS_REG_CONSTQ;
              } else {
                  if (COST_IS_UNDEFINED (IDX_PLUS_OTHER)) return false;
                  *total = GET_MODE_SIZE(mode) > 2 ? COST_PLUS_OTHER + 4 : COST_PLUS_OTHER;
              }
              return true;
          }

          if (GET_CODE(b) == SYMBOL_REF || (GET_CODE(b) == CONST && GET_CODE(XEXP(b,0)) == PLUS && GET_CODE(XEXP(XEXP(b,0),0)) == SYMBOL_REF)) {
              if (COST_IS_UNDEFINED (IDX_PLUS_OTHER)) return false;
              *total = COST_PLUS_OTHER + 4;
              return true;
          }

          if (!m68k_68000_10_costs_engine (a, mode, code, 0, total, speed)) break;
          if (!m68k_68000_10_costs_engine (b, mode, code, 1, &total2, speed)) break;

          *total += total2 + (GET_MODE_SIZE(mode) > 2 ? COST_TABLE[IDX_PLUS_LONG_EXTRA] : COST_TABLE[IDX_PLUS_SHORT_EXTRA]);
          return true;
        }
      case COMPARE:
      case AND:
      case XOR:
      case IOR:
        {
          rtx a = XEXP(x, 0);
          rtx b = XEXP(x, 1);
          if (GET_CODE(b) == CONST_INT) {
              if (REG_P(a)) {
                  if (COST_IS_UNDEFINED (IDX_LOGIC_REG_CONST)) return false;
                  *total = GET_MODE_SIZE(mode) > 2 ? COST_LOGIC_REG_CONST + 6 : COST_LOGIC_REG_CONST;
                  return true;
              }
              if (m68k_68000_10_costs_engine (a, mode, code, 0, total, speed)) {
                  *total += GET_MODE_SIZE(mode) > 2 ? 20 : 12;
                  return true;
              }
          } else if (REG_P(a)) {
              if (m68k_68000_10_costs_engine (b, mode, code, 1, total, speed)) {
                  *total += GET_MODE_SIZE(mode) > 2 ? 4 : (code == XOR ? 8 : 6);
                  return true;
              }
          } else if (REG_P(b)) {
              if (m68k_68000_10_costs_engine (a, mode, code, 1, total, speed)) {
                  *total += GET_MODE_SIZE(mode) > 2 ? 12 : 8;
                  return true;
              }
          }

          if (m68k_68000_10_costs_engine (a, mode, code, 0, total, speed)
              && m68k_68000_10_costs_engine (b, mode, code, 1, &total2, speed)) {
              *total += total2 + (GET_MODE_SIZE(mode) > 2 ? 4 : (code == XOR ? 8 : 6));
              return true;
          }
          break;
        }
      case MEM:
        {
          rtx y = XEXP (x, 0);
          int yc = GET_CODE (y);
          int idx = IDX_MEM_OTHER;

          if (yc == REG || yc == PRE_INC || yc == POST_INC || yc == POST_DEC || yc == PRE_DEC)
            idx = IDX_MEM_PLUS_REG_DISP;
          else if (yc == CONST_INT || yc == SYMBOL_REF
                   || (yc == PLUS && GET_CODE (XEXP (y, 0)) == SYMBOL_REF))
            idx = IDX_MEM_OTHER;
          else if (yc == PLUS)
            {
              rtx a0 = XEXP (y, 0);
              rtx a1 = XEXP (y, 1);
              if (REG_P (a0) && CONST_INT_P (a1))
                {
                  HOST_WIDE_INT off = INTVAL (a1);
                  idx = (off >= -32768 && off <= 32767)
                          ? IDX_MEM_PLUS_REG_DISP     /* (d16,An) - same cost as (An) */
                          : IDX_MEM_PLUS_REG_REG;     /* out-of-range: needs extra add */
                }
              else
                idx = IDX_MEM_PLUS_REG_REG;       /* (An,Xn) or (plus (plus reg reg) c) */
            }
          else
            idx = IDX_MEM_PLUS_REG_REG;

          if (COST_IS_UNDEFINED (idx)) return false;
          *total = COST_TABLE[idx];
          if (mode != QImode && mode != HImode)
            *total += COST_TABLE[IDX_MEM_LONG_EXTRA];
          if (opno && yc == PRE_DEC)
            *total += COST_TABLE[IDX_MEM_PREDEC_EXTRA];
          return true;
        }
      default:
        break;
      }
    return false;
}

/* ============================================
   Unified Engine (for other CPUs)
   ============================================ */
static int
m68k_mult_const_cost_common (HOST_WIDE_INT n, rtx dst, machine_mode mode ATTRIBUTE_UNUSED, bool speed ATTRIBUTE_UNUSED)
{
    if (n == 0 || n == 1) return COST_MOVEQ;
    unsigned HOST_WIDE_INT un = (n < 0) ? -(unsigned HOST_WIDE_INT) n : (unsigned HOST_WIDE_INT) n;

    if (n > 0) {
        int shift = exact_log2 (un);
        if (shift >= 0) return COST_LSL_SHIFT (shift);
    }
    if (n > 0 && un > 1 && (un & (un - 1)) == 1) {
        int shift = exact_log2 (un - 1);
        if (shift >= 0) return COST_LSL_SHIFT (shift) + COST_ADD_L;
    }
    if (n > 0 && un > 2 && (un & (un + 1)) == 0) {
        int shift = exact_log2 (un + 1);
        if (shift >= 0) return COST_LSL_SHIFT (shift) + COST_SUB_L;
    }

    int bits = 0;
    int l = 0;
    HOST_WIDE_INT nn = n;

    if (nn > 0) {
        if (GET_CODE (dst) == ZERO_EXTEND || REG_P (dst)) {
            while (nn) { if (nn & 1) ++bits; nn >>= 1; }
            if (bits == 1 && REG_P (dst)) return COST_MOVEQ;
        } else {
            while (nn || l) { if ((nn & 1) != l) { l = !l; ++bits; } nn >>= 1; }
        }
        return 12 + bits;
    }
    return -1;
}

static bool
m68k_costs_engine (rtx x, machine_mode mode, int outer_code ATTRIBUTE_UNUSED,
                   int opno ATTRIBUTE_UNUSED, int *total, bool speed)
{
    int code = GET_CODE (x);
    int total2;

    switch (code)
      {
      case PRE_DEC:
      case POST_INC:
      case IF_THEN_ELSE:
      case ZERO_EXTEND:
      case SIGN_EXTEND:
      case TRUNCATE:
      case ROTATE:
      case ROTATERT:
      case LABEL_REF:
      case SUBREG:
      case STRICT_LOW_PART:
      case NEG:
      case NOT:
        *total = 0;
        return true;

      case CONST_INT:
        {
          HOST_WIDE_INT v = INTVAL (x);
          int kind = const_int_cost (v);
          bool is_long = GET_MODE_SIZE (mode) > 2;
          int idx;
          if (kind == 0) idx = IDX_CONST_INT_Q;
          else if (!is_long) idx = IDX_CONST_INT_W;
          else if (kind == 1) idx = IDX_CONST_INT_L_1;
          else idx = IDX_CONST_INT_L_2;
          if (COST_IS_UNDEFINED (idx)) return false;
          *total = COST_TABLE[idx];
          return true;
        }

      case CONST_DOUBLE:
        if (COST_IS_UNDEFINED (IDX_CONST_DOUBLE)) return false;
        *total = COST_CONST_DOUBLE;
        return true;

      case SYMBOL_REF:
        if (COST_IS_UNDEFINED (IDX_SYMBOL)) return false;
        *total = COST_SYMBOL;
        return true;

      case CONST:
        {
          rtx inner = XEXP (x, 0);
          if (GET_CODE (inner) == PLUS && SYMBOL_REF_P (XEXP (inner, 0)) && CONST_INT_P (XEXP (inner, 1))) {
              if (COST_IS_UNDEFINED (IDX_SYMBOL)) return false;
              *total = COST_SYMBOL;
          } else {
              *total = 0;
          }
          return true;
        }

      case REG:
        if (opno) {
            if (COST_IS_UNDEFINED (IDX_REG_SRC)) return false;
            *total = COST_TABLE[IDX_REG_SRC];
        } else {
            if (COST_IS_UNDEFINED (IDX_REG_DST)) return false;
            *total = COST_TABLE[IDX_REG_DST];
        }
        return true;

      case MEM:
        {
          rtx addr = XEXP (x, 0);
          int idx = IDX_MEM_OTHER;
          switch (GET_CODE (addr))
            {
            case REG:
            case POST_INC:
            case PRE_DEC:
              idx = IDX_MEM_PLUS_REG_DISP;
              break;
            case PLUS:
              {
                rtx a = XEXP (addr, 0);
                rtx b = XEXP (addr, 1);
                if (REG_P (a) && CONST_INT_P (b)) {
                    HOST_WIDE_INT off = INTVAL (b);
                    idx = (off >= -32768 && off <= 32767) ? IDX_MEM_PLUS_REG_DISP : IDX_MEM_PLUS_REG_REG;
                } else if (REG_P (a) && REG_P (b)) idx = IDX_MEM_PLUS_REG_REG;
                break;
              }
            default:
              break;
            }
          if (COST_IS_UNDEFINED (idx)) return false;
          *total = COST_TABLE[idx];
          if (GET_MODE_SIZE (mode) > 2) *total += 2;
          return true;
        }

      case PLUS:
      case MINUS:
        {
          rtx a = XEXP (x, 0);
          rtx b = XEXP (x, 1);
          if (REG_P (a) && REG_P (b)) {
              if (COST_IS_UNDEFINED (IDX_PLUS_REG_REG)) return false;
              *total = COST_PLUS_REG_REG;
              return true;
          }
          if (REG_P (a) && CONST_INT_P (b)) {
              HOST_WIDE_INT v = INTVAL (b);
              int idx = IDX_PLUS_OTHER;
              if (v >= -8 && v <= 8) idx = IDX_PLUS_REG_CONSTQ;
              else if (REGNO (a) == SP_REG) {
                  if (COST_IS_UNDEFINED (IDX_PLUS_OTHER)) return false;
                  *total = 8;
                  return true;
              }
              else idx = (GET_MODE_SIZE (mode) > 2) ? IDX_PLUS_REG_CONSTL : IDX_PLUS_REG_CONSTW;
              if (COST_IS_UNDEFINED (idx)) return false;
              *total = COST_TABLE[idx];
              return true;
          }
          if (COST_IS_UNDEFINED (IDX_PLUS_OTHER)) return false;
          *total = (GET_CODE (a) == PLUS) ? COST_PLUS_OTHER : COST_PLUS_OTHER / 2;
          return true;
        }

      case AND:
      case IOR:
      case XOR:
      case COMPARE:
        {
          rtx a = XEXP (x, 0);
          rtx b = XEXP (x, 1);
          if (REG_P (a) && REG_P (b)) {
              *total = (code == XOR) ? 0 : 1;
              return true;
          }
          if ((REG_P (a) && CONST_INT_P (b)) || (REG_P (b) && CONST_INT_P (a))) {
              if (COST_IS_UNDEFINED (IDX_LOGIC_REG_CONST)) return false;
              *total = GET_MODE_SIZE (mode) > 2 ? COST_LOGIC_REG_CONST + 2 : COST_LOGIC_REG_CONST;
              return true;
          }
          *total = 0;
          return true;
        }

      case ASHIFT:
      case ASHIFTRT:
      case LSHIFTRT:
        {
          rtx a = XEXP (x, 0);
          rtx b = XEXP (x, 1);
          if (REG_P (a) && CONST_INT_P (b)) {
              int idx = GET_MODE_SIZE (mode) > 2 ? IDX_SHIFT_CONST : IDX_SHIFT_CONST_W;
              if (COST_IS_UNDEFINED (idx)) return false;
              *total = COST_TABLE[idx];
              return true;
          }
          *total = 0;
          return true;
        }

      case MULT:
        {
          rtx dst = XEXP (x, 0);
          rtx src = XEXP (x, 1);
          if (COST_IS_UNDEFINED (IDX_LSL_SHIFT) || COST_IS_UNDEFINED (IDX_LSL_SHIFT_STEP) || COST_IS_UNDEFINED (IDX_ADD_L) || COST_IS_UNDEFINED (IDX_SUB_L)) return false;

          if (CONST_INT_P (src)) {
              HOST_WIDE_INT n = INTVAL (src);
              int c = m68k_mult_const_cost_common (n, dst, mode, speed);
              if (c >= 0) {
                  *total = c;
                  return true;
              }
          }
          int idx = GET_MODE_SIZE (mode) > 2 ? IDX_MULU_L : IDX_MULU_W;
          if (COST_IS_UNDEFINED (idx)) return false;
          *total = (idx == IDX_MULU_L) ? COST_MULU_L : COST_MULU_W;
          return true;
        }

      case DIV:
      case UDIV:
      case MOD:
      case UMOD:
        {
          int idx = GET_MODE_SIZE (mode) > 2 ? IDX_DIV_L : IDX_DIV_W;
          if (COST_IS_UNDEFINED (idx)) return false;
          *total = (idx == IDX_DIV_L) ? COST_DIV_L : COST_DIV_W;
          return true;
        }

      case EQ:
      case NE:
      case LT:
      case LE:
      case GT:
      case GE:
      case LTU:
      case LEU:
      case GTU:
      case GEU:
        if (COST_IS_UNDEFINED (IDX_BRANCH)) return false;
        *total = COST_BRANCH;
        return true;

      case SET:
        {
          rtx dest = XEXP (x, 0);
          rtx src = XEXP (x, 1);
          if (REG_P (dest) && REG_P (src)) {
              if (COST_IS_UNDEFINED (IDX_SET_REG_REG)) return false;
              *total = COST_SET_REG_REG;
              return true;
          }
          if (!m68k_costs_engine (dest, mode, code, 0, total, speed)) return false;
          if (!m68k_costs_engine (src, mode, code, 1, &total2, speed)) return false;
          *total += total2;
          if (CONST_INT_P (src) && INTVAL (src) == 0) {
              if (REG_P (dest) || MEM_P (dest)) *total = 0;
          }
          return true;
        }

      case CALL:
        {
          rtx mem = XEXP (x, 0);
          rtx b = XEXP (mem, 0);
          if (REG_P (b) || GET_CODE (b) == PLUS) *total = 0;
          else {
              if (COST_IS_UNDEFINED (IDX_CALL_OTHER)) return false;
              *total = COST_CALL_OTHER;
          }
          return true;
        }

      default:
        *total = 4;
        return true;
      }
}


static bool
m68k_rtx_costs (rtx x, machine_mode mode, int outer_code,
		int opno ATTRIBUTE_UNUSED,
		int *total, bool speed ATTRIBUTE_UNUSED)
{
  int code = GET_CODE (x);

  switch (code)
    {
    case CONST_INT:
      if (x == const0_rtx) *total = 0;
      else *total = const_int_cost (INTVAL (x));
      return true;

    case CONST:
    case LABEL_REF:
    case SYMBOL_REF:
      *total = 3;
      return true;

    case CONST_DOUBLE:
      if ((GET_RTX_CLASS (outer_code) == RTX_COMPARE
	   || GET_RTX_CLASS (outer_code) == RTX_COMM_COMPARE)
          && (x == CONST0_RTX (SFmode) || x == CONST0_RTX (DFmode)))
	*total = 4;
      else
	*total = 5;
      return true;

#define MULL_COST				\
  (TUNE_68060 ? 2				\
   : TUNE_68040 ? 5				\
   : (TUNE_CFV2 && TUNE_EMAC) ? 3		\
   : (TUNE_CFV2 && TUNE_MAC) ? 4		\
   : TUNE_CFV2 ? 8				\
   : TARGET_COLDFIRE ? 3 : 13)

#define MULW_COST				\
  (TUNE_68060 ? 2				\
   : TUNE_68040 ? 3				\
   : TUNE_68000_10 ? 5				\
   : (TUNE_CFV2 && TUNE_EMAC) ? 3		\
   : (TUNE_CFV2 && TUNE_MAC) ? 2		\
   : TUNE_CFV2 ? 8				\
   : TARGET_COLDFIRE ? 2 : 8)

#define DIVW_COST				\
  (TARGET_CF_HWDIV ? 11				\
   : TUNE_68000_10 || TARGET_COLDFIRE ? 12 : 27)

    case PLUS:
      if (mode == SImode
	  && GET_CODE (XEXP (x, 1)) == REG
	  && GET_CODE (XEXP (x, 0)) == MULT
	  && GET_CODE (XEXP (XEXP (x, 0), 0)) == REG
	  && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
	  && (INTVAL (XEXP (XEXP (x, 0), 1)) == 2
	      || INTVAL (XEXP (XEXP (x, 0), 1)) == 4
	      || INTVAL (XEXP (XEXP (x, 0), 1)) == 8))
	{
	    *total = COSTS_N_INSNS (TARGET_COLDFIRE ? 2 : 3);
	    return true;
	}
      return false;

    case ASHIFT:
    case ASHIFTRT:
    case LSHIFTRT:
      if (TUNE_68060)
	{
          *total = COSTS_N_INSNS(1);
	  return true;
	}
      if (TUNE_68000_10)
        {
	  if (GET_CODE (XEXP (x, 1)) == CONST_INT)
	    {
	      if (INTVAL (XEXP (x, 1)) < 16)
	        *total = COSTS_N_INSNS (2) + INTVAL (XEXP (x, 1)) / 2;
	      else
	        *total = COSTS_N_INSNS (4) + (INTVAL (XEXP (x, 1)) - 16) / 2;
	    }
	  else
	    *total = COSTS_N_INSNS (10);
	  return true;
        }
      if (GET_CODE (XEXP (x, 1)) == CONST_INT
	  && (INTVAL (XEXP (x, 1)) == 16))
	{
	  *total = COSTS_N_INSNS (2);
	  return true;
	}
      if (GET_CODE (XEXP (x, 1)) == CONST_INT
	  && !(INTVAL (XEXP (x, 1)) > 0
	       && INTVAL (XEXP (x, 1)) <= 8))
	{
	  *total = COSTS_N_INSNS (TARGET_COLDFIRE ? 1 : 3);
	  return true;
	}
      return false;

    case MULT:
      if ((GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
	   || GET_CODE (XEXP (x, 0)) == SIGN_EXTEND)
	  && mode == SImode)
        *total = COSTS_N_INSNS (MULW_COST);
      else if (mode == QImode || mode == HImode)
        *total = COSTS_N_INSNS (MULW_COST);
      else
        *total = COSTS_N_INSNS (MULL_COST);
      return true;

    case DIV:
    case UDIV:
    case MOD:
    case UMOD:
      if (mode == QImode || mode == HImode)
        *total = COSTS_N_INSNS (DIVW_COST);
      else if (TARGET_CF_HWDIV)
        *total = COSTS_N_INSNS (18);
      else
	*total = COSTS_N_INSNS (43);
      return true;

    case ZERO_EXTRACT:
      if (GET_RTX_CLASS (outer_code) == RTX_COMPARE
	  || GET_RTX_CLASS (outer_code) == RTX_COMM_COMPARE)
        *total = 0;
      return false;

    default:
      return false;
    }
}

bool
m68k_68000_10_costs (rtx x, machine_mode mode, int outer_code,
		int opno,
		int *total, bool speed )
{
  int code = GET_CODE (x);

  int total2 = 0;
  switch (code)
    {
    case CALL:
      {
	rtx a = XEXP(x, 0);
	if (MEM_P(a))
	  {
	    rtx b = XEXP(a, 0);
	    if (REG_P(b))
	      {
		*total = 16;
		return true;
	      }
	    if (GET_CODE(b) == PLUS)
	      {
		if (REG_P(XEXP(b, 0)))
		  {
		    *total = 18;
		    return true;
		  }
	      }
	    else if (SYMBOL_REF_P(b) || GET_CODE(b) == CONST_INT)
	      {
		*total = 20;
		return true;
	      }
	  }
	*total = 22;
	return true;
      }
    case EQ:
    case NE:
    case GE:
    case GT:
    case GTU:
    case LE:
    case LT:
      return false;
    case ZERO_EXTRACT:
      if (outer_code == COMPARE && GET_CODE (XEXP (x, 1)) == CONST_INT && INTVAL (XEXP (x, 1)) == 1)
      {
    	*total = 2;
    	return true;
      }
      return false;
    case POST_INC:
    case TRUNCATE:
      *total = 0;
      return true;
    case PRE_DEC:
      *total = 2;
      return true;
    case REG:
      *total = opno ? 4 : 0;
      return true;
    case SYMBOL_REF:
      *total = 12;
      return true;
    case IF_THEN_ELSE:
      *total = 10;
      return true;
    case SET:
      {
	rtx a = XEXP(x, 0);
	rtx b = XEXP(x, 1);
	if (m68k_68000_10_costs (a, mode, code, 0, total, speed)
	    && m68k_68000_10_costs (b, mode, code, 1, &total2, speed))
	  {
	    *total += total2;
	    if (MEM_P(a) && GET_CODE(b) == CONST_INT && INTVAL(b) == 0)
	      {
		*total += GET_MODE_SIZE(mode) > 2 ? 8 : 4;
		if (GET_CODE(XEXP(a, 0)) == PRE_DEC)
		  *total += 2;
	      }
	    return true;
	  }
	break;
      }
    case CONST:
      {
	rtx a = XEXP(x, 0);
	if (GET_CODE(a) == PLUS && SYMBOL_REF_P(XEXP(a,0)) && GET_CODE(XEXP(a, 1)) == CONST_INT)
	  {
	    *total = 12;
	    return true;
	  }
	*total = 0;
	break;
      }
    case MULT:
      {
	rtx a = XEXP(x, 0);
	rtx b = XEXP(x, 1);

	if (GET_CODE(b) == CONST_INT)
	  {
	    int n = INTVAL(b);
	    if (n < 0) n = -n;
	    int x = 0;
	    unsigned y = 1;
	    for (; y < n; y += y, ++x)
	      ;
	    if (y == n)
	      {
		*total = (GET_MODE_SIZE(mode) > 2 ? 8 : 6);
		*total += 2 * x;
		return true;
	      }
	  }

	if (GET_CODE (a) == ZERO_EXTEND
	     || GET_CODE (a) == SIGN_EXTEND)
	  {
	    *total = 0;
	    mode = HImode;
	  }
	else
	if (!m68k_68000_10_costs (a, mode, code, 0, total, speed))
	  break;

	if (speed)
	  {
	    int f = GET_MODE_SIZE(mode) > 2 ? 150 : 50;
	    if (GET_CODE(b) == CONST_INT && GET_MODE_SIZE(mode) == 2)
	      {
		unsigned i = INTVAL(b);
		int bits = 0, l = 0;
		if (i > 0)
		  {
		    if (GET_CODE (a) == ZERO_EXTEND)
		      while (i)
			{
			  ++bits;
			  i>>=1;
			}
		    else
		      while (i || l)
			{
			  if ((i&1) != l)
			    {
			      l = !l;
			      ++bits;
			    }
			  i >>= 1;
			}

		    f = 38 + 2 * bits;
		  }
	      }
	    *total += f;
	  }
	else
	  *total = GET_MODE_SIZE(mode) > 2 ? 48 : 16;

	return true;
      }
    case DIV:
    case UDIV:
    case MOD:
    case UMOD:
      {
	rtx a = XEXP(x, 0);
	if (m68k_68000_10_costs (a, mode, code, 0, total, speed))
	  {
	    if (speed)
	      *total += GET_MODE_SIZE(mode) > 2 ? 410 : 136;
	    else
	      *total += GET_MODE_SIZE(mode) > 2 ? 108 : 36;
	    return true;
	  }
	break;
      }
    case ASHIFT:
    case ASHIFTRT:
    case LSHIFTRT:
      {
	rtx a = XEXP(x, 0);
	rtx b = XEXP(x, 1);
	if (REG_P(a) || SUBREG_P(a))
	  {
	    if (GET_CODE(b) == CONST_INT)
	      {
		*total = (GET_MODE_SIZE(mode) > 2 ? 8 : 6);
		*total += 2 * INTVAL(b);
		return true;
	      }
	    *total = (GET_MODE_SIZE(mode) > 2 ? 8 : 6) ;
	    if (speed)
	      *total += 16;
	    return true;
	  }
	if (m68k_68000_10_costs (XEXP(x, 0), mode, code, 0, total, speed))
	  {
	    *total += 8;
	    return true;
	  }
	break;
      }
    case SUBREG:
    case STRICT_LOW_PART:
      return m68k_68000_10_costs (XEXP(x, 0), GET_MODE(XEXP(x, 0)), code, 0, total, speed);
    case ZERO_EXTEND:
    case SIGN_EXTEND:
      if (m68k_68000_10_costs (XEXP(x, 0), GET_MODE(XEXP(x, 0)), code, 0, total, speed))
	{
	  *total += 8;
	  return true;
	}
      return false;
    case CONST_INT:
      {
	int kind = const_int_cost (INTVAL(x));
	if (kind == 0)
	  *total = 4;
	else if (kind == 1)
	  *total = GET_MODE_SIZE(mode) > 2 ? 10 : 8;
	else
	  *total = GET_MODE_SIZE(mode) > 2 ? 12 : 8;
	return true;
      }
    case NEG:
    case NOT:
      {
	rtx a = XEXP(x, 0);
	if (REG_P(a))
	  {
	    *total = GET_MODE_SIZE(mode) > 2 ? 6 : 4;
	    return true;
	  }
	if (m68k_68000_10_costs (a, mode, code, 0, total, speed))
	  {
	    *total += GET_MODE_SIZE(mode) > 2 ? 12 : 8;
	    return true;
	  }
	break;
      }
    case MINUS:
    case PLUS:
      {
	rtx a = XEXP(x, 0);
	rtx b = XEXP(x, 1);

	if (GET_CODE(b) == MULT)
	  return false;

	if (GET_CODE(a) == PLUS && GET_CODE(b) == CONST_INT && (unsigned)(INTVAL(b) + 128) < 255)
	  {
	    if (REG_P(XEXP(a,0)) && REG_P(XEXP(a,1)))
	      {
		*total = 12;
		return true;
	      }
	  }

	if (REG_P(a) && REGNO(a) >= 8 && REGNO(a) < 16) {
	    if (GET_CODE(b) == CONST && GET_CODE(XEXP(b, 0)) == UNSPEC)
	    {
		    *total = 8;
		    return true;
	    }
	    if (GET_CODE(b) == CONST_INT && (unsigned)(INTVAL(b) + 32768) < 65535)
	    {
		    *total = 8;
		    return true;
	    }
	}

	if (REG_P(a) && REG_P(b))
	  {
	    *total = GET_MODE_SIZE(mode) > 2 ? 8 : 4;
	    return true;
	  }

	if (GET_CODE(b) == CONST_INT)
	  {
	    int i = INTVAL(b);
	    if (REG_P(a) && (USE_MOVQ(i) || (REGNO(a) >= 8 && REGNO(a) < 16 && (unsigned)(INTVAL(b) + 32768) < 65535)))
	      *total = GET_MODE_SIZE(mode) > 2 ? 8 : 4;
	    else
	      *total = GET_MODE_SIZE(mode) > 2 ? 16 : 8;
	    return true;
	  }

	if (GET_CODE(b) == SYMBOL_REF ||
		    (GET_CODE(b) == CONST && GET_CODE(XEXP(b,0)) == PLUS && GET_CODE(XEXP(XEXP(b,0),0)) == SYMBOL_REF))
	{
	    *total = 16;
	    return true;
	}

	if (!m68k_68000_10_costs (a, mode, code, 0, total, speed))
	  break;
	if (!m68k_68000_10_costs (b, mode, code, 1, &total2, speed))
	  break;

	*total += total2 + (GET_MODE_SIZE(mode) > 2 ? 6 : 4);
	return true;
      }
    case COMPARE:
    case AND:
    case XOR:
    case IOR:
      {
	rtx a = XEXP(x, 0);
	rtx b = XEXP(x, 1);
	if (GET_CODE(b) == CONST_INT)
	  {
	    if (REG_P(a))
	      {
		*total = GET_MODE_SIZE(mode) > 2 ? (code == AND ? 14 : 16) : 8;
		return true;
	      }
	    if (m68k_68000_10_costs (a, mode, code, 0, total, speed))
	      {
		*total += GET_MODE_SIZE(mode) > 2 ? 20 : 12;
		return true;
	      }
	  }
	else
	if (REG_P(a))
	  {
	    if (m68k_68000_10_costs (b, mode, code, 1, total, speed))
	      {
		*total += GET_MODE_SIZE(mode) > 2 ? 4 : (code == XOR ? 8 : 6);
		return true;
	      }
	  }
	else
	if (REG_P(b))
	  if (m68k_68000_10_costs (a, mode, code, 1, total, speed))
	    {
	      *total += GET_MODE_SIZE(mode) > 2 ? 12 : 8;
	      return true;
	    }

	if (m68k_68000_10_costs (a, mode, code, 0, total, speed)
	    && m68k_68000_10_costs (b, mode, code, 1, &total2, speed))
	  {
	    *total += total2 + (GET_MODE_SIZE(mode) > 2 ? 4 : (code == XOR ? 8 : 6));
	    return true;
	  }
	break;
      }
    case MEM:
      {
	rtx y = XEXP(x, 0);
	int yc = GET_CODE(y);
	if (yc == REG || yc == PRE_INC || yc == POST_INC || yc == POST_DEC || yc == PRE_DEC)
	  *total = 4;
	else if (yc == CONST_INT || yc == SYMBOL_REF || (yc == PLUS && GET_CODE(XEXP(y, 0)) == SYMBOL_REF))
	  *total = 12;
	else if (yc == PLUS && GET_CODE(XEXP(y, 0)) == PLUS)
	  *total = 10;
	else
	  *total = 8;

	if (mode != QImode && mode != HImode)
	  *total += 4;

	if (opno && yc == PRE_DEC)
	  *total += 2;

	return true;
      }
    default:
      break;
    }
  return false;
}

static bool
m68k_costs_engine0 (rtx x, machine_mode mode, int outer_code ,
                   int opno , int *total, bool speed) {
  bool r = m68k_68000_10_costs_engine (x, mode, outer_code, opno , total, speed);
  if (r && mode == DImode) {
    *total *= 2;
  }
  return r;
}


/* Debug harness: run both engines, compare totals, dump RTX on mismatch. */
static bool m68k_sbf_debug = false;
static bool m68k_sbf_debug_init = false;
static long m68k_sbf_mismatches = 0;

static bool
m68k_68000_10_costs_diff (rtx x, machine_mode mode, int outer_code,
                          int opno, int *total, bool speed)
{
    if (!m68k_sbf_debug_init)
      {
#if M68K_SWITCHES_MODE
        m68k_sbf_debug = getenv ("M68K_SBF_DIFF") != NULL;
#endif
        m68k_sbf_debug_init = true;
        if (m68k_sbf_debug)
          fprintf (stderr, "m68k: SBF diff mode enabled\n");
      }

    if (!m68k_sbf_debug)
      return m68k_costs_engine0 (x, mode, outer_code, opno, total, speed);

    int ref_total = 0;
    int new_total = 0;

    bool ref_ok = m68k_rtx_costs      (x, mode, outer_code, opno, &ref_total, speed);
    bool new_ok = m68k_costs_engine0  (x, mode, outer_code, opno, &new_total, speed);

    if (ref_ok != new_ok || (ref_ok && ref_total != new_total))
      {
        m68k_sbf_mismatches++;
        fprintf (stderr,
                 "=== SBF MISMATCH #%ld ===\n"
                 "  code=%s mode=%s outer_code=%d opno=%d speed=%d\n"
                 "  reference: ok=%d total=%d\n"
                 "  table:     ok=%d total=%d\n"
                 "  rtx: ",
                 m68k_sbf_mismatches,
                 GET_RTX_NAME (GET_CODE (x)),
                 GET_MODE_NAME (mode),
                 outer_code, opno, (int) speed,
                 ref_ok, ref_total,
                 new_ok, new_total);
        debug_rtx (x);
      }

    *total = new_total;
    return new_ok;
}

/* ============================================
   Exported entry point
   ============================================ */

bool
m68k_costs (rtx x,
	    machine_mode mode,
	    int outer_code ATTRIBUTE_UNUSED,
	    int opno ATTRIBUTE_UNUSED,
	    int *total,
	    bool speed)
{
    static bool initialized;
    static bool original_costs;
    static bool sbf_costs;
    if (!initialized)
      {
        initialized = true;

#if M68K_SWITCHES_MODE
        original_costs = getenv("M68K_ORIGINAL_COSTS") != 0;
        sbf_costs = getenv("M68K_SBF_COSTS") != 0;
#endif
        if (!original_costs)
        	m68k_init_costs (speed);
      }
    if (original_costs)
    	return m68k_rtx_costs(x, mode, outer_code, opno, total, speed);
    if (sbf_costs)
    	return m68k_68000_10_costs(x, mode, outer_code, opno, total, speed);

    if (TUNE_68000_10)
        return m68k_68000_10_costs_diff (x, mode, outer_code, opno, total, speed);

    return m68k_costs_engine0 (x, mode, outer_code, opno, total, speed);
}
