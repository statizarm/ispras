//
// Created by art on 11/1/21.
//
#include <string>
#include <fstream>

#include <gtest/gtest.h>
#include <filesystem>

#include "cnf.h"

#define SAT_FILES_DIR "tests/sat"
#define UNSAT_FILES_DIR "tests/unsat"

class StressTest : public ::testing::Test {
 protected:
  void SetUp() override {
    std::filesystem::path sat_dir_path = std::filesystem::current_path().parent_path() / SAT_FILES_DIR;
    std::filesystem::path unsat_dir_path = std::filesystem::current_path().parent_path() / UNSAT_FILES_DIR;


    for (const auto &entry : std::filesystem::directory_iterator(sat_dir_path)) {
      if (entry.path().extension() == ".cnf") {
        this->sat_files_.push_back(entry.path());
      }
    }

    for (const auto &entry : std::filesystem::directory_iterator(unsat_dir_path)) {
      if (entry.path().extension() == ".cnf") {
        this->unsat_files_.push_back(entry.path());
      }
    }
  }

  void TearDown() override { }

  std::vector<std::filesystem::path> sat_files_;
  std::vector<std::filesystem::path> unsat_files_;
};

static std::unique_ptr<CNF> cnf_from_file(const std::filesystem::path &path) {
  std::ifstream in(path);

  if (!in.is_open()) {
    return nullptr;
  }

  return CNF::load_cnf(in);
}

TEST_F(StressTest, SolveSatFilesTest) {
  for (auto &file : sat_files_) {
    if (auto cnf = cnf_from_file(file); cnf == nullptr) {
      std::cerr << "File not found: " << file << std::endl;
      continue;
    } else {
      EXPECT_TRUE(cnf->solve()) << "Failed test for file: " << file << std::endl;
    }
  }
}

TEST_F(StressTest, SolveUnsatFilesTest) {
  for (auto &file : unsat_files_) {
    if (auto cnf = cnf_from_file(file); cnf == nullptr) {
      std::cerr << "File not found: " << file << std::endl;
    } else {
      EXPECT_FALSE(cnf->solve()) << "Failed test for file: " << file << std::endl;
    }
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
