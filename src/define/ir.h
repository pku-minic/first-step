#ifndef FIRSTSTEP_DEFINE_IR_H_
#define FIRSTSTEP_DEFINE_IR_H_

#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <cstddef>

#include "define/token.h"

// base class of all IRs
class IRBase {
 public:
  virtual ~IRBase() = default;

  // TODO
};

// some type definitions
using IRPtr = std::shared_ptr<IRBase>;
using IRPtrList = std::vector<IRPtr>;

// function definition
class FunDefIR : public IRBase {
 public:
  FunDefIR(const std::string &name, std::size_t arg_num, IRPtrList insts)
      : name_(name), arg_num_(arg_num), insts_(std::move(insts)) {}

 private:
  std::string name_;
  std::size_t arg_num_;
  IRPtrList insts_;
};

// assignment
class AssignIR : public IRBase {
 public:
  AssignIR(IRPtr vreg, IRPtr val)
      : vreg_(std::move(vreg)), val_(std::move(val)) {}

 private:
  IRPtr vreg_, val_;
};

// conditional branch
class BranchIR : public IRBase {
 public:
  BranchIR(IRPtr cond, IRPtr target)
      : cond_(std::move(cond)), target_(std::move(target)) {}

 private:
  IRPtr cond_, target_;
};

// unconditional jump
class JumpIR : public IRBase {
 public:
  JumpIR(IRPtr target) : target_(std::move(target)) {}

 private:
  IRPtr target_;
};

// function call
class CallIR : public IRBase {
 public:
  CallIR(IRPtr func, IRPtrList args)
      : func_(std::move(func)), args_(std::move(args)) {}

 private:
  IRPtr func_;
  IRPtrList args_;
};

// function return
class ReturnIR : public IRBase {
 public:
  ReturnIR(IRPtr val) : val_(std::move(val)) {}

 private:
  IRPtr val_;
};

// binary operation
class BinaryIR : public IRBase {
 public:
  BinaryIR(Operator op, IRPtr dest, IRPtr lhs, IRPtr rhs)
      : op_(op), dest_(std::move(dest)),
        lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

 private:
  Operator op_;
  IRPtr dest_, lhs_, rhs_;
};

// unary operation
class UnaryIR : public IRBase {
 public:
  UnaryIR(Operator op, IRPtr dest, IRPtr opr)
      : op_(op), dest_(std::move(dest)), opr_(std::move(opr)) {}

 private:
  Operator op_;
  IRPtr dest_, opr_;
};

// virtual register
class VirtRegIR : public IRBase {
 public:
  VirtRegIR() : id_(next_id_++) {}

 private:
  static std::size_t next_id_;
  std::size_t id_;
};

// label
class LabelIR : public IRBase {
 public:
  LabelIR() : id_(next_id_++) {}

 private:
  static std::size_t next_id_;
  std::size_t id_;
};

// integer
class IntIR : public IRBase {
 public:
  IntIR(int val) : val_(val) {}

 private:
  int val_;
};

#endif  // FIRSTSTEP_DEFINE_IR_H_
