#pragma once
#include <fmt/core.h>

#include <array>
#include <string>

#include "simeng/arch/riscv/rvv/RVV.hh"
#include "simeng/arch/riscv/rvv/RVVUtils.hh"

namespace simeng {
namespace arch {
namespace riscv {

extern std::array<std::string, 32> reg_disasm;
extern std::array<std::string, 8> lmul_disasm;
extern std::array<std::string, 4> eew_disasm;

rvv_insn_desc rvv_vsetxvlx_predecode(const uint32_t insn);
rvv_insn_desc rvv_ldst_ustride_predecode(const uint32_t insn);
rvv_insn_desc rvv_ldst_strided_predecode(const uint32_t insn);
rvv_insn_desc rvv_ldst_uindexed_predecode(const uint32_t insn);
rvv_insn_desc rvv_ldst_oindexed_predecode(const uint32_t insn);
rvv_insn_desc rvv_ldst_ufaultfirst_predecode(const uint32_t insn);
rvv_insn_desc rvv_ldst_wholereg_predecode(const uint32_t insn);

rvv_insn_desc predecode_mopc_lfp_sfp(const uint32_t insn);
rvv_insn_desc predecode_mopc_opv(const uint32_t insn);
rvv_insn_desc rvv_predecode(const uint32_t insn);

}  // namespace riscv
}  // namespace arch
}  // namespace simeng