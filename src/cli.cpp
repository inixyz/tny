#include <cstdlib>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "tnyvec.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Too few arguments" << std::endl;
    exit(EXIT_FAILURE);
  }

  tnyvec::Env env;

  for (int i = 1; i < argc; i++) {
    std::ifstream source_file(argv[i]);
    std::stringstream source_buffer;
    source_buffer << source_file.rdbuf();

    std::vector<std::string> toks = tnyvec::lex(source_buffer.str());
    tnyvec::exec(tnyvec::parse(toks), env);
  }

  return EXIT_SUCCESS;
}