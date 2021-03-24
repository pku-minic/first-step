#include <fstream>

#include "front/lexer.h"
#include "front/parser.h"
#include "back/interpreter/interpreter.h"

using namespace std;

int main(int argc, const char *argv[]) {
  // read file from command line argument
  if (argc < 2) return 1;
  ifstream ifs(argv[1]);
  // create lexer, parser and interpreter
  Lexer lexer(ifs);
  Parser parser(lexer);
  Interpreter intp;
  // parse the input file
  while (auto ast = parser.ParseNext()) {
    if (!intp.AddFunctionDef(std::move(ast))) break;
  }
  // quit if there is any error
  auto err_num = lexer.error_num() + parser.error_num() + intp.error_num();
  if (err_num) return err_num;
  // evaluate the program
  auto ret = intp.Eval();
  if (!ret) return intp.error_num();
  return *ret;
}
