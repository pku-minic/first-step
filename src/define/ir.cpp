#include "define/ir.h"

#include <cassert>

// definition of static class members
std::size_t VirtRegVal::next_id_ = 0;
std::size_t LabelVal::next_id_ = 0;

ValPtr FunctionDef::AddVirtReg() {
  auto vreg = std::make_shared<VirtRegVal>();
  vregs_.push_back(vreg);
  return vreg;
}

void FunctionDef::Dump(std::ostream &os) const {
  os << "; function" << std::endl;
  os << name_ << ':' << std::endl;
  for (const auto &inst : insts_) {
    inst->Dump(os);
    os << std::endl;
  }
  os << std::endl;
}

void AssignInst::Dump(std::ostream &os) const {
  os << "  ";
  dest_->Dump(os);
  os << " = ";
  val_->Dump(os);
}

void BranchInst::Dump(std::ostream &os) const {
  os << "  " << (bnez_ ? "bnez" : "beqz") << ' ';
  cond_->Dump(os);
  os << ", ";
  target_->Dump(os);
}

void JumpInst::Dump(std::ostream &os) const {
  os << "  jump ";
  target_->Dump(os);
}

void LabelInst::Dump(std::ostream &os) const {
  label_->Dump(os);
  os << ':';
}

void CallInst::Dump(std::ostream &os) const {
  os << "  ";
  dest_->Dump(os);
  os << " = " << func_->name() << '(';
  for (std::size_t i = 0; i < args_.size(); ++i) {
    if (i) os << ", ";
    args_[i]->Dump(os);
  }
  os << ')';
}

void ReturnInst::Dump(std::ostream &os) const {
  os << "  return ";
  val_->Dump(os);
}

void BinaryInst::Dump(std::ostream &os) const {
  os << "  ";
  dest_->Dump(os);
  os << " = ";
  lhs_->Dump(os);
  os << ' ';
  switch (op_) {
    case Operator::Add: os << '+'; break;
    case Operator::Sub: os << '-'; break;
    case Operator::Mul: os << '*'; break;
    case Operator::Div: os << '/'; break;
    case Operator::Mod: os << '%'; break;
    case Operator::Less: os << '<'; break;
    case Operator::LessEq: os << "<="; break;
    case Operator::Eq: os << "=="; break;
    case Operator::NotEq: os << "!="; break;
    default: assert(false && "unknown binary operator");
  }
  os << ' ';
  rhs_->Dump(os);
}

void UnaryInst::Dump(std::ostream &os) const {
  os << "  ";
  dest_->Dump(os);
  os << " = ";
  switch (op_) {
    case Operator::Sub: os << '-'; break;
    case Operator::LNot: os << '!'; break;
    default: assert(false && "unknown unary operator");
  }
  opr_->Dump(os);
}

void VirtRegVal::Dump(std::ostream &os) const {
  os << "vreg" << id_;
}

void ArgRefVal::Dump(std::ostream &os) const {
  os << "arg" << id_;
}

void LabelVal::Dump(std::ostream &os) const {
  os << "label" << id_;
}

void IntVal::Dump(std::ostream &os) const {
  os << val_;
}
