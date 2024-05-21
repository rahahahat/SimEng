#include <cmath>

#include "../InstructionMetadata.hh"
#include "simeng/arch/riscv/rvv/RVVDecode.hh"
#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
namespace arch {
namespace riscv {
rvv_insn_desc rvv_vmvr_predecode(const uint32_t insn) {
  uint16_t vd = GET_BIT_SS(insn, 7, 11);
  uint16_t vs2 = GET_BIT_SS(insn, 20, 24);
  uint8_t vm = GET_BIT(insn, 25);
  uint16_t nr = GET_BIT_SS(insn, 15, 19) + 1;

  std::string mnemonic = fmt::format("vmv{}r.v", nr);
  uint32_t opcode;
  switch (nr) {
    case 0x1:
      opcode = MATCH_VMV1R_V;
    case 0x2:
      opcode = MATCH_VMV2R_V;
    case 0x4:
      opcode = MATCH_VMV4R_V;
    case 0x8:
      opcode = MATCH_VMV8R_V;
  }

  std::string opstr = fmt::format("v{}, v{}", vd, vs2);

  return rvv_insn_desc{.id = RVV_INSN_TYPE::RVV_VMVR,
                       .opcode = opcode,
                       .encoding = insn,
                       .eew = 0,
                       .implicit_src_cnt = 2,
                       .implicit_dest_cnt = 1,
                       .opr_cnt = 3,
                       .insn_len = 4,
                       .mnemonic = mnemonic,
                       .operand_str = opstr,
                       .imp_srcs = {vs2, nr},
                       .imp_dests = {vd},
                       .operands = {INIT_RVV_OPR(RISCV_OP_VREG, reg, vd),
                                    INIT_RVV_OPR(RISCV_OP_VREG, reg, vs2),
                                    INIT_RVV_OPR(RISCV_OP_IMM, imm, nr)}};
};
}  // namespace riscv
}  // namespace arch
}  // namespace simeng