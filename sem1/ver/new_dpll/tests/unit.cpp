//
// Created by art on 11/1/21.
//
#include <gtest/gtest.h>

#include "cnf.h"

TEST(buildCNFTest, buildFromStringWithTest) {
  std::string source = "c A sample .cnf file.\n"
                       "p cnf 3 2\n"
                       "1 -3 0\n"
                       "2 3 -1 0\n";

  std::istringstream in(source);

  auto cnf = CNF::load_cnf(in);

  ASSERT_EQ(cnf->solve(), true);
}

TEST(buildCNFTest, buildAndSolveWrongExpressionTest) {
  std::string source = "c A sample .cnf file.\n"
                       "p cnf 1 2\n"
                       "1 0\n"
                       "-1 0\n";

  std::istringstream in(source);

  auto cnf = CNF::load_cnf(in);

  ASSERT_EQ(cnf->solve(), false);
}

TEST(buildCNFTest, buildAndSolveWrongExpressionInLogFromatTest) {
  std::string source = "c A sample .cnf file.\n"
                       "p cnf 1 2\n"
                       "1 0\n"
                       "-1 0\n"
                       "hmm\n";

  std::istringstream in(source);

  auto cnf = CNF::load_cnf(in);
  ASSERT_EQ(cnf->solve(), false);
}

TEST(buildCNFTest, buildAndSolveWrongExpressionWithThreeClTest) {
  std::string source = "c A sample .cnf file.\n"
                       "p cnf 3 3\n"
                       "-1 -2 0\n"
                       "-1 2 0\n"
                       "1 0\n"
                       "hmm\n";

  std::istringstream in(source);

  auto cnf = CNF::load_cnf(in);
  ASSERT_EQ(cnf->solve(), false);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
