#include "AArch64RegressionTest.hh"

namespace {

using InstNeon = AArch64RegressionTest;

TEST_P(InstNeon, fmov) {
  // FP64 vector from immediate
  RUN_AARCH64(R"(
    fmov v0.2d, 1.0
    fmov v1.2d, -0.125
  )");
  EXPECT_EQ((getVectorRegisterElement<double, 0>(0)), 1.0);
  EXPECT_EQ((getVectorRegisterElement<double, 1>(0)), 1.0);
  EXPECT_EQ((getVectorRegisterElement<double, 0>(1)), -0.125);
  EXPECT_EQ((getVectorRegisterElement<double, 1>(1)), -0.125);
}

TEST_P(InstNeon, fsub) {
  initialHeapData_.resize(32);
  double* heap = reinterpret_cast<double*>(initialHeapData_.data());
  heap[0] = 1.0;
  heap[1] = -42.76;
  heap[2] = -0.125;
  heap[3] = 321.0;

  RUN_AARCH64(R"(
    # Get heap address
    mov x0, 0
    mov x8, 214
    svc #0

    ldr q0, [x0]
    ldr q1, [x0, #16]
    fsub v2.2d, v0.2d, v1.2d
  )");
  EXPECT_EQ((getVectorRegisterElement<double, 0>(2)), 1.125);
  EXPECT_EQ((getVectorRegisterElement<double, 1>(2)), -363.76);
}

TEST_P(InstNeon, movi) {
  RUN_AARCH64(R"(
    movi v0.4s, 42
    movi v1.4s, 42, lsl #8
    movi v2.4s, 3, lsl #24
  )");
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 0>(0)), 42u);
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 1>(0)), 42u);
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 2>(0)), 42u);
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 3>(0)), 42u);

  EXPECT_EQ((getVectorRegisterElement<uint32_t, 0>(1)), (42u << 8));
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 1>(1)), (42u << 8));
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 2>(1)), (42u << 8));
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 3>(1)), (42u << 8));

  EXPECT_EQ((getVectorRegisterElement<uint32_t, 0>(2)), (3u << 24));
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 1>(2)), (3u << 24));
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 2>(2)), (3u << 24));
  EXPECT_EQ((getVectorRegisterElement<uint32_t, 3>(2)), (3u << 24));
}

INSTANTIATE_TEST_SUITE_P(AArch64, InstNeon, ::testing::Values(EMULATION),
                         coreTypeToString);

}  // namespace
