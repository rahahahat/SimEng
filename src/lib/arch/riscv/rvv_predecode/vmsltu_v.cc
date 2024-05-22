#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {

rvv_insn_desc rvv_vmsltu_predecode(const uint32_t insn) {
  uint8_t vm = GET_BIT(insn, 25);
  uint16_t vs2 = GET_BIT_SS(insn, 20, 24);
  uint16_t rs1 = GET_BIT_SS(insn, 15, 19);
  uint16_t vd = GET_BIT_SS(insn, 7, 11);

  std::string mnemonic = "vmsltu.vx";
  std::string opstr = fmt::format("v{}, {} {} {}", vd, vs2, reg_disasm[rs1],
                                  !vm ? ", v0.t" : std::string());

  return rvv_insn_desc{.id = RVV_INSN_TYPE::RVV_VMSLTU,
                       .opcode = MATCH_VMSLTU_VX,
                       .encoding = insn,
                       .eew = 0,
                       .lmul_type = LMUL_CALC_TYPE::SYSREG,
                       .implicit_src_cnt = 2,
                       .implicit_dest_cnt = 1,
                       .opr_cnt = 3,
                       .insn_len = 4,
                       .mnemonic = mnemonic,
                       .operand_str = opstr,
                       .imp_srcs = {vs2, rs1},
                       .imp_dests = {vd},
                       .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, vs2),
                                    INIT_RVV_OPR(RISCV_OP_REG, reg, rs1)}};
};

}  // namespace riscv
}  // namespace arch
}  // namespace simeng