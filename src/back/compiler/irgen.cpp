#include "back/compiler/irgen.h"

#include <iostream>

ValPtr IRGenerator::LogError(std::string_view message) {
  std::cerr << "error(irgen): " << message << std::endl;
  ++error_num_;
  return nullptr;
}

xstl::Guard IRGenerator::NewEnvironment() {
  vregs_ = xstl::MakeNestedMap(vregs_);
  return xstl::Guard([this] { vregs_ = vregs_->outer(); });
}

void IRGenerator::Dump(std::ostream &os) const {
  for (const auto &it : funcs_) it.second->Dump(os);
}

ValPtr IRGenerator::GenerateOn(const FunDefAST &ast) {
  // create function definition IR
  func_ = std::make_shared<FunctionDef>(ast.name(), ast.args().size());
  // add to function map
  if (!funcs_.insert({func_->name(), func_}).second) {
    return LogError("function has already been defined");
  }
  // enter argument environment
  auto env = NewEnvironment();
  // add definitions of arguments
  for (std::size_t i = 0; i < ast.args().size(); ++i) {
    vregs_->AddItem(ast.args()[i], std::make_shared<ArgRefVal>(i));
  }
  // generate body
  ast.body()->GenerateIR(*this);
  return nullptr;
}

ValPtr IRGenerator::GenerateOn(const BlockAST &ast) {
  // enter a new environment
  auto env = NewEnvironment();
  // generate on all statements
  for (const auto &stmt : ast.stmts()) {
    stmt->GenerateIR(*this);
    if (error_num_) return nullptr;
  }
  return nullptr;
}

ValPtr IRGenerator::GenerateOn(const DefineAST &ast) {
  // generate expression
  auto expr = ast.expr()->GenerateIR(*this);
  if (!expr) return nullptr;
  // add symbol definition
  auto vreg = func_->AddVirtReg();
  if (!vregs_->AddItem(ast.name(), vreg)) {
    return LogError("symbol has already been defined");
  }
  // generate assign instruction
  func_->PushInst<AssignInst>(std::move(vreg), std::move(expr));
  return nullptr;
}

ValPtr IRGenerator::GenerateOn(const AssignAST &ast) {
  // generate expression
  auto expr = ast.expr()->GenerateIR(*this);
  if (!expr) return nullptr;
  // get virtual register of the symbol
  auto vreg = vregs_->GetItem(ast.name());
  if (!vreg) return LogError("symbol has not been defined");
  // generate assign instruction
  func_->PushInst<AssignInst>(std::move(vreg), std::move(expr));
  return nullptr;
}

ValPtr IRGenerator::GenerateOn(const IfAST &ast) {
  // generate condition
  auto cond = ast.cond()->GenerateIR(*this);
  if (!cond) return nullptr;
  // create labels
  auto false_branch = std::make_shared<LabelVal>();
  auto end_if = ast.else_then() ? std::make_shared<LabelVal>() : nullptr;
  // generate contional branch
  func_->PushInst<BranchInst>(false, std::move(cond), false_branch);
  // generate the true branch
  ast.then()->GenerateIR(*this);
  if (ast.else_then()) func_->PushInst<JumpInst>(end_if);
  // generate the false branch
  func_->PushInst<LabelInst>(std::move(false_branch));
  if (ast.else_then()) {
    ast.else_then()->GenerateIR(*this);
    func_->PushInst<LabelInst>(std::move(end_if));
  }
  return nullptr;
}

ValPtr IRGenerator::GenerateOn(const ReturnAST &ast) {
  // generate return value
  auto expr = ast.expr()->GenerateIR(*this);
  if (!expr) return nullptr;
  // generate return instruction
  func_->PushInst<ReturnInst>(std::move(expr));
  return nullptr;
}

ValPtr IRGenerator::GenerateOn(const BinaryAST &ast) {
  // check if is logical operator
  if (ast.op() == Operator::LAnd || ast.op() == Operator::LOr) {
    // logical AND operation, generate labels
    auto end_logic = std::make_shared<LabelVal>();
    // generate lhs first
    auto lhs = ast.lhs()->GenerateIR(*this);
    if (!lhs) return nullptr;
    // generate conditional branch
    func_->PushInst<BranchInst>(ast.op() == Operator::LOr, lhs, end_logic);
    // generate rhs
    auto rhs = ast.rhs()->GenerateIR(*this);
    if (!rhs) return nullptr;
    func_->PushInst<AssignInst>(lhs, rhs);
    // generate label definition
    func_->PushInst<LabelInst>(std::move(end_logic));
    return lhs;
  }
  else {
    // generate lhs & rhs
    auto lhs = ast.lhs()->GenerateIR(*this);
    auto rhs = ast.rhs()->GenerateIR(*this);
    if (!lhs || !rhs) return nullptr;
    // generate binary operation
    auto dest = func_->AddVirtReg();
    func_->PushInst<BinaryInst>(ast.op(), dest, std::move(lhs),
                                std::move(rhs));
    return dest;
  }
}

ValPtr IRGenerator::GenerateOn(const UnaryAST &ast) {
  // generate operand
  auto opr = ast.opr()->GenerateIR(*this);
  if (!opr) return nullptr;
  // generate unary operation
  auto dest = func_->AddVirtReg();
  func_->PushInst<UnaryInst>(ast.op(), dest, std::move(opr));
  return dest;
}

ValPtr IRGenerator::GenerateOn(const FunCallAST &ast) {
  // get the function definition
  auto it = funcs_.find(ast.name());
  if (it == funcs_.end()) {
    it = lib_funcs_.find(ast.name());
    if (it == lib_funcs_.end()) return LogError("function not found");
  }
  // generate arguments
  if (ast.args().size() != it->second->arg_num()) {
    return LogError("argument count mismatch");
  }
  ValPtrList args;
  for (const auto &i : ast.args()) {
    auto arg = i->GenerateIR(*this);
    if (!arg) return nullptr;
    args.push_back(std::move(arg));
  }
  // generate function call
  auto dest = func_->AddVirtReg();
  func_->PushInst<CallInst>(dest, it->second, std::move(args));
  return dest;
}

ValPtr IRGenerator::GenerateOn(const IntAST &ast) {
  return std::make_shared<IntVal>(ast.val());
}

ValPtr IRGenerator::GenerateOn(const IdAST &ast) {
  // get virtual register of the symbol
  auto vreg = vregs_->GetItem(ast.id());
  if (!vreg) return LogError("symbol has not been defined");
  return vreg;
}
