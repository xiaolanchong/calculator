#pragma once

#include <string>
#include <optional>
#include <limits>

enum class TokenType
{
   Number,      /// any number, e.g. '1233'
   LeftParen,   /// (
   RightParen,  /// )
   Plus,        /// +
   Minus,       /// -
   Multiply,    /// *
   End,         /// pseudo token, the end of the parsed string reached
};

/// <summary>
/// Number type the calculator operates with, change to double if needed.
/// </summary>
using NumberType = int;

struct Token
{
   TokenType m_type;     /// Token type.
   NumberType m_number;  /// Filled if the type is <c>Number</c>.
   std::string m_value;  /// Value in the input string, for debug, error reporting, etc. 

   Token(TokenType t, std::string value)
      : m_type(t), m_number(std::numeric_limits<NumberType>::min() / 2), m_value(std::move(value))
   {}

   Token(NumberType n, std::string value)
      : m_type(TokenType::Number), m_number(n), m_value(std::move(value))
   {}
};

/// <summary>
/// Class to convert a series of symbol to a stream of tokens
/// </summary>
class Lexer
{
public:
   explicit Lexer(std::string expr);

   Token GetNext();
   TokenType LookAhead();

   bool IsEmpty() const;

private:
   /// <summary>
   /// Tries to get a token with the given character in the current position.
   /// </summary>
   /// <param name="ch">Character to get</param>
   /// <param name="tt">Token type to assign</param>
   /// <param name="t">Token object to populate if the extraction is successful</param>
   /// <returns>true if extraction of the token was successful</returns>
   bool tryGetToken(char ch, TokenType tt, Token& t);

   /// <summary>
   /// Initial expression
   /// </summary>
   const std::string m_expr;

   /// <summary>
   /// Current position if <c>m_expr</c>.
   /// </summary>
   std::string::const_iterator m_pos;

   /// <summary>
   /// Token if <c>LookAhead</c> has been called.
   /// </summary>
   std::optional<Token> m_lookedToken;
};
