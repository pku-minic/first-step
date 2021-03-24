#ifndef FIRSTSTEP_FRONT_PARSER_H_
#define FIRSTSTEP_FRONT_PARSER_H_

#include <string_view>
#include <functional>
#include <initializer_list>
#include <cstddef>

#include "front/lexer.h"
#include "define/ast.h"
#include "define/token.h"

class Parser {
 public:
  Parser(Lexer &lexer) : lexer_(lexer) {
    error_num_ = 0;
    NextToken();
  }

  // parse next symbol
  ASTPtr ParseNext() {
    return cur_token_ == Token::End ? nullptr : ParseFunDef();
  }

  // count of error
  std::size_t error_num() const { return error_num_; }

 private:
  Token NextToken() { return cur_token_ = lexer_.NextToken(); }
  // check if the current token is the specific character
  bool IsTokenChar(char c) const {
    return cur_token_ == Token::Other && lexer_.other_val() == c;
  }
  // check if the current token is the specific keyword
  bool IsTokenKey(Keyword key) const {
    return cur_token_ == Token::Keyword && lexer_.key_val() == key;
  }
  // check if the current token is the specific operator
  bool IsTokenOp(Operator op) const {
    return cur_token_ == Token::Operator && lexer_.op_val() == op;
  }

  // print error message to stderr
  ASTPtr LogError(std::string_view message);

  ASTPtr ParseFunDef();
  ASTPtr ParseBlock();
  ASTPtr ParseStatement();
  ASTPtr ParseDefineAssign();
  ASTPtr ParseIfElse();
  ASTPtr ParseReturn();
  ASTPtr ParseExpr();
  ASTPtr ParseLAndExpr();
  ASTPtr ParseEqExpr();
  ASTPtr ParseRelExpr();
  ASTPtr ParseAddExpr();
  ASTPtr ParseMulExpr();
  ASTPtr ParseUnaryExpr();
  ASTPtr ParseValue();
  ASTPtr ParseFunCall();

  // parse binary expression
  ASTPtr ParseBinary(std::function<ASTPtr()> parser,
                     std::initializer_list<Operator> ops);
  // make sure current token is identifier
  bool ExpectId();
  // make sure current token is specific character and goto next token
  bool ExpectChar(char c);

  Lexer &lexer_;
  std::size_t error_num_;
  Token cur_token_;
};

#endif  // FIRSTSTEP_FRONT_PARSER_H_
