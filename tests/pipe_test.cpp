#include "gtest/gtest.h"
#include "pipe.h"

TEST(SquareRootTest, PositiveNos) { 
	pipe_t p;
}

TEST (SquareRootTest, ZeroAndNegativeNos) { 
	pipe_t * p = NULL;
	ASSERT_EQ(0, p);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

