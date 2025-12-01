#include "../src/astronomical.h"
#include "../src/prayer_times.h"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  putenv((char *)"TZ=UTC");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
