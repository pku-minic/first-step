#ifndef FIRSTSTEP_DEFINE_IR_H_
#define FIRSTSTEP_DEFINE_IR_H_

#include <ostream>
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <cstddef>

#include "define/token.h"

// base class of all instructions
class InstBase {
 public:
  virtual ~InstBase() = default;

  virtual void Dump(std::ostream &os) const = 0;
};

// type definitions about instructions
using InstPtr = std::shared_ptr<InstBase>;
using InstPtrList = std::vector<InstPtr>;

// base class of all values
class ValueBase {
 public:
  virtual ~ValueBase() = default;

  virtual void Dump(std::ostream &os) const = 0;
};

// type definitions about values
using ValPtr = std::shared_ptr<ValueBase>;
using ValPtrList = std::vector<ValPtr>;

// function definition
class FunctionDef {
 public:
  FunctionDef(const std::string &name, std::size_t arg_num)
      : name_(name), arg_num_(arg_num) {}

  // create a new virtual register definition
  ValPtr AddVirtReg();
  // create & push instruction to current function
  template <typename Inst, typename... Args>
  void PushInst(Args &&...args) {
    insts_.emplace_back(
        std::make_shared<Inst>(std::forward<Args>(args)...));
  }

  // dump the content of function
  void Dump(std::ostream &os) const;

  // getters
  const std::string &name() const { return name_; }
  std::size_t arg_num() const { return arg_num_; }
  const ValPtrList &vregs() const { return vregs_; }
  // empty if is library function ('input' and 'print')
  const InstPtrList &insts() const { return insts_; }

 private:
  std::string name_;
  std::size_t arg_num_;
  ValPtrList vregs_;
  InstPtrList insts_;
};

// type definitions about function definitions
using FunDefPtr = std::shared_ptr<FunctionDef>;


// assignment
class AssignInst : public InstBase {
 public:
  AssignInst(ValPtr dest, ValPtr val)
      : dest_(std::move(dest)), val_(std::move(val)) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  const ValPtr &dest() const { return dest_; }
  const ValPtr &val() const { return val_; }

 private:
  ValPtr dest_, val_;
};

// conditional branch
class BranchInst : public InstBase {
 public:
  BranchInst(bool bnez, ValPtr cond, ValPtr target)
      : bnez_(bnez), cond_(std::move(cond)), target_(std::move(target)) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  // bnez (true) or beqz (false)
  bool bnez() const { return bnez_; }
  const ValPtr &cond() const { return cond_; }
  const ValPtr &target() const { return target_; }

 private:
  bool bnez_;
  ValPtr cond_, target_;
};

// unconditional jump
class JumpInst : public InstBase {
 public:
  JumpInst(ValPtr target) : target_(std::move(target)) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  const ValPtr &target() const { return target_; }

 private:
  ValPtr target_;
};

// label definition
class LabelInst : public InstBase {
 public:
  LabelInst(ValPtr label) : label_(std::move(label)) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  const ValPtr &label() const { return label_; }

 private:
  ValPtr label_;
};

// function call
class CallInst : public InstBase {
 public:
  CallInst(ValPtr dest, FunDefPtr func, ValPtrList args)
      : dest_(std::move(dest)), func_(std::move(func)),
        args_(std::move(args)) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  const ValPtr &dest() const { return dest_; }
  const FunDefPtr &func() const { return func_; }
  const ValPtrList &args() const { return args_; }

 private:
  ValPtr dest_;
  FunDefPtr func_;
  ValPtrList args_;
};

// function return
class ReturnInst : public InstBase {
 public:
  ReturnInst(ValPtr val) : val_(std::move(val)) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  const ValPtr &val() const { return val_; }

 private:
  ValPtr val_;
};

// binary operation
class BinaryInst : public InstBase {
 public:
  BinaryInst(Operator op, ValPtr dest, ValPtr lhs, ValPtr rhs)
      : op_(op), dest_(std::move(dest)),
        lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  Operator op() const { return op_; }
  const ValPtr &dest() const { return dest_; }
  const ValPtr &lhs() const { return lhs_; }
  const ValPtr &rhs() const { return rhs_; }

 private:
  Operator op_;
  ValPtr dest_, lhs_, rhs_;
};

// unary operation
class UnaryInst : public InstBase {
 public:
  UnaryInst(Operator op, ValPtr dest, ValPtr opr)
      : op_(op), dest_(std::move(dest)), opr_(std::move(opr)) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  Operator op() const { return op_; }
  const ValPtr &dest() const { return dest_; }
  const ValPtr &opr() const { return opr_; }

 private:
  Operator op_;
  ValPtr dest_, opr_;
};


// virtual register
class VirtRegVal : public ValueBase {
 public:
  VirtRegVal() : id_(next_id_++) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  std::size_t id() const { return id_; }

 private:
  static std::size_t next_id_;
  std::size_t id_;
};

// argument reference
class ArgRefVal : public ValueBase {
 public:
  ArgRefVal(std::size_t id) : id_(id) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  std::size_t id() const { return id_; }

 private:
  std::size_t id_;
};

// label
class LabelVal : public ValueBase {
 public:
  LabelVal() : id_(next_id_++) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  std::size_t id() const { return id_; }

 private:
  static std::size_t next_id_;
  std::size_t id_;
};

// integer
class IntVal : public ValueBase {
 public:
  IntVal(int val) : val_(val) {}

  void Dump(std::ostream &os) const override;
  
  // getters
  int val() const { return val_; }

 private:
  int val_;
};

#endif  // FIRSTSTEP_DEFINE_IR_H_
