#pragma once

#include "Lexer.h"
#include <utility>
#include <optional>
#include <stdexcept>
#include <memory>

/*
  LL(1) math expression parser with in-place expression evaluator
   Grammar rules:
   expr = exprMulDiv | exprMulDiv +/- expr
   exprMulDiv = arg1 | exprMulDiv * arg2
   arg1 = (+/-)? arg2
   arg2 = number | '(' expr ')
*/

class LLParser
{
public:
   NumberType ParseAndCalculate(const std::string& e);

   /// <summary>
   /// Checks that nothing has bean left in the input string, everything has been parsed
   /// </summary>
   /// <returns></returns>
   bool IsEmpty() const;

private:

   enum class OpType
   {
      Plus,
      Minus,
      Multiply,
   };
   using LeftArg = std::pair<NumberType, OpType>;
   using LeftArgOpt = std::optional<LeftArg>;

   enum class FirstArg
   {
      Yes,
      No
   };

   /// <summary>
   /// Parses the next addition/subtraction, e.g. 6-2*2
   /// </summary>
   /// <param name="leftArg">Argument of the previous operation</param>
   /// <returns>Numeric value of the subexpression</returns>
   NumberType exprPlusMinus(LeftArgOpt leftArg);

   /// <summary>
   /// Parses the next multiplication/division subexpression, e.g. 5*(6-2)
   /// </summary>
   /// <param name="leftArg">Argument of the previous operation</param>
   /// <returns>Numeric value of the subexpression</returns>
   NumberType exprMulDiv(LeftArgOpt leftArg);

   /// <summary>
   /// Parses the next expression argument
   /// </summary>
   /// <param name="isFirstArg">Is it the 1st argument in a subexpression, e.g. 6 in 6 + 1 - 2</param>
   /// <returns>Numeric value of the argument, e.g 6 for 6, 7 for (5+1+1)</returns>
   NumberType arg(FirstArg isFirstArg);

   NumberType doOperation(LeftArgOpt leftArg, NumberType secondArg);

private:

   std::unique_ptr<Lexer> m_lexer;
};
