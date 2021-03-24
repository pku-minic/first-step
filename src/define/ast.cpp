#include "define/ast.h"

#include "back/interpreter/interpreter.h"

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
