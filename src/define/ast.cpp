#include "define/ast.h"

#include "back/interpreter/interpreter.h"
#include "back/compiler/irgen.h"

std::optional<int> FunDefAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> BlockAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> DefineAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> AssignAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> IfAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> ReturnAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> BinaryAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> UnaryAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> FunCallAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> IntAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

std::optional<int> IdAST::Eval(Interpreter &intp) const {
  return intp.EvalOn(*this);
}

ValPtr FunDefAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr BlockAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr DefineAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr AssignAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr IfAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr ReturnAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr BinaryAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr UnaryAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr FunCallAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr IntAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

ValPtr IdAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}
