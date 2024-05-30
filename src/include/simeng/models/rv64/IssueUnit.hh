#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

#include "simeng/ArchitecturalRegisterFileSet.hh"
#include "simeng/Core.hh"
#include "simeng/memory/FlatMemoryInterface.hh"
#include "simeng/pipeline/DecodeUnit.hh"
#include "simeng/pipeline/ExecuteUnit.hh"
#include "simeng/pipeline/FetchUnit.hh"
#include "simeng/pipeline/LoadStoreQueue.hh"
#include "simeng/pipeline/WritebackUnit.hh"

namespace simeng {
namespace models {
namespace rv64 {

class IssueUnit {
 public:
  IssueUnit(pipeline::ExecuteUnit& exunit, pipeline::LoadStoreQueue lsq,
            pipeline::PipelineBuffer<std::shared_ptr<Instruction>>
                decodeToIssueBuffer,
            pipeline::PipelineBuffer<std::shared_ptr<Instruction>>
                issueToExecuteBuffer)
}
}  // namespace rv64
}  // namespace models
}  // namespace simeng