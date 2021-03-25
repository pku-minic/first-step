#ifndef FIRSTSTEP_DEFINE_IR_H_
#define FIRSTSTEP_DEFINE_IR_H_

#include <ostream>
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <cstddef>

#include "define/token.h"

// forwarded declaration of function definition
class FunctionDef;

// base class of all instructions
class InstBase {
 public:
  virtual ~InstBase() = default;

  // dump RISC-V assembly of the current instruction
  virtual void Dump(std::ostream &os, const FunctionDef &func) const = 0;
};

// type definitions about instructions
using InstPtr = std::shared_ptr<InstBase>;
using InstPtrList = std::vector<InstPtr>;

// base class of all values
class ValueBase {
 public:
  virtual ~ValueBase() = default;

  // dump RISC-V assembly of read operation
  virtual void DumpRead(std::ostream &os) const = 0;
  // dump RISC-V assembly of write operation
  virtual void DumpWrite(std::ostream &os) const = 0;
};

// type definitions about values
using ValPtr = std::shared_ptr<ValueBase>;
using ValPtrList = std::vector<ValPtr>;

// function definition
class FunctionDef {
 public:
  FunctionDef(const std::string &name, std::size_t arg_num)
      : name_(name), arg_num_(arg_num), slot_num_(0) {}

  // create & push instruction to current function
  template <typename Inst, typename... Args>
  void PushInst(Args &&...args) {
    insts_.emplace_back(
        std::make_shared<Inst>(std::forward<Args>(args)...));
  }

  // create a new stack slot definition
  ValPtr AddSlot();
  // dump RISC-V assembly of the current function
  void Dump(std::ostream &os) const;

  // getters
  const std::string &name() const { return name_; }
  std::size_t arg_num() const { return arg_num_; }
  std::size_t slot_offset() const {
    return ((arg_num_ + slot_num_) / 4 + 1) * 16;
  }

 private:
  std::string name_;
  std::size_t arg_num_, slot_num_;
  // empty if is library function ('input' and 'print')
  InstPtrList insts_;
};

// type definitions about function definitions
using FunDefPtr = std::shared_ptr<FunctionDef>;


// assignment
class AssignInst : public InstBase {
 public:
  AssignInst(ValPtr dest, ValPtr val)
      : dest_(std::move(dest)), val_(std::move(val)) {}

  void Dump(std::ostream &os, const FunctionDef &func) const override;

 private:
  ValPtr dest_, val_;
};

// conditional branch
class BranchInst : public InstBase {
 public:
  BranchInst(bool bnez, ValPtr cond, ValPtr label)
      : bnez_(bnez), cond_(std::move(cond)), label_(std::move(label)) {}

  void Dump(std::ostream &os, const FunctionDef &func) const override;

 private:
  // bnez (true) or beqz (false)
  bool bnez_;
  ValPtr cond_, label_;
};

// unconditional jump
class JumpInst : public InstBase {
 public:
  JumpInst(ValPtr label) : label_(std::move(label)) {}

  void Dump(std::ostream &os, const FunctionDef &func) const override;

 private:
  ValPtr label_;
};

// label definition
class LabelInst : public InstBase {
 public:
  LabelInst(ValPtr label) : label_(std::move(label)) {}

  void Dump(std::ostream &os, const FunctionDef &func) const override;

 private:
  ValPtr label_;
};

// function call
class CallInst : public InstBase {
 public:
  CallInst(ValPtr dest, FunDefPtr func, ValPtrList args)
      : dest_(std::move(dest)), func_(std::move(func)),
        args_(std::move(args)) {}

  void Dump(std::ostream &os, const FunctionDef &func) const override;

 private:
  ValPtr dest_;
  FunDefPtr func_;
  ValPtrList args_;
};

// function return
class ReturnInst : public InstBase {
 public:
  ReturnInst(ValPtr val) : val_(std::move(val)) {}

  void Dump(std::ostream &os, const FunctionDef &func) const override;

 private:
  ValPtr val_;
};

// binary operation
class BinaryInst : public InstBase {
 public:
  BinaryInst(Operator op, ValPtr dest, ValPtr lhs, ValPtr rhs)
      : op_(op), dest_(std::move(dest)),
        lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

  void Dump(std::ostream &os, const FunctionDef &func) const override;

 private:
  Operator op_;
  ValPtr dest_, lhs_, rhs_;
};

// unary operation
class UnaryInst : public InstBase {
 public:
  UnaryInst(Operator op, ValPtr dest, ValPtr opr)
      : op_(op), dest_(std::move(dest)), opr_(std::move(opr)) {}

  void Dump(std::ostream &os, const FunctionDef &func) const override;

 private:
  Operator op_;
  ValPtr dest_, opr_;
};


// stack slot
class SlotVal : public ValueBase {
 public:
  SlotVal(std::size_t id) : id_(id) {}

  void DumpRead(std::ostream &os) const override;
  void DumpWrite(std::ostream &os) const override;

 private:
  std::size_t id_;
};

// argument reference
class ArgRefVal : public ValueBase {
 public:
  ArgRefVal(std::size_t id) : id_(id) {}

  void DumpRead(std::ostream &os) const override;
  void DumpWrite(std::ostream &os) const override;

 private:
  std::size_t id_;
};

// label
class LabelVal : public ValueBase {
 public:
  LabelVal() : id_(next_id_++) {}

  void DumpRead(std::ostream &os) const override;
  void DumpWrite(std::ostream &os) const override;

 private:
  static std::size_t next_id_;
  std::size_t id_;
};

// integer
class IntVal : public ValueBase {
 public:
  IntVal(int val) : val_(val) {}

  void DumpRead(std::ostream &os) const override;
  void DumpWrite(std::ostream &os) const override;

 private:
  int val_;
};

#endif  // FIRSTSTEP_DEFINE_IR_H_
