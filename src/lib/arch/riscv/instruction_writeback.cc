#include <bitset>

#include "InstructionMetadata.hh"
#include "simeng/arch/riscv/Architecture.hh"
#include "simeng/arch/riscv/Instruction.hh"
#include "simeng/arch/riscv/rvv/RVV.hh"

namespace simeng {
namespace arch {
namespace riscv {

void Instruction::writeback(simeng::RegisterFileSet& rfs) {
  auto results = getResults();
  auto destinations = getDestinationRegisters();
  if (isInstruction(InsnType::isRVVStore)) {
  } else if (isInstruction(InsnType::isRVV)) {
    const struct vtype_reg& vtype =
        decode_vtype(sourceValues_[sourceRegisterCount_ - 1].get<uint64_t>());

  } else if (isStoreData()) {
    for (size_t i = 0; i < results.size(); i++) {
      auto reg = destinations[i];
      rfs.set(reg, results[i]);
    }
  } else {
    for (size_t i = 0; i < results.size(); i++) {
      auto reg = destinations[i];
      rfs.set(reg, results[i]);
    }
  }
};

}  // namespace riscv
}  // namespace arch
}  // namespace simeng