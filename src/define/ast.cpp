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

IRPtr FunDefAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr BlockAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr DefineAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr AssignAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr IfAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr ReturnAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr BinaryAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr UnaryAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr FunCallAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr IntAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}

IRPtr IdAST::GenerateIR(IRGenerator &gen) const {
  return gen.GenerateOn(*this);
}
