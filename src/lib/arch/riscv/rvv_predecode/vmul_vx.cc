// vmul.vx 31..26 = 0x25 vm vs2 rs1 14..12 = 0x6 vd 6..0 = 0x57

#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {

rvv_insn_desc rvv_vmul_vx_predecode(const uint32_t insn) {
  uint16_t rs1 = GET_BIT_SS(insn, 15, 19);
  uint16_t vs2 = GET_BIT_SS(insn, 20, 24);
  uint16_t vd = GET_BIT_SS(insn, 7, 11);
  uint64_t vm = GET_BIT(insn, 25);

  std::string mnemonic = "vmul.vx";
  std::string opstr = fmt::format("v{}, v{}, {}{}", vd, vs2, reg_disasm[rs1],
                                  !vm ? ", v0.t" : std::string());

  return rvv_insn_desc{.id = RVV_INSN_TYPE::RVV_VMUL,
                       .opcode = MATCH_VMUL_VX,
                       .encoding = insn,
                       .eew = 0,
                       .implicit_src_cnt = 2,
                       .implicit_dest_cnt = 1,
                       .opr_cnt = 3,
                       .insn_len = 4,
                       .mnemonic = mnemonic,
                       .operand_str = opstr,
                       .imp_srcs = {vs2, rs1},
                       .imp_dests = {vd},
                       .operands = {
                           INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                           INIT_RVV_OPR(RISCV_OP_VREG, reg, vs2),
                           INIT_RVV_OPR(RISCV_OP_REG, reg, rs1),
                       }};
};

}  // namespace riscv
}  // namespace arch
}  // namespace simeng