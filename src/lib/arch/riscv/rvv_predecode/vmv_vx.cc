

#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {

rvv_insn_desc rvv_vmv_vx_predecode(const uint32_t insn) {
  uint16_t rs1_imm = GET_BIT_SS(insn, 15, 19);
  uint16_t vd = GET_BIT_SS(insn, 7, 11);

  std::string mnemonic = "vmv.v";
  if (GET_BIT_SS(insn, 12, 14) == 0x4) {
    mnemonic += ".x ";
    std::string opstr = fmt::format("v{}, {}", vd, reg_disasm[rs1_imm]);

    return rvv_insn_desc{
        .id = RVV_INSN_TYPE::RVV_VMV,
        .opcode = MATCH_VMV_V_X,
        .encoding = insn,
        .eew = 0,
        .implicit_src_cnt = 1,
        .implicit_dest_cnt = 1,
        .opr_cnt = 2,
        .insn_len = 4,
        .mnemonic = mnemonic,
        .operand_str = opstr,
        .imp_srcs = {rs1_imm},
        .imp_dests = {vd},
        .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                     INIT_RVV_OPR(RISCV_OP_REG, reg, rs1_imm)}};

  } else if (GET_BIT_SS(insn, 12, 14) == 0x3) {
    mnemonic += ".i";
    std::string opstr = fmt::format("v{}, {}", vd, rs1_imm);
    return rvv_insn_desc{
        .id = RVV_INSN_TYPE::RVV_VMV,
        .opcode = MATCH_VMV_V_I,
        .encoding = insn,
        .eew = 0,
        .implicit_src_cnt = 1,
        .implicit_dest_cnt = 1,
        .opr_cnt = 2,
        .insn_len = 4,
        .mnemonic = mnemonic,
        .operand_str = opstr,
        .imp_srcs = {rs1_imm},
        .imp_dests = {vd},
        .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                     INIT_RVV_OPR(RISCV_OP_IMM, imm, rs1_imm)}};
  }
  return rvv_insn_desc{};
};

}  // namespace riscv
}  // namespace arch
}  // namespace simeng