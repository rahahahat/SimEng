

#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {

rvv_insn_desc rvv_vmerge_vvm_predecode(const uint32_t insn) {
  uint8_t vm = GET_BIT(insn, 25);
  uint16_t vs2 = GET_BIT_SS(insn, 20, 24);
  uint16_t vs1 = GET_BIT_SS(insn, 15, 19);
  uint16_t vd = GET_BIT_SS(insn, 7, 11);

  std::string mnemonic = "vmerge.vvm";
  std::string opstr = fmt::format("v{}, v{} v{} {}", vd, vs2, vs1,
                                  !vm ? ", v0.t" : std::string());
  return rvv_insn_desc{.id = RVV_INSN_TYPE::RVV_VMERGE_VVM,
                       .opcode = MATCH_VMERGE_VVM,
                       .encoding = insn,
                       .eew = 0,
                       .lmul_type = LMUL_CALC_TYPE::SYSREG,
                       .implicit_src_cnt = 3,
                       .implicit_dest_cnt = 1,
                       .opr_cnt = 4,
                       .insn_len = 4,
                       .mnemonic = mnemonic,
                       .operand_str = opstr,
                       .imp_srcs = {vs2, vs1, 0},
                       .imp_dests = {vd},
                       .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, vs2),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, vs1),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, 0)}};
};

}  // namespace riscv
}  // namespace arch
}  // namespace simeng