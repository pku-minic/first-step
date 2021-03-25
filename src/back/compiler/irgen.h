#ifndef FIRSTSTEP_BACK_COMPILER_IRGEN_H_
#define FIRSTSTEP_BACK_COMPILER_IRGEN_H_

#include <ostream>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <cstddef>

#include "define/ir.h"
#include "define/ast.h"

#include "xstl/guard.h"
#include "xstl/nested.h"

class IRGenerator {
 public:
  IRGenerator() : error_num_(0) {
    // register all of the library functions
    lib_funcs_.insert({"input", std::make_shared<FunctionDef>("input", 0)});
    lib_funcs_.insert({"print", std::make_shared<FunctionDef>("print", 1)});
  }

  // dump generated IRs
  void Dump(std::ostream &os) const;

  // visitor methods
  ValPtr GenerateOn(const FunDefAST &ast);
  ValPtr GenerateOn(const BlockAST &ast);
  ValPtr GenerateOn(const DefineAST &ast);
  ValPtr GenerateOn(const AssignAST &ast);
  ValPtr GenerateOn(const IfAST &ast);
  ValPtr GenerateOn(const ReturnAST &ast);
  ValPtr GenerateOn(const BinaryAST &ast);
  ValPtr GenerateOn(const UnaryAST &ast);
  ValPtr GenerateOn(const FunCallAST &ast);
  ValPtr GenerateOn(const IntAST &ast);
  ValPtr GenerateOn(const IdAST &ast);

  // count of error
  std::size_t error_num() const { return error_num_; }

 private:
  // print error message to stderr
  ValPtr LogError(std::string_view message);
  // enter a new environment
  xstl::Guard NewEnvironment();

  std::size_t error_num_;
  // current function
  FunDefPtr func_;
  // all defined functions
  std::unordered_map<std::string_view, FunDefPtr> funcs_;
  // all predefined library functions
  std::unordered_map<std::string_view, FunDefPtr> lib_funcs_;
  // all defined variables (virtual registers)
  xstl::NestedMapPtr<std::string_view, ValPtr> vregs_;
};

#endif  // FIRSTSTEP_BACK_COMPILER_IRGEN_H_
