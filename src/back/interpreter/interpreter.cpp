#include "back/interpreter/interpreter.h"

#include <iostream>
#include <cassert>

std::optional<int> Interpreter::LogError(std::string_view message) {
  std::cerr << "error(interpreter): " << message << std::endl;
  ++error_num_;
  return {};
}

xstl::Guard Interpreter::NewEnvironment() {
  envs_ = xstl::MakeNestedMap(envs_);
  return xstl::Guard([this] { envs_ = envs_->outer(); });
}

std::optional<int> Interpreter::CallLibFunction(std::string_view name,
                                                const ASTPtrList &args) {
  if (name == "input") {
    // check arguments
    if (!args.empty()) return LogError("argument count mismatch");
    // read an integer from stdin
    int ret;
    std::cin >> ret;
    return ret;
  }
  else if (name == "print") {
    // check arguments
    if (args.size() != 1) return LogError("argument count mismatch");
    // evaluate argument
    auto arg = args[0]->Eval(*this);
    if (!arg) return {};
    // print to stdout
    std::cout << *arg << std::endl;
    return 0;
  }
  else {
    // not a library function call
    return {};
  }
}

bool Interpreter::AddFunctionDef(ASTPtr func) {
  // get function name
  read_func_name_ = true;
  func->Eval(*this);
  assert(!func_name_.empty() && "not a function");
  read_func_name_ = false;
  // check redefinition
  if (funcs_.count(func_name_)) {
    LogError("function has already been defined");
    return false;
  }
  // add to function map
  funcs_.insert({func_name_, std::move(func)});
  return true;
}

std::optional<int> Interpreter::Eval() {
  // find the 'main' function
  auto it = funcs_.find("main");
  if (it == funcs_.end()) return LogError("'main' function not found");
  // initialize the root environment
  envs_ = xstl::MakeNestedMap<std::string_view, std::optional<int>>();
  // evaluate 'main' function
  return it->second->Eval(*this);
}

std::optional<int> Interpreter::EvalOn(const FunDefAST &ast) {
  if (read_func_name_) {
    // just read the function name
    func_name_ = ast.name();
    return {};
  }
  else {
    // set up return value
    auto succ = envs_->AddItem(kRetVal, {});
    assert(succ && "environment corrupted");
    static_cast<void>(succ);
    // evaluate function body
    ast.body()->Eval(*this);
    // get & check return value
    auto ret_val = envs_->GetItem(kRetVal, false);
    if (!ret_val) return LogError("function has no return value");
    return ret_val;
  }
}

std::optional<int> Interpreter::EvalOn(const BlockAST &ast) {
  // enter a new environment
  auto env = NewEnvironment();
  // evaluate all statements in block
  for (const auto &stmt : ast.stmts()) {
    stmt->Eval(*this);
    // stop if there is an error
    if (error_num_) return {};
  }
  return {};
}

std::optional<int> Interpreter::EvalOn(const DefineAST &ast) {
  // evaluate the expression
  auto expr = ast.expr()->Eval(*this);
  if (!expr) return {};
  // update the current environment
  if (!envs_->AddItem(ast.name(), expr)) {
    return LogError("symbol has already been defined");
  }
  return {};
}

std::optional<int> Interpreter::EvalOn(const AssignAST &ast) {
  // evaluate the expression
  auto expr = ast.expr()->Eval(*this);
  if (!expr) return {};
  // update value of the symbol
  auto envs = envs_;
  bool succ = false;
  while (envs) {
    // try to update the value
    if (envs->UpdateItem(ast.name(), expr, false)) {
      succ = true;
      break;
    }
    // do not cross the boundary of function
    if (envs->GetItem(kRetVal, false)) break;
    envs = envs->outer();
  }
  // check if success
  if (!succ) return LogError("symbol has not been defined");
  return {};
}

std::optional<int> Interpreter::EvalOn(const IfAST &ast) {
  // evaluate the condition
  auto cond = ast.cond()->Eval(*this);
  if (!cond) return {};
  if (*cond) {
    // evaluate the true branch
    ast.then()->Eval(*this);
  }
  else if (ast.else_then()) {
    // evaluate the false branch
    ast.else_then()->Eval(*this);
  }
  return {};
}

std::optional<int> Interpreter::EvalOn(const ReturnAST &ast) {
  // evaluate the return value
  auto expr = ast.expr()->Eval(*this);
  if (!expr) return {};
  // update the current return value
  auto succ = envs_->UpdateItem(kRetVal, expr);
  assert(succ && "environment corrupted");
  static_cast<void>(succ);
  return {};
}

std::optional<int> Interpreter::EvalOn(const BinaryAST &ast) {
  // check if is logical operator
  if (ast.op() == Operator::LAnd || ast.op() == Operator::LOr) {
    // evaluate lhs first
    auto lhs = ast.lhs()->Eval(*this);
    if (!lhs || (ast.op() == Operator::LAnd && !*lhs) ||
        (ast.op() == Operator::LOr && *lhs)) {
      return lhs;
    }
    // then evaluate rhs
    return ast.rhs()->Eval(*this);
  }
  else {
    // evaluate the lhs & rhs
    auto lhs = ast.lhs()->Eval(*this), rhs = ast.rhs()->Eval(*this);
    if (!lhs || !rhs) return {};
    // perform binary operation
    switch (ast.op()) {
      case Operator::Add: return *lhs + *rhs;
      case Operator::Sub: return *lhs - *rhs;
      case Operator::Mul: return *lhs * *rhs;
      case Operator::Div: return *lhs / *rhs;
      case Operator::Mod: return *lhs % *rhs;
      case Operator::Less: return *lhs < *rhs;
      case Operator::LessEq: return *lhs <= *rhs;
      case Operator::Eq: return *lhs == *rhs;
      case Operator::NotEq: return *lhs != *rhs;
      default: assert(false && "unknown binary operator");
    }
    return {};
  }
}

std::optional<int> Interpreter::EvalOn(const UnaryAST &ast) {
  // evaluate the operand
  auto opr = ast.opr()->Eval(*this);
  if (!opr) return {};
  // perform unary operation
  switch (ast.op()) {
    case Operator::Sub: return -*opr;
    case Operator::LNot: return !*opr;
    default: assert(false && "unknown unary operator");
  }
  return {};
}

std::optional<int> Interpreter::EvalOn(const FunCallAST &ast) {
  // handle library function call
  auto ret = CallLibFunction(ast.name(), ast.args());
  if (error_num_ || ret) return ret;
  // find the specific function
  auto it = funcs_.find(ast.name());
  if (it == funcs_.end()) return LogError("function not found");
  // make a new environment for arguments
  auto env = NewEnvironment();
  // evaluate arguments
  const auto &func_args = static_cast<FunDefAST *>(it->second.get())->args();
  if (ast.args().size() != func_args.size()) {
    return LogError("argument count mismatch");
  }
  for (std::size_t i = 0; i < func_args.size(); ++i) {
    // evaluate the current argument
    auto arg = ast.args()[i]->Eval(*this);
    if (!arg) return {};
    // add to environment
    if (!envs_->AddItem(func_args[i], arg)) {
      return LogError("redifinition of argument");
    }
  }
  // call the specific function
  return it->second->Eval(*this);
}

std::optional<int> Interpreter::EvalOn(const IntAST &ast) {
  return ast.val();
}

std::optional<int> Interpreter::EvalOn(const IdAST &ast) {
  // find in the current environment
  auto val = envs_->GetItem(ast.id());
  if (!val) return LogError("symbol has not been defined");
  return val;
}
