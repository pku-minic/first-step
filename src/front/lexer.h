#ifndef FIRSTSTEP_FRONT_LEXER_H_
#define FIRSTSTEP_FRONT_LEXER_H_

#include <istream>
#include <string>
#include <string_view>
#include <cstddef>

#include "define/token.h"

class Lexer {
 public:
  Lexer(std::istream &in) : in_(in) {
    in_ >> std::noskipws;
    last_char_ = ' ';
    error_num_ = 0;
  }

  // get next token from input stream
  Token NextToken();

  // count of error
  std::size_t error_num() const { return error_num_; }
  // identifiers
  const std::string &id_val() const { return id_val_; }
  // integer value
  int int_val() const { return int_val_; }
  // keywords
  Keyword key_val() const { return key_val_; }
  // operators
  Operator op_val() const { return op_val_; }
  // other characters
  char other_val() const { return other_val_; }

 private:
  // read next character
  void NextChar() { in_ >> last_char_; }

  // print error message to stderr
  Token LogError(std::string_view message);

  // handle tokens
  Token HandleId();
  Token HandleInteger();
  Token HandleOperator();
  Token HandleComment();
  Token HandleEOL();

  std::istream &in_;
  char last_char_;
  std::size_t error_num_;
  // value of token
  std::string id_val_;
  int int_val_;
  Keyword key_val_;
  Operator op_val_;
  char other_val_;
};

#endif  // FIRSTSTEP_FRONT_LEXER_H_
