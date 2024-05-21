#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {
rvv_insn_desc rvv_vmacc_predecode(const uint32_t insn, uint32_t func3) {
  uint16_t vd = GET_BIT_SS(insn, 7, 11);
  uint16_t vs2 = GET_BIT_SS(insn, 20, 24);
  uint16_t rs1_vs1 = GET_BIT_SS(insn, 15, 19);
  uint8_t vm = GET_BIT(insn, 25);
  std::string mnemonic = "vmacc.v";
  mnemonic += "v";
  std::string opstr = fmt::format("v{}, v{}, v{}, {}", vd, rs1_vs1, vs2,
                                  !vm ? ", v0.t" : std::string());
  return rvv_insn_desc{.id = RVV_INSN_TYPE::RVV_VMACCV,
                       .opcode = MATCH_VMACC_VV,
                       .encoding = insn,
                       .eew = 0,
                       .lmul_type = LMUL_CALC_TYPE::SYSREG,
                       .implicit_src_cnt = 3,
                       .implicit_dest_cnt = 1,
                       .opr_cnt = 4,
                       .insn_len = 4,
                       .mnemonic = mnemonic,
                       .operand_str = opstr,
                       .imp_srcs = {vd, rs1_vs1, vs2, vm},
                       .imp_dests = {vd},
                       .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, rs1_vs1),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, vs2),
                                    INIT_RVV_OPR(RISCV_OP_IMM, imm, vm)}};
}
}  // namespace riscv
}  // namespace arch
}  // namespace simeng