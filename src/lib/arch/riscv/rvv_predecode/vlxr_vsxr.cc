#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {

rvv_insn_desc rvv_ldst_wholereg_predecode(const uint32_t insn) {
  uint32_t mjop = GET_BIT_SS(insn, 0, 6);
  uint8_t vd_or_vs3 = GET_BIT_SS(insn, 7, 11);
  uint8_t width = (GET_BIT_SS(insn, 12, 14));
  uint8_t rs1 = GET_BIT_SS(insn, 15, 19);
  uint32_t lumop = GET_BIT_SS(insn, 20, 24);
  uint8_t vm = GET_BIT(insn, 25);
  //   uint32_t mop = GET_BIT_SS(insn, 26, 27);
  uint8_t mew = GET_BIT(insn, 28);
  uint8_t nf = GET_BIT_SS(insn, 29, 31);

  uint16_t acw = mew ? 7 + (width % 4) : (width % 4 + 3);
  acw = std::pow(2, acw);

  std::string mnemonic =
      fmt::format("v{}{}r{}.v", mjop == 0x7 ? "l" : "s", nf + 1,
                  mjop == 0x7 ? "e" + acw : std::string());

  std::string opstr = fmt::format("v{}, ({})", vd_or_vs3, reg_disasm[rs1]);

  uint32_t opcode = 0x0 | mjop | (uint32_t)width << 12 | lumop << 20 | vm << 25;

  if (mjop == 0x7) {
    return rvv_insn_desc{
        .id = RVV_INSN_TYPE::RVV_LD_WHOLEREG,
        .opcode = opcode,
        .encoding = insn,
        .eew = acw,
        .implicit_src_cnt = 2,
        .implicit_dest_cnt = 1,
        .opr_cnt = 3,
        .insn_len = 4,
        .mnemonic = mnemonic,
        .operand_str = opstr,
        .imp_srcs = {rs1, riscv_sysreg::RISCV_V_SYSREG_VTYPE},
        .imp_dests = {vd_or_vs3},
        .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd_or_vs3),
                     INIT_RVV_OPR(RISCV_OP_REG, reg, rs1),
                     INIT_RVV_OPR(RISCV_OP_SYSREG, reg,
                                  riscv_sysreg::RISCV_V_SYSREG_VTYPE)}};
  } else {
    return rvv_insn_desc{
        .id = RVV_INSN_TYPE::RVV_ST_WHOLEREG,
        .opcode = opcode,
        .encoding = insn,
        .eew = acw,
        .implicit_src_cnt = 3,
        .implicit_dest_cnt = 0,
        .opr_cnt = 3,
        .insn_len = 4,
        .mnemonic = mnemonic,
        .operand_str = opstr,
        .imp_srcs = {vd_or_vs3, rs1, riscv_sysreg::RISCV_V_SYSREG_VTYPE},
        .imp_dests = {},
        .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd_or_vs3),
                     INIT_RVV_OPR(RISCV_OP_REG, reg, rs1),
                     INIT_RVV_OPR(RISCV_OP_SYSREG, reg,
                                  riscv_sysreg::RISCV_V_SYSREG_VTYPE)}};
  }
}

}  // namespace riscv
}  // namespace arch
}  // namespace simeng