The application is a simple arithmetic expression parsing and evaluation with + , -, *  operations (no division), where arguments can be grouped by parentheses, e.g.:

* 6 + 5 - 2*3, answer is 5
* -5*(-2) + ((2+3)*8), answer is 50

Consists of:
1) Lexer converting a series of symbols to lexems or tokens, e.g. "6 + 5" -> "Number Plus Number". Ignores whitespaces.
2) LL(1) grammar recursive parser.
3) LR(1) grammar stack based parser.
