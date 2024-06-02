#include <cmath>

#include "InstructionMetadata.hh"
#include "simeng/arch/riscv/Instruction.hh"
#include "simeng/arch/riscv/rvv/RVV.hh"

namespace simeng {
namespace arch {
namespace riscv {

#define get_eew(var, MNEMONIC)    \
  switch (metadata_.opcode) {     \
    case MATCH_##MNEMONIC##8_V:   \
      var = 8;                    \
      break;                      \
    case MATCH_##MNEMONIC##16_V:  \
      var = 16;                   \
      break;                      \
    case MATCH_##MNEMONIC##32_V:  \
      var = 32;                   \
      break;                      \
    case MATCH_##MNEMONIC##64_V:  \
      var = 64;                   \
      break;                      \
    case MATCH_##MNEMONIC##128_V: \
      var = 128;                  \
      break;                      \
  }

std::vector<simeng::memory::MemoryAccessTarget> coalesce_vmem_acceses(
    std::vector<simeng::memory::MemoryAccessTarget> addrs,
    uint16_t max_coalesce_size) {
  std::vector<memory::MemoryAccessTarget> output;
  uint64_t start_addr = addrs[0].address;
  uint16_t size = addrs[0].size;
  uint64_t end_addr = start_addr + max_coalesce_size;
  for (uint16_t x = 1; x < addrs.size(); x++) {
    uint64_t eff_addr = addrs[x].address + addrs[x].size;
    if (eff_addr <= end_addr) {
      size += addrs[x].size;
    } else {
      output.push_back({start_addr, size});
      start_addr = addrs[x].address;
      size = addrs[x].size;
      end_addr = start_addr + max_coalesce_size;
    }
  }
  output.push_back({start_addr, size});
  return output;
};

template <typename T>
std::vector<simeng::memory::MemoryAccessTarget> gen_vi_addrs(
    uint64_t vlen, uint16_t emul, uint16_t sew, uint64_t base,
    uint16_t start_idx, std::array<simeng::RegisterValue, 26> srcs) {
  std::vector<memory::MemoryAccessTarget> addrs;
  uint16_t vlenb = vlen / 8;
  uint16_t veewb = sizeof(T);
  uint16_t vsewb = sew / 8;
  for (uint16_t m = 0; m < emul; m++) {
    const T* offt = srcs[start_idx + m].getAsVector<T>();
    for (uint16_t x = 0; x < vlenb / veewb; x++) {
      uint64_t addr = base + offt[x];
      addrs.push_back({addr, vsewb});
    }
  }
  return addrs;
}

#define VIDX_ADDR_GEN(...)                                                   \
  switch (eew) {                                                             \
    case 8:                                                                  \
      setMemoryAddresses(gen_vi_addrs<uint8_t>(__VA_ARGS__));                \
      break;                                                                 \
    case 16:                                                                 \
      setMemoryAddresses(gen_vi_addrs<uint16_t>(__VA_ARGS__));               \
      break;                                                                 \
    case 32:                                                                 \
      setMemoryAddresses(gen_vi_addrs<uint32_t>(__VA_ARGS__));               \
      break;                                                                 \
    case 64:                                                                 \
      setMemoryAddresses(gen_vi_addrs<uint64_t>(__VA_ARGS__));               \
      break;                                                                 \
    default:                                                                 \
      std::cerr << "Invalid eew provided to indexed ls/st addr gen: " << eew \
                << std::endl;                                                \
      std::exit(1);                                                          \
  }  // namespace riscv

std::vector<simeng::memory::MemoryAccessTarget> gen_strided_addrs(
    uint32_t vlen, uint16_t lmul, uint16_t eew, uint64_t base,
    uint64_t stride) {
  uint16_t vlenb = vlen / 8;
  uint16_t veewb = eew / 8;
  std::vector<simeng::memory::MemoryAccessTarget> addrs;
  for (uint16_t x = 0; x < (vlenb / veewb) * lmul; x++) {
    uint64_t addr = base + (x * stride);
    addrs.push_back({addr, veewb});
  }

  return addrs;
}

span<const memory::MemoryAccessTarget> Instruction::generateAddressesForRVV() {
  uint32_t vlen = architecture_.vlen;
  uint16_t max_coalesce_size = 16;
  vtype_reg vtype = decode_vtype(
      getSysRegFunc_({RegisterType::SYSTEM,
                      static_cast<uint16_t>(architecture_.getSystemRegisterTag(
                          RISCV_V_SYSREG_VTYPE))})
          .get<uint64_t>());
  switch (metadata_.id) {
    case RVV_INSN_TYPE::RVV_LD_USTRIDE: {
      uint64_t base = sourceValues_[0].get<uint64_t>();
      auto vaddrs = gen_strided_addrs(vlen, vtype.vlmul, eew, base, eew / 8);
      setMemoryAddresses(coalesce_vmem_acceses(vaddrs, max_coalesce_size));
    } break;
    case RVV_INSN_TYPE::RVV_LD_STRIDED: {
      uint64_t base = sourceValues_[0].get<uint64_t>();
      uint64_t stride = sourceValues_[1].get<uint64_t>();
      auto vaddrs = gen_strided_addrs(vlen, vtype.vlmul, eew, base, stride);
      vaddrs = coalesce_vmem_acceses(vaddrs, max_coalesce_size);
      setMemoryAddresses(vaddrs);
    } break;
    case RVV_INSN_TYPE::RVV_LD_WHOLEREG: {
      uint64_t base = sourceValues_[0].get<uint64_t>();
      uint16_t mul = vectorImmediates[0];
      auto vaddrs = gen_strided_addrs(vlen, mul, eew, base, eew / 8);
      setMemoryAddresses(coalesce_vmem_acceses(vaddrs, max_coalesce_size));
    } break;
    case RVV_INSN_TYPE::RVV_ST_WHOLEREG: {
      uint16_t mul = vectorImmediates[0];
      uint64_t base = sourceValues_[mul].get<uint64_t>();
      uint64_t stride = 1;
      auto vaddrs = gen_strided_addrs(vlen, mul, 8, base, stride);
      vaddrs = coalesce_vmem_acceses(vaddrs, max_coalesce_size);
      setMemoryAddresses(vaddrs);
    } break;
    case RVV_INSN_TYPE::RVV_ST_USTRIDE: {
      uint64_t base = sourceValues_[vtype.vlmul].get<uint64_t>();
      auto vaddrs = gen_strided_addrs(vlen, vtype.vlmul, eew, base, eew / 8);
      vaddrs = coalesce_vmem_acceses(vaddrs, max_coalesce_size);
      setMemoryAddresses(vaddrs);
    } break;
    case RVV_INSN_TYPE::RVV_ST_STRIDED: {
      uint64_t base = sourceValues_[vtype.vlmul].get<uint64_t>();
      uint64_t stride = sourceValues_[vtype.vlmul + 1].get<uint64_t>();
      auto vaddrs = gen_strided_addrs(vlen, vtype.vlmul, eew, base, stride);
      setMemoryAddresses(vaddrs);
    } break;
    case RVV_INSN_TYPE::RVV_LD_OINDEXED:
    case RVV_INSN_TYPE::RVV_LD_UINDEXED: {
      uint64_t base = sourceValues_[0].get<uint64_t>();
      float emul = ((float)eew / (float)vtype.sew) * vtype.vlmul;
      VIDX_ADDR_GEN(vlen, emul, vtype.sew, base, 1, sourceValues_);
    } break;
    case RVV_INSN_TYPE::RVV_ST_OINDEXED:
    case RVV_INSN_TYPE::RVV_ST_UINDEXED: {
      uint64_t base = sourceValues_[vtype.vlmul].get<uint64_t>();
      float emul = ((float)eew / (float)vtype.sew) * vtype.vlmul;
      VIDX_ADDR_GEN(vlen, emul, vtype.sew, base, vtype.vlmul + 1,
                    sourceValues_);
    } break;
    default: {
      std::cerr << "Default statement in generating addresses for VMemInsns"
                << std::endl;
      std::exit(1);
    }
  }
  return getGeneratedAddresses();
}

span<const memory::MemoryAccessTarget> Instruction::generateAddresses() {
  assert(
      (isInstruction(InsnType::isLoad) || isInstruction(InsnType::isStore)) &&
      "generateAddresses called on non-load-or-store instruction");

  uint64_t address;
  if (isInstruction(InsnType::isRVV)) {
    if (isInstruction(InsnType::isRVVLoad) ||
        isInstruction(InsnType::isRVVStore)) {
      return generateAddressesForRVV();
    }
    std::cerr << "Unsupported RVV Memory Insns type for generation of addresses"
              << std::endl;
    std::exit(1);
  }

  if (isInstruction(InsnType::isLoad) && isInstruction(InsnType::isStore) &&
      isInstruction(InsnType::isAtomic)) {
    // Atomics
    // Metadata operands[2] corresponds to instruction sourceRegValues[1]
    assert(metadata_.operands[2].type == RISCV_OP_REG &&
           "metadata_ operand not of correct type during RISC-V address "
           "generation");
    address = sourceValues_[1].get<uint64_t>();
  } else if (isInstruction(InsnType::isLoad) &&
             isInstruction(InsnType::isAtomic)) {
    // Load reserved
    // Metadata operands[1] corresponds to instruction sourceRegValues[0]
    assert(metadata_.operands[1].type == RISCV_OP_REG &&
           "metadata_ operand not of correct type during RISC-V address "
           "generation");
    address = sourceValues_[0].get<uint64_t>();
  } else if (isInstruction(InsnType::isStore) &&
             isInstruction(InsnType::isAtomic)) {
    // Store conditional
    assert(metadata_.operands[2].type == RISCV_OP_REG &&
           "metadata_ operand not of correct type during RISC-V address "
           "generation");
    address = sourceValues_[1].get<uint64_t>();
  } else if (isInstruction(InsnType::isLoad)) {
    assert(metadata_.operands[1].type == RISCV_OP_MEM &&
           "metadata_ operand not of correct type during RISC-V address "
           "generation");
    address = sourceValues_[0].get<uint64_t>() + sourceImm_;
  } else {
    assert((metadata_.operands[1].type == RISCV_OP_MEM) &&
           "metadata_ operand not of correct type during RISC-V address "
           "generation");

    address = sourceValues_[1].get<uint64_t>() + sourceImm_;
  }

  // Atomics
  if (Opcode::RISCV_AMOADD_D <= metadata_.opcode &&
      metadata_.opcode <= Opcode::RISCV_AMOXOR_W_RL) {  // Atomics
    // THIS IS DEPENDENT ON CAPSTONE ENCODING AND COULD BREAK IF CHANGED
    int size = ((metadata_.opcode - 182) / 4) % 2;  // 1 = Word, 0 = Double
    if (size == 1) {
      // Word
      setMemoryAddresses({{address, 4}});
    } else {
      // Double
      setMemoryAddresses({{address, 8}});
    }
    return getGeneratedAddresses();
  }

  switch (metadata_.opcode) {
    case Opcode::RISCV_SD:
      [[fallthrough]];
    case Opcode::RISCV_LD:
      [[fallthrough]];
    case Opcode::RISCV_FSD:
      [[fallthrough]];
    case Opcode::RISCV_FLD: {
      setMemoryAddresses({{address, 8}});
      break;
    }
    case Opcode::RISCV_SW:
      [[fallthrough]];
    case Opcode::RISCV_LW:
      [[fallthrough]];
    case Opcode::RISCV_LWU:
      [[fallthrough]];
    case Opcode::RISCV_FSW:
      [[fallthrough]];
    case Opcode::RISCV_FLW: {
      setMemoryAddresses({{address, 4}});
      break;
    }
    case Opcode::RISCV_SH:
      [[fallthrough]];
    case Opcode::RISCV_LH:
      [[fallthrough]];
    case Opcode::RISCV_LHU: {
      setMemoryAddresses({{address, 2}});
      break;
    }
    case Opcode::RISCV_SB:
      [[fallthrough]];
    case Opcode::RISCV_LB:
      [[fallthrough]];
    case Opcode::RISCV_LBU: {
      setMemoryAddresses({{address, 1}});
      break;
    }

    // Atomics
    case Opcode::RISCV_LR_W:
      [[fallthrough]];
    case Opcode::RISCV_LR_W_AQ:
      [[fallthrough]];
    case Opcode::RISCV_LR_W_RL:
      [[fallthrough]];
    case Opcode::RISCV_LR_W_AQ_RL: {
      setMemoryAddresses({{sourceValues_[0].get<uint64_t>(), 4}});
      break;
    }
    case Opcode::RISCV_LR_D:
      [[fallthrough]];
    case Opcode::RISCV_LR_D_AQ:
      [[fallthrough]];
    case Opcode::RISCV_LR_D_RL:
      [[fallthrough]];
    case Opcode::RISCV_LR_D_AQ_RL: {
      setMemoryAddresses({{sourceValues_[0].get<uint64_t>(), 8}});
      break;
    }
    case Opcode::RISCV_SC_W:
      [[fallthrough]];
    case Opcode::RISCV_SC_W_AQ:
      [[fallthrough]];
    case Opcode::RISCV_SC_W_RL:
      [[fallthrough]];
    case Opcode::RISCV_SC_W_AQ_RL: {
      setMemoryAddresses({{sourceValues_[1].get<uint64_t>(), 4}});
      break;
    }
    case Opcode::RISCV_SC_D:
      [[fallthrough]];
    case Opcode::RISCV_SC_D_AQ:
      [[fallthrough]];
    case Opcode::RISCV_SC_D_RL:
      [[fallthrough]];
    case Opcode::RISCV_SC_D_AQ_RL: {
      setMemoryAddresses({{sourceValues_[1].get<uint64_t>(), 8}});
      break;
    }
    default:
      exceptionEncountered_ = true;
      exception_ = InstructionException::ExecutionNotYetImplemented;
      break;
  }
  return getGeneratedAddresses();
}

}  // namespace riscv
}  // namespace arch
}  // namespace simeng