#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {
rvv_insn_desc rvv_vadd_predecode(const uint32_t insn, uint32_t func3) {
  uint16_t vd = GET_BIT_SS(insn, 7, 11);
  uint16_t vs2 = GET_BIT_SS(insn, 20, 24);
  uint16_t rs1_vs1_uimm = GET_BIT_SS(insn, 15, 19);
  uint8_t vm = GET_BIT(insn, 25);
  std::string mnemonic = "vadd.v";

  switch (func3) {
    case 0x0: {
      mnemonic += "v";
      std::string opstr = fmt::format("v{}, v{}, v{} {}", vd, vs2, rs1_vs1_uimm,
                                      !vm ? ", v0.t" : std::string());
      return rvv_insn_desc{
          .id = RVV_INSN_TYPE::RVV_VADDV,
          .opcode = MATCH_VADD_VV,
          .encoding = insn,
          .eew = 0,
          .implicit_src_cnt = 3,
          .implicit_dest_cnt = 1,
          .opr_cnt = 4,
          .insn_len = 4,
          .mnemonic = mnemonic,
          .operand_str = opstr,
          .imp_srcs = {vs2, rs1_vs1_uimm, vm},
          .imp_dests = {vd},
          .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                       INIT_RVV_OPR(RISCV_OP_VREG, reg, vs2),
                       INIT_RVV_OPR(RISCV_OP_VREG, reg, rs1_vs1_uimm),
                       INIT_RVV_OPR(RISCV_OP_IMM, imm, vm)}};
    }
    case 0x4: {
      mnemonic += "x";
      std::string opstr =
          fmt::format("v{}, v{}, {} {}", vd, vs2, reg_disasm[rs1_vs1_uimm],
                      !vm ? ", v0.t" : std::string());

      return rvv_insn_desc{
          .id = RVV_INSN_TYPE::RVV_VADDV,
          .opcode = MATCH_VADD_VX,
          .encoding = insn,
          .eew = 0,
          .implicit_src_cnt = 3,
          .implicit_dest_cnt = 1,
          .opr_cnt = 4,
          .insn_len = 4,
          .mnemonic = mnemonic,
          .operand_str = opstr,
          .imp_srcs = {vs2, rs1_vs1_uimm, vm},
          .imp_dests = {vd},
          .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                       INIT_RVV_OPR(RISCV_OP_VREG, reg, vs2),
                       INIT_RVV_OPR(RISCV_OP_REG, reg, rs1_vs1_uimm),
                       INIT_RVV_OPR(RISCV_OP_IMM, imm, vm)}};
    }
  }
}
}  // namespace riscv
}  // namespace arch
}  // namespace simeng