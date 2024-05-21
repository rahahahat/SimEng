#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {
rvv_insn_desc rvv_vid_predecode(const uint32_t insn) {
  uint16_t vd = GET_BIT_SS(insn, 7, 11);
  uint8_t vm = GET_BIT(insn, 25);
  std::string mnemonic = "vid.v";
  std::string opstr =
      fmt::format("v{}, {}", vd, !vm ? ", v0.t" : std::string());
  return rvv_insn_desc{.id = RVV_INSN_TYPE::RVV_VIDV,
                       .opcode = MATCH_VID_V,
                       .encoding = insn,
                       .eew = 0,
                       .implicit_src_cnt = 0,
                       .implicit_dest_cnt = 1,
                       .opr_cnt = 1,
                       .insn_len = 4,
                       .mnemonic = mnemonic,
                       .operand_str = opstr,
                       .imp_srcs = {vm},
                       .imp_dests = {vd},
                       .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                                    INIT_RVV_OPR(RISCV_OP_IMM, imm, vm)}};
};
}  // namespace riscv
}  // namespace arch
}  // namespace simeng