#include "RISCVRegressionTest.hh"
namespace {

using RVV = RISCVRegressionTest;

#define RISCV_V_SYSREG_VL 0xC20
#define RISCV_V_SYSREG_VTYPE 0xC21
#define RISCV_V_SYSREG_VLENB 0xC22

// TEST_P(RVV, vle64) {
//   initialHeapData_.resize(16);
//   uint64_t* heap = reinterpret_cast<uint64_t*>(initialHeapData_.data());
//   heap[0] = 0xDEADBEEFBEEFDEAD;
//   heap[1] = 0xDEADBEEFDEADBEEF;
//   RUN_RISCV(R"(
//     li a7, 214
//     ecall
//     add t2, t2, a0
//     vle64.v v1, (t2)
//   )");

//   const uint64_t* vreg = getRVVRegister<uint64_t>((uint8_t)1);
//   EXPECT_EQ(vreg[0], heap[0]);
//   EXPECT_EQ(vreg[1], heap[1]);
// }

// TEST_P(RVV, vle32) {
//   initialHeapData_.resize(16);
//   uint32_t* heap = reinterpret_cast<uint32_t*>(initialHeapData_.data());
//   heap[0] = 0xDEAD;
//   heap[1] = 0xBEEF;
//   heap[2] = 0xABCD;
//   heap[3] = 0xDCBA;
//   RUN_RISCV(R"(
//     li a7, 214
//     ecall
//     add t2, t2, a0
//     vle32.v v1, (t2)
//   )");

//   const uint32_t* vreg = getRVVRegister<uint32_t>((uint8_t)1);
//   EXPECT_EQ(vreg[0], heap[0]);
//   EXPECT_EQ(vreg[1], heap[1]);
//   EXPECT_EQ(vreg[2], heap[2]);
//   EXPECT_EQ(vreg[3], heap[3]);
// }

// TEST_P(RVV, vle16) {
//   initialHeapData_.resize(16);
//   uint16_t* heap = reinterpret_cast<uint16_t*>(initialHeapData_.data());
//   heap[0] = 0xAA;
//   heap[1] = 0xBB;
//   heap[2] = 0xCC;
//   heap[3] = 0xDD;
//   heap[4] = 0xEE;
//   heap[5] = 0xFF;
//   heap[6] = 0x11;
//   heap[7] = 0x22;
//   RUN_RISCV(R"(
//     li a7, 214
//     ecall
//     add t2, t2, a0
//     vle16.v v1, (t2)
//   )");

//   const uint16_t* vreg = getRVVRegister<uint16_t>((uint8_t)1);
//   EXPECT_EQ(vreg[0], heap[0]);
//   EXPECT_EQ(vreg[1], heap[1]);
//   EXPECT_EQ(vreg[2], heap[2]);
//   EXPECT_EQ(vreg[3], heap[3]);
//   EXPECT_EQ(vreg[4], heap[4]);
//   EXPECT_EQ(vreg[5], heap[5]);
//   EXPECT_EQ(vreg[6], heap[6]);
//   EXPECT_EQ(vreg[7], heap[7]);
// }

// TEST_P(RVV, vle8) {
//   initialHeapData_.resize(16);
//   uint8_t* heap = reinterpret_cast<uint8_t*>(initialHeapData_.data());
//   for (uint8_t x = 0; x < 16; x++) {
//     heap[x] = x + 1;
//   }
//   RUN_RISCV(R"(
//     li a7, 214
//     ecall
//     add t2, t2, a0
//     vle8.v v1, (t2)
//   )");

//   const uint8_t* vreg = getRVVRegister<uint8_t>((uint8_t)1);
//   for (uint8_t x = 0; x < 16; x++) {
//     EXPECT_EQ(vreg[x], heap[x]);
//   }
// }

TEST_P(RVV, vsetvli) {
  RUN_RISCV(R"(
    vsetvli t0, zero, e16, m2, ta, ma
  )");
  const uint64_t vtype_enc = getSysRegister(RISCV_V_SYSREG_VTYPE);
  EXPECT_EQ(vtype_enc, (uint64_t)0b0000000011001001);
}

TEST_P(RVV, vid) {
  RUN_RISCV(R"(
    vsetvli a3, zero, e32, m4, ta, ma
    vid.v v8
  )");
  for (int x = 0; x < 4; x++) {
    const uint32_t* vreg = getRVVRegister<uint32_t>(8 + x);
    for (int y = 0; y < 4; y++) {
      EXPECT_EQ(vreg[y], (4 * x) + y);
    }
  }
  RUN_RISCV(R"(
    vsetvli a3, zero, e16, m4, ta, ma
    vid.v v8
  )");
  for (int x = 0; x < 4; x++) {
    const uint16_t* vreg = getRVVRegister<uint16_t>(8 + x);
    for (int y = 0; y < 8; y++) {
      EXPECT_EQ(vreg[y], (8 * x) + y);
    }
  }
  RUN_RISCV(R"(
    vsetvli a3, zero, e8, m4, ta, ma
    vid.v v8
  )");
  for (int x = 0; x < 4; x++) {
    const uint8_t* vreg = getRVVRegister<uint8_t>(8 + x);
    for (int y = 0; y < 16; y++) {
      EXPECT_EQ(vreg[y], (16 * x) + y);
    }
  }
  RUN_RISCV(R"(
    vsetvli a3, zero, e64, m4, ta, ma
    vid.v v8
  )");
  for (int x = 0; x < 4; x++) {
    const uint64_t* vreg = getRVVRegister<uint64_t>(8 + x);
    for (int y = 0; y < 2; y++) {
      EXPECT_EQ(vreg[y], (2 * x) + y);
    }
  }

  RUN_RISCV(R"(
    vsetvli a3, zero, e64, m1, ta, ma
    vid.v v9
  )");

  for (int x = 0; x < 1; x++) {
    const uint64_t* vreg = getRVVRegister<uint64_t>(9 + x);
    for (int y = 0; y < 2; y++) {
      EXPECT_EQ(vreg[y], (2 * x) + y);
    }
  }
}

TEST_P(RVV, vsll_vi) {
  RUN_RISCV(R"(
    vsetvli a3, zero, e64, m2, ta, ma
    vid.v v9
    vsll.vi v10, v9, 1
  )");

  for (int x = 0; x < 1; x++) {
    const uint64_t* vreg = getRVVRegister<uint64_t>(10 + x);
    for (int y = 0; y < 2; y++) {
      EXPECT_EQ(vreg[y], ((2 * x) + y) << 1);
    }
  }
}

TEST_P(RVV, vadd_vx) {
  RUN_RISCV(R"(
    vsetvli a3, zero, e64, m2, ta, ma
    vid.v v9
    li t0, 8
    vadd.vx v11, v9, t0
  )");

  for (int x = 0; x < 2; x++) {
    const uint64_t* vreg = getRVVRegister<uint64_t>(11 + x);
    for (int y = 0; y < 2; y++) {
      EXPECT_EQ(vreg[y], ((2 * x) + y) + 8);
    }
  }
}

TEST_P(RVV, vadd_vv) {
  RUN_RISCV(R"(
    vsetvli a3, zero, e64, m4, ta, ma
    vid.v v9
    vid.v v13
    vadd.vv v17, v9, v13
  )");

  for (int x = 0; x < 4; x++) {
    const uint64_t* vreg = getRVVRegister<uint64_t>(17 + x);
    for (int y = 0; y < 2; y++) {
      EXPECT_EQ(vreg[y], ((2 * x) + y) * 2);
    }
  }
}

TEST_P(RVV, vluxei) {
  initialHeapData_.resize(32);
  uint32_t* heap = reinterpret_cast<uint32_t*>(initialHeapData_.data());
  for (uint32_t x = 0; x < 8; x++) {
    heap[x] = 1000 + x + 1;
  }

  RUN_RISCV(R"(
    li a7, 214
    ecall
    add t2, t2, a0
    vsetvli a3, zero, e64, m4, ta, ma
    vid.v v8
    vsll.vi v12, v8, 2
    vadd.vx v16, v12, t2
    vsetvli zero, zero, e32, m2, ta, ma
    vluxei64.v v24, (zero), v16
  )");
  for (int x = 0; x < 2; x++) {
    const uint32_t* vreg = getRVVRegister<uint32_t>(24 + x);
    for (int y = 0; y < 4; y++) {
      EXPECT_EQ(vreg[y], 1000 + (x * 4) + 1 + y);
    }
  }
}

// TEST_P(RVV, vsoxei) {
//   initialHeapData_.resize(32);
//   uint32_t* heap = reinterpret_cast<uint32_t*>(initialHeapData_.data());
//   for (uint32_t x = 0; x < 8; x++) {
//     heap[x] = 1000 + x + 1;
//   }

//   RUN_RISCV(R"(
//     li a7, 214
//     ecall
//     add t2, t2, a0
//     vsetvli a3, zero, e64, m4, ta, ma
//     vid.v v8
//     vsll.vi v12, v8, 2
//     vadd.vx v16, v12, t2
//     vsetvli zero, zero, e32, m2, ta, ma
//     vluxei64.v v24, (zero), v16
//   )");
//   for (int x = 0; x < 2; x++) {
//     const uint32_t* vreg = getRVVRegister<uint32_t>(24 + x);
//     for (int y = 0; y < 4; y++) {
//       EXPECT_EQ(vreg[y], 1000 + (x * 4) + 1 + y);
//     }
//   }
// }

// TEST_P(RVV, vluxei_decode) {
//   initialHeapData_.resize(16);
//   uint64_t* heap =
//   reinterpret_cast<uint64_t*>(initialHeapData_.data()); heap[0] =
//   0xDEADBEEFBEEFDEAD; heap[1] = 0xDEADBEEFDEADBEEF; RUN_RISCV(R"(
//     vluxei8.v      v26, (zero), v16
//     vluxei16.v      v26, (zero), v16
//     vluxei32.v      v26, (zero), v16
//     vluxei64.v      v26, (zero), v16
//   )");
// }
// TEST_P(RVV, vsoxei_decode) {
//   initialHeapData_.resize(16);
//   uint64_t* heap =
//   reinterpret_cast<uint64_t*>(initialHeapData_.data()); heap[0] =
//   0xDEADBEEFBEEFDEAD; heap[1] = 0xDEADBEEFDEADBEEF; RUN_RISCV(R"(
//     vsoxei8.v      v26, (zero), v16
//     vsoxei16.v      v26, (zero), v16
//     vsoxei32.v      v26, (zero), v16
//     vsoxei64.v      v26, (zero), v16
//   )");
// }

INSTANTIATE_TEST_SUITE_P(RISCV, RVV,
                         ::testing::Values(std::make_tuple(EMULATION, "{}")),
                         paramToString);

}  // namespace