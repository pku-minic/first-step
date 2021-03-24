#ifndef FIRSTSTEP_DEFINE_AST_H_
#define FIRSTSTEP_DEFINE_AST_H_

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "define/token.h"

// base class of all ASTs
class BaseAST {
 public:
  virtual ~BaseAST() = default;

  // TODO
};

// some type definitions
using ASTPtr = std::unique_ptr<BaseAST>;
using ASTPtrList = std::vector<ASTPtr>;
using IdList = std::vector<std::string>;

// function definition
class FunDefAST : public BaseAST {
 public:
  FunDefAST(const std::string &name, IdList args, ASTPtr body)
      : name_(name), args_(std::move(args)), body_(std::move(body)) {}

  // getters
  const std::string &name() const { return name_; }
  const IdList &args() const { return args_; }
  const ASTPtr &body() const { return body_; }

 private:
  std::string name_;
  IdList args_;
  ASTPtr body_;
};

// statement block
class BlockAST : public BaseAST {
 public:
  BlockAST(ASTPtrList stmts) : stmts_(std::move(stmts)) {}

  // getters
  const ASTPtrList &stmts() const { return stmts_; }

 private:
  ASTPtrList stmts_;
};

// assign statement
class AssignAST : public BaseAST {
 public:
  AssignAST(const std::string &name, ASTPtr expr)
      : name_(name), expr_(std::move(expr)) {}

  // getters
  const std::string &name() const { return name_; }
  const ASTPtr &expr() const { return expr_; }

 private:
  std::string name_;
  ASTPtr expr_;
};

// if-else statement
class IfAST : public BaseAST {
 public:
  IfAST(ASTPtr cond, ASTPtr then, ASTPtr else_then)
      : cond_(std::move(cond)), then_(std::move(then)),
        else_then_(std::move(else_then)) {}

  // getters
  const ASTPtr &cond() const { return cond_; }
  const ASTPtr &then() const { return then_; }
  const ASTPtr &else_then() const { return else_then_; }

 private:
  ASTPtr cond_, then_, else_then_;
};

// return statement
class ReturnAST : public BaseAST {
 public:
  ReturnAST(ASTPtr expr) : expr_(std::move(expr)) {}

  // getters
  const ASTPtr &expr() const { return expr_; }

 private:
  ASTPtr expr_;
};

// binary expression
class BinaryAST : public BaseAST {
 public:
  BinaryAST(Operator op, ASTPtr lhs, ASTPtr rhs)
      : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

  // getters
  Operator op() const { return op_; }
  const ASTPtr &lhs() const { return lhs_; }
  const ASTPtr &rhs() const { return rhs_; }

 private:
  Operator op_;
  ASTPtr lhs_, rhs_;
};

// unary expression
class UnaryAST : public BaseAST {
 public:
  UnaryAST(Operator op, ASTPtr opr) : op_(op), opr_(std::move(opr)) {}

  // getters
  Operator op() const { return op_; }
  const ASTPtr &opr() const { return opr_; }

 private:
  Operator op_;
  ASTPtr opr_;
};

// function call
class FunCallAST : public BaseAST {
 public:
  FunCallAST(const std::string &name, ASTPtrList args)
      : name_(name), args_(std::move(args)) {}

  // getters
  const std::string &name() const { return name_; }
  const ASTPtrList &args() const { return args_; }

 private:
  std::string name_;
  ASTPtrList args_;
};

// integer literal
class IntAST : public BaseAST {
 public:
  IntAST(int val) : val_(val) {}

  // getters
  int val() const { return val_; }

 private:
  int val_;
};

// identifier
class IdAST : public BaseAST {
 public:
  IdAST(const std::string &id) : id_(id) {}

  // getters
  const std::string &id() const { return id_; }

 private:
  std::string id_;
};

#endif  // FIRSTSTEP_DEFINE_AST_H_
