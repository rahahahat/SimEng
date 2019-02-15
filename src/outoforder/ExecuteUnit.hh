#pragma once

#include "../BranchPredictor.hh"
#include "../Instruction.hh"
#include "../PipelineBuffer.hh"
#include "DispatchIssueUnit.hh"

namespace simeng {
namespace outoforder {

/** An execute unit for an out-of-order pipeline. Executes instructions and
 * forwards results to the dispatch/issue stage. */
class ExecuteUnit {
 public:
  /** Constructs an execute unit with references to an input and output buffer,
   * the decode unit, the currently used branch predictor, and a pointer to
   * process memory. */
  ExecuteUnit(PipelineBuffer<std::shared_ptr<Instruction>>& fromDecode,
              PipelineBuffer<std::shared_ptr<Instruction>>& toWriteback,
              DispatchIssueUnit& dispatchIssueUnit, BranchPredictor& predictor,
              char* memory);

  /** Tick the execute unit. Executes the current instruction and forwards the
   * results back to the dispatch/issue stage. */
  void tick();

  /** Query whether a branch misprediction was discovered this cycle. */
  bool shouldFlush() const;

  /** Retrieve the target instruction address associated with the most recently
   * discovered misprediction. */
  uint64_t getFlushAddress() const;

  /** Retrieve the sequence ID associated with the most recently discovered
   * misprediction. */
  uint64_t getFlushSeqId() const;

 private:
  /** A buffer of instructions to execute. */
  PipelineBuffer<std::shared_ptr<Instruction>>& fromDecodeBuffer;

  /** A buffer for writing executed instructions into. */
  PipelineBuffer<std::shared_ptr<Instruction>>& toWritebackBuffer;

  /** A reference to the decode unit, for forwarding operands. */
  DispatchIssueUnit& dispatchIssueUnit;

  /** A reference to the branch predictor, for updating with prediction results.
   */
  BranchPredictor& predictor;

  /** A pointer to process memory. */
  char* memory;

  /** Whether the core should be flushed after this cycle. */
  bool shouldFlush_;

  /** The target instruction address the PC should be reset to after this cycle.
   */
  uint64_t pc;

  /** The sequence ID of the youngest instruction that should remain after the
   * current flush. */
  uint64_t flushAfter;
};

}  // namespace outoforder
}  // namespace simeng