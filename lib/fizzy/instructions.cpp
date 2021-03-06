// Fizzy: A fast WebAssembly interpreter
// Copyright 2019-2020 The Fizzy Authors.
// SPDX-License-Identifier: Apache-2.0

#include "instructions.hpp"

namespace fizzy
{
namespace
{
constexpr InstructionMetrics instruction_metrics_table[256] = {
    // 5.4.1 Control instructions
    /* unreachable         = 0x00 */ {0, 0},
    /* nop                 = 0x01 */ {0, 0},
    /* block               = 0x02 */ {0, 0},
    /* loop                = 0x03 */ {0, 0},
    /* if_                 = 0x04 */ {1, -1},
    /* else_               = 0x05 */ {0, 0},
    /*                       0x06 */ {},
    /*                       0x07 */ {},
    /*                       0x08 */ {},
    /*                       0x09 */ {},
    /*                       0x0a */ {},
    /* end                 = 0x0b */ {0, 0},

    // TODO: After br code is unreachable so stack height should be reset (according to the target
    //       label?) Similarly for return.
    /* br                  = 0x0c */ {0, 0},
    /* br_if               = 0x0d */ {1, -1},
    /* br_table            = 0x0e */ {1, -1},
    /* return_             = 0x0f */ {0, 0},

    // TODO: Call require number of stack items equal the number of inputs in the target function
    //       (call_indirect one more item). They can return 0 or 1 item.
    //       Here is simplified setup without inspecting function types where we assume zero
    //       function arguments and one returned value.
    /* call                = 0x10 */ {0, 1},
    /* call_indirect       = 0x11 */ {1, 0},

    /*                       0x12 */ {},
    /*                       0x13 */ {},
    /*                       0x14 */ {},
    /*                       0x15 */ {},
    /*                       0x16 */ {},
    /*                       0x17 */ {},
    /*                       0x18 */ {},
    /*                       0x19 */ {},

    // 5.4.2 Parametric instructions
    /* drop                = 0x1a */ {1, -1},
    /* select              = 0x1b */ {3, -2},

    /*                       0x1c */ {},
    /*                       0x1d */ {},
    /*                       0x1e */ {},
    /*                       0x1f */ {},

    // 5.4.3 Variable instructions
    /* local_get           = 0x20 */ {0, 1},
    /* local_set           = 0x21 */ {1, -1},
    /* local_tee           = 0x22 */ {1, 0},
    /* global_get          = 0x23 */ {0, 1},
    /* global_set          = 0x24 */ {1, -1},

    /*                       0x25 */ {},
    /*                       0x26 */ {},
    /*                       0x27 */ {},

    // 5.4.4 Memory instructions
    /* i32_load            = 0x28 */ {1, 0},
    /* i64_load            = 0x29 */ {1, 0},
    /* f32_load            = 0x2a */ {1, 0},
    /* f64_load            = 0x2b */ {1, 0},
    /* i32_load8_s         = 0x2c */ {1, 0},
    /* i32_load8_u         = 0x2d */ {1, 0},
    /* i32_load16_s        = 0x2e */ {1, 0},
    /* i32_load16_u        = 0x2f */ {1, 0},
    /* i64_load8_s         = 0x30 */ {1, 0},
    /* i64_load8_u         = 0x31 */ {1, 0},
    /* i64_load16_s        = 0x32 */ {1, 0},
    /* i64_load16_u        = 0x33 */ {1, 0},
    /* i64_load32_s        = 0x34 */ {1, 0},
    /* i64_load32_u        = 0x35 */ {1, 0},
    /* i32_store           = 0x36 */ {2, -2},
    /* i64_store           = 0x37 */ {2, -2},
    /* f32_store           = 0x38 */ {2, -2},
    /* f64_store           = 0x39 */ {2, -2},
    /* i32_store8          = 0x3a */ {2, -2},
    /* i32_store16         = 0x3b */ {2, -2},
    /* i64_store8          = 0x3c */ {2, -2},
    /* i64_store16         = 0x3d */ {2, -2},
    /* i64_store32         = 0x3e */ {2, -2},
    /* memory_size         = 0x3f */ {0, 1},
    /* memory_grow         = 0x40 */ {1, 0},

    // 5.4.5 Numeric instructions
    /* i32_const           = 0x41 */ {0, 1},
    /* i64_const           = 0x42 */ {0, 1},
    /* f32_const           = 0x43 */ {0, 1},
    /* f64_const           = 0x44 */ {0, 1},

    /* i32_eqz             = 0x45 */ {1, 0},
    /* i32_eq              = 0x46 */ {2, -1},
    /* i32_ne              = 0x47 */ {2, -1},
    /* i32_lt_s            = 0x48 */ {2, -1},
    /* i32_lt_u            = 0x49 */ {2, -1},
    /* i32_gt_s            = 0x4a */ {2, -1},
    /* i32_gt_u            = 0x4b */ {2, -1},
    /* i32_le_s            = 0x4c */ {2, -1},
    /* i32_le_u            = 0x4d */ {2, -1},
    /* i32_ge_s            = 0x4e */ {2, -1},
    /* i32_ge_u            = 0x4f */ {2, -1},

    /* i64_eqz             = 0x50 */ {1, 0},
    /* i64_eq              = 0x51 */ {2, -1},
    /* i64_ne              = 0x52 */ {2, -1},
    /* i64_lt_s            = 0x53 */ {2, -1},
    /* i64_lt_u            = 0x54 */ {2, -1},
    /* i64_gt_s            = 0x55 */ {2, -1},
    /* i64_gt_u            = 0x56 */ {2, -1},
    /* i64_le_s            = 0x57 */ {2, -1},
    /* i64_le_u            = 0x58 */ {2, -1},
    /* i64_ge_s            = 0x59 */ {2, -1},
    /* i64_ge_u            = 0x5a */ {2, -1},

    /* f32_eq              = 0x5b */ {2, -1},
    /* f32_ne              = 0x5c */ {2, -1},
    /* f32_lt              = 0x5d */ {2, -1},
    /* f32_gt              = 0x5e */ {2, -1},
    /* f32_le              = 0x5f */ {2, -1},
    /* f32_ge              = 0x60 */ {2, -1},

    /* f64_eq              = 0x61 */ {2, -1},
    /* f64_ne              = 0x62 */ {2, -1},
    /* f64_lt              = 0x63 */ {2, -1},
    /* f64_gt              = 0x64 */ {2, -1},
    /* f64_le              = 0x65 */ {2, -1},
    /* f64_ge              = 0x66 */ {2, -1},

    /* i32_clz             = 0x67 */ {1, 0},
    /* i32_ctz             = 0x68 */ {1, 0},
    /* i32_popcnt          = 0x69 */ {1, 0},
    /* i32_add             = 0x6a */ {2, -1},
    /* i32_sub             = 0x6b */ {2, -1},
    /* i32_mul             = 0x6c */ {2, -1},
    /* i32_div_s           = 0x6d */ {2, -1},
    /* i32_div_u           = 0x6e */ {2, -1},
    /* i32_rem_s           = 0x6f */ {2, -1},
    /* i32_rem_u           = 0x70 */ {2, -1},
    /* i32_and             = 0x71 */ {2, -1},
    /* i32_or              = 0x72 */ {2, -1},
    /* i32_xor             = 0x73 */ {2, -1},
    /* i32_shl             = 0x74 */ {2, -1},
    /* i32_shr_s           = 0x75 */ {2, -1},
    /* i32_shr_u           = 0x76 */ {2, -1},
    /* i32_rotl            = 0x77 */ {2, -1},
    /* i32_rotr            = 0x78 */ {2, -1},

    /* i64_clz             = 0x79 */ {1, 0},
    /* i64_ctz             = 0x7a */ {1, 0},
    /* i64_popcnt          = 0x7b */ {1, 0},
    /* i64_add             = 0x7c */ {2, -1},
    /* i64_sub             = 0x7d */ {2, -1},
    /* i64_mul             = 0x7e */ {2, -1},
    /* i64_div_s           = 0x7f */ {2, -1},
    /* i64_div_u           = 0x80 */ {2, -1},
    /* i64_rem_s           = 0x81 */ {2, -1},
    /* i64_rem_u           = 0x82 */ {2, -1},
    /* i64_and             = 0x83 */ {2, -1},
    /* i64_or              = 0x84 */ {2, -1},
    /* i64_xor             = 0x85 */ {2, -1},
    /* i64_shl             = 0x86 */ {2, -1},
    /* i64_shr_s           = 0x87 */ {2, -1},
    /* i64_shr_u           = 0x88 */ {2, -1},
    /* i64_rotl            = 0x89 */ {2, -1},
    /* i64_rotr            = 0x8a */ {2, -1},

    /* f32_abs             = 0x8b */ {1, 0},
    /* f32_neg             = 0x8c */ {1, 0},
    /* f32_ceil            = 0x8d */ {1, 0},
    /* f32_floor           = 0x8e */ {1, 0},
    /* f32_trunc           = 0x8f */ {1, 0},
    /* f32_nearest         = 0x90 */ {1, 0},
    /* f32_sqrt            = 0x91 */ {1, 0},
    /* f32_add             = 0x92 */ {2, -1},
    /* f32_sub             = 0x93 */ {2, -1},
    /* f32_mul             = 0x94 */ {2, -1},
    /* f32_div             = 0x95 */ {2, -1},
    /* f32_min             = 0x96 */ {2, -1},
    /* f32_max             = 0x97 */ {2, -1},
    /* f32_copysign        = 0x98 */ {2, -1},

    /* f64_abs             = 0x99 */ {1, 0},
    /* f64_neg             = 0x9a */ {1, 0},
    /* f64_ceil            = 0x9b */ {1, 0},
    /* f64_floor           = 0x9c */ {1, 0},
    /* f64_trunc           = 0x9d */ {1, 0},
    /* f64_nearest         = 0x9e */ {1, 0},
    /* f64_sqrt            = 0x9f */ {1, 0},
    /* f64_add             = 0xa0 */ {2, -1},
    /* f64_sub             = 0xa1 */ {2, -1},
    /* f64_mul             = 0xa2 */ {2, -1},
    /* f64_div             = 0xa3 */ {2, -1},
    /* f64_min             = 0xa4 */ {2, -1},
    /* f64_max             = 0xa5 */ {2, -1},
    /* f64_copysign        = 0xa6 */ {2, -1},

    /* i32_wrap_i64        = 0xa7 */ {1, 0},
    /* i32_trunc_f32_s     = 0xa8 */ {1, 0},
    /* i32_trunc_f32_u     = 0xa9 */ {1, 0},
    /* i32_trunc_f64_s     = 0xaa */ {1, 0},
    /* i32_trunc_f64_u     = 0xab */ {1, 0},
    /* i64_extend_i32_s    = 0xac */ {1, 0},
    /* i64_extend_i32_u    = 0xad */ {1, 0},
    /* i64_trunc_f32_s     = 0xae */ {1, 0},
    /* i64_trunc_f32_u     = 0xaf */ {1, 0},
    /* i64_trunc_f64_s     = 0xb0 */ {1, 0},
    /* i64_trunc_f64_u     = 0xb1 */ {1, 0},
    /* f32_convert_i32_s   = 0xb2 */ {1, 0},
    /* f32_convert_i32_u   = 0xb3 */ {1, 0},
    /* f32_convert_i64_s   = 0xb4 */ {1, 0},
    /* f32_convert_i64_u   = 0xb5 */ {1, 0},
    /* f32_demote_f64      = 0xb6 */ {1, 0},
    /* f64_convert_i32_s   = 0xb7 */ {1, 0},
    /* f64_convert_i32_u   = 0xb8 */ {1, 0},
    /* f64_convert_i64_s   = 0xb9 */ {1, 0},
    /* f64_convert_i64_u   = 0xba */ {1, 0},
    /* f64_promote_f32     = 0xbb */ {1, 0},
    /* i32_reinterpret_f32 = 0xbc */ {1, 0},
    /* i64_reinterpret_f64 = 0xbd */ {1, 0},
    /* f32_reinterpret_i32 = 0xbe */ {1, 0},
    /* f64_reinterpret_i64 = 0xbf */ {1, 0},
};
}  // namespace

const InstructionMetrics* get_instruction_metrics_table() noexcept
{
    return instruction_metrics_table;
}

}  // namespace fizzy
