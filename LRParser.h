#pragma once

#include "Lexer.h"
#include <string>
#include <vector>
#include <memory>

/*
  LR(1) math expression parser with in-place expression evaluator.
  One lookahead to solve shift/reduce conflicts: 
      exprMulDiv _ * arg2,
      exprMulDiv + arg2 _ * arg2
   Grammar rules:
   expr = exprMulDiv | exprMulDiv +/- expr
   exprMulDiv = arg1 | exprMulDiv * arg2
   arg1 = (+/-)? arg2
   arg2 = number | '(' expr ')
*/
class LRParser
{
public:
   NumberType ParseAndCalculate(const std::string& e);

   bool IsEmpty() const
   {
      return m_parseStack.empty() && m_lexer && m_lexer->IsEmpty();
   }

private:

   enum class NonTerminalType
   {
      PlusMinusExpr,
      MulDivExpr,
      Arg1,
      Arg2,
   };

   using OptTerminal = std::optional<Token>;

   struct NonTerminal
   {
      NonTerminalType m_type;
      NumberType m_number;
   };

   using OptNonTerminal = std::optional<NonTerminal>;

   struct Item
   {
      OptNonTerminal m_nonTerminal;
      OptTerminal    m_terminal;
   };

   void shift();
   bool tryReduce();

   bool reduceToArg1();
   bool reduceToArg2();
   bool reduceToMulDivExpr();
   bool reduceToPlusMinExpr();

   void pop(size_t itemNumber);
   void push(NonTerminal&& nonTerminal);
   void push(Token&& terminal);

   OptTerminal getToken(size_t offset = 0) const
   {
      if (m_parseStack.size() <= offset)
         return Token{ TokenType::End, std::string{} };
      return m_parseStack[m_parseStack.size() - offset - 1].m_terminal;
   }

   OptNonTerminal getNonTerminal(size_t offset = 0) const
   {
      if (m_parseStack.size() <= offset)
         return {};
      return m_parseStack[m_parseStack.size() - offset - 1].m_nonTerminal;
   }

private:

   std::unique_ptr<Lexer> m_lexer;

   std::vector<Item> m_parseStack;
};

