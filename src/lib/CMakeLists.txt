set(SIMENG_SOURCES
    arch/aarch64/Architecture.cc
    arch/aarch64/ExceptionHandler.cc
    arch/aarch64/Instruction.cc
    arch/aarch64/Instruction_address.cc
    arch/aarch64/Instruction_decode.cc
    arch/aarch64/Instruction_execute.cc
    arch/aarch64/InstructionMetadata.cc
    arch/aarch64/MicroDecoder.cc
    arch/riscv/Architecture.cc
    arch/riscv/ExceptionHandler.cc
    arch/riscv/Instruction.cc
    arch/riscv/Instruction_address.cc
    arch/riscv/Instruction_decode.cc
    arch/riscv/Instruction_execute.cc
    arch/riscv/InstructionMetadata.cc
    config/ModelConfig.cc
    kernel/Linux.cc
    kernel/LinuxProcess.cc
    memory/FixedLatencyMemoryInterface.cc
    memory/FlatMemoryInterface.cc
    models/emulation/Core.cc
    models/inorder/Core.cc
    models/outoforder/Core.cc
    pipeline/A64FXPortAllocator.cc
    pipeline/BalancedPortAllocator.cc
    pipeline/M1PortAllocator.cc
    pipeline/DecodeUnit.cc
    pipeline/DispatchIssueUnit.cc
    pipeline/ExecuteUnit.cc
    pipeline/FetchUnit.cc
    pipeline/LoadStoreQueue.cc
    pipeline/MappedRegisterFileSet.cc
    pipeline/RegisterAliasTable.cc
    pipeline/RenameUnit.cc
    pipeline/ReorderBuffer.cc
    pipeline/WritebackUnit.cc
    AlwaysNotTakenPredictor.cc
    ArchitecturalRegisterFileSet.cc
    CMakeLists.txt
    CoreInstance.cc
    Elf.cc
    GenericPredictor.cc
    PerceptronPredictor.cc
    RegisterFileSet.cc
    RegisterValue.cc
    SpecialFileDirGen.cc
    arch/riscv/RvvPredecode.cc
    arch/riscv/rvv_predecode/vsetxvlx.cc
    arch/riscv/rvv_predecode/vle_vse.cc
    arch/riscv/rvv_predecode/vlo_vso_xei.cc
    arch/riscv/rvv_predecode/vlu_vsu_xei.cc
    arch/riscv/rvv_predecode/vlse_vsse.cc
    arch/riscv/rvv_predecode/vlxr_vsxr.cc
    arch/riscv/rvv_predecode/vlexff.cc
    Backward.cc
)

configure_file(${capstone_SOURCE_DIR}/arch/AArch64/AArch64GenInstrInfo.inc AArch64GenInstrInfo.inc COPYONLY)
configure_file(${capstone_SOURCE_DIR}/arch/RISCV/RISCVGenInstrInfo.inc RISCVGenInstrInfo.inc COPYONLY)

add_library(libsimeng SHARED ${SIMENG_SOURCES} ${SIMENG_HEADERS})
set_target_properties(libsimeng PROPERTIES OUTPUT_NAME simeng)

target_include_directories(libsimeng PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(libsimeng PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(libsimeng capstone fmt bfd)

set_target_properties(libsimeng PROPERTIES VERSION ${SimEng_VERSION})
set_target_properties(libsimeng PROPERTIES SOVERSION ${SimEng_VERSION_MAJOR})

install(TARGETS libsimeng DESTINATION lib)

# Store build metadata in version.hh
get_target_property(SIMENG_COMPILE_OPTIONS libsimeng COMPILE_OPTIONS)
get_target_property(SIMENG_COMPILE_DEFINITIONS libsimeng COMPILE_DEFINITIONS)
get_target_property(SIMENG_VERSION libsimeng VERSION)
configure_file(${PROJECT_SOURCE_DIR}/src/include/simeng/version.hh.in ${PROJECT_SOURCE_DIR}/src/include/simeng/version.hh)
