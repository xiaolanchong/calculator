#include "Lexer.h"
#include <stdexcept>
#include <cstdlib>

Lexer::Lexer(std::string expr)
   : m_expr(std::move(expr))
   , m_pos(m_expr.begin())
{}

Token Lexer::GetNext()
{
   if (m_lookedToken)
   {
      auto tok = m_lookedToken;
      m_lookedToken.reset();
      return *tok;
   }

   // skip white spaces
   for (; m_pos != m_expr.end() && (*m_pos == ' '|| *m_pos == '\t'); ++m_pos) {}

   if (m_pos == m_expr.end())
      return Token{ TokenType::End, {} };

   // try to get a math operation
   Token t{ TokenType::End, {} };
   if (
      tryGetToken('(', TokenType::LeftParen, t) ||
      tryGetToken(')', TokenType::RightParen, t) ||
      tryGetToken('+', TokenType::Plus, t) ||
      tryGetToken('-', TokenType::Minus, t) ||
      tryGetToken('*', TokenType::Multiply, t)
      )
      return t;

   // try to get a number
   std::string number;
   for (; m_pos != m_expr.end() && *m_pos >= '0' && *m_pos <= '9'; ++m_pos)
   {
      number += *m_pos;
   }
   if (!number.empty())
      return Token{ std::atoi(number.c_str()), number };

   throw std::runtime_error(std::string("Unknown character: ") + *m_pos);
}

TokenType Lexer::LookAhead()
{
   if (m_lookedToken)
      return m_lookedToken->m_type;
   m_lookedToken = GetNext();
   return m_lookedToken->m_type;
}

bool Lexer::IsEmpty() const
{
   return m_pos == m_expr.end() &&
      (!m_lookedToken.has_value() || m_lookedToken->m_type == TokenType::End);
}

bool Lexer::tryGetToken(char ch, TokenType tt, Token& t)
{
   if (*m_pos == ch)
   {
      ++m_pos;
      t = Token{ tt, std::string(1, ch) };
      return true;
   }
   return false;
}