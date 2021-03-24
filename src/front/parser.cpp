#include "front/parser.h"

#include <iostream>
#include <algorithm>

ASTPtr Parser::LogError(std::string_view message) {
  std::cerr << "error(parser): " << message << std::endl;
  ++error_num_;
  return nullptr;
}

ASTPtr Parser::ParseFunDef() {
  // get function name
  if (!ExpectId()) return nullptr;
  auto name = lexer_.id_val();
  NextToken();
  // check & eat '('
  if (!ExpectChar('(')) return nullptr;
  // get formal arguments
  IdList args;
  if (!IsTokenChar(')')) {
    for (;;) {
      // get name of the current argument
      if (!ExpectId()) return nullptr;
      args.push_back(lexer_.id_val());
      NextToken();
      // eat ','
      if (!IsTokenChar(',')) break;
      NextToken();
    }
  }
  // check & eat ')'
  if (!ExpectChar(')')) return nullptr;
  // get function body
  auto body = ParseBlock();
  if (!body) return nullptr;
  return std::make_unique<FunDefAST>(name, std::move(args),
                                     std::move(body));
}

ASTPtr Parser::ParseBlock() {
  // check & eat '{'
  if (!ExpectChar('{')) return nullptr;
  // get statements
  ASTPtrList stmts;
  while (!IsTokenChar('}')) {
    auto stmt = ParseStatement();
    if (!stmt) return nullptr;
    stmts.push_back(std::move(stmt));
  }
  // eat '}'
  NextToken();
  return std::make_unique<BlockAST>(std::move(stmts));
}

ASTPtr Parser::ParseStatement() {
  switch (cur_token_) {
    case Token::Id: return ParseDefineAssign();
    case Token::Keyword: {
      if (lexer_.key_val() == Keyword::If) {
        return ParseIfElse();
      }
      else if (lexer_.key_val() == Keyword::Return) {
        return ParseReturn();
      }
      // fallthrough
    }
  }
  return LogError("invalid statement");
}

ASTPtr Parser::ParseDefineAssign() {
  // get name of variable
  auto name = lexer_.id_val();
  NextToken();
  // check if is define/assign
  if (!IsTokenOp(Operator::Define) && !IsTokenOp(Operator::Assign)) {
    return LogError("expected ':=' or '='");
  }
  bool is_define = lexer_.op_val() == Operator::Define;
  NextToken();
  // get expression
  auto expr = ParseExpr();
  if (!expr) return nullptr;
  if (is_define) return std::make_unique<DefineAST>(name, std::move(expr));
  return std::make_unique<AssignAST>(name, std::move(expr));
}

ASTPtr Parser::ParseIfElse() {
  // eat 'if'
  NextToken();
  // get condition
  auto cond = ParseExpr();
  if (!cond) return nullptr;
  // get 'then' body
  auto then = ParseBlock();
  if (!then) return nullptr;
  // get 'else-then' body
  ASTPtr else_then;
  if (IsTokenKey(Keyword::Else)) {
    // eat 'else'
    NextToken();
    else_then = IsTokenKey(Keyword::If) ? ParseIfElse() : ParseBlock();
    if (!else_then) return nullptr;
  }
  return std::make_unique<IfAST>(std::move(cond), std::move(then),
                                 std::move(else_then));
}

ASTPtr Parser::ParseReturn() {
  // eat 'return'
  NextToken();
  // get return value
  auto expr = ParseExpr();
  if (!expr) return nullptr;
  return std::make_unique<ReturnAST>(std::move(expr));
}

ASTPtr Parser::ParseExpr() {
  return ParseBinary([this] { return ParseLAndExpr(); }, {Operator::LOr});
}

ASTPtr Parser::ParseLAndExpr() {
  return ParseBinary([this] { return ParseEqExpr(); }, {Operator::LAnd});
}

ASTPtr Parser::ParseEqExpr() {
  return ParseBinary([this] { return ParseRelExpr(); },
                     {Operator::Eq, Operator::NotEq});
}

ASTPtr Parser::ParseRelExpr() {
  return ParseBinary([this] { return ParseAddExpr(); },
                     {Operator::Less, Operator::LessEq});
}

ASTPtr Parser::ParseAddExpr() {
  return ParseBinary([this] { return ParseMulExpr(); },
                     {Operator::Add, Operator::Sub});
}

ASTPtr Parser::ParseMulExpr() {
  return ParseBinary([this] { return ParseUnaryExpr(); },
                     {Operator::Mul, Operator::Div, Operator::Mod});
}

ASTPtr Parser::ParseUnaryExpr() {
  if (cur_token_ == Token::Operator) {
    // get operator
    auto op = lexer_.op_val();
    NextToken();
    // check if is a valid operator
    switch (op) {
      case Operator::Sub: case Operator::LNot: break;
      default: return LogError("invalid unary operator");
    }
    // get operand
    auto opr = ParseValue();
    if (!opr) return nullptr;
    return std::make_unique<UnaryAST>(op, std::move(opr));
  }
  else {
    return ParseValue();
  }
}

ASTPtr Parser::ParseValue() {
  switch (cur_token_) {
    case Token::Integer: {
      // get integer literal
      auto val = lexer_.int_val();
      NextToken();
      return std::make_unique<IntAST>(val);
    }
    case Token::Id: {
      // get identifier
      auto id = lexer_.id_val();
      NextToken();
      // check if the current token is '('
      return IsTokenChar('(') ? ParseFunCall()
                              : std::make_unique<IdAST>(id);
    }
    case Token::Other: {
      if (lexer_.other_val() == '(') {
        // eat '('
        NextToken();
        // get expression
        auto expr = ParseExpr();
        if (!expr) return nullptr;
        // check & eat ')'
        ExpectChar(')');
        return expr;
      }
      // fallthrough
    }
  }
  return LogError("invalid value");
}

ASTPtr Parser::ParseFunCall() {
  // get function name
  auto name = lexer_.id_val();
  // eat '('
  NextToken();
  // get arguments
  ASTPtrList args;
  if (!IsTokenChar(')')) {
    for (;;) {
      // get the current argument
      auto expr = ParseExpr();
      if (!expr) return nullptr;
      args.push_back(std::move(expr));
      NextToken();
      // eat ','
      if (!IsTokenChar(',')) break;
      NextToken();
    }
  }
  // check & eat ')'
  if (!ExpectChar(')')) return nullptr;
  return std::make_unique<FunCallAST>(name, std::move(args));
}

ASTPtr Parser::ParseBinary(std::function<ASTPtr()> parser,
                           std::initializer_list<Operator> ops) {
  // get left-hand side expression
  auto lhs = parser();
  if (!lhs) return nullptr;
  // get the rest things
  while (cur_token_ == Token::Operator &&
         std::find(ops.begin(), ops.end(), lexer_.op_val()) != ops.end()) {
    // get operator
    auto op = lexer_.op_val();
    NextToken();
    // get right-hand side expression
    auto rhs = parser();
    if (!rhs) return nullptr;
    // update lhs
    lhs = std::make_unique<BinaryAST>(op, std::move(lhs), std::move(rhs));
  }
  return lhs;
}

bool Parser::ExpectId() {
  if (cur_token_ != Token::Id) {
    LogError("expected identifier");
    return false;
  }
  return true;
}

bool Parser::ExpectChar(char c) {
  if (!IsTokenChar(c)) {
    std::string msg = "expected '";
    msg = msg + c + "'";
    LogError(msg);
    return false;
  }
  NextToken();
  return true;
}
