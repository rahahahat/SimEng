#include "WritebackUnit.hh"

#include <iostream>

namespace simeng {

WritebackUnit::WritebackUnit(PipelineBuffer<std::shared_ptr<Instruction>>& fromExecute, RegisterFile& registerFile) : fromExecuteBuffer(fromExecute), registerFile(registerFile) {}

void WritebackUnit::tick() {

  auto uop = fromExecuteBuffer.getHeadSlots()[0];
  if (uop == nullptr) {
    return;
  }

  auto results = uop->getResults();
  auto destinations = uop->getDestinationRegisters();
  for (size_t i = 0; i < results.size(); i++) {
    auto reg = destinations[i];
    registerFile.set(reg, results[i]);
  }

  fromExecuteBuffer.getHeadSlots()[0] = nullptr;
}

} // namespace simeng