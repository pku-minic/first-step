#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include "front/lexer.h"
#include "front/parser.h"
#include "back/interpreter/interpreter.h"
#include "back/compiler/irgen.h"

using namespace std;

void Interpret(istream &in) {
  // create lexer, parser and interpreter
  Lexer lexer(in);
  Parser parser(lexer);
  Interpreter intp;
  // parse the input file
  while (auto ast = parser.ParseNext()) {
    if (!intp.AddFunctionDef(move(ast))) break;
  }
  // quit if there is any error
  auto err_num = lexer.error_num() + parser.error_num() + intp.error_num();
  if (err_num) exit(err_num);
  // evaluate the program
  auto ret = intp.Eval();
  if (!ret) exit(intp.error_num());
  exit(*ret);
}

void Compile(istream &in) {
  // create lexer, parser and IR generator
  Lexer lexer(in);
  Parser parser(lexer);
  IRGenerator gen;
  // parse the input file
  while (auto ast = parser.ParseNext()) {
    ast->GenerateIR(gen);
    if (gen.error_num()) break;
  }
  // quit if there is any error
  auto err_num = lexer.error_num() + parser.error_num() + gen.error_num();
  if (err_num) exit(err_num);
  // dump generated IRs
  gen.Dump(cout);
}

int main(int argc, const char *argv[]) {
  // read file from command line argument
  if (argc < 2) return 1;
  ifstream ifs(argv[1]);
  // check if need to compile the input file
  if (argc >= 3 && !strcmp(argv[2], "-c")) {
    Compile(ifs);
  }
  else {
    Interpret(ifs);
  }
  return 0;
}
