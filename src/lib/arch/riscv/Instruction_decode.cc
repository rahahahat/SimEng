#include <bitset>

#include "InstructionMetadata.hh"
#include "simeng/arch/riscv/Architecture.hh"
#include "simeng/arch/riscv/Instruction.hh"
#include "simeng/arch/riscv/rvv/RVV.hh"

namespace simeng {
namespace arch {
namespace riscv {

/********************
 * HELPER FUNCTIONS
 *******************/

/** Parses the Capstone `riscv_reg` value to generate an architectural register
 * representation.
 *
 * WARNING: this conversion is FRAGILE, and relies on the structure of the
 * `riscv_reg` enum. Updates to the Capstone library version may cause this to
 * break. */
Register csRegToRegister(unsigned int reg, const riscv::Architecture& arch) {
  // Check from top of the range downwards

  // Metadata could produce either 64-bit floating point register or 32-bit
  // floating point register. Map both encodings to the same SimEng register.
  // Only 64-bit registers are supported

  if (reg - 1 == riscv_sysreg::RISCV_V_SYSREG_VTYPE) {
    return {RegisterType::SYSTEM,
            static_cast<uint16_t>(arch.getSystemRegisterTag(reg))};
  }
  // Modulus ensures only 64 bit registers are recognised
  if (RISCV_REG_F31_64 >= reg && reg >= RISCV_REG_F0_64 && reg % 2 == 0) {
    // Register ft0.64 has encoding 34 with subsequent encodings interleaved
    // with 32 bit floating point registers. See
    // capstone-lib-src/include/riscv.h
    // Division always results in integer as reg is required to be even by if
    // condition and ft0.64 is also even
    return {RegisterType::FLOAT,
            static_cast<uint16_t>((reg - RISCV_REG_F0_64) / 2)};
  }

  // Modulus ensures only 32 bit registers are recognised
  if (RISCV_REG_F31_32 >= reg && reg >= RISCV_REG_F0_32 && reg % 2 == 1) {
    // Register ft0.32 has encoding 33 with subsequent encodings interleaved
    // with 64 bit floating point registers. See
    // capstone-lib-src/include/riscv.h
    // Division always results in integer as reg is required to be odd by if
    // condition and ft0.32 is also odd
    return {RegisterType::FLOAT,
            static_cast<uint16_t>((reg - RISCV_REG_F0_32) / 2)};
  }

  if (RISCV_REG_X31 >= reg && reg >= RISCV_REG_X1) {
    // Capstone produces 1 indexed register operands
    return {RegisterType::GENERAL, static_cast<uint16_t>(reg - 1)};
  }

  if (reg == RISCV_REG_X0) {
    // Zero register
    return RegisterType::ZERO_REGISTER;
  }

  assert(false && "Decoding failed due to unknown register identifier");
  return {std::numeric_limits<uint8_t>::max(),
          std::numeric_limits<uint16_t>::max()};
}

Register decodeVecReg(unsigned int reg) {
  return {RegisterType::VECTOR, static_cast<uint16_t>(reg)};
}
/******************
 * RVV DECODING LOGIC
 *****************/
void Instruction::decode_rvv() {
  uint64_t vlen = architecture_.vlen;
  switch (metadata_.id) {
    case RVV_INSN_TYPE::RVV_LD_USTRIDE:     // vle
    case RVV_INSN_TYPE::RVV_LD_STRIDED:     // vlse
    case RVV_INSN_TYPE::RVV_LD_UINDEXED:    // vluxei
    case RVV_INSN_TYPE::RVV_LD_OINDEXED:    // vloxei
    case RVV_INSN_TYPE::RVV_LD_USTRIDEFF:   // vleff
    case RVV_INSN_TYPE::RVV_LD_WHOLEREG: {  // vlxre
      if (metadata_.operands[0].reg != 0) {
        // Catch zero register references and pre-complete those operands
        destinationRegisters_[destinationRegisterCount_] =
            decodeVecReg(metadata_.operands[0].reg);
        destinationRegisterCount_++;
      }
      for (uint8_t x = 1; x < metadata_.operandCount; x++) {
        simeng::cs_riscv_op op = metadata_.operands[x];
        switch (op.type) {
          case 0x1: {  // reg
            std::cout << "Decoding reg: " << op.reg << std::endl;
            sourceRegisters_[sourceRegisterCount_] =
                csRegToRegister(op.reg + 1, architecture_);
            if (op.reg == 0) {
              // Catch zero register references and pre-complete those operands
              sourceValues_[sourceRegisterCount_] = RegisterValue(0, 8);
              sourceOperandsPending_--;
            }
            sourceRegisterCount_++;
            sourceOperandsPending_++;
            break;
          }
          case 0x2: {  // imm
            vectorImmediates[vecImmCount] = op.imm;
            vecImmCount++;
            break;
          }
          case 0x4: {  // vreg
            sourceRegisters_[sourceRegisterCount_] = decodeVecReg(op.reg);
            if (op.reg == 0) {
              // Catch zero register references and pre-complete those operands
              sourceValues_[sourceRegisterCount_] = RegisterValue(0, vlen);
              sourceOperandsPending_--;
            }
            sourceRegisterCount_++;
            sourceOperandsPending_++;
            break;
          }
          case 0x5: {  // sysreg
            sourceRegisters_[sourceRegisterCount_] = {
                RegisterType::SYSTEM,
                static_cast<uint16_t>(
                    architecture_.getSystemRegisterTag(op.reg))};
            sourceRegisterCount_++;
            sourceOperandsPending_++;
            break;
          }
        }
      }
      break;
    }
    case RVV_INSN_TYPE::RVV_ST_USTRIDE:     // vse
    case RVV_INSN_TYPE::RVV_ST_STRIDED:     // vsse
    case RVV_INSN_TYPE::RVV_ST_UINDEXED:    // vsuxei
    case RVV_INSN_TYPE::RVV_ST_OINDEXED:    // vsoxei
    case RVV_INSN_TYPE::RVV_ST_WHOLEREG: {  // vsre
      for (uint8_t x = 0; x < metadata_.operandCount; x++) {
        simeng::cs_riscv_op op = metadata_.operands[x];
        switch (op.type) {
          case 0x1: {  // reg
            sourceRegisters_[sourceRegisterCount_] =
                csRegToRegister(op.reg + 1, architecture_);
            if (op.reg == 0) {
              // Catch zero register references and pre-complete those operands
              sourceValues_[sourceRegisterCount_] = RegisterValue(0, 8);
              sourceOperandsPending_--;
            }
            sourceRegisterCount_++;
            sourceOperandsPending_++;
            break;
          }
          case 0x2: {  // imm
            vectorImmediates[vecImmCount] = op.imm;
            vecImmCount++;
            break;
          }
          case 0x4: {  // vreg
            sourceRegisters_[sourceRegisterCount_] = decodeVecReg(op.reg);
            if (op.reg == 0) {
              // Catch zero register references and pre-complete those operands
              sourceValues_[sourceRegisterCount_] = RegisterValue(0, vlen);
              sourceOperandsPending_--;
            }
            sourceRegisterCount_++;
            sourceOperandsPending_++;
            break;
          }
        }
      }
      break;
    }
  }
}

/******************
 * DECODING LOGIC
 *****************/
void Instruction::decode(uint64_t sysreg) {
  if (metadata_.id == RISCV_INS_INVALID) {
    exception_ = InstructionException::EncodingUnallocated;
    exceptionEncountered_ = true;
    return;
  }

  std::cout << "Comes here" << std::endl;

  if (metadata_.id > RVV_INSN_TYPE::RVV_INSNS &&
      metadata_.id < RVV_INSN_TYPE::RVV_INSN_END) {
    setInstructionType(InsnType::isRVV);
    if (metadata_.id > RVV_LOAD && metadata_.id < RVV_STORE) {
      setInstructionType(InsnType::isLoad);
      setInstructionType(InsnType::isRVVLoad);
    } else if (metadata_.id > RVV_STORE && metadata_.id < RVV_CONF) {
      setInstructionType(InsnType::isStore);
      setInstructionType(InsnType::isRVVStore);
    } else {
      setInstructionType(InsnType::isRVVConf);
    }
    std::cout << "Decoding RVV" << std::endl;
    decode_rvv();
    return;
  }

  // Identify branches
  switch (metadata_.opcode) {
    case Opcode::RISCV_BEQ:
    case Opcode::RISCV_BNE:
    case Opcode::RISCV_BLT:
    case Opcode::RISCV_BLTU:
    case Opcode::RISCV_BGE:
    case Opcode::RISCV_BGEU:
    case Opcode::RISCV_JAL:
    case Opcode::RISCV_JALR:
      setInstructionType(InsnType::isBranch);
      break;
      // Identify loads/stores
    case Opcode::RISCV_LR_D:
    case Opcode::RISCV_LR_D_AQ:
    case Opcode::RISCV_LR_D_RL:
    case Opcode::RISCV_LR_D_AQ_RL:
    case Opcode::RISCV_LR_W:
    case Opcode::RISCV_LR_W_AQ:
    case Opcode::RISCV_LR_W_RL:
    case Opcode::RISCV_LR_W_AQ_RL:
      setInstructionType(InsnType::isAtomic);
      [[fallthrough]];
    case Opcode::RISCV_LB:
    case Opcode::RISCV_LBU:
    case Opcode::RISCV_LH:
    case Opcode::RISCV_LHU:
    case Opcode::RISCV_LW:
    case Opcode::RISCV_LWU:
    case Opcode::RISCV_LD:
    case Opcode::RISCV_FLW:
    case Opcode::RISCV_FLD:
      setInstructionType(InsnType::isLoad);
      break;
    case Opcode::RISCV_SC_D:
    case Opcode::RISCV_SC_D_AQ:
    case Opcode::RISCV_SC_D_RL:
    case Opcode::RISCV_SC_D_AQ_RL:
    case Opcode::RISCV_SC_W:
    case Opcode::RISCV_SC_W_AQ:
    case Opcode::RISCV_SC_W_RL:
    case Opcode::RISCV_SC_W_AQ_RL:
      setInstructionType(InsnType::isAtomic);
      [[fallthrough]];
    case Opcode::RISCV_SB:
    case Opcode::RISCV_SW:
    case Opcode::RISCV_SH:
    case Opcode::RISCV_SD:
    case Opcode::RISCV_FSW:
    case Opcode::RISCV_FSD:
      setInstructionType(InsnType::isStore);
      break;
  }

  if (Opcode::RISCV_AMOADD_D <= metadata_.opcode &&
      metadata_.opcode <= Opcode::RISCV_AMOXOR_W_RL) {
    // Atomics: both load and store
    setInstructionType(InsnType::isLoad);
    setInstructionType(InsnType::isStore);
    setInstructionType(InsnType::isAtomic);
  }

  // Extract explicit register accesses and immediates
  for (size_t i = 0; i < metadata_.operandCount; i++) {
    const auto& op = metadata_.operands[i];

    // First operand is always of REG type but could be either source or
    // destination
    if (i == 0 && op.type == RISCV_OP_REG) {
      // If opcode is branch or store (but not atomic or jump) the first operand
      // is a source register, for all other instructions the first operand is a
      // destination register
      if ((isInstruction(InsnType::isBranch) &&
           metadata_.opcode != Opcode::RISCV_JAL &&
           metadata_.opcode != Opcode::RISCV_JALR) ||
          (isInstruction(InsnType::isStore) &&
           !isInstruction(InsnType::isAtomic))) {
        sourceRegisters_[sourceRegisterCount_] =
            csRegToRegister(op.reg, architecture_);

        if (sourceRegisters_[sourceRegisterCount_] ==
            RegisterType::ZERO_REGISTER) {
          // Catch zero register references and pre-complete those operands
          sourceValues_[sourceRegisterCount_] = RegisterValue(0, 8);
        } else {
          sourceOperandsPending_++;
        }

        sourceRegisterCount_++;
      } else {
        /**
         * Register writes to x0 are discarded so no destination register is
         * set.
         *
         * While the execution stage may still write to the results array,
         * when Instruction::getResults and
         * Instruction::getDestinationRegisters are called during writeback,
         * zero sized spans are returned (determined by the value of
         * destinationRegisterCount). This in turn means no register update is
         * performed.
         *
         * TODO this will break if there are more than 2 destination registers
         * with one being the zero register e.g. if an instruction implicitly
         * writes to a system register. The current implementation could mean
         * that the second result is discarded
         *
         */
        if (csRegToRegister(op.reg, architecture_) !=
            RegisterType::ZERO_REGISTER) {
          destinationRegisters_[destinationRegisterCount_] =
              csRegToRegister(op.reg, architecture_);

          destinationRegisterCount_++;
        }
      }
    } else if (i > 0) {
      // First operand is never of MEM or IMM type, every register operand after
      // the first is a source register
      if (op.type == RISCV_OP_REG) {
        //  Second or third register operand
        sourceRegisters_[sourceRegisterCount_] =
            csRegToRegister(op.reg, architecture_);

        if (sourceRegisters_[sourceRegisterCount_] ==
            RegisterType::ZERO_REGISTER) {
          // Catch zero register references and pre-complete those operands
          sourceValues_[sourceRegisterCount_] = RegisterValue(0, 8);
        } else {
          sourceOperandsPending_++;
        }

        sourceRegisterCount_++;
      } else if (op.type == RISCV_OP_MEM) {
        // Memory operand
        // Extract reg number from capstone object
        sourceRegisters_[sourceRegisterCount_] =
            csRegToRegister(op.mem.base, architecture_);
        sourceImm_ = op.mem.disp;
        sourceRegisterCount_++;
        sourceOperandsPending_++;
      } else if (op.type == RISCV_OP_IMM) {
        // Immediate operand
        sourceImm_ = op.imm;
      } else {
        // Something has gone wrong
        assert(false &&
               "Unexpected register type in non-first "
               "operand position");
      }
    } else {
      // Something has gone wrong
      assert(false &&
             "Unexpected register type in first "
             "operand position");
    }
  }

  if ((Opcode::RISCV_SLL <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_SLLW) ||
      (Opcode::RISCV_SRA <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_SRAW) ||
      (Opcode::RISCV_SRL <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_SRLW)) {
    // Shift instructions
    setInstructionType(InsnType::isShift);
  }

  if ((Opcode::RISCV_XOR <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_XORI) ||
      (Opcode::RISCV_OR <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_ORI) ||
      (Opcode::RISCV_AND <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_ANDI) ||
      (Opcode::RISCV_FSGNJN_D <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_FSGNJ_S)) {
    // Logical instructions
    setInstructionType(InsnType::isLogical);
  }

  if ((Opcode::RISCV_SLT <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_SLTU) ||
      (Opcode::RISCV_FEQ_D <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_FEQ_S) ||
      (Opcode::RISCV_FLE_D <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_FLT_S) ||
      (Opcode::RISCV_FMAX_D <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_FMIN_S)) {
    // Compare instructions
    setInstructionType(InsnType::isCompare);
  }

  if ((Opcode::RISCV_MUL <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_MULW) ||
      (Opcode::RISCV_FMADD_D <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_FMADD_S) ||
      (Opcode::RISCV_FMSUB_D <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_FMUL_S) ||
      (Opcode::RISCV_FNMADD_D <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_FNMSUB_S)) {
    // Multiply instructions
    setInstructionType(InsnType::isMultiply);
  }

  if ((Opcode::RISCV_REM <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_REMW) ||
      (Opcode::RISCV_DIV <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_DIVW) ||
      (Opcode::RISCV_FDIV_D <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_FDIV_S) ||
      (Opcode::RISCV_FSQRT_D <= metadata_.opcode &&
       metadata_.opcode <= Opcode::RISCV_FSQRT_S)) {
    // Divide instructions
    setInstructionType(InsnType::isDivide);
  }

  if ((metadata_.opcode >= Opcode::RISCV_FADD_D &&
       metadata_.opcode <= Opcode::RISCV_FDIV_S) ||
      (metadata_.opcode >= Opcode::RISCV_FEQ_D &&
       metadata_.opcode <= Opcode::RISCV_FSW)) {
    // Floating point operation
    setInstructionType(InsnType::isFloat);
    if ((metadata_.opcode >= Opcode::RISCV_FCVT_D_L &&
         metadata_.opcode <= Opcode::RISCV_FCVT_W_S)) {
      setInstructionType(InsnType::isConvert);
    }
  }

  // Set branch type
  switch (metadata_.opcode) {
    case Opcode::RISCV_BEQ:
    case Opcode::RISCV_BNE:
    case Opcode::RISCV_BLT:
    case Opcode::RISCV_BLTU:
    case Opcode::RISCV_BGE:
    case Opcode::RISCV_BGEU:
      branchType_ = BranchType::Conditional;
      knownOffset_ = sourceImm_;
      break;
    case Opcode::RISCV_JAL:
    case Opcode::RISCV_JALR:
      branchType_ = BranchType::Unconditional;
      knownOffset_ = sourceImm_;
      break;
  }
}

}  // namespace riscv
}  // namespace arch
}  // namespace simeng