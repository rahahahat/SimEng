#pragma once
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {

#define CONCAT(B) B
#define GET_BIT(src, pos) ((src & (0b01 << pos)) >> pos)
#define GET_BIT_SS(src, s_pos, e_pos) \
  ((src >> s_pos) & (0xFFFFFFFF >> (31 - (e_pos - s_pos))))
#define INIT_RVV_OPR(t, m, v) ((simeng::cs_riscv_op){.type = t, .CONCAT(m) = v})

#define get_eew(var, MNEMONIC)    \
  switch (metadata_.opcode) {     \
    case MATCH_##MNEMONIC##8_V:   \
      var = 8;                    \
      break;                      \
    case MATCH_##MNEMONIC##16_V:  \
      var = 16;                   \
      break;                      \
    case MATCH_##MNEMONIC##32_V:  \
      var = 32;                   \
      break;                      \
    case MATCH_##MNEMONIC##64_V:  \
      var = 64;                   \
      break;                      \
    case MATCH_##MNEMONIC##128_V: \
      var = 128;                  \
      break;                      \
  }

}  // namespace riscv
}  // namespace arch
}  // namespace simeng