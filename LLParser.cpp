#include "LLParser.h"


NumberType LLParser::doOperation(LeftArgOpt leftArg, NumberType secondArg)
{
   if (!leftArg)
      return secondArg;
   switch (leftArg->second)
   {
   case OpType::Plus:
      return leftArg->first + secondArg;
   case OpType::Minus:
      return leftArg->first - secondArg;
   case OpType::Multiply:
      return leftArg->first * secondArg;
   default:
      throw std::logic_error("Unknown operation");
   }
}

NumberType LLParser::exprPlusMinus(LeftArgOpt leftArg)
{
   const NumberType firstArg = exprMulDiv({});
   const NumberType opRes = doOperation(leftArg, firstArg);
   switch (m_lexer->LookAhead())
   {
   case TokenType::Plus:
   case TokenType::Minus:
   {
      auto opToken = m_lexer->GetNext();
      return exprPlusMinus(LeftArg{ opRes, opToken.m_type == TokenType::Minus ? OpType::Minus : OpType::Plus });
   }
   default:
      return opRes;
   }
}

NumberType LLParser::exprMulDiv(LeftArgOpt leftArg)
{
   const NumberType firstArg = arg(leftArg ? FirstArg::No : FirstArg::Yes);
   const NumberType opRes = doOperation(leftArg, firstArg);
   switch (m_lexer->LookAhead())
   {
   case TokenType::Multiply:
   {
      auto opToken = m_lexer->GetNext();
      return exprMulDiv(LeftArg{ opRes, OpType::Multiply });
   }
   default:
      return opRes;
   }
}

NumberType LLParser::arg(FirstArg isFirstArg)
{
   const auto t = m_lexer->GetNext();
   switch (t.m_type)
   {
   case TokenType::Number:
   {
      return t.m_number;
   }
   case TokenType::LeftParen:
   {
      const auto firstArg = exprPlusMinus(LeftArgOpt{});
      const auto endToken = m_lexer->GetNext();
      if (endToken.m_type != TokenType::RightParen)
         throw std::runtime_error(std::string("( expected, got ") + endToken.m_value);
      return firstArg;
   }
   case TokenType::Minus:
   {
      if (isFirstArg == FirstArg::Yes)
         return -arg(FirstArg::No);
      break;
   }
   case TokenType::Plus:
   {
      if (isFirstArg == FirstArg::Yes)
         return arg(FirstArg::No);
      break;
   }
   default:;
      break;
   }
   throw std::runtime_error(std::string("Unexpected token: ") + t.m_value);
}

NumberType LLParser::ParseAndCalculate(const std::string& e)
{
   m_lexer = std::make_unique<Lexer>(e);
   return exprPlusMinus({});
}

bool LLParser::IsEmpty() const
{
   return m_lexer && m_lexer->IsEmpty();
}