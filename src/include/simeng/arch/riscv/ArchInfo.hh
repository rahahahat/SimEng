#pragma once

#include "simeng/arch/ArchInfo.hh"

namespace simeng {
namespace arch {
namespace riscv {

// A temporary enum to hold system register addresses
// TODO this should be removed upon relevant capstone updates
typedef enum riscv_sysreg {
  RISCV_SYSREG_FFLAGS = 0x001,
  RISCV_SYSREG_FRM = 0x002,
  RISCV_SYSREG_FCSR = 0x003,
  RISCV_SYSREG_CYCLE = 0xC00,
  RISCV_SYSREG_TIME = 0xC01,
  RISCV_SYSREG_INSTRET = 0xC02,
  RISCV_V_SYSREG_VSTART = 0x008,
  RISCV_V_SYSREG_VXSTAT = 0x009,
  RISCV_V_SYSREG_VXRM = 0x00A,
  RISCV_V_SYSREG_VCSR = 0x00F,
  RISCV_V_SYSREG_VL = 0xC20,
  RISCV_V_SYSREG_VTYPE = 0xC21,
  RISCV_V_SYSREG_VLENB = 0xC22
} riscv_sysreg;

//  A struct of RISC-V specific constants
namespace constantsPool {
const uint8_t addressAlignMask = 0x3;
const uint8_t addressAlignMaskCompressed = 0x1;
const uint8_t minInstWidthBytes = 4;
const uint8_t minInstWidthBytesCompressed = 2;
};  // namespace constantsPool

/** A class to hold and generate riscv specific architecture configuration
 * options. */
class ArchInfo : public simeng::arch::ArchInfo {
 public:
  ArchInfo(ryml::ConstNodeRef config)
      : sysRegisterEnums_(
            {riscv_sysreg::RISCV_SYSREG_FFLAGS, riscv_sysreg::RISCV_SYSREG_FRM,
             riscv_sysreg::RISCV_SYSREG_FCSR, riscv_sysreg::RISCV_SYSREG_CYCLE,
             riscv_sysreg::RISCV_SYSREG_TIME,
             riscv_sysreg::RISCV_SYSREG_INSTRET,
             riscv_sysreg::RISCV_V_SYSREG_VSTART,
             riscv_sysreg::RISCV_V_SYSREG_VXSTAT,
             riscv_sysreg::RISCV_V_SYSREG_VXRM,
             riscv_sysreg::RISCV_V_SYSREG_VCSR, riscv_sysreg::RISCV_V_SYSREG_VL,
             riscv_sysreg::RISCV_V_SYSREG_VTYPE,
             riscv_sysreg::RISCV_V_SYSREG_VLENB}) {
    // Generate the config-defined physical register structure and quantities
    ryml::ConstNodeRef regConfig = config["Register-Set"];
    archRegStruct_ = {{8, 32},
                      {8, 32},
                      {8, static_cast<uint16_t>(sysRegisterEnums_.size())},
                      /** use SIMENG CONFIG FOR VLEN*/
                      {16, 32}};

    uint16_t gpCount = regConfig["GeneralPurpose-Count"].as<uint16_t>();
    uint16_t fpCount = regConfig["FloatingPoint-Count"].as<uint16_t>();
    uint16_t vlen = config["Core"]["Vlen"].as<uint16_t>();
    physRegStruct_ = {{8, gpCount},
                      {8, fpCount},
                      {8, static_cast<uint16_t>(sysRegisterEnums_.size())},
                      {vlen, 32}};
    physRegQuantities_ = {gpCount, fpCount,
                          static_cast<uint16_t>(sysRegisterEnums_.size()), 32};
  }

  /** Get the set of system register enums currently supported. */
  const std::vector<uint64_t>& getSysRegEnums() const override {
    return sysRegisterEnums_;
  }

  /** Get the structure of the architecture register fileset(s). */
  const std::vector<simeng::RegisterFileStructure>& getArchRegStruct()
      const override {
    return archRegStruct_;
  }

  /** Get the structure of the physical register fileset(s) as defined in the
   * simulation configuration. */
  const std::vector<simeng::RegisterFileStructure>& getPhysRegStruct()
      const override {
    return physRegStruct_;
  }

  /** Get the quantities of the physical register in each fileset as defined in
   * the simulation configuration. */
  const std::vector<uint16_t>& getPhysRegQuantities() const override {
    return physRegQuantities_;
  }

 private:
  /** The vector of all system register Capstone enum values used in the
   * associated Architecture class. */
  const std::vector<uint64_t> sysRegisterEnums_;

  /** The structure of the architectural register filesets within the
   * implemented aarch64 architecture. */
  std::vector<simeng::RegisterFileStructure> archRegStruct_;

  /** The structure of the physical register filesets within the
   * implemented aarch64 architecture. */
  std::vector<simeng::RegisterFileStructure> physRegStruct_;

  /** The quantities of the physical register within each filesets of the
   * implemented aarch64 architecture. */
  std::vector<uint16_t> physRegQuantities_;
};

}  // namespace riscv
}  // namespace arch
}  // namespace simeng