#include "LRParser.h"
#include <stdexcept>


NumberType LRParser::ParseAndCalculate(const std::string& e)
{
   m_parseStack.clear();
   m_lexer = std::make_unique<Lexer>(e);

   while (!m_lexer->IsEmpty())
   {
      if (!tryReduce())
         shift();
   }

   while (tryReduce()) {}

   if (!m_parseStack.empty() && m_parseStack.front().m_nonTerminal &&
      m_parseStack.front().m_nonTerminal->m_number)
   {
      auto number = m_parseStack.front().m_nonTerminal->m_number;
      m_parseStack.erase(m_parseStack.begin());
      return number;
   }

   throw std::runtime_error("Can't parse completely");
}

void LRParser::shift()
{
   push(m_lexer->GetNext());
}

bool LRParser::tryReduce()
{
   if (m_parseStack.empty())
      return false;

   if (reduceToArg1() ||
      reduceToArg2() ||
      reduceToMulDivExpr() ||
      reduceToPlusMinExpr())
      return true;
   return false;
}

bool LRParser::reduceToArg1()
{
   const auto last = getNonTerminal();
   if (!last)
      return false;
   if (last->m_type != NonTerminalType::Arg2)
      return false;

   const auto itemPreLast = getToken(1);
   
   if (!itemPreLast)
      return false;

   // (6 or <start>6
   if (itemPreLast->m_type == TokenType::LeftParen || itemPreLast->m_type == TokenType::End)
   {
      pop(1);
      push(NonTerminal{ NonTerminalType::Arg1, last->m_number });
      return true;
   }

   /// <start>+6 or <start>-6 or (+6 or (-6
   if (itemPreLast->m_type == TokenType::Minus || itemPreLast->m_type == TokenType::Plus)
   {
      const auto itemPrePreLast = getToken(2);
      if (itemPrePreLast && (itemPrePreLast->m_type == TokenType::LeftParen ||
                            (itemPrePreLast->m_type == TokenType::End)))
      {
         auto number = last->m_number;
         pop(2);
         push(NonTerminal{ NonTerminalType::Arg1, itemPreLast->m_type == TokenType::Minus ? -number : number });
         return true;
      }
   }

   return false;
}

bool LRParser::reduceToArg2()
{
   const auto tok = getToken();
   if (tok && tok->m_type == TokenType::Number)
   {
      auto number = tok->m_number;
      pop(1);
      push(NonTerminal{ NonTerminalType::Arg2, number });
      return true;
   }

   const auto tok1 = getToken(2);
   const auto tok2 = getNonTerminal(1);
   const auto tok3 = getToken(0);
   if (tok1 && tok2 && tok3 &&
      tok1->m_type == TokenType::LeftParen &&
      tok2->m_type == NonTerminalType::PlusMinusExpr &&
      tok3->m_type == TokenType::RightParen)
   {
      pop(3);
      push(NonTerminal{ NonTerminalType::Arg2, tok2->m_number });
      return true;
   }
   return false;
}

bool LRParser::reduceToMulDivExpr()
{
   {
      const auto tok3 = getNonTerminal(2);
      const auto tok2 = getToken(1);
      const auto tok1 = getNonTerminal(0);
      if (tok1 && tok2 && tok3 &&
         tok1->m_type == NonTerminalType::Arg2 &&
         tok2->m_type == TokenType::Multiply &&
         tok3->m_type == NonTerminalType::MulDivExpr)
      {
         pop(3);
         push(NonTerminal{ NonTerminalType::MulDivExpr, tok1->m_number * tok3->m_number });
         return true;
      }
   }

   const auto tok = getNonTerminal();
   if (tok && 
      (tok->m_type == NonTerminalType::Arg1 || tok->m_type == NonTerminalType::Arg2))
   {
      pop(1);
      push(NonTerminal{NonTerminalType::MulDivExpr, tok->m_number});
      return true;
   }

   return false;
}

bool LRParser::reduceToPlusMinExpr()
{
   {
      const auto tok1 = getNonTerminal(2);
      const auto tok2 = getToken(1);
      const auto tok3 = getNonTerminal(0);
      if (tok1 && tok2 && tok3 &&
         tok1->m_type == NonTerminalType::PlusMinusExpr &&
         (tok2->m_type == TokenType::Plus || tok2->m_type == TokenType::Minus) &&
         tok3->m_type == NonTerminalType::MulDivExpr &&
         m_lexer->LookAhead() != TokenType::Multiply)
      {
         pop(3);
         auto number = tok2->m_type == TokenType::Plus ?
            tok1->m_number + tok3->m_number : tok1->m_number - tok3->m_number;
         push(NonTerminal{ NonTerminalType::PlusMinusExpr, number });
         return true;
      }
   }

   const auto tok = getNonTerminal();
   if (tok && tok->m_type == NonTerminalType::MulDivExpr &&
      (m_lexer->LookAhead() != TokenType::Multiply))
   {
      pop(1);
      push(NonTerminal{ NonTerminalType::PlusMinusExpr, tok->m_number });
      return true;
   }

   return false;
}

void LRParser::pop(size_t number)
{
   m_parseStack.resize(m_parseStack.size() - number);
}

void LRParser::push(NonTerminal&& nonTerminal)
{
   m_parseStack.push_back(Item{ std::move(nonTerminal), OptTerminal{} });
}

void LRParser::push(Token&& terminal)
{
   m_parseStack.push_back(Item{ OptNonTerminal{}, OptTerminal{std::move(terminal)} });
}