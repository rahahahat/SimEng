#pragma once

#include <string>
#include <vector>

#include "simeng/arch/riscv/rvv/RVVEncoding.hh"

namespace simeng {
/** Forward Declaration of struct which was cloned from capstone so it could be
 * changed for SimEng. Clone is present in InstructionMetadata.hh*/
struct cs_riscv_op;

namespace arch {
namespace riscv {

enum RV_MAJOR_OPCODES : uint8_t { OP_V = 0x57, LOAD_FP = 0x7, STORE_FP = 0x27 };

enum RVV_INSTR_GROUP : uint16_t { VSETINSNS = 0x7057 };

enum RVV_INSN_TYPE {
  RVV_INSNS = 999,
  RVV_LOAD,
  RVV_LD_USTRIDE,
  RVV_LD_STRIDED,
  RVV_LD_UINDEXED,
  RVV_LD_OINDEXED,
  RVV_LD_USTRIDEFF,
  RVV_LD_WHOLEREG,
  RVV_STORE,
  RVV_ST_USTRIDE,
  RVV_ST_STRIDED,
  RVV_ST_UINDEXED,
  RVV_ST_OINDEXED,
  RVV_ST_WHOLEREG,
  RVV_CONF,
  RVV_VSETXVLX,
  RVV_MISC,
  RVV_VIDV,
  RVV_VADDV,
  RVV_VMACCV,
  RVV_VSLLV,
  RVV_INSN_END,
};

struct rvv_insn_desc {
  unsigned int id;
  unsigned int opcode;
  uint32_t encoding;
  uint16_t eew = 0;
  uint8_t implicit_src_cnt;
  uint8_t implicit_dest_cnt;
  uint8_t opr_cnt;
  uint8_t insn_len;
  std::string mnemonic;
  std::string operand_str;
  std::vector<uint16_t> imp_srcs;
  std::vector<uint16_t> imp_dests;
  std::vector<simeng::cs_riscv_op> operands;
};

}  // namespace riscv
}  // namespace arch
}  // namespace simeng