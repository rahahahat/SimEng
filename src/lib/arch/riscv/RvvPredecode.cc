#include <unordered_map>

#include "InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"

namespace simeng {
namespace arch {
namespace riscv {

std::array<std::string, 32> reg_disasm = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "fp", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

std::array<std::string, 8> lmul_disasm = {"m1", "m2",  "m4",  "m8",
                                          "",   "mf8", "mf4", "mf2"};
std::array<std::string, 4> eew_disasm = {"8", "16", "32", "64"};

rvv_insn_desc predecode_mopc_opv(const uint32_t insn) {
  uint16_t func3 = GET_BIT_SS(insn, 12, 14);
  switch (func3) {
    case 0x7:
      return rvv_vsetxvlx_predecode(insn);
  }
}

rvv_insn_desc predecode_mopc_lfp_sfp(const uint32_t insn) {
  uint8_t mop = GET_BIT_SS(insn, 26, 27);
  uint8_t lumop = GET_BIT_SS(insn, 20, 24);
  switch (mop) {
    case 0x0:
      switch (lumop) {
        case 0x0:
          // unit-strided
          return rvv_ldst_ustride_predecode(insn);
        case 0x08: {
          if (!GET_BIT_SS(insn, 25, 28)) return rvv_insn_desc{};
          // whole-register
          return rvv_ldst_wholereg_predecode(insn);
        }
        case 0x10:
          // fault-only-first
          return rvv_ldst_ufaultfirst_predecode(insn);
        default:
          return rvv_insn_desc{};
      }
    case 0x1:
      // Indexed-Unordered
      return rvv_ldst_uindexed_predecode(insn);
    case 0x2:
      // Strided
      return rvv_ldst_strided_predecode(insn);
    case 0x3:
      // Indexed-Ordered
      return rvv_ldst_oindexed_predecode(insn);
    default:
      return rvv_insn_desc{};
  }
}

rvv_insn_desc rvv_predecode(const uint32_t insn) {
  uint16_t mopc = GET_BIT_SS(insn, 0, 6);
  switch (mopc) {
    case RV_MAJOR_OPCODES::OP_V:
      return predecode_mopc_opv(insn);
    case RV_MAJOR_OPCODES::LOAD_FP:
    case RV_MAJOR_OPCODES::STORE_FP:
      return predecode_mopc_lfp_sfp(insn);
    default:
      return rvv_insn_desc{};
  }
}

}  // namespace riscv

}  // namespace arch

}  // namespace simeng
