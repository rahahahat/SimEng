#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {

rvv_insn_desc rvv_ldst_ufaultfirst_predecode(const uint32_t insn) {
  uint32_t mjop = GET_BIT_SS(insn, 0, 6);
  uint8_t vd = GET_BIT_SS(insn, 7, 11);
  uint8_t width = (GET_BIT_SS(insn, 12, 14));
  uint8_t rs1 = GET_BIT_SS(insn, 15, 19);
  uint8_t lumop = GET_BIT_SS(insn, 20, 24);
  uint8_t mew = GET_BIT(insn, 28);
  uint8_t vm = GET_BIT(insn, 25);
  uint8_t nf = GET_BIT_SS(insn, 29, 31);

  uint16_t acw = mew ? 7 + (width % 4) : (width % 4 + 3);
  acw = std::pow(2, acw);

  std::string mnemonic = fmt::format("vle{}ff.v", acw);

  std::string opstr = fmt::format("v{}, ({}){}", vd, reg_disasm[rs1],
                                  !vm ? ", v0.t" : std::string());
  uint32_t opcode = mjop | (uint32_t)width << 12 | (uint32_t)mew << 28;

  return rvv_insn_desc{
      .id = RVV_INSN_TYPE::RVV_LD_USTRIDEFF,
      .opcode = opcode,
      .implicit_src_cnt = 3,
      .implicit_dest_cnt = 1,
      .opr_cnt = 4,
      .eew = acw,
      .insn_len = 4,
      .mnemonic = mnemonic,
      .operand_str = opstr,
      .imp_srcs = {rs1, vm, riscv_sysreg::RISCV_V_SYSREG_VTYPE},
      .imp_dests = {vd},
      .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                   INIT_RVV_OPR(RISCV_OP_REG, reg, rs1),
                   INIT_RVV_OPR(RISCV_OP_IMM, imm, vm),
                   INIT_RVV_OPR(RISCV_OP_SYSREG, reg,
                                riscv_sysreg::RISCV_V_SYSREG_VTYPE)}};
}

}  // namespace riscv
}  // namespace arch
}  // namespace simeng