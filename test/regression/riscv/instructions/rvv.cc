#include "RISCVRegressionTest.hh"

namespace {

using RVV = RISCVRegressionTest;

TEST_P(RVV, vle64) {
  initialHeapData_.resize(16);
  uint64_t* heap = reinterpret_cast<uint64_t*>(initialHeapData_.data());
  heap[0] = 0xDEADBEEFBEEFDEAD;
  heap[1] = 0xDEADBEEFDEADBEEF;
  RUN_RISCV(R"(
    li a7, 214
    ecall
    add t2, t2, a0
    vle64.v v1, (t2)
  )");

  const uint64_t* vreg = getRVVRegister<uint64_t>((uint8_t)1);
  EXPECT_EQ(vreg[0], heap[0]);
  EXPECT_EQ(vreg[1], heap[1]);
}

TEST_P(RVV, vle32) {
  initialHeapData_.resize(16);
  uint32_t* heap = reinterpret_cast<uint32_t*>(initialHeapData_.data());
  heap[0] = 0xDEAD;
  heap[1] = 0xBEEF;
  heap[2] = 0xABCD;
  heap[3] = 0xDCBA;
  RUN_RISCV(R"(
    li a7, 214
    ecall
    add t2, t2, a0
    vle32.v v1, (t2)
  )");

  const uint32_t* vreg = getRVVRegister<uint32_t>((uint8_t)1);
  EXPECT_EQ(vreg[0], heap[0]);
  EXPECT_EQ(vreg[1], heap[1]);
  EXPECT_EQ(vreg[2], heap[2]);
  EXPECT_EQ(vreg[3], heap[3]);
}

TEST_P(RVV, vle16) {
  initialHeapData_.resize(16);
  uint16_t* heap = reinterpret_cast<uint16_t*>(initialHeapData_.data());
  heap[0] = 0xAA;
  heap[1] = 0xBB;
  heap[2] = 0xCC;
  heap[3] = 0xDD;
  heap[4] = 0xEE;
  heap[5] = 0xFF;
  heap[6] = 0x11;
  heap[7] = 0x22;
  RUN_RISCV(R"(
    li a7, 214
    ecall
    add t2, t2, a0
    vle16.v v1, (t2)
  )");

  const uint16_t* vreg = getRVVRegister<uint16_t>((uint8_t)1);
  EXPECT_EQ(vreg[0], heap[0]);
  EXPECT_EQ(vreg[1], heap[1]);
  EXPECT_EQ(vreg[2], heap[2]);
  EXPECT_EQ(vreg[3], heap[3]);
  EXPECT_EQ(vreg[4], heap[4]);
  EXPECT_EQ(vreg[5], heap[5]);
  EXPECT_EQ(vreg[6], heap[6]);
  EXPECT_EQ(vreg[7], heap[7]);
}

TEST_P(RVV, vle8) {
  initialHeapData_.resize(16);
  uint8_t* heap = reinterpret_cast<uint8_t*>(initialHeapData_.data());
  for (uint8_t x = 0; x < 16; x++) {
    heap[x] = x + 1;
  }
  RUN_RISCV(R"(
    li a7, 214
    ecall
    add t2, t2, a0
    vle8.v v1, (t2)
  )");

  const uint8_t* vreg = getRVVRegister<uint8_t>((uint8_t)1);
  for (uint8_t x = 0; x < 16; x++) {
    EXPECT_EQ(vreg[x], heap[x]);
  }
}

INSTANTIATE_TEST_SUITE_P(RISCV, RVV,
                         ::testing::Values(std::make_tuple(EMULATION, "{}")),
                         paramToString);

}  // namespace
