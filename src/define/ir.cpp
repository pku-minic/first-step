#include "define/ir.h"

#include <cassert>

namespace {

// register for storing the intermediate results
constexpr const char *kResultReg = "t0";
// register for storing the temporary data
constexpr const char *kTempReg = "t1";

}  // namespace

// definition of static class members
std::size_t LabelVal::next_id_ = 0;

ValPtr FunctionDef::AddSlot() {
  return std::make_shared<SlotVal>(slot_num_++);
}

void FunctionDef::Dump(std::ostream &os) const {
  // dump header
  os << "  .text" << std::endl;
  os << "  .globl " << name_ << std::endl;
  os << name_ << ':' << std::endl;
  // dump prologue
  os << "  addi sp, sp, -" << slot_offset() << std::endl;
  os << "  sw ra, " << (slot_offset() - 4) << "(sp)" << std::endl;
  for (std::size_t i = 0; i < arg_num_; ++i) {
    os << "  sw s" << i << ", " << (slot_offset() - 4 * (i + 2)) << "(sp)"
       << std::endl;
    os << "  mv s" << i << ", a" << i << std::endl;
  }
  // dump instructions
  for (const auto &inst : insts_) inst->Dump(os, *this);
  os << std::endl;
}

void AssignInst::Dump(std::ostream &os, const FunctionDef &func) const {
  val_->DumpRead(os);
  dest_->DumpWrite(os);
}

void BranchInst::Dump(std::ostream &os, const FunctionDef &func) const {
  cond_->DumpRead(os);
  os << "  " << (bnez_ ? "bnez" : "beqz") << ' ' << kResultReg << ", ";
  label_->DumpRead(os);
  os << std::endl;
}

void JumpInst::Dump(std::ostream &os, const FunctionDef &func) const {
  os << "  j ";
  label_->DumpRead(os);
  os << std::endl;
}

void LabelInst::Dump(std::ostream &os, const FunctionDef &func) const {
  label_->DumpRead(os);
  os << ':' << std::endl;
}

void CallInst::Dump(std::ostream &os, const FunctionDef &func) const {
  // generate arguments
  for (std::size_t i = 0; i < args_.size(); ++i) {
    args_[i]->DumpRead(os);
    os << "  mv a" << i << ", " << kResultReg << std::endl;
  }
  // generate function call
  os << "  call " << func_->name() << std::endl;
  os << "  mv " << kResultReg << ", a0" << std::endl;
  dest_->DumpWrite(os);
}

void ReturnInst::Dump(std::ostream &os, const FunctionDef &func) const {
  // dump return value
  val_->DumpRead(os);
  os << "  mv a0, " << kResultReg << std::endl;
  // dump epilogue
  for (std::size_t i = 0; i < func.arg_num(); ++i) {
    os << "  lw s" << i << ", " << (func.slot_offset() - 4 * (i + 2))
       << "(sp)" << std::endl;
  }
  os << "  lw ra, " << (func.slot_offset() - 4) << "(sp)" << std::endl;
  os << "  addi sp, sp, " << func.slot_offset() << std::endl;
  os << "  ret" << std::endl;
}

void BinaryInst::Dump(std::ostream &os, const FunctionDef &func) const {
  // dump lhs & rhs
  lhs_->DumpRead(os);
  os << "  mv " << kTempReg << ", " << kResultReg << std::endl;
  rhs_->DumpRead(os);
  // perform binary operation
  if (op_ == Operator::LessEq) {
    os << "  sgt " << kResultReg << ", " << kTempReg << ", " << kResultReg
       << std::endl;
    os << "  seqz " << kResultReg << ", " << kResultReg << std::endl;
  }
  else if (op_ == Operator::Eq || op_ == Operator::NotEq) {
    os << "  xor " << kResultReg << ", " << kTempReg << ", " << kResultReg
       << std::endl;
    os << "  " << (op_ == Operator::Eq ? "seqz" : "snez") << ' '
       << kResultReg << ", " << kResultReg << std::endl;
  }
  else {
    os << "  ";
    switch (op_) {
      case Operator::Add: os << "add"; break;
      case Operator::Sub: os << "sub"; break;
      case Operator::Mul: os << "mul"; break;
      case Operator::Div: os << "div"; break;
      case Operator::Mod: os << "rem"; break;
      case Operator::Less: os << "slt"; break;
      default: assert(false && "unknown binary operator");
    }
    os << ' ' << kResultReg << ", " << kTempReg << ", " << kResultReg
       << std::endl;
  }
  // store the result to dest
  dest_->DumpWrite(os);
}

void UnaryInst::Dump(std::ostream &os, const FunctionDef &func) const {
  // generate operand
  opr_->DumpRead(os);
  // perform unary operation
  os << "  ";
  switch (op_) {
    case Operator::Sub: os << "neg"; break;
    case Operator::LNot: os << "seqz"; break;
    default: assert(false && "unknown unary operator");
  }
  os << ' ' << kResultReg << ", " << kResultReg << std::endl;
  // store the result to dest
  dest_->DumpWrite(os);
}

void SlotVal::DumpRead(std::ostream &os) const {
  os << "  lw " << kResultReg << ", " << (id_ * 4) << "(sp)" << std::endl;
}

void SlotVal::DumpWrite(std::ostream &os) const {
  os << "  sw " << kResultReg << ", " << (id_ * 4) << "(sp)" << std::endl;
}

void ArgRefVal::DumpRead(std::ostream &os) const {
  os << "  mv " << kResultReg << ", s" << id_ << std::endl;
}

void ArgRefVal::DumpWrite(std::ostream &os) const {
  os << "  mv s" << id_ << ", " << kResultReg << std::endl;
}

void LabelVal::DumpRead(std::ostream &os) const {
  os << ".label" << id_;
}

void LabelVal::DumpWrite(std::ostream &os) const {
  assert(false && "writing a label");
}

void IntVal::DumpRead(std::ostream &os) const {
  os << "  li " << kResultReg << ", " << val_ << std::endl;
}

void IntVal::DumpWrite(std::ostream &os) const {
  assert(false && "writing an integer");
}
