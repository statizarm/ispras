#include <iostream>
#include <vector>
#include <fstream>
#include "cnf.h"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cerr << "Provide path to .cnf file" << std::endl;
    exit(-1);
  }

  const char* path = argv[1];

  std::ifstream in(path);

  if (!in.is_open()) {
    std::cerr << "File not found: " << path << std::endl;
    exit(-1);
  }

  auto cnf = CNF::load_cnf(in);

  if (cnf->solve()) {
    std::cout << "SAT" << std::endl;
  } else {
    std::cout << "UNSAT" << std::endl;
  }

  return 0;
}
