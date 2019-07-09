#include "A64Architecture.hh"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "A64InstructionMetadata.hh"

namespace simeng {
namespace arch {
namespace aarch64 {

std::unordered_map<uint32_t, A64Instruction> A64Architecture::decodeCache;
std::forward_list<A64InstructionMetadata> A64Architecture::metadataCache;

A64Architecture::A64Architecture(kernel::Linux& kernel) : linux_(kernel) {
  if (cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &capstoneHandle) != CS_ERR_OK) {
    std::cerr << "Could not create capstone handle" << std::endl;
    exit(1);
  }

  cs_option(capstoneHandle, CS_OPT_DETAIL, CS_OPT_ON);
}
A64Architecture::~A64Architecture() { cs_close(&capstoneHandle); }

uint8_t A64Architecture::predecode(const void* ptr, uint8_t bytesAvailable,
                                   uint64_t instructionAddress,
                                   BranchPrediction prediction,
                                   MacroOp& output) const {
  assert(bytesAvailable >= 4 && "Fewer than 4 bytes supplied to A64 decoder");

  // Dereference the instruction pointer to obtain the instruction word
  const uint32_t insn = *static_cast<const uint32_t*>(ptr);
  const uint8_t* encoding = reinterpret_cast<const uint8_t*>(ptr);

  if (!decodeCache.count(insn)) {
    // Generate a fresh decoding, and add to cache
    cs_insn rawInsn;
    cs_detail rawDetail;
    rawInsn.detail = &rawDetail;

    size_t size = 4;
    uint64_t address = 0;

    bool success =
        cs_disasm_iter(capstoneHandle, &encoding, &size, &address, &rawInsn);

    auto metadata = success ? A64InstructionMetadata(rawInsn)
                            : A64InstructionMetadata(encoding);

    // Cache the metadata
    metadataCache.emplace_front(metadata);

    // Get the latencies for this instruction
    auto latencies = getLatencies(metadata);

    // Create and cache an instruction using the metadata and latencies
    decodeCache.insert(
        {insn, {metadataCache.front(), latencies.first, latencies.second}});
  }

  // Retrieve the cached instruction
  std::shared_ptr<Instruction> uop =
      std::make_shared<A64Instruction>(decodeCache.find(insn)->second);

  uop->setInstructionAddress(instructionAddress);
  uop->setBranchPrediction(prediction);

  // Bundle uop into output macro-op and return
  output.resize(1);
  output[0] = uop;

  return 4;
}

std::shared_ptr<ExceptionHandler> A64Architecture::handleException(
    const std::shared_ptr<Instruction>& instruction,
    const ArchitecturalRegisterFileSet& registerFileSet,
    MemoryInterface& memory) const {
  return std::make_shared<A64ExceptionHandler>(instruction, registerFileSet,
                                               memory, linux_);
}

std::vector<RegisterFileStructure> A64Architecture::getRegisterFileStructures()
    const {
  return {
      {8, 32},   // General purpose
      {16, 32},  // Vector
      {1, 1}     // NZCV
  };
}

ProcessStateChange A64Architecture::getInitialState() const {
  ProcessStateChange changes;

  uint64_t stackPointer = linux_.getInitialStackPointer();
  // Set the stack pointer register
  changes.modifiedRegisters.push_back({A64RegisterType::GENERAL, 31});
  changes.modifiedRegisterValues.push_back(stackPointer);

  return changes;
}

bool A64Architecture::canRename(Register reg) const { return true; }

std::pair<uint8_t, uint8_t> A64Architecture::getLatencies(
    A64InstructionMetadata& metadata) const {
  const std::pair<uint8_t, uint8_t> FPSIMD_LATENCY = {6, 1};

  // Look up the instruction opcode to get the latency
  switch (metadata.opcode) {
    case A64Opcode::AArch64_FADDv2f64:
      return FPSIMD_LATENCY;
    case A64Opcode::AArch64_FMULv2f64:
      return FPSIMD_LATENCY;
  }

  // Assume single-cycle, non-blocking for all other instructions
  return {1, 1};
}

}  // namespace aarch64
}  // namespace arch
}  // namespace simeng