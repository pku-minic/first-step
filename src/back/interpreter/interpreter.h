#ifndef FIRSTSTEP_BACK_INTERPRETER_INTERPRETER_H_
#define FIRSTSTEP_BACK_INTERPRETER_INTERPRETER_H_

#include <optional>
#include <string_view>
#include <unordered_map>
#include <cstddef>

#include "define/ast.h"

#include "xstl/nested.h"
#include "xstl/guard.h"

class Interpreter {
 public:
  Interpreter() : error_num_(0) {}

  // add the specific function definition to interpreter
  // returns false if failed
  bool AddFunctionDef(ASTPtr func);
  // evaluate the current program
  // returns return value of 'main' function, or 'nullopt' if failed
  std::optional<int> Eval();

  // visitor methods
  std::optional<int> EvalOn(const FunDefAST &ast);
  std::optional<int> EvalOn(const BlockAST &ast);
  std::optional<int> EvalOn(const DefineAST &ast);
  std::optional<int> EvalOn(const AssignAST &ast);
  std::optional<int> EvalOn(const IfAST &ast);
  std::optional<int> EvalOn(const ReturnAST &ast);
  std::optional<int> EvalOn(const BinaryAST &ast);
  std::optional<int> EvalOn(const UnaryAST &ast);
  std::optional<int> EvalOn(const FunCallAST &ast);
  std::optional<int> EvalOn(const IntAST &ast);
  std::optional<int> EvalOn(const IdAST &ast);

  // count of error
  std::size_t error_num() const { return error_num_; }

 private:
  // name of return value
  static constexpr const char *kRetVal = "$ret";

  // print error message to stderr
  std::optional<int> LogError(std::string_view message);
  // enter a new environment
  xstl::Guard NewEnvironment();
  // perform library function call
  std::optional<int> CallLibFunction(std::string_view name,
                                     const ASTPtrList &args);

  std::size_t error_num_;
  // read function name only, but not evaluate the function
  bool read_func_name_;
  // name of the current function
  std::string_view func_name_;
  // all function definitions
  std::unordered_map<std::string_view, ASTPtr> funcs_;
  // environments
  xstl::NestedMapPtr<std::string_view, std::optional<int>> envs_;
};

#endif  // FIRSTSTEP_BACK_INTERPRETER_INTERPRETER_H_
