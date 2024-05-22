

#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {

rvv_insn_desc rvv_vmadd_vv_predecode(const uint32_t insn) {
  uint16_t vd = GET_BIT_SS(insn, 7, 11);
  uint16_t vs2 = GET_BIT_SS(insn, 20, 24);
  uint16_t vs1 = GET_BIT_SS(insn, 15, 19);
  uint8_t vm = GET_BIT(insn, 25);
  std::string mnemonic = "vmadd.vv";
  std::string opstr = fmt::format("v{}, v{}, v{}, {}", vd, vs1, vs2,
                                  !vm ? ", v0.t" : std::string());
  return rvv_insn_desc{.id = RVV_INSN_TYPE::RVV_VMADDV,
                       .opcode = MATCH_VMADD_VV,
                       .encoding = insn,
                       .eew = 0,
                       .lmul_type = LMUL_CALC_TYPE::SYSREG,
                       .implicit_src_cnt = 4,
                       .implicit_dest_cnt = 1,
                       .opr_cnt = 5,
                       .insn_len = 4,
                       .mnemonic = mnemonic,
                       .operand_str = opstr,
                       .imp_srcs = {vs1, vd, vs2, vm},
                       .imp_dests = {vd},
                       .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, vs1),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, vs2),
                                    INIT_RVV_OPR(RISCV_OP_IMM, imm, vm)}};
}
}  // namespace riscv
}  // namespace arch
}  // namespace simeng