// #include "simeng/arch/riscv/RvvDecode.h"

// #include <bitset>

// #include "../ConfigInit.hh"
// #include "arch/riscv/InstructionMetadata.hh"
// #include "gmock/gmock.h"
// #include "simeng/arch/riscv/Instruction.hh"
// #include "simeng/version.hh"

// namespace simeng {
// namespace arch {
// namespace riscv {

// // TEST(RvvDecode, vset_i_vl_i_) {
// //   uint32_t insn = 0x80107057;
// //   std::string str = rvv_predecode(insn, RV_MAJOR_OPCODES::OP_V);
// //   std::cout << str << std::endl;
// //   str = rvv_predecode(0x0c16f657, RV_MAJOR_OPCODES::OP_V);
// //   std::cout << str << std::endl;
// //   str = rvv_predecode(0xcc947657, RV_MAJOR_OPCODES::OP_V);
// //   std::cout << str << std::endl;
// //   EXPECT_EQ(1, 1);
// // }

// TEST(RvvDecode, vle_vse) {
//   uint32_t insn = 0x02030187;
//   std::string str;
//   auto decode = rvv_predecode(insn, RV_MAJOR_OPCODES::LOAD_FP);
//   str = decode.mnemonic + " " + decode.operand_str;
//   std::cout << str << std::endl;
//   decode = rvv_predecode(0x02035187, RV_MAJOR_OPCODES::LOAD_FP);
//   str = decode.mnemonic + " " + decode.operand_str;
//   std::cout << str << std::endl;
//   decode = rvv_predecode(0x02036187, RV_MAJOR_OPCODES::LOAD_FP);
//   str = decode.mnemonic + " " + decode.operand_str;
//   std::cout << str << std::endl;
//   decode = rvv_predecode(0x02037187, RV_MAJOR_OPCODES::LOAD_FP);
//   str = decode.mnemonic + " " + decode.operand_str;
//   std::cout << str << std::endl;
//   decode = rvv_predecode(0x020301a7, RV_MAJOR_OPCODES::STORE_FP);
//   str = decode.mnemonic + " " + decode.operand_str;
//   std::cout << str << std::endl;
//   decode = rvv_predecode(0x020351a7, RV_MAJOR_OPCODES::STORE_FP);
//   str = decode.mnemonic + " " + decode.operand_str;
//   std::cout << str << std::endl;
//   decode = rvv_predecode(0x020361a7, RV_MAJOR_OPCODES::STORE_FP);
//   str = decode.mnemonic + " " + decode.operand_str;
//   std::cout << str << std::endl;
//   decode = rvv_predecode(0x020371a7, RV_MAJOR_OPCODES::STORE_FP);
//   str = decode.mnemonic + " " + decode.operand_str;
//   std::cout << str << std::endl;
//   EXPECT_EQ(1, 1);
// }

// }  // namespace riscv
// }  // namespace arch
// }  // namespace simeng