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

std::vector<simeng::memory::MemoryAccessTarget> gen_strided_addrs(
    uint16_t eew, uint64_t base, uint64_t stride, uint32_t vlen) {
  std::vector<simeng::memory::MemoryAccessTarget> addrs;
  for (uint16_t x = 0; x < vlen / eew; x++) {
    uint64_t addr = base + (x * (eew / 8)) + stride;
    std::cout << "GEN ADDRS: " << addr << " ";
    uint16_t size = eew / 8;
    addrs.push_back({addr, size});
  }
  std::cout << std::endl;
  return addrs;
}

span<const memory::MemoryAccessTarget> Instruction::generateAddressesForRVV() {
  uint32_t vlen = architecture_.vlen;
  simeng::cs_riscv_op vtype = metadata_.operands[metadata_.operandCount - 1];
  switch (metadata_.id) {
    case RVV_INSN_TYPE::RVV_LD_USTRIDE: {
      uint16_t eew = 0;
      get_eew(eew, VLE);
      uint8_t nf = metadata_.operands[3].imm;
      uint64_t base = sourceValues_[0].get<uint64_t>();
      std::cout << "BASE ADDR: " << base << std::endl;
      auto vaddrs = gen_strided_addrs(eew, base, 0, vlen);
      setMemoryAddresses(vaddrs);
      break;
    }
    case RVV_INSN_TYPE::RVV_LD_STRIDED: {
      uint16_t eew = 0;
      get_eew(eew, VLSE);
      uint8_t nf = metadata_.operands[4].imm;
      uint64_t base = sourceValues_[0].get<uint64_t>();
      uint64_t stride = sourceValues_[1].get<uint64_t>();
      auto vaddrs = gen_strided_addrs(eew, base, stride, vlen);
      setMemoryAddresses(vaddrs);
      break;
    };
    // case RVV_INSN_TYPE::RVV_LD_OINDEXED:
    // case RVV_INSN_TYPE::RVV_LD_UINDEXED:
    //   uint64_t vtype = sourceValues_[sourceRegisterCount_ -
    //   1].get<uint64_t>(); uint16_t eew = 0; switch (metadata_.opcode) {
    //     case MATCH_VLUXEI8_V:
    //     case MATCH_VLOXEI8_V:
    //       eew = 8;
    //       break;
    //     case MATCH_VLUXEI16_V:
    //     case MATCH_VLOXEI16_V:
    //       eew = 16;
    //       break;
    //     case MATCH_VLUXEI32_V:
    //     case MATCH_VLOXEI32_V:
    //       eew = 32;
    //       break;
    //     case MATCH_VLUXEI64_V:
    //     case MATCH_VLOXEI64_V:
    //       eew = 64;
    //       break;
    //     case MATCH_VLUXEI128_V:
    //     case MATCH_VLOXEI128_V:
    //       eew = 128;
    //       break;
    //   }
    //   uint8_t nf = metadata_.operands[3].imm;
    //   uint64_t base = sourceValues_[0].get<uint64_t>();
    //   uint64_t stride = sourceValues_[1].getAsVector<>();
    default:
      std::cerr << "Default statement in generating addresses for VMemInsns"
                << std::endl;
      std::exit(1);
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
      std::cout << "Comes here for addr generation" << std::endl;
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