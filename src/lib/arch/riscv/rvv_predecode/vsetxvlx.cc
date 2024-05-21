#include <bitset>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"
namespace simeng {
namespace arch {
namespace riscv {

std::string disasm_zimm(uint16_t zimm) {
  uint8_t vlmul = zimm & 0b11;
  uint8_t vsew = GET_BIT_SS(zimm, 3, 5);
  uint8_t vta = GET_BIT(zimm, 6);
  uint8_t vma = GET_BIT(zimm, 7);
  std::string disasm = fmt::format("e{}", eew_disasm[vsew]);
  if (vlmul) disasm += fmt::format(", {}", lmul_disasm[vlmul]);
  if (vta) disasm += ", ta";
  if (vma) disasm += ", ma";
  return disasm;
}

rvv_insn_desc rvv_vsetxvlx_predecode(const uint32_t insn) {
  uint8_t opc = GET_BIT_SS(insn, 30, 31);
  std::string dsm;
  rvv_insn_desc desc;

  switch (opc) {
    case 0b11: {  // vsetivli
      uint16_t rd = GET_BIT_SS(insn, 7, 11);
      uint16_t uimm = GET_BIT_SS(insn, 15, 19);
      uint16_t zimm = GET_BIT_SS(insn, 20, 29);
      desc = {.id = RVV_INSN_TYPE::RVV_VSETXVLX,
              .opcode = MATCH_VSETIVLI,
              .encoding = insn,
              .eew = 0,
              .implicit_src_cnt = 2,
              .implicit_dest_cnt = 3,
              .opr_cnt = 5,
              .insn_len = 4,
              .mnemonic = "vsetivli",
              .operand_str = fmt::format("{}, {:#x}, {}", reg_disasm[rd], uimm,
                                         disasm_zimm(zimm)),
              .imp_srcs = {uimm, zimm},
              .imp_dests = {rd, riscv_sysreg::RISCV_V_SYSREG_VL,
                            riscv_sysreg::RISCV_V_SYSREG_VTYPE},
              .operands = {INIT_RVV_OPR(RISCV_OP_REG, reg, rd),
                           INIT_RVV_OPR(RISCV_OP_IMM, imm, uimm),
                           INIT_RVV_OPR(RISCV_OP_IMM, imm, zimm),
                           INIT_RVV_OPR(RISCV_OP_IMM, reg,
                                        riscv_sysreg::RISCV_V_SYSREG_VTYPE),
                           INIT_RVV_OPR(RISCV_OP_SYSREG, reg,
                                        riscv_sysreg::RISCV_V_SYSREG_VL)}};
      break;
    }
    case 0b10: {  // vsetvl
      uint16_t rd = GET_BIT_SS(insn, 7, 11);
      uint16_t rs1 = GET_BIT_SS(insn, 15, 19);
      uint16_t rs2 = GET_BIT_SS(insn, 20, 24);
      desc = {.id = RVV_INSN_TYPE::RVV_VSETXVLX,
              .opcode = MATCH_VSETVL,
              .encoding = insn,
              .eew = 0,
              .implicit_src_cnt = 2,
              .implicit_dest_cnt = 3,
              .opr_cnt = 5,
              .insn_len = 4,
              .mnemonic = "vsetvl",
              .operand_str = fmt::format("{}, {}, {}", reg_disasm[rd],
                                         reg_disasm[rs1], reg_disasm[rs2]),
              .imp_srcs = {rs1, rs2},
              .imp_dests = {rd, riscv_sysreg::RISCV_V_SYSREG_VL,
                            riscv_sysreg::RISCV_V_SYSREG_VTYPE},
              .operands = {INIT_RVV_OPR(RISCV_OP_REG, reg, rd),
                           INIT_RVV_OPR(RISCV_OP_REG, reg, rs1),
                           INIT_RVV_OPR(RISCV_OP_REG, reg, rs2),
                           INIT_RVV_OPR(RISCV_OP_SYSREG, reg,
                                        riscv_sysreg::RISCV_V_SYSREG_VTYPE),
                           INIT_RVV_OPR(RISCV_OP_SYSREG, reg,
                                        riscv_sysreg::RISCV_V_SYSREG_VL)}};
      break;
    }
    default: {  // vsetvli
      uint16_t rd = GET_BIT_SS(insn, 7, 11);
      uint16_t rs1 = GET_BIT_SS(insn, 15, 19);
      uint16_t zimm = GET_BIT_SS(insn, 20, 30);
      desc = {.id = RVV_INSN_TYPE::RVV_VSETXVLX,
              .opcode = MATCH_VSETVLI,
              .encoding = insn,
              .eew = 0,
              .implicit_src_cnt = 2,
              .implicit_dest_cnt = 3,
              .opr_cnt = 3,
              .insn_len = 4,
              .mnemonic = "vsetvli",
              .operand_str = fmt::format("{}, {}, {}", reg_disasm[rd],
                                         reg_disasm[rs1], disasm_zimm(zimm)),
              .imp_srcs = {rs1, zimm},
              .imp_dests = {rd, riscv_sysreg::RISCV_V_SYSREG_VL,
                            riscv_sysreg::RISCV_V_SYSREG_VTYPE},
              .operands = {INIT_RVV_OPR(RISCV_OP_REG, reg, rd),
                           INIT_RVV_OPR(RISCV_OP_REG, reg, rs1),
                           INIT_RVV_OPR(RISCV_OP_IMM, imm, zimm)}};
      break;
    }
  }
  return desc;
}

}  // namespace riscv
}  // namespace arch
}  // namespace simeng