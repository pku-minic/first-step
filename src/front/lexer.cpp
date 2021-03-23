#include "front/lexer.h"

#include <iostream>
#include <utility>
#include <cctype>
#include <cstdlib>

namespace {

const char *kKeywords[] = {FIRSTSTEP_KEYWORDS(FIRSTSTEP_EXPAND_SECOND)};
const char *kOperators[] = {FIRSTSTEP_OPERATORS(FIRSTSTEP_EXPAND_SECOND)};

// get index of a string in string array
template <typename T, std::size_t N>
int GetIndex(std::string_view str, T (&str_array)[N]) {
  for (std::size_t i = 0; i < N; ++i) {
    if (str == str_array[i]) return i;
  }
  return -1;
}

bool IsOperatorChar(char c) {
  const char op_chars[] = "+-*/%<=!&|";
  for (const auto &i : op_chars) {
    if (i == c) return true;
  }
  return false;
}

}  // namespace

Token Lexer::LogError(std::string_view message) {
  std::cerr << "error(lexer): " << message << std::endl;
  ++error_num_;
  return Token::Error;
}

Token Lexer::HandleId() {
  // read string
  std::string id;
  do {
    id += last_char_;
    NextChar();
  } while (!IsEOL() && (std::isalnum(last_char_) || last_char_ == '_'));
  // check if string is keyword
  int index = GetIndex(id, kKeywords);
  if (index < 0) {
    id_val_ = std::move(id);
    return Token::Id;
  }
  else {
    key_val_ = static_cast<Keyword>(index);
    return Token::Keyword;
  }
}

Token Lexer::HandleInteger() {
  // read string
  std::string num;
  do {
    num += last_char_;
    NextChar();
  } while (!IsEOL() && std::isdigit(last_char_));
  // try to convert to integer
  char *end_pos;
  int_val_ = std::strtol(num.c_str(), &end_pos, 10);
  return *end_pos || (num[0] == '0' && num.size() > 1)
             ? LogError("invalid number") : Token::Integer;
}

Token Lexer::HandleOperator() {
  // read string
  std::string op;
  do {
    op += last_char_;
    NextChar();
  } while (!IsEOL() && IsOperatorChar(last_char_));
  // check if operator is valid
  int index = GetIndex(op, kOperators);
  if (index < 0) return LogError("invalid operator");
  op_val_ = static_cast<Operator>(index);
  return Token::Operator;
}

Token Lexer::HandleComment() {
  // eat '#'
  NextChar();
  while (!IsEOL()) NextChar();
  return NextToken();
}

Token Lexer::HandleEOL() {
  do {
    NextChar();
  } while (IsEOL() && !in_.eof());
  return NextToken();
}

Token Lexer::NextToken() {
  // end of file
  if (in_.eof()) return Token::End;
  // skip spaces
  while (!IsEOL() && std::isspace(last_char_)) NextChar();
  // skip comments
  if (last_char_ == '#') return HandleComment();
  // id or keyword
  if (std::isalpha(last_char_) || last_char_ == '_') return HandleId();
  // number
  if (std::isdigit(last_char_)) return HandleInteger();
  // operator or id
  if (IsOperatorChar(last_char_)) return HandleOperator();
  // end of line
  if (IsEOL()) return HandleEOL();
  // other characters
  other_val_ = last_char_;
  NextChar();
  return Token::Other;
}
