#ifndef FIRSTSTEP_DEFINE_TOKEN_H_
#define FIRSTSTEP_DEFINE_TOKEN_H_

// all supported keywords
#define FIRSTSTEP_KEYWORDS(e) \
  e(If, "if") e(Else, "else") e(Return, "return")

// all supported operators
#define FIRSTSTEP_OPERATORS(e) \
  e(Add, "+") e(Sub, "-") e(Mul, "*") e(Div, "/") e(Mod, "%") \
  e(Less, "<") e(LessEq, "<=") e(Eq, "==") e(NotEq, "!=") \
  e(LAnd, "&&") e(LOr, "||") e(LNot, "!") e(Define, ":=") e(Assign, "=")

// expand first element to comma-separated list
#define FIRSTSTEP_EXPAND_FIRST(i, ...)      i,
// expand second element to comma-separated list
#define FIRSTSTEP_EXPAND_SECOND(i, j, ...)  j,

enum class Token {
  Error, End,
  Id, Integer, Keyword, Operator, Other,
};

enum class Keyword { FIRSTSTEP_KEYWORDS(FIRSTSTEP_EXPAND_FIRST) };
enum class Operator { FIRSTSTEP_OPERATORS(FIRSTSTEP_EXPAND_FIRST) };

#endif  // FIRSTSTEP_DEFINE_TOKEN_H_
