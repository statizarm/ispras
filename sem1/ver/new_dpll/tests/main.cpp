//
// Created by art on 11/1/21.
//
#include <gtest/gtest.h>

#include "cnf.h"

TEST(solveTest, solveCorrectExpressionTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a"), Literal("b")},
      std::vector<Literal>{Literal("b"), Literal("a", true)}
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveCNFWithRepeatTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a"), Literal("a", true)}
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveCNFWithPureLitTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a"), Literal("b")},
      std::vector<Literal>{Literal("c"), Literal("b", false)},
      std::vector<Literal>{Literal("c"), Literal("a", false)},
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveCNFWithSingleLitEntryTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a")},
      std::vector<Literal>{Literal("b")},
      std::vector<Literal>{Literal("c")},
      std::vector<Literal>{Literal("d")},
      std::vector<Literal>{Literal("e")},
      std::vector<Literal>{Literal("f")}
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveCNFWithSingleLitTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a")},
      std::vector<Literal>{Literal("a")},
      std::vector<Literal>{Literal("a")},
      std::vector<Literal>{Literal("a")},
      std::vector<Literal>{Literal("a")},
      std::vector<Literal>{Literal("a")}
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveCNFWithSingleOpposedLitTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a", true)},
      std::vector<Literal>{Literal("a", true)},
      std::vector<Literal>{Literal("a", true)},
      std::vector<Literal>{Literal("a", true)},
      std::vector<Literal>{Literal("a", true)},
      std::vector<Literal>{Literal("a", true)}
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveEmptyCNFTest) {
  std::vector<Clause> clauses;

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveEmptyClausesTest) {
  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>(),
      std::vector<Literal>(),
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), false);
}

TEST(solveTest, solveCorrectWithDoubleOpposedLitExpressionTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a"), Literal("b")},
      std::vector<Literal>{Literal("b", true), Literal("a", true)}
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveWrongExpressionTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("b", true), Literal("a", true)},
      std::vector<Literal>{Literal("b")},
      std::vector<Literal>{Literal("b", true)}
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), false);
}

TEST(solveTest, solveTrivialCorrectExpressionTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a")}
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveTrivialWrongExpressionTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a")},
      std::vector<Literal>{Literal("a", true)}
  };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), false);
}

TEST(solveTest, solveAllOpposedTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a", true), Literal("b", true)},
      std::vector<Literal>{Literal("b", true), Literal("a", true)},
      };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

TEST(solveTest, solveExpressionWithDuplicatesTest) {

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a"), Literal("a"), Literal("b")},
      std::vector<Literal>{Literal("b", true), Literal("b", true)},
      };

  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  ASSERT_EQ(cnf.solve(), true);
}

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

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
