#include "define/ir.h"

// definition of static class members
std::size_t VirtRegVal::next_id_ = 0;
std::size_t LabelVal::next_id_ = 0;

ValPtr FunctionDef::AddVirtReg() {
  auto vreg = std::make_shared<VirtRegVal>();
  vregs_.push_back(vreg);
  return vreg;
}
