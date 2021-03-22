# first-step

Take your first step in writing a compiler.

## EBNF of first-step

```ebnf
Program     ::= {FunctionDef};
FunctionDef ::= IDENT "(" [ArgsDef] ")" Block;
ArgsDef     ::= IDENT {"," IDENT};

Block       ::= "{" {Statement} "}";
Statement   ::= IDENT "=" Expression
              | "if" Expression Block ["else" Block]
              | "return" Expression;

Expression  ::= LOrExpr;
LOrExpr     ::= LAndExpr {"||" LAndExpr};
LAndExpr    ::= EqExpr {"&&" EqExpr};
EqExpr      ::= RelExpr {("==" | "!=") RelExpr};
RelExpr     ::= AddExpr {("<" | "<=") AddExpr};
AddExpr     ::= MulExpr {("+" | "-") MulExpr};
MulExpr     ::= UnaryExpr {("*" | "/" | "%") UnaryExpr};
UnaryExpr   ::= ["-" | "!"] Value;
Value       ::= INTEGER
              | IDENT
              | IDENT "(" [Args] ")"
              | "(" Expression ")";
Args        ::= Expression {"," Expression};
```

## License

Copyright (C) 2010-2021 MaxXing. License GPLv3.
